/**
 * @file demo_ghz.cpp
 * @brief Three-qubit Greenberger–Horne–Zeilinger (GHZ) state preparation.
 *
 * The 3-qubit GHZ state is the canonical multi-party entanglement
 * resource:
 *
 *   |GHZ⟩ = (|000⟩ + |111⟩)/√2.
 *
 * Two equivalent circuits prepare it from |000⟩, both of which this demo
 * runs and compares:
 *
 *   Plan A (canonical, H + CNOT + CNOT):
 *     H(q0)  ── CNOT(q0,q1) ── CNOT(q1,q2)
 *
 *   Plan B (using the identity CNOT(c,t) = H(t)·CZ(c,t)·H(t)):
 *     H(q0) ── H(q1) ── CZ(q0,q1) ── H(q1) ── H(q2) ── CZ(q1,q2) ── H(q2)
 *
 * Each CNOT is rewritten as a CZ wrapped in two Hadamards on the target
 * qubit. The two circuits should produce the same state vector up to a
 * global phase. The demo computes the inner product between the two
 * resulting states and reports its absolute value: a value of 1 (within
 * tolerance) means the states are identical up to phase.
 *
 * The Plan-A state is then sampled 1000 times and written to
 *   data/ghz_samples.csv
 * (single @c outcome column, big-endian bit-strings). Born probabilities
 * predict ≈ 50 % @c 000 and ≈ 50 % @c 111, with all other outcomes never
 * observed.
 */

#include <cmath>
#include <complex>
#include <iostream>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

namespace {

void buildGHZCanonical(Register& reg) {
    HadamardR H;
    CNOTGateR CNOT;
    H.ApplyToSingle(reg, 0);
    CNOT.ApplyToPair(reg, 0, 1);
    CNOT.ApplyToPair(reg, 1, 2);
}

void buildGHZViaCZ(Register& reg) {
    HadamardR H;
    CZGateR CZ;
    // Rewrite each CNOT as H_t · CZ · H_t. The first H_q1 from rewriting
    // CNOT(q0,q1) cancels with the second H_q1 from rewriting CNOT(q1,q2)
    // is *not* available here since they're separated by CZ(q0,q1); the
    // sequence is therefore the full unrolled form below.
    H.ApplyToSingle(reg, 0);
    H.ApplyToSingle(reg, 1);
    CZ.ApplyToPair(reg, 0, 1);
    H.ApplyToSingle(reg, 1);
    H.ApplyToSingle(reg, 2);
    CZ.ApplyToPair(reg, 1, 2);
    H.ApplyToSingle(reg, 2);
}

}  // namespace

int main() {
    qsim::rng::seed(42);

    std::cout << "GHZ-state demo: build (|000⟩+|111⟩)/√2 two ways and verify\n"
              << "they agree up to a global phase.\n";

    // Plan A — canonical H + CNOT + CNOT.
    Register regA(3);
    std::cout << "\nPlan A: H + CNOT + CNOT\nInitial:\n  ";
    regA.Print();
    buildGHZCanonical(regA);
    std::cout << "Final:\n  ";
    regA.Print();

    // Plan B — H + CZ rewriting.
    Register regB(3);
    std::cout << "\nPlan B: H + CZ rewriting\nInitial:\n  ";
    regB.Print();
    buildGHZViaCZ(regB);
    std::cout << "Final:\n  ";
    regB.Print();

    // Compare states up to a global phase: |⟨A|B⟩| should equal 1.
    auto ip = regA.FindInnerProduct(regB);
    double overlap = std::abs(ip);
    std::cout << "\n|⟨A|B⟩| = " << overlap << "  (expected 1 ± "
              << qsim::kAmplitudeTolerance << ")\n";
    if (std::abs(overlap - 1.0) < 1e-9) {
        std::cout << "Plans A and B agree up to global phase. ✓\n";
    } else {
        std::cout << "WARNING: Plans A and B differ by more than tolerance.\n";
    }

    // Sample 1000 outcomes from the Plan-A GHZ.
    constexpr int kSamples = 1000;
    regA.WriteMeasurementsCSV("data/ghz_samples.csv", kSamples);
    std::cout << "\nWrote " << kSamples
              << " samples of Plan-A GHZ to data/ghz_samples.csv\n";
    return 0;
}
