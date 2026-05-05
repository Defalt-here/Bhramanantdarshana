# Cross-validation against Qiskit

Every `.qsim` circuit under `cross_validation/circuits/` is run through
both this simulator (via `qsim_cli`) and Qiskit's `Statevector`. The
amplitudes are then aligned (Qiskit is little-endian, this project is
big-endian) and compared.

## Pass criterion

A circuit **passes** iff the **maximum absolute amplitude difference**
between the two state vectors is strictly less than

```
TOLERANCE = 1e-10
```

per basis state. At full double precision two correctly-implemented gate
sequences should agree to roughly `1e-15`; the threshold of `1e-10`
intentionally keeps seven orders of magnitude of headroom for accumulated
round-off across long circuits. Current observed diffs sit between
`0` and `~2.4e-17`.

The tolerance is set in `cross_validation/qiskit_compare.py` as the
`TOLERANCE` constant — change it there if you ever need a tighter or
looser bound, and document the change in `BACKEND_DEVIATIONS.md`.

## Setup

```bash
python -m venv venv
source venv/bin/activate
pip install -r cross_validation/requirements.txt
make qsim_cli            # builds build/qsim_cli used by the harness
```

Qiskit is pinned to `1.2.4` ([requirements.txt](requirements.txt)) so the
comparison is reproducible across machines.

## Run

```bash
make cross-validate
# or:
python cross_validation/qiskit_compare.py cross_validation/circuits/
```

Two CSVs are produced.

### `cross_validation/results.csv` — summary, one row per circuit

| Column           | Description                                                                |
|------------------|----------------------------------------------------------------------------|
| `circuit`        | Circuit name (the `.qsim` filename without the extension).                 |
| `n_qubits`       | Register size declared by the circuit.                                     |
| `n_amplitudes`   | `2^n_qubits`.                                                              |
| `qsim_norm`      | √(Σ|αᵢ|²) of the qsim state vector — should be 1.                          |
| `qiskit_norm`    | √(Σ|αᵢ|²) of the Qiskit state vector — should be 1.                        |
| `max_abs_diff`   | Per-basis-state maximum of `|α_qsim(i) − α_qiskit(i)|`. The PASS criterion is on this column. |
| `mean_abs_diff`  | Mean per-basis-state amplitude difference (gives a sense of average error). |
| `threshold`      | `TOLERANCE` (currently `1e-10`).                                           |
| `verdict`        | `PASS` if `max_abs_diff < threshold`, otherwise `FAIL`.                    |

### `cross_validation/results_detail.csv` — per-amplitude side-by-side

One row per non-trivial basis state across all circuits. "Non-trivial"
means at least one of the two amplitudes has magnitude ≥ `1e-12`. This
keeps the file small (no rows for the long lists of analytic zeros).

| Column        | Description                                                |
|---------------|------------------------------------------------------------|
| `circuit`     | Same circuit name as above.                                |
| `basis_state` | Big-endian bit-string of the basis state (e.g. `0110`).    |
| `qsim_real`   | Real part of qsim's amplitude for this basis state.        |
| `qsim_imag`   | Imaginary part of qsim's amplitude.                        |
| `qiskit_real` | Real part of Qiskit's amplitude.                           |
| `qiskit_imag` | Imaginary part of Qiskit's amplitude.                      |
| `abs_diff`    | `|α_qsim − α_qiskit|`.                                     |

This is the format intended for the report: drop the rows for the
circuit you want to show, and you have a publication-ready table of
"qsim vs Qiskit, side by side, including the diff".

## Interpreting failures

`FAIL` means `max_abs_diff ≥ 1e-10`. Two common causes:

1. A regression in a gate implementation (most likely culprit when only
   one gate-specific circuit fails).
2. A bit-ordering bug between Qiskit (little-endian by default) and this
   simulator (big-endian; qubit 0 = MSB). The harness reverses every
   index before comparing, so both representations end up in big-endian.
   When adding a new circuit that unexpectedly fails, double-check that
   `qiskit_statevector()` reverses bits correctly for it.

## Adding a circuit

1. Drop a new `.qsim` file under `cross_validation/circuits/`. The
   grammar is documented in [`../cli/PROTOCOL.md`](../cli/PROTOCOL.md).
2. Re-run `make cross-validate`. The new circuit appears automatically
   in the run output and in both CSVs.

The harness skips IO-only commands (`print`, `measure_all`,
`output_state`) when building the Qiskit version of the circuit, so you
can leave them in the `.qsim` file if a demo also reads it.
