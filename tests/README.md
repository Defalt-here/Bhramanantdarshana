# Tests

This directory holds a hand-rolled test suite for the C++ simulator. There
is no GoogleTest/Catch2 dependency; the harness is a single header
([test_harness.h](test_harness.h)) under 80 lines.

## Building and running

From the repository root:

```
make tests          # builds build/run_all
make run-tests      # runs build/run_all and refreshes tests/unit_test_log.txt
```

Or directly:

```
./build/run_all
```

The runner exits with status equal to the number of failed tests (0 means
all passed), which is convenient for CI and `make` chaining.

## Where the log goes

Every run rewrites `tests/unit_test_log.txt`. The log records:

- the timestamp of the run,
- the RNG seed (always `20260505` so statistical tests are reproducible),
- one `PASS` / `FAIL` line per test, prefixed with `<suite>.<name>`,
- a per-failure detail block listing the file and line of every triggered
  assertion (a single failing test can report several assertion failures
  because the harness records and continues rather than aborting),
- a final `[N/M passed, K failed]` summary.

## Test layout

| File                              | Suite focus                                    | Tests |
|-----------------------------------|------------------------------------------------|-------|
| `test_register.cpp`               | `Register` class, normalisation, measurement   | 9     |
| `test_single_qubit_gates.cpp`     | `QGates` family on `Qubit`                     | 14    |
| `test_two_qubit_gates.cpp`        | `CNOTGateR`, `CZGateR`, `SWAPGateR`, `Toffoli` | 10    |
| `test_register_gates.cpp`         | Register-level single-qubit gates              | 8     |
| `test_measurement.cpp`            | Collapse, peek, Born sampling, sticky outcomes | 5     |
| `test_integration.cpp`            | Bell, GHZ, Deutsch (4 oracles), `XYZ` algebra  | 8     |
| `test_properties.cpp`             | 100-state property checks of `H²=I` etc.       | 6     |
| `test_statistical.cpp`            | χ² goodness-of-fit at 5 % significance         | 4     |

## Reading χ² results in `test_statistical.cpp`

For each circuit the test takes 10 000 samples, builds an empirical
histogram, and compares it to the analytic Born-rule distribution via

```
χ² = Σᵢ (Oᵢ − Eᵢ)² / Eᵢ.
```

We then check `χ² < χ²_crit(α=0.05, df=k−1)` where `k` is the number of
non-trivial outcomes. The critical values used are:

| df | χ²_crit (α = 0.05) |
|----|--------------------|
| 1  | 3.841              |
| 3  | 7.815              |
| 7  | 14.067             |

A failure means the empirical distribution diverges from theory at the 5 %
significance level — almost always due to a bug in the gate
implementation or a regression in the RNG, not a freak sample. With 10 000
samples and a properly working simulator, χ² should sit comfortably below
the critical value.
