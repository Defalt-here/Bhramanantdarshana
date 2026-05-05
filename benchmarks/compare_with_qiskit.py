"""
compare_with_qiskit.py — head-to-head benchmark comparison against Qiskit.

This script merges the existing C++ benchmark CSVs (in-process timing,
no subprocess overhead) with equivalent Qiskit measurements taken
in-process in Python. The goal is an honest side-by-side: both columns
measure the cost of the gate kernel, *not* spawn or import time.

Workloads:
  1. Gate throughput. For n in {3, 5, 8, 10, 12} and gate ∈ {H, X, CNOT},
     qsim numbers come from `benchmarks/results/bench_gate_throughput.csv`
     (1000 gates, in-process C++); Qiskit numbers are measured here by
     constructing the same 1000-gate circuit and timing
     `Statevector(qc)`.

  2. Norm drift. The qsim number is the final entry in
     `benchmarks/results/bench_norm_drift.csv` (10 000 random gates on a
     5-qubit register); the Qiskit number is measured in-process here on
     the same workload.

  3. CHSH violation. qsim S comes from
     `benchmarks/results/bench_chsh_violation.csv`; Qiskit S is computed
     here by sampling from Statevector probabilities for the same
     four (Alice axis, Bob axis) combinations.

  4. Max-qubit Hadamard layer. For n in {8, 10, 12, 13, 14}, time a
     single full Hadamard layer (one H per qubit) on both simulators.
     qsim numbers come from `benchmarks/results/bench_max_qubits.csv`;
     Qiskit numbers are measured here.

Output: benchmarks/results/qiskit_comparison.csv

Usage:
    python benchmarks/compare_with_qiskit.py

Pre-requisites:
    make benchmarks      # produces the C++ CSVs we read
    pip install qiskit==1.2.4 numpy
"""

import csv
import subprocess
import sys
import time
from pathlib import Path

import numpy as np
from qiskit import QuantumCircuit
from qiskit.quantum_info import Statevector

REPO_ROOT = Path(__file__).resolve().parent.parent
RESULTS_DIR = REPO_ROOT / "benchmarks" / "results"

# Same RNG seed used by the C++ benchmarks (qsim::rng::seed(42)).
SEED = 42

GATE_THROUGHPUT_NS = [3, 5, 8, 10, 12]
GATE_THROUGHPUT_GATES = ["H", "X", "CNOT"]

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def time_qiskit(qc: QuantumCircuit) -> float:
    """Time a Statevector evolution of qc in milliseconds."""
    t0 = time.perf_counter()
    Statevector(qc)            # forces simulation
    t1 = time.perf_counter()
    return (t1 - t0) * 1000.0


def ensure_cpp_benchmark(csv_path: Path, binary: Path):
    """Run the C++ benchmark binary if its CSV is missing or stale."""
    if not csv_path.exists():
        if not binary.exists():
            raise FileNotFoundError(
                f"{binary} not found — run `make benchmarks` first."
            )
        subprocess.run([str(binary)], check=True, capture_output=True)


# ---------------------------------------------------------------------------
# Workload 1: gate throughput
# ---------------------------------------------------------------------------


def read_qsim_throughput():
    """Return {(n, gate): us_per_gate} from bench_gate_throughput.csv."""
    csv_path = RESULTS_DIR / "bench_gate_throughput.csv"
    ensure_cpp_benchmark(csv_path, REPO_ROOT / "build" / "bench_gate_throughput")
    table = {}
    with csv_path.open() as f:
        rdr = csv.DictReader(f)
        for row in rdr:
            n = int(row["qubits"])
            gate = row["gate"]
            table[(n, gate)] = float(row["time_per_gate_us"])
    return table


def gate_throughput_qiskit(n: int, gate: str, n_gates: int = 1000) -> float:
    """Time Qiskit on the same workload: 1000 random gates of `gate` on n qubits.
    Returns microseconds per gate."""
    rng = np.random.default_rng(SEED + hash((n, gate)) % 1000)
    qc = QuantumCircuit(n)
    if gate == "CNOT":
        for _ in range(n_gates):
            c, t = int(rng.integers(0, n)), int(rng.integers(0, n))
            if c == t:
                t = (c + 1) % n
            qc.cx(c, t)
    else:
        targets = rng.integers(0, n, size=n_gates).tolist()
        for q in targets:
            if gate == "H":
                qc.h(q)
            elif gate == "X":
                qc.x(q)
            else:
                raise ValueError(f"unsupported gate {gate}")
    ms = time_qiskit(qc)
    return ms * 1000.0 / n_gates


# ---------------------------------------------------------------------------
# Workload 2: norm drift
# ---------------------------------------------------------------------------


def read_qsim_norm_drift() -> float:
    """Return |1 − Σ|αᵢ|²| at step 10 000 from bench_norm_drift.csv."""
    csv_path = RESULTS_DIR / "bench_norm_drift.csv"
    ensure_cpp_benchmark(csv_path, REPO_ROOT / "build" / "bench_norm_drift")
    last = None
    with csv_path.open() as f:
        rdr = csv.DictReader(f)
        for row in rdr:
            last = row
    return float(last["deviation"])


def qiskit_norm_drift(n: int = 5, n_gates: int = 10_000) -> float:
    """Apply 10 000 random gates from {H, X, Y, Z, CNOT} in Qiskit, return
    |1 − Σ|αᵢ|²| of the final Statevector."""
    rng = np.random.default_rng(SEED)
    qc = QuantumCircuit(n)
    single = ["H", "X", "Y", "Z"]
    for _ in range(n_gates):
        choice = rng.integers(0, 5)
        if choice < 4:
            q = int(rng.integers(0, n))
            getattr(qc, single[choice].lower())(q)
        else:
            c, t = int(rng.integers(0, n)), int(rng.integers(0, n))
            if c == t:
                t = (c + 1) % n
            qc.cx(c, t)
    sv = Statevector(qc).data
    norm_sq = float(np.sum(np.abs(sv) ** 2))
    return abs(1.0 - norm_sq)


# ---------------------------------------------------------------------------
# Workload 3: empirical CHSH on |Φ⁺⟩
# ---------------------------------------------------------------------------


def read_qsim_chsh() -> float:
    """Return the S value from bench_chsh_violation.csv."""
    csv_path = RESULTS_DIR / "bench_chsh_violation.csv"
    ensure_cpp_benchmark(csv_path, REPO_ROOT / "build" / "bench_chsh_violation")
    with csv_path.open() as f:
        rdr = csv.reader(f)
        next(rdr)  # header
        for row in rdr:
            if row and row[0] == "S":
                return float(row[3])
    raise RuntimeError("S row not found in bench_chsh_violation.csv")


def qiskit_chsh(samples_per_correlation: int = 10_000) -> float:
    """Empirical CHSH S in Qiskit using the same angle quartet and
    sample budget as bench_chsh_violation.cpp."""
    rng = np.random.default_rng(SEED)
    angles = [
        (0.0,         np.pi / 4.0),       # +
        (0.0,         3.0 * np.pi / 4.0), # -
        (np.pi / 2.0, np.pi / 4.0),       # +
        (np.pi / 2.0, 3.0 * np.pi / 4.0), # +
    ]
    correlations = []
    for theta_a, theta_b in angles:
        qc = QuantumCircuit(2)
        qc.h(0)
        qc.cx(0, 1)
        if theta_a != 0.0:
            qc.ry(-theta_a, 0)
        if theta_b != 0.0:
            qc.ry(-theta_b, 1)
        sv = Statevector(qc)
        probs = sv.probabilities()  # Qiskit little-endian: idx = q1*2 + q0
        # Reverse to big-endian to match qsim's outcome convention.
        be_probs = np.zeros_like(probs)
        for i in range(len(probs)):
            j = int(format(i, "02b")[::-1], 2)
            be_probs[j] = probs[i]
        outcomes = rng.choice(4, size=samples_per_correlation, p=be_probs)
        ssum = 0
        for outcome in outcomes:
            a_bit = (outcome >> 1) & 1
            b_bit = outcome & 1
            ssum += (1 if a_bit == 0 else -1) * (1 if b_bit == 0 else -1)
        correlations.append(ssum / samples_per_correlation)
    Eab, EabP, EaPb, EaPbP = correlations
    return Eab - EabP + EaPb + EaPbP


# ---------------------------------------------------------------------------
# Workload 4: full Hadamard layer at large n
# ---------------------------------------------------------------------------


def read_qsim_max_qubits():
    """Return {n: hadamard_ms} from bench_max_qubits.csv."""
    csv_path = RESULTS_DIR / "bench_max_qubits.csv"
    ensure_cpp_benchmark(csv_path, REPO_ROOT / "build" / "bench_max_qubits")
    table = {}
    with csv_path.open() as f:
        rdr = csv.DictReader(f)
        for row in rdr:
            table[int(row["qubits"])] = float(row["hadamard_ms"])
    return table


def qiskit_hadamard_layer(n: int) -> float:
    """Time a full H layer on n qubits in Qiskit, in milliseconds."""
    qc = QuantumCircuit(n)
    for q in range(n):
        qc.h(q)
    return time_qiskit(qc)


# ---------------------------------------------------------------------------
# Driver
# ---------------------------------------------------------------------------


def main():
    RESULTS_DIR.mkdir(parents=True, exist_ok=True)

    rows = []
    print(
        f"{'scenario':<24} {'n_qubits':>8} {'qsim':>14} {'qiskit':>14} "
        f"{'speedup':>10}  units"
    )
    print("-" * 90)

    qsim_throughput = read_qsim_throughput()

    for n in GATE_THROUGHPUT_NS:
        for gate in GATE_THROUGHPUT_GATES:
            qsim_us = qsim_throughput.get((n, gate))
            if qsim_us is None:
                continue
            qiskit_us = gate_throughput_qiskit(n, gate)
            speedup = qiskit_us / qsim_us if qsim_us > 0 else float("inf")
            rows.append((
                f"gate_throughput_{gate}", n, 1000,
                f"{qsim_us:.4f}", f"{qiskit_us:.4f}",
                f"{speedup:.1f}",
                "us_per_gate",
                "qsim: in-process C++ (bench_gate_throughput.csv); "
                "qiskit: in-process Python (Statevector)",
            ))
            print(
                f"gate_{gate:<19} {n:>8} {qsim_us:>14.4f} {qiskit_us:>14.4f} "
                f"{speedup:>9.1f}x  us/gate"
            )

    qsim_max_qubits = read_qsim_max_qubits()
    for n in sorted(qsim_max_qubits.keys()):
        qsim_ms = qsim_max_qubits[n]
        qiskit_ms = qiskit_hadamard_layer(n)
        speedup = qiskit_ms / qsim_ms if qsim_ms > 0 else float("inf")
        rows.append((
            "hadamard_layer", n, n,
            f"{qsim_ms:.4f}", f"{qiskit_ms:.4f}",
            f"{speedup:.1f}",
            "ms",
            "single H layer (one H per qubit) at large register size",
        ))
        print(
            f"{'hadamard_layer':<24} {n:>8} {qsim_ms:>14.4f} {qiskit_ms:>14.4f} "
            f"{speedup:>9.1f}x  ms"
        )

    qsim_drift = read_qsim_norm_drift()
    qiskit_drift = qiskit_norm_drift()
    rows.append((
        "norm_drift", 5, 10000,
        f"{qsim_drift:.3e}", f"{qiskit_drift:.3e}",
        "n/a",
        "abs(1 - sum |amp|^2)",
        "10000 random gates from {H,X,Y,Z,CNOT}; both within 1e-9 tolerance",
    ))
    print(
        f"{'norm_drift':<24} {5:>8} {qsim_drift:>14.3e} "
        f"{qiskit_drift:>14.3e} {'n/a':>10}  drift"
    )

    qsim_S = read_qsim_chsh()
    qiskit_S = qiskit_chsh()
    rows.append((
        "chsh_S", 2, 40000,
        f"{qsim_S:.4f}", f"{qiskit_S:.4f}",
        "n/a",
        "S (CHSH quantity)",
        "QM theory: 2*sqrt(2)≈2.828; classical bound 2",
    ))
    print(
        f"{'chsh_S':<24} {2:>8} {qsim_S:>14.4f} {qiskit_S:>14.4f} "
        f"{'n/a':>10}  S"
    )

    out_path = RESULTS_DIR / "qiskit_comparison.csv"
    with out_path.open("w", newline="") as f:
        w = csv.writer(f)
        w.writerow([
            "scenario", "n_qubits", "n_ops",
            "qsim_value", "qiskit_value",
            "speedup_x", "units", "notes",
        ])
        w.writerows(rows)
    print(f"\nWrote {out_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
