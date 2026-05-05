/**
 * @file test_two_qubit_gates.cpp
 * @brief Tests for the register-level two-qubit gates: CNOT, CZ, SWAP.
 */
#include <complex>
#include <vector>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "test_harness.h"

using C = std::complex<double>;
using Vec = std::vector<C>;

namespace {
Vec basis(int idx, std::size_t n) { Vec v(n, C(0, 0)); v[idx] = C(1, 0); return v; }
constexpr double kInv2 = 0.7071067811865476;
}

TEST(CNOT, BasisState00) {
    CNOTGateR CNOT; Register r(2, {{"00", C(1, 0)}});
    CNOT.ApplyToPair(r, 0, 1);
    EXPECT_STATE_EQ(r, basis(0, 4), qsim::kAmplitudeTolerance);
}

TEST(CNOT, BasisState01) {
    CNOTGateR CNOT; Register r(2, {{"01", C(1, 0)}});
    CNOT.ApplyToPair(r, 0, 1);
    EXPECT_STATE_EQ(r, basis(1, 4), qsim::kAmplitudeTolerance);
}

TEST(CNOT, BasisState10FlipsTo11) {
    CNOTGateR CNOT; Register r(2, {{"10", C(1, 0)}});
    CNOT.ApplyToPair(r, 0, 1);
    EXPECT_STATE_EQ(r, basis(3, 4), qsim::kAmplitudeTolerance);  // |11⟩
}

TEST(CNOT, BasisState11FlipsTo10) {
    CNOTGateR CNOT; Register r(2, {{"11", C(1, 0)}});
    CNOT.ApplyToPair(r, 0, 1);
    EXPECT_STATE_EQ(r, basis(2, 4), qsim::kAmplitudeTolerance);  // |10⟩
}

TEST(CNOT, SuperpositionControlYieldsBellPhiPlus) {
    HadamardR H; CNOTGateR CNOT;
    Register r(2);
    H.ApplyToSingle(r, 0);
    CNOT.ApplyToPair(r, 0, 1);
    Vec expected(4, C(0, 0));
    expected[0] = C(kInv2, 0);
    expected[3] = C(kInv2, 0);
    EXPECT_STATE_EQ(r, expected, qsim::kAmplitudeTolerance);
}

TEST(SWAP, Swaps01And10) {
    SWAPGateR SW; Register r(2, {{"01", C(1, 0)}});
    SW.ApplyToPair(r, 0, 1);
    EXPECT_STATE_EQ(r, basis(2, 4), qsim::kAmplitudeTolerance);  // → |10⟩
}

TEST(SWAP, Swaps10And01) {
    SWAPGateR SW; Register r(2, {{"10", C(1, 0)}});
    SW.ApplyToPair(r, 0, 1);
    EXPECT_STATE_EQ(r, basis(1, 4), qsim::kAmplitudeTolerance);  // → |01⟩
}

TEST(CZ, FlipsOnly11Sign) {
    CZGateR CZ;
    {
        Register r(2, {{"00", C(1, 0)}}); CZ.ApplyToPair(r, 0, 1);
        EXPECT_STATE_EQ(r, basis(0, 4), qsim::kAmplitudeTolerance);
    }
    {
        Register r(2, {{"01", C(1, 0)}}); CZ.ApplyToPair(r, 0, 1);
        EXPECT_STATE_EQ(r, basis(1, 4), qsim::kAmplitudeTolerance);
    }
    {
        Register r(2, {{"10", C(1, 0)}}); CZ.ApplyToPair(r, 0, 1);
        EXPECT_STATE_EQ(r, basis(2, 4), qsim::kAmplitudeTolerance);
    }
    {
        Register r(2, {{"11", C(1, 0)}}); CZ.ApplyToPair(r, 0, 1);
        Vec want(4, C(0, 0)); want[3] = C(-1, 0);
        EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
    }
}

TEST(Toffoli, AllControlsClearLeavesStateAlone) {
    ToffoliGateR T;
    Register r(3, {{"010", C(1, 0)}});
    T.ApplyToTriple(r, 0, 1, 2);
    EXPECT_STATE_EQ(r, basis(2, 8), qsim::kAmplitudeTolerance);  // |010⟩ = idx 2
}

TEST(Toffoli, BothControlsSetFlipsTarget) {
    ToffoliGateR T;
    Register r(3, {{"110", C(1, 0)}});                    // idx 6
    T.ApplyToTriple(r, 0, 1, 2);
    EXPECT_STATE_EQ(r, basis(7, 8), qsim::kAmplitudeTolerance);  // → |111⟩ idx 7
}
