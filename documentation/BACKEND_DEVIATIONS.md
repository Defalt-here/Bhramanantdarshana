# BACKEND_DEVIATIONS.md

This file lists every place where the implemented backend deviates from the
project report. Each entry records what the report says, what the code
does, and why. Reconcile during the next report-update pass.

---

## 1. Big-endian indexing was not consistently followed in the original code

**Report (implicit).** The `Quantum_registers_cl.cpp` `Print` and `Collapse`
methods, and the bit-string constructor, all consume / produce big-endian
bit-strings (qubit 0 = leftmost = MSB of the state index).

**Original code.** `HadamardR::ApplyToSingle` and `XGateR::ApplyToSingle`
used `mask = 1 << qubitIndex`, treating qubit 0 as the **LSB** —
little-endian. This made the gates internally inconsistent with the rest
of the codebase. The bug was masked in `DeutscheAlgo_example.cpp` because
the demo's overall structure happened to be symmetric, but
single-oracle traces produced incorrect intermediate states.

**Now.** All gates use `mask = 1 << (reg.bits - 1 - qubitIndex)` and the
big-endian convention is documented in `include/Conventions.h`. Cross-
validation against Qiskit (10/10 circuits, max diff 2.4e-17) confirms the
backend is consistent end-to-end.

---

## 2. CHSH benchmark uses optimal CHSH angles, not just Z and X

**Report / Stage 6 spec.** "Bell state, measure correlations along Z and X
axes for both qubits. Compute the CHSH quantity S using all four
combinations of (Z,Z), (Z,X), (X,Z), (X,X) measurement bases. Quantum
mechanics predicts S ≈ 2√2; classical bound is 2."

**What the code does.** With Alice and Bob each restricted to {Z, X}, the
CHSH quantity for `|Φ⁺⟩` is exactly 2 (the classical bound). To actually
demonstrate the quantum-mechanical violation `S ≈ 2√2`, the standard
optimal angle choice is

  Alice: `a = 0` (Z),     `a' = π/2` (X);
  Bob:   `b = π/4`,       `b' = 3π/4`.

`benchmarks/bench_chsh_violation.cpp` uses these angles, labels each row
of the output CSV with the actual angles used, and observes
`S ≈ 2.83` (theory `2√2 ≈ 2.828`).

The report can be updated to either (a) keep the Z/X claim and call out
that the implementation deliberately uses rotated bases, or (b) drop the
"all four (Z,X) combinations" wording.

---

## 3. The "comma-operator" bug in `Qubit::Collapse` was silent in the
##    original implementation

**Original code.** Lines 62–68 of `Qubits_cl.cpp` wrote
`ampliA,val[0] = {1.0, 0.0}` (and similar for the other branch). C++'s
comma operator only assigns the right-hand operand, so `ampliA` was never
updated. Subsequent reads of `ampliA` therefore returned the pre-collapse
value, which broke `MeasureWithoutCollapse` (which reads `ampliA`) right
after `Collapse`.

**Now.** Both `val[i]` and `ampliA`/`ampliB` are assigned with separate
statements. A unit test in `tests/test_register.cpp` pins the corrected
behaviour.

---

## 4. RNG migrated from C `srand`/`rand` to `std::mt19937_64`

**Report.** Refers to "random sampling" without specifying the generator.

**Original code.** Used `srand(time(0))` and `rand() / RAND_MAX`, which
produced low-quality samples on glibc and was difficult to reseed
reproducibly.

**Now.** All sampling goes through `qsim::rng` (`include/RNG.h`,
`src/RNG.cpp`), which exposes a single shared `std::mt19937_64` engine
with `seed()`, `uniform_double()`, and `engine()` accessors. Every demo,
test, and benchmark calls `qsim::rng::seed(<fixed>)` at the top of `main`
so runs are reproducible and the seed is recorded in the test log.

---

## 5. `Register::FindInnerProduct` now takes a `const&` argument

**Original code.** `complex FindInnerProduct(Register other) const` — pass
by value, copying the entire `2^n` state vector on every call.

**Now.** `complex FindInnerProduct(const Register& other) const`. No
behaviour change for valid callers; saves a deep copy that scales with
register size.

---

## 6. `_cl.cpp` files were split into header + source pairs

**Original layout.** Single-file translation units with inline class
definitions (`Quantum_registers_cl.cpp`, `Gates_cl.cpp`, etc.) included
each other via `#include "X_cl.cpp"`.

**Now.** Each class lives in `include/<Name>.h` + `src/<Name>.cpp`. The
build is driven by a top-level `Makefile`. Pre-existing demos (Deutsch,
Parity Checker) were ported to `demos/` with includes updated and a
`qsim::rng::seed(42)` call added. Their behaviour is unchanged.
