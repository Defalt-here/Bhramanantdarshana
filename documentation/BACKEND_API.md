# BACKEND_API.md

Public reference for the C++ backend. Every class and free function in
`include/` is listed below with its signature, complexity, and a one-line
description. Full Doxygen comments live next to each declaration in the
header.

All qubit indices are **big-endian** (qubit 0 = MSB of the state-vector
index). See [`include/Conventions.h`](../include/Conventions.h) for the
worked example and tolerance constants.

---

## `include/Conventions.h`

```cpp
namespace qsim {
inline constexpr double kNormTolerance      = 1e-9;
inline constexpr double kAmplitudeTolerance = 1e-10;
inline constexpr int    kMaxQubits          = 14;
}
```

| Symbol                         | Type                       | Meaning                                                     |
|--------------------------------|----------------------------|-------------------------------------------------------------|
| `qsim::kNormTolerance`         | `constexpr double`         | Allowed `|Σ|αᵢ|² − 1|` after a sequence of gates.           |
| `qsim::kAmplitudeTolerance`    | `constexpr double`         | Per-amplitude tolerance for state-vector equality.          |
| `qsim::kMaxQubits`             | `constexpr int`            | Hard cap on register size (16 KB amplitudes at the cap).    |

---

## `include/RNG.h`

```cpp
namespace qsim::rng {
void               seed(std::uint64_t s);
std::uint64_t      default_seed();
double             uniform_double();
std::mt19937_64&   engine();
}
```

| Function                | Complexity | Description                                                   |
|-------------------------|------------|---------------------------------------------------------------|
| `seed(s)`               | O(1)       | Deterministically reseed the shared engine.                   |
| `default_seed()`        | O(1)       | 64-bit seed drawn from `std::random_device`.                  |
| `uniform_double()`      | O(1)       | Sample from `[0, 1)` on the shared engine.                    |
| `engine()`              | O(1)       | Direct access for non-uniform draws.                          |

---

## `include/Qubits.h` — `class Qubit`

| Member                                   | Complexity | Description                                                   |
|------------------------------------------|------------|---------------------------------------------------------------|
| `Qubit(complex a, complex b)`            | O(1)       | Build a qubit `α|0⟩ + β|1⟩` (auto-normalised).                |
| `void Print() const`                     | O(1)       | Print the state in Dirac notation.                            |
| `double MagnitudeSquareSum() const`      | O(1)       | `|α|² + |β|²`.                                                |
| `double GetProbabilityOf0() const`       | O(1)       | Born-rule `P(0)`.                                             |
| `double GetProbabilityOf1() const`       | O(1)       | Born-rule `P(1)`.                                             |
| `int MeasureWithoutCollapse() const`     | O(1)       | Sample 0 or 1, leaving the state unchanged.                   |
| `bool Collapse()`                        | O(1)       | Destructive measurement; returns the measured bit.            |
| `complex FindInnerProduct(const Qubit&)` | O(1)       | Quantum inner product `⟨ψ|φ⟩`.                                |

Public data members `ampliA`, `ampliB`, `val[2]` mirror α and β for the
single-qubit `QGates` classes.

---

## `include/Register.h` — `class Register`

| Member                                                   | Complexity         | Description                                                                  |
|----------------------------------------------------------|--------------------|------------------------------------------------------------------------------|
| `Register(int n)`                                        | O(2ⁿ)              | n-qubit register in the all-zeros state `|00…0⟩`.                            |
| `Register(int n, const map<string, complex>&)`           | O(2ⁿ + m)          | Register with arbitrary initial amplitudes; auto-normalises.                 |
| `int bits`                                               | —                  | Number of qubits.                                                            |
| `vector<complex> val`                                    | —                  | State vector of length `2^bits`, big-endian-indexed.                         |
| `double MagnitudeSquareSum() const`                      | O(2ⁿ)              | `Σ|αᵢ|²`.                                                                    |
| `double GetProbab(int i) const`                          | O(1)               | Born-rule `P(|i⟩)`.                                                          |
| `complex FindInnerProduct(const Register&) const`        | O(2ⁿ)              | Quantum inner product `⟨this|other⟩`.                                        |
| `void Print() const`                                     | O(2ⁿ)              | Print in Dirac notation.                                                     |
| `string Collapse()`                                      | O(2ⁿ)              | Destructive measurement; returns big-endian outcome bit-string.              |
| `string MeasureWithoutCollapse() const`                  | O(2ⁿ)              | Sampling that leaves the state unchanged.                                    |
| `void WriteMeasurementsCSV(const string&, int) const`    | O(N · 2ⁿ)          | Write N samples to a single-column `outcome` CSV.                            |
| `void ToJson(std::ostream&) const`                       | O(2ⁿ)              | Serialise as JSON (`bits`, `amplitudes`).                                    |

---

## `include/Gates.h` — `class QGates` and derivatives

Abstract base:

```cpp
class QGates {
    virtual Qubit apply(const Qubit&) const = 0;
};
```

Concrete classes (all `O(1)` per `apply`):

| Class      | Matrix                            | Identity                       |
|------------|-----------------------------------|--------------------------------|
| `Identity` | `I`                               | `I² = I`                       |
| `Hadamard` | `H = (1/√2) · [[1, 1], [1, -1]]`  | `H² = I`                       |
| `PauliX`   | `[[0, 1], [1, 0]]`                | `X² = I`                       |
| `PauliY`   | `[[0, -i], [i, 0]]`               | `Y² = I`                       |
| `PauliZ`   | `[[1, 0], [0, -1]]`               | `Z² = I`                       |
| `SGate`    | `[[1, 0], [0, i]]`                | `S² = Z`                       |
| `TGate`    | `[[1, 0], [0, e^(iπ/4)]]`         | `T² = S`, `T⁴ = Z`             |

---

## `include/RegisterGates.h` — `class RGates` and derivatives

Abstract base. Each entry point has a default body that asserts; concrete
gates override only the entry points whose arity they implement.

```cpp
class RGates {
    virtual void Apply(Register&)                                const;
    virtual void ApplyToSingle(Register&, int qubit)             const;
    virtual void ApplyToPair(Register&, int control, int target) const;
    virtual void ApplyToTriple(Register&, int c1, int c2, int target) const;
};
```

### Single-qubit gates (override `Apply` and `ApplyToSingle`)

All have time `O(2ⁿ)`, space `O(1)`.

| Class       | Matrix                                              |
|-------------|-----------------------------------------------------|
| `IdentityR` | `I`                                                 |
| `HadamardR` | Hadamard                                            |
| `XGateR`    | Pauli-X                                             |
| `YGateR`    | Pauli-Y                                             |
| `ZGateR`    | Pauli-Z                                             |
| `SGateR`    | S                                                   |
| `TGateR`    | T                                                   |
| `RxR(θ)`    | `[[cos(θ/2), -i sin(θ/2)], [-i sin(θ/2), cos(θ/2)]]` |
| `RyR(θ)`    | `[[cos(θ/2), -sin(θ/2)], [sin(θ/2), cos(θ/2)]]`     |
| `RzR(θ)`    | `diag(e^(-iθ/2), e^(+iθ/2))`                        |

### Two-qubit gates (override `ApplyToPair`)

Time `O(2ⁿ)`, space `O(1)`.

| Class       | Matrix on basis `{|00⟩,|01⟩,|10⟩,|11⟩}` |
|-------------|------------------------------------------|
| `CNOTGateR` | `diag(I, X)` block on control = 1        |
| `CZGateR`   | `diag(1, 1, 1, -1)`                      |
| `SWAPGateR` | exchanges qubits                         |

### Three-qubit gate (override `ApplyToTriple`)

Time `O(2ⁿ)`, space `O(1)`.

| Class          | Action                                                      |
|----------------|-------------------------------------------------------------|
| `ToffoliGateR` | `|c₁, c₂, t⟩ → |c₁, c₂, t ⊕ (c₁ ∧ c₂)⟩`                     |
