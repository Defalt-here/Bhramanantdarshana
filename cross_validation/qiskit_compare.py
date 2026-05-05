"""
qiskit_compare.py — verify the C++ simulator against Qiskit's Statevector.

For every .qsim file in the input directory:

    1. Parse the .qsim circuit (subset of the qsim_cli grammar).
    2. Build the equivalent qiskit.QuantumCircuit.
    3. Run Qiskit's Statevector simulator to get the analytic state.
    4. Run ./build/qsim_cli on the same .qsim file with `output_state`
       and read the JSON state vector it emits.
    5. Reverse Qiskit's bit ordering (Qiskit is little-endian; this
       project is big-endian) and compute statistics on the difference.

Pass criterion. A circuit PASSES iff the **maximum absolute amplitude
difference** between the qsim and Qiskit state vectors is strictly less
than `TOLERANCE` (currently 1e-10). At full double precision, two
correctly-implemented gate sequences should agree to ~1e-15; the 1e-10
threshold is conservative and intentionally loose enough to absorb
accumulated round-off across hundreds of gates.

Outputs.
  * `cross_validation/results.csv`        — one row per circuit
                                            (summary statistics).
  * `cross_validation/results_detail.csv` — one row per non-trivial
                                            basis state, showing the
                                            qsim and Qiskit amplitudes
                                            side by side along with
                                            the absolute difference.

Usage:
    python qiskit_compare.py [circuits_dir]
defaulting to ./circuits.
"""

import argparse
import csv
import json
import os
import subprocess
import sys
import tempfile
from pathlib import Path

import numpy as np
from qiskit import QuantumCircuit
from qiskit.quantum_info import Statevector

REPO_ROOT = Path(__file__).resolve().parent.parent
QSIM_CLI = REPO_ROOT / "build" / "qsim_cli"

TOLERANCE = 1e-10
"""Maximum absolute amplitude difference (per basis state) allowed for a
circuit to be considered PASS. Two correctly-implemented gate sequences
at double precision should agree to ~1e-15; 1e-10 leaves seven orders of
magnitude of headroom."""

DETAIL_THRESHOLD = 1e-12
"""Below this magnitude (on both sides), an amplitude is considered
'numerical zero' and skipped from the detail CSV. The summary statistics
still cover every basis state."""


def parse_qsim(path: Path):
    """Return (n_qubits, list of (op, args)) parsed from a .qsim file."""
    n_qubits = None
    ops = []
    for line_no, raw in enumerate(path.read_text().splitlines(), start=1):
        s = raw.strip()
        if not s or s.startswith("#"):
            continue
        toks = s.split()
        head, args = toks[0], toks[1:]
        if head == "qubits":
            n_qubits = int(args[0])
        else:
            if n_qubits is None:
                raise ValueError(
                    f"{path}:{line_no} expected `qubits N` first, got {raw!r}"
                )
            ops.append((head, args))
    if n_qubits is None:
        raise ValueError(f"{path} missing `qubits N` declaration")
    return n_qubits, ops


def build_qiskit_circuit(n_qubits: int, ops):
    """Translate the parsed ops into a qiskit.QuantumCircuit."""
    qc = QuantumCircuit(n_qubits)
    for op, args in ops:
        if op in ("print", "measure_all", "output_state"):
            continue
        if op == "H":      qc.h(int(args[0]))
        elif op == "X":    qc.x(int(args[0]))
        elif op == "Y":    qc.y(int(args[0]))
        elif op == "Z":    qc.z(int(args[0]))
        elif op == "S":    qc.s(int(args[0]))
        elif op == "T":    qc.t(int(args[0]))
        elif op == "RX":   qc.rx(float(args[1]), int(args[0]))
        elif op == "RY":   qc.ry(float(args[1]), int(args[0]))
        elif op == "RZ":   qc.rz(float(args[1]), int(args[0]))
        elif op == "CNOT": qc.cx(int(args[0]), int(args[1]))
        elif op == "CZ":   qc.cz(int(args[0]), int(args[1]))
        elif op == "SWAP": qc.swap(int(args[0]), int(args[1]))
        elif op == "TOFFOLI":
            qc.ccx(int(args[0]), int(args[1]), int(args[2]))
        else:
            raise ValueError(f"unsupported op: {op}")
    return qc


def qiskit_statevector(qc: QuantumCircuit) -> np.ndarray:
    """Return Qiskit's amplitudes re-indexed to big-endian order.

    Qiskit places the lowest-index qubit (q0) as the LSB of the state
    integer; this project places q0 as the MSB. Permute by reversing the
    binary representation of every index.
    """
    sv = Statevector(qc).data
    n = qc.num_qubits
    out = np.zeros_like(sv)
    for i in range(len(sv)):
        bits = format(i, f"0{n}b")
        j = int(bits[::-1], 2)
        out[j] = sv[i]
    return out


def cpp_statevector(circuit_path: Path) -> np.ndarray:
    """Run qsim_cli on the circuit, capture its output_state JSON."""
    if not QSIM_CLI.exists():
        raise FileNotFoundError(
            f"{QSIM_CLI} not found — build it with `make qsim_cli` first."
        )
    with tempfile.NamedTemporaryFile("w+", suffix=".json", delete=False) as tmp:
        tmp_json = Path(tmp.name)
    try:
        program = circuit_path.read_text() + f"\noutput_state {tmp_json}\n"
        proc = subprocess.run(
            [str(QSIM_CLI)],
            input=program,
            text=True,
            capture_output=True,
            check=False,
        )
        if proc.returncode != 0:
            raise RuntimeError(
                f"qsim_cli failed on {circuit_path.name}:\n{proc.stderr}"
            )
        data = json.loads(tmp_json.read_text())
        amps = np.array(
            [complex(re, im) for re, im in data["amplitudes"]],
            dtype=complex,
        )
        return amps
    finally:
        try:
            tmp_json.unlink()
        except FileNotFoundError:
            pass


class CircuitResult:
    __slots__ = (
        "name", "n_qubits", "n_amplitudes",
        "qsim_norm", "qiskit_norm",
        "max_abs_diff", "mean_abs_diff",
        "qsim_amps", "qiskit_amps",
        "verdict",
    )

    def __init__(self, name, n_qubits, qsim_amps, qiskit_amps):
        self.name = name
        self.n_qubits = n_qubits
        self.n_amplitudes = len(qsim_amps)
        self.qsim_norm = float(np.sqrt(np.sum(np.abs(qsim_amps) ** 2)))
        self.qiskit_norm = float(np.sqrt(np.sum(np.abs(qiskit_amps) ** 2)))
        diffs = np.abs(qsim_amps - qiskit_amps)
        self.max_abs_diff = float(np.max(diffs))
        self.mean_abs_diff = float(np.mean(diffs))
        self.qsim_amps = qsim_amps
        self.qiskit_amps = qiskit_amps
        self.verdict = "PASS" if self.max_abs_diff < TOLERANCE else "FAIL"


def compare_circuit(circuit_path: Path) -> CircuitResult:
    n, ops = parse_qsim(circuit_path)
    qc = build_qiskit_circuit(n, ops)
    qiskit_amps = qiskit_statevector(qc)
    cpp_amps = cpp_statevector(circuit_path)
    if cpp_amps.shape != qiskit_amps.shape:
        raise RuntimeError(
            f"shape mismatch on {circuit_path.name}: "
            f"cpp={cpp_amps.shape} qiskit={qiskit_amps.shape}"
        )
    return CircuitResult(circuit_path.stem, n, cpp_amps, qiskit_amps)


def write_summary_csv(path: Path, results):
    with path.open("w", newline="") as f:
        w = csv.writer(f)
        w.writerow([
            "circuit", "n_qubits", "n_amplitudes",
            "qsim_norm", "qiskit_norm",
            "max_abs_diff", "mean_abs_diff",
            "threshold", "verdict",
        ])
        for r in results:
            w.writerow([
                r.name, r.n_qubits, r.n_amplitudes,
                f"{r.qsim_norm:.10f}",  f"{r.qiskit_norm:.10f}",
                f"{r.max_abs_diff:.6e}", f"{r.mean_abs_diff:.6e}",
                f"{TOLERANCE:.0e}",      r.verdict,
            ])


def write_detail_csv(path: Path, results):
    with path.open("w", newline="") as f:
        w = csv.writer(f)
        w.writerow([
            "circuit", "basis_state",
            "qsim_real", "qsim_imag",
            "qiskit_real", "qiskit_imag",
            "abs_diff",
        ])
        for r in results:
            n = r.n_qubits
            for i in range(r.n_amplitudes):
                a = r.qsim_amps[i]
                b = r.qiskit_amps[i]
                if abs(a) < DETAIL_THRESHOLD and abs(b) < DETAIL_THRESHOLD:
                    continue
                bits = format(i, f"0{n}b")
                w.writerow([
                    r.name, bits,
                    f"{a.real:.16f}", f"{a.imag:.16f}",
                    f"{b.real:.16f}", f"{b.imag:.16f}",
                    f"{abs(a - b):.6e}",
                ])


def main():
    ap = argparse.ArgumentParser(description=__doc__.strip())
    ap.add_argument(
        "circuits_dir", nargs="?",
        default=str(Path(__file__).parent / "circuits"),
        help="directory containing .qsim files",
    )
    args = ap.parse_args()
    circuits_dir = Path(args.circuits_dir)
    if not circuits_dir.is_dir():
        print(f"error: {circuits_dir} is not a directory", file=sys.stderr)
        return 1

    paths = sorted(circuits_dir.glob("*.qsim"))
    if not paths:
        print(f"no .qsim circuits found under {circuits_dir}", file=sys.stderr)
        return 1

    print(f"Pass tolerance: max_abs_diff < {TOLERANCE:.0e} per basis state")
    print(f"Comparing {len(paths)} circuit(s) against Qiskit Statevector")
    print(
        f"{'circuit':<32} "
        f"{'n_qubits':>8} "
        f"{'max_abs_diff':>16} "
        f"{'mean_abs_diff':>16}  verdict"
    )
    print("-" * 86)

    results = []
    failed = 0
    for path in paths:
        try:
            res = compare_circuit(path)
        except Exception as e:
            print(f"  {path.stem:<30} ERROR: {e}", file=sys.stderr)
            failed += 1
            continue
        results.append(res)
        if res.verdict != "PASS":
            failed += 1
        print(
            f"  {res.name:<30} "
            f"{res.n_qubits:>8} "
            f"{res.max_abs_diff:>16.3e} "
            f"{res.mean_abs_diff:>16.3e}  {res.verdict}"
        )

    summary_path = Path(__file__).parent / "results.csv"
    detail_path = Path(__file__).parent / "results_detail.csv"
    write_summary_csv(summary_path, results)
    write_detail_csv(detail_path, results)
    print()
    print(f"Wrote {summary_path}  ({len(results)} circuits)")
    print(f"Wrote {detail_path}   (per-amplitude side-by-side)")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
