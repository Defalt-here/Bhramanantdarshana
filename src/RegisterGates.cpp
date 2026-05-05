/**
 * @file RegisterGates.cpp
 * @brief Implementation of the register-level @c RGates hierarchy.
 *
 * Single-qubit gates are applied via the bit-pairing pattern: scan every
 * state index, locate the unique partner under the target-qubit bitmask,
 * and apply the 2×2 unitary to the amplitude pair. This is the textbook
 * approach (Nielsen & Chuang, §4.3); it touches every amplitude exactly
 * twice and stays within @c O(2^n) time / @c O(1) extra space.
 */

#include "RegisterGates.h"

#include <algorithm>
#include <cmath>
#include <complex>

namespace {
/// 1/√2, used by HadamardR.
const double kInvSqrt2 = 1.0 / std::sqrt(2.0);

/// Bitmask selecting qubit @p qubitIndex in an @p nBits register.
/// Big-endian: qubit 0 → bit (nBits-1), qubit (nBits-1) → bit 0.
inline int qubit_mask(int nBits, int qubitIndex) {
    return 1 << (nBits - 1 - qubitIndex);
}
}  // namespace

// -- HadamardR ---------------------------------------------------------------

void HadamardR::ApplyToSingle(Register& reg, int qubitIndex) const {
    const int size = static_cast<int>(reg.val.size());
    const int mask = qubit_mask(reg.bits, qubitIndex);

    for (int i = 0; i < size; ++i) {
        // Process each (i, j) pair only once: take only the side where the
        // target bit is 0. j is the corresponding state where it is 1.
        if (!(i & mask)) {
            const int j = i | mask;
            const std::complex<double> a = reg.val[i];
            const std::complex<double> b = reg.val[j];
            reg.val[i] = (a + b) * kInvSqrt2;
            reg.val[j] = (a - b) * kInvSqrt2;
        }
    }
}

void HadamardR::Apply(Register& reg) const {
    for (int q = 0; q < reg.bits; ++q) {
        ApplyToSingle(reg, q);
    }
}

// -- XGateR ------------------------------------------------------------------

void XGateR::ApplyToSingle(Register& reg, int qubitIndex) const {
    const int size = static_cast<int>(reg.val.size());
    const int mask = qubit_mask(reg.bits, qubitIndex);

    for (int i = 0; i < size; ++i) {
        const int j = i ^ mask;
        // Swap each pair only once.
        if (i < j) std::swap(reg.val[i], reg.val[j]);
    }
}

void XGateR::Apply(Register& reg) const {
    for (int q = 0; q < reg.bits; ++q) {
        ApplyToSingle(reg, q);
    }
}

// -- IdentityR ---------------------------------------------------------------

void IdentityR::ApplyToSingle(Register& reg, int qubitIndex) const {
    (void)reg;
    (void)qubitIndex;
}

void IdentityR::Apply(Register& reg) const {
    (void)reg;
}

// -- YGateR ------------------------------------------------------------------

void YGateR::ApplyToSingle(Register& reg, int qubitIndex) const {
    const int size = static_cast<int>(reg.val.size());
    const int mask = qubit_mask(reg.bits, qubitIndex);
    const std::complex<double> ic(0.0, 1.0);

    for (int i = 0; i < size; ++i) {
        if (!(i & mask)) {
            const int j = i | mask;
            const std::complex<double> a = reg.val[i];
            const std::complex<double> b = reg.val[j];
            // Y maps (a, b) → (-i·b, i·a).
            reg.val[i] = -ic * b;
            reg.val[j] =  ic * a;
        }
    }
}

void YGateR::Apply(Register& reg) const {
    for (int q = 0; q < reg.bits; ++q) ApplyToSingle(reg, q);
}

// -- ZGateR ------------------------------------------------------------------

void ZGateR::ApplyToSingle(Register& reg, int qubitIndex) const {
    const int size = static_cast<int>(reg.val.size());
    const int mask = qubit_mask(reg.bits, qubitIndex);

    for (int i = 0; i < size; ++i) {
        if (i & mask) reg.val[i] = -reg.val[i];
    }
}

void ZGateR::Apply(Register& reg) const {
    for (int q = 0; q < reg.bits; ++q) ApplyToSingle(reg, q);
}

// -- SGateR ------------------------------------------------------------------

void SGateR::ApplyToSingle(Register& reg, int qubitIndex) const {
    const int size = static_cast<int>(reg.val.size());
    const int mask = qubit_mask(reg.bits, qubitIndex);
    const std::complex<double> ic(0.0, 1.0);

    for (int i = 0; i < size; ++i) {
        if (i & mask) reg.val[i] = ic * reg.val[i];
    }
}

void SGateR::Apply(Register& reg) const {
    for (int q = 0; q < reg.bits; ++q) ApplyToSingle(reg, q);
}

// -- TGateR ------------------------------------------------------------------

void TGateR::ApplyToSingle(Register& reg, int qubitIndex) const {
    const int size = static_cast<int>(reg.val.size());
    const int mask = qubit_mask(reg.bits, qubitIndex);
    // e^(iπ/4) = cos(π/4) + i·sin(π/4) = (1 + i)/√2.
    const double piOver4 = 3.14159265358979323846 / 4.0;
    const std::complex<double> phase(std::cos(piOver4), std::sin(piOver4));

    for (int i = 0; i < size; ++i) {
        if (i & mask) reg.val[i] = phase * reg.val[i];
    }
}

void TGateR::Apply(Register& reg) const {
    for (int q = 0; q < reg.bits; ++q) ApplyToSingle(reg, q);
}

// -- RxR ---------------------------------------------------------------------

RxR::RxR(double theta) : theta_(theta) {}

void RxR::ApplyToSingle(Register& reg, int qubitIndex) const {
    const int size = static_cast<int>(reg.val.size());
    const int mask = qubit_mask(reg.bits, qubitIndex);
    const double c = std::cos(theta_ / 2.0);
    const double s = std::sin(theta_ / 2.0);
    const std::complex<double> ic(0.0, 1.0);

    for (int i = 0; i < size; ++i) {
        if (!(i & mask)) {
            const int j = i | mask;
            const std::complex<double> a = reg.val[i];
            const std::complex<double> b = reg.val[j];
            // | c       -is | |a|
            // | -is      c  | |b|
            reg.val[i] =  c * a + (-ic * s) * b;
            reg.val[j] = (-ic * s) * a +  c * b;
        }
    }
}

void RxR::Apply(Register& reg) const {
    for (int q = 0; q < reg.bits; ++q) ApplyToSingle(reg, q);
}

// -- RyR ---------------------------------------------------------------------

RyR::RyR(double theta) : theta_(theta) {}

void RyR::ApplyToSingle(Register& reg, int qubitIndex) const {
    const int size = static_cast<int>(reg.val.size());
    const int mask = qubit_mask(reg.bits, qubitIndex);
    const double c = std::cos(theta_ / 2.0);
    const double s = std::sin(theta_ / 2.0);

    for (int i = 0; i < size; ++i) {
        if (!(i & mask)) {
            const int j = i | mask;
            const std::complex<double> a = reg.val[i];
            const std::complex<double> b = reg.val[j];
            // | c   -s | |a|
            // | s    c | |b|
            reg.val[i] = c * a - s * b;
            reg.val[j] = s * a + c * b;
        }
    }
}

void RyR::Apply(Register& reg) const {
    for (int q = 0; q < reg.bits; ++q) ApplyToSingle(reg, q);
}

// -- RzR ---------------------------------------------------------------------

RzR::RzR(double theta) : theta_(theta) {}

void RzR::ApplyToSingle(Register& reg, int qubitIndex) const {
    const int size = static_cast<int>(reg.val.size());
    const int mask = qubit_mask(reg.bits, qubitIndex);
    const double half = theta_ / 2.0;
    // Diagonal phases: e^(-iθ/2) on |...0...⟩, e^(+iθ/2) on |...1...⟩.
    const std::complex<double> phase0(std::cos(-half), std::sin(-half));
    const std::complex<double> phase1(std::cos( half), std::sin( half));

    for (int i = 0; i < size; ++i) {
        reg.val[i] *= (i & mask) ? phase1 : phase0;
    }
}

void RzR::Apply(Register& reg) const {
    for (int q = 0; q < reg.bits; ++q) ApplyToSingle(reg, q);
}

// ===========================================================================
//                          TWO-QUBIT GATES
// ===========================================================================

// -- CNOTGateR ---------------------------------------------------------------

void CNOTGateR::ApplyToPair(Register& reg, int control, int target) const {
    assert(control != target && "CNOT requires distinct control and target");
    const int size = static_cast<int>(reg.val.size());
    const int controlMask = qubit_mask(reg.bits, control);
    const int targetMask  = qubit_mask(reg.bits, target);

    // Walk indices where control = 1 and target = 0; swap with the partner
    // where control = 1 and target = 1. Each pair is touched exactly once.
    for (int i = 0; i < size; ++i) {
        if ((i & controlMask) && !(i & targetMask)) {
            std::swap(reg.val[i], reg.val[i ^ targetMask]);
        }
    }
}

// -- CZGateR -----------------------------------------------------------------

void CZGateR::ApplyToPair(Register& reg, int control, int target) const {
    assert(control != target && "CZ requires distinct control and target");
    const int size = static_cast<int>(reg.val.size());
    const int controlMask = qubit_mask(reg.bits, control);
    const int targetMask  = qubit_mask(reg.bits, target);

    // Diagonal: only the |..1..1..⟩ amplitude flips sign.
    for (int i = 0; i < size; ++i) {
        if ((i & controlMask) && (i & targetMask)) {
            reg.val[i] = -reg.val[i];
        }
    }
}

// -- SWAPGateR ---------------------------------------------------------------

void SWAPGateR::ApplyToPair(Register& reg, int control, int target) const {
    assert(control != target && "SWAP requires distinct qubits");
    const int size = static_cast<int>(reg.val.size());
    const int controlMask = qubit_mask(reg.bits, control);
    const int targetMask  = qubit_mask(reg.bits, target);

    // Indices whose control and target bits differ form pairs (i, i^pairMask).
    // Only the |c=1, t=0⟩-side index is processed so each pair swaps once.
    const int pairMask = controlMask | targetMask;
    for (int i = 0; i < size; ++i) {
        if ((i & controlMask) && !(i & targetMask)) {
            std::swap(reg.val[i], reg.val[i ^ pairMask]);
        }
    }
}

// ===========================================================================
//                         THREE-QUBIT GATES
// ===========================================================================

// -- ToffoliGateR ------------------------------------------------------------

void ToffoliGateR::ApplyToTriple(Register& reg, int c1, int c2, int target) const {
    assert(c1 != c2 && c1 != target && c2 != target
           && "Toffoli requires three distinct qubits");
    const int size = static_cast<int>(reg.val.size());
    const int c1Mask     = qubit_mask(reg.bits, c1);
    const int c2Mask     = qubit_mask(reg.bits, c2);
    const int targetMask = qubit_mask(reg.bits, target);

    // For each index where both controls are set and the target bit is 0,
    // swap with its target-flipped partner. Each pair is touched once.
    for (int i = 0; i < size; ++i) {
        if ((i & c1Mask) && (i & c2Mask) && !(i & targetMask)) {
            std::swap(reg.val[i], reg.val[i ^ targetMask]);
        }
    }
}
