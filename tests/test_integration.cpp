/**
 * @file test_integration.cpp
 * @brief End-to-end checks that exercise the simulator on textbook circuits.
 *
 * Each test builds a known-target state (Bell, GHZ, post-Deutsch register)
 * by composing the published gate sequence and verifies the final state
 * vector matches the analytic answer within @c qsim::kAmplitudeTolerance.
 */
#include <cmath>
#include <complex>
#include <vector>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "test_harness.h"

using C = std::complex<double>;
using Vec = std::vector<C>;
constexpr double kInv2 = 0.7071067811865476;

TEST(Integration, SingleHadamardOnOneQubit) {
    Register r(1); HadamardR H;
    H.Apply(r);
    EXPECT_STATE_EQ(r, (Vec{C(kInv2, 0), C(kInv2, 0)}), qsim::kAmplitudeTolerance);
}

TEST(Integration, BellPhiPlusPreparation) {
    Register r(2); HadamardR H; CNOTGateR CNOT;
    H.ApplyToSingle(r, 0);
    CNOT.ApplyToPair(r, 0, 1);
    Vec want(4, C(0, 0)); want[0] = C(kInv2, 0); want[3] = C(kInv2, 0);
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}

TEST(Integration, GHZPreparation) {
    Register r(3); HadamardR H; CNOTGateR CNOT;
    H.ApplyToSingle(r, 0);
    CNOT.ApplyToPair(r, 0, 1);
    CNOT.ApplyToPair(r, 1, 2);
    Vec want(8, C(0, 0)); want[0] = C(kInv2, 0); want[7] = C(kInv2, 0);
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}

namespace {
// Apply Deutsch oracle for f given as truth table {f(0), f(1)}.
void deutschOracle(Register& reg, int f0, int f1) {
    const int size = static_cast<int>(reg.val.size());
    for (int i = 0; i < size; ++i) {
        const int x = (i >> 1) & 1;
        const int y =  i & 1;
        const int fx = x ? f1 : f0;
        const int j = (x << 1) | (y ^ fx);
        if (i < j) std::swap(reg.val[i], reg.val[j]);
    }
}

// Run a Deutsch oracle and return the post-circuit state vector.
Register runDeutsch(int f0, int f1) {
    Register reg(2, {{"01", C(1, 0)}});
    HadamardR H;
    H.ApplyToSingle(reg, 0);
    H.ApplyToSingle(reg, 1);
    deutschOracle(reg, f0, f1);
    H.ApplyToSingle(reg, 0);
    return reg;
}

// Probability that qubit 0 (MSB) measures 1.
double probInputBitOne(const Register& reg) {
    double p = 0.0;
    // qubit 0 = MSB → bits 2 and 3 of a 2-qubit register are the |1·⟩ branch.
    p += std::norm(reg.val[2]);
    p += std::norm(reg.val[3]);
    return p;
}
}  // namespace

TEST(Integration, DeutschConstant0YieldsInputZero) {
    Register r = runDeutsch(0, 0);
    EXPECT_NEAR(probInputBitOne(r), 0.0, 1e-12);
}

TEST(Integration, DeutschConstant1YieldsInputZero) {
    Register r = runDeutsch(1, 1);
    EXPECT_NEAR(probInputBitOne(r), 0.0, 1e-12);
}

TEST(Integration, DeutschIdentityYieldsInputOne) {
    Register r = runDeutsch(0, 1);
    EXPECT_NEAR(probInputBitOne(r), 1.0, 1e-12);
}

TEST(Integration, DeutschNotYieldsInputOne) {
    Register r = runDeutsch(1, 0);
    EXPECT_NEAR(probInputBitOne(r), 1.0, 1e-12);
}

TEST(Integration, AllPauliXYZComposition) {
    // X · Y · Z applied twice (i.e. X-Y-Z-X-Y-Z) on a qubit equals
    // -I (pure global phase): each Pauli squares to I, but XY = iZ etc.
    // induce a global -1. We pick a register whose initial state has a
    // non-trivial first amplitude so the global phase is observable.
    Register r(1, {{"0", C(0.6, 0)}, {"1", C(0.0, 0.8)}});
    XGateR X; YGateR Y; ZGateR Z;
    Register snap = r;
    for (int round = 0; round < 2; ++round) {
        X.Apply(r); Y.Apply(r); Z.Apply(r);
    }
    // (XYZ)² applied to |ψ⟩ → -|ψ⟩ (pure global phase, invisible to Born),
    // but the state vector does pick up the sign.
    EXPECT_NEAR_COMPLEX(r.val[0], -snap.val[0], 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], -snap.val[1], 1e-12);
}
