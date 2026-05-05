/**
 * @file Gates.cpp
 * @brief Implementation of the single-qubit @c QGates hierarchy.
 *
 * Each gate is a direct transcription of its 2×2 matrix into a constructor
 * call. The new @c Qubit is auto-normalised by the @c Qubit constructor —
 * unitarity guarantees the result was already unit-norm, so this just
 * shaves off any floating-point drift.
 */

#include "Gates.h"

#include <cmath>
#include <complex>

namespace {
/// Cached value of 1/√2, used by the Hadamard gate.
const double kInvSqrt2 = 1.0 / std::sqrt(2.0);

/// Cached value of π, used to build T-gate phase factors.
constexpr double kPi = 3.14159265358979323846;
}  // namespace

// -- Identity ----------------------------------------------------------------

Qubit Identity::apply(const Qubit& q) const {
    return Qubit(q.val[0], q.val[1]);
}

// -- Hadamard ----------------------------------------------------------------

Qubit Hadamard::apply(const Qubit& q) const {
    std::complex<double> a = q.ampliA;
    std::complex<double> b = q.ampliB;
    return Qubit(kInvSqrt2 * (a + b), kInvSqrt2 * (a - b));
}

// -- Pauli-X -----------------------------------------------------------------

Qubit PauliX::apply(const Qubit& q) const {
    return Qubit(q.ampliB, q.ampliA);
}

// -- Pauli-Y -----------------------------------------------------------------

Qubit PauliY::apply(const Qubit& q) const {
    const std::complex<double> i(0.0, 1.0);
    return Qubit(-i * q.ampliB, i * q.ampliA);
}

// -- Pauli-Z -----------------------------------------------------------------

Qubit PauliZ::apply(const Qubit& q) const {
    return Qubit(q.ampliA, -q.ampliB);
}

// -- S -----------------------------------------------------------------------

Qubit SGate::apply(const Qubit& q) const {
    const std::complex<double> i(0.0, 1.0);
    return Qubit(q.ampliA, i * q.ampliB);
}

// -- T -----------------------------------------------------------------------

Qubit TGate::apply(const Qubit& q) const {
    std::complex<double> phase(std::cos(kPi / 4.0), std::sin(kPi / 4.0));
    return Qubit(q.ampliA, phase * q.ampliB);
}
