# Bhramanantdarshana

> A from-first-principles **C++ quantum register and gate simulator**, built
> as an undergraduate final-year project. Standard library only — no
> external quantum SDKs (Eigen, Boost, Qiskit, …) — so every layer of the
> simulator is readable by a third-year undergraduate.

---

## Tested

| Layer                    | Coverage                                                                                              |
|--------------------------|-------------------------------------------------------------------------------------------------------|
| **Unit tests**           | 64 / 64 passing across 8 suites (Register, single-qubit, two-qubit, register-gates, measurement, integration, properties, statistical). |
| **Benchmarks**           | 4 benchmarks — gate throughput, norm drift, max-qubit allocation, **CHSH violation** (S ≈ 2.83 vs theory 2√2). |
| **Cross-validation**     | 10 / 10 circuits agree with Qiskit `1.2.4`'s `Statevector`. Pass criterion: `max_abs_diff < 1e-10` per basis state; observed diffs `0` to `~2.4e-17`. |
| **Qiskit benchmark comparison** | Side-by-side timing for gate throughput, norm drift, and empirical CHSH; written to `benchmarks/results/qiskit_comparison.csv`. |

Refresh commands:

```bash
make run-tests          # → tests/unit_test_log.txt
make run-benchmarks     # → benchmarks/results/bench_*.csv
make cross-validate     # → cross_validation/results.csv + results_detail.csv
make compare-benchmarks # → benchmarks/results/qiskit_comparison.csv
```

### Cross-validation outputs (report-ready)

`make cross-validate` produces two CSVs designed for inclusion in a
report:

* `cross_validation/results.csv` — one row per circuit with
  `n_qubits, n_amplitudes, qsim_norm, qiskit_norm, max_abs_diff,
  mean_abs_diff, threshold, verdict`. The `threshold` column makes the
  pass criterion explicit on every row.
* `cross_validation/results_detail.csv` — one row per non-trivial basis
  state with `qsim_real, qsim_imag, qiskit_real, qiskit_imag, abs_diff`,
  so the actual computed amplitudes appear next to Qiskit's reference.

---

## Quick start

```bash
make all                  # library + demos + tests + benchmarks + qsim_cli
make run-tests            # ./build/run_all + refresh tests/unit_test_log.txt
make run-benchmarks       # all 4 benchmarks → benchmarks/results/*.csv
make cross-validate       # python cross_validation/qiskit_compare.py
```

Compile flags are `-std=c++17 -Wall -Wextra -O2 -Iinclude`.

Run a single demo:

```bash
./build/demo_bell                            # 4 Bell states + 1000 samples each
./build/demo_grover                          # 3-qubit Grover search
./build/demo_deutsch_jozsa 5                 # n = 5 input qubits
./build/demo_bernstein_vazirani 10110101     # recover s = 10110101
```

Or drive the simulator from any language via the stdin protocol:

```bash
printf 'qubits 2\nH 0\nCNOT 0 1\nprint\noutput_state /tmp/bell.json\n' \
  | ./build/qsim_cli
```

The grammar is documented in [`cli/PROTOCOL.md`](cli/PROTOCOL.md).

---

## Why it exists

Most "tutorial" quantum simulators either hide details behind abstractions
or overwhelm newcomers with framework overhead. This project shows:

- a quantum register is just a normalised complex vector,
- single-qubit gates act via pairwise amplitude mixing under bit masks,
- two-qubit gates use the same idea with two masks,
- measurement turns amplitudes → probabilities → sampled bit-strings,
- algorithms (Deutsch, Deutsch–Jozsa, Bernstein–Vazirani, Grover) exploit
  superposition and phase kickback to beat classical query counts.

Every public class and method has a Doxygen comment block explaining the
matrix, action, and a key algebraic identity.

---

## Conventions

- **Big-endian indexing.** Qubit 0 is the most-significant bit of the
  state-vector index. State `|abc⟩` (qubit 0 = a, qubit 1 = b, qubit 2 = c)
  is stored at `val[4a + 2b + c]`. See
  [`include/Conventions.h`](include/Conventions.h) for the worked
  3-qubit example.
- **Tolerances.** `qsim::kNormTolerance = 1e-9`,
  `qsim::kAmplitudeTolerance = 1e-10`.
- **Register cap.** `qsim::kMaxQubits = 14` (≈ 256 KB amplitudes).

---

## Gate set

| Arity        | Class names                                                                            |
|--------------|----------------------------------------------------------------------------------------|
| Single-qubit | `IdentityR`, `HadamardR`, `XGateR`, `YGateR`, `ZGateR`, `SGateR`, `TGateR`, `RxR(θ)`, `RyR(θ)`, `RzR(θ)` |
| Two-qubit    | `CNOTGateR`, `CZGateR`, `SWAPGateR`                                                    |
| Three-qubit  | `ToffoliGateR`                                                                         |

The textbook single-qubit `Identity`, `Hadamard`, `PauliX/Y/Z`, `SGate`,
`TGate` live in [`include/Gates.h`](include/Gates.h) and act on
single-qubit `Qubit` objects.

Full reference: [`BACKEND_API.md`](BACKEND_API.md).

---

## File layout

```
include/                 # public headers
src/                     # implementation
demos/                   # 8 standalone demos: Bell, GHZ, Deutsch, DJ, BV,
                         #   Grover, phase kickback, parity checker
tests/                   # 8 test suites + run_all + harness (header-only)
benchmarks/              # 4 benchmarks + run_all.sh; results/*.csv
cli/                     # qsim_cli.cpp + PROTOCOL.md (stdin → simulator)
cross_validation/        # qiskit_compare.py + 10 .qsim circuits + results.csv
visualization/           # plotter.py — bar + line overlay
data/                    # canonical sample CSVs (coin flips, Bell, GHZ, ...)
quantum-frontend/        # Next.js frontend (out of scope here)
```

---

## Algorithm demos

| Demo                       | What it does                                                                  |
|----------------------------|-------------------------------------------------------------------------------|
| `demo_bell`                | Constructs all four Bell states, samples 1000 outcomes from each.             |
| `demo_ghz`                 | 3-qubit GHZ via H+CNOT+CNOT, then via H+CZ rewrite, asserts agreement.        |
| `demo_deutsch`             | All four single-bit oracles; deterministically classifies constant/balanced.  |
| `demo_deutsch_jozsa`       | n-qubit Deutsch–Jozsa with 4 oracle types (default n = 4).                    |
| `demo_bernstein_vazirani`  | Recovers an n-bit hidden string in one quantum query.                         |
| `demo_grover`              | 3-qubit Grover; sweeps iteration count vs empirical and theoretical success.  |
| `demo_phase_kickback`      | Two-qubit demo of the kickback mechanism behind every Deutsch-style oracle.   |
| `demo_parity_checker`      | Side-by-side classical vs quantum parity checking on 3-bit inputs.            |

---

## Cross-validation

`make cross-validate` runs every `.qsim` circuit under
`cross_validation/circuits/` through both this simulator and Qiskit's
`Statevector`, prints a per-circuit max-amplitude diff, and writes
`cross_validation/results.csv`. All ten circuits currently pass at
diff ≤ 2.4e-17. See [`cross_validation/README.md`](cross_validation/README.md)
for setup.

---

## Visualization

```bash
python visualization/plotter.py data/bell_500_samples.csv
# → data/bell_500_samples_plot.png
```

`plotter.py` reads the consolidated single-column `outcome` CSV format
produced by `Register::WriteMeasurementsCSV` and the CLI's `measure_all`
command, groups outcomes, and renders a bar + line overlay.

---

## Documents

All long-form documentation lives under [`documentation/`](documentation/).

- [`documentation/BACKEND_API.md`](documentation/BACKEND_API.md) — every
  public class and function.
- [`documentation/BACKEND_DEVIATIONS.md`](documentation/BACKEND_DEVIATIONS.md)
  — places where the implementation diverges from the original project
  report (kept until the report is updated).
- [`documentation/PROJECT_DOCUMENTATION.md`](documentation/PROJECT_DOCUMENTATION.md)
  — extended project write-up.
- [`documentation/DOCUMENTATION_INDEX.md`](documentation/DOCUMENTATION_INDEX.md)
  — index over the rest (presentation PDF, prep guide, etc.).
- [`cli/PROTOCOL.md`](cli/PROTOCOL.md) — `.qsim` grammar and JSON schema.
- [`tests/README.md`](tests/README.md) — running tests, reading χ² output.
- [`cross_validation/README.md`](cross_validation/README.md) — Qiskit
  cross-validation setup.

---

## Status & contributions

Pull requests, critiques, and naming suggestions welcome. The pedagogical
constraint (everything readable by a third-year undergraduate, standard
library only) is intentional — please keep additions minimal and
self-contained.
