/**
 * @file test_register_gates.cpp
 * @brief Tests for register-level single-qubit gates (HadamardR, XGateR,
 *        YGateR, ZGateR, SGateR, TGateR) acting on multi-qubit registers.
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

TEST(RegisterGates, HadamardOn1QubitProducesPlus) {
    HadamardR H; Register r(1);
    H.Apply(r);
    Vec want = {C(kInv2, 0), C(kInv2, 0)};
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}

TEST(RegisterGates, HadamardOn2QubitsProducesUniform) {
    HadamardR H; Register r(2);
    H.Apply(r);
    Vec want(4, C(0.5, 0));
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}

TEST(RegisterGates, HadamardOn3QubitsProducesUniform) {
    HadamardR H; Register r(3);
    H.Apply(r);
    Vec want(8, C(1.0 / std::sqrt(8.0), 0));
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}

TEST(RegisterGates, XAppliedToAllFlipsAllBits) {
    XGateR X; Register r(3);
    X.Apply(r);
    Vec want(8, C(0, 0)); want[7] = C(1, 0);   // |000⟩ → |111⟩
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}

TEST(RegisterGates, YOnSingleQubitOf2QubitRegister) {
    // Y on qubit 0 of |00⟩ → i|10⟩ (index 2).
    YGateR Y; Register r(2);
    Y.ApplyToSingle(r, 0);
    Vec want(4, C(0, 0)); want[2] = C(0, 1);
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}

TEST(RegisterGates, ZOnSingleQubitNegatesAmplitudesWhereSet) {
    ZGateR Z; Register r(2, {{"00", C(1, 0)}, {"01", C(1, 0)}});  // pre-norm
    Z.ApplyToSingle(r, 1);
    // After Z on qubit 1: |00⟩ unchanged, |01⟩ → -|01⟩.
    Vec want(4, C(0, 0));
    want[0] = C( kInv2, 0);
    want[1] = C(-kInv2, 0);
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}

TEST(RegisterGates, SAndTOnSingleQubitMatchPhases) {
    SGateR S; TGateR T;
    Register a(1, {{"1", C(1, 0)}});
    S.ApplyToSingle(a, 0);
    EXPECT_NEAR_COMPLEX(a.val[1], C(0, 1), 1e-12);

    Register b(1, {{"1", C(1, 0)}});
    T.ApplyToSingle(b, 0);
    EXPECT_NEAR_COMPLEX(b.val[1], C(kInv2, kInv2), 1e-12);
}

TEST(RegisterGates, HOnQubit0Of3QubitRegister) {
    // H on qubit 0 of |000⟩ → (|000⟩ + |100⟩)/√2.
    // Big-endian: qubit 0 is MSB → mask = 1 << 2 = 4.
    HadamardR H; Register r(3);
    H.ApplyToSingle(r, 0);
    Vec want(8, C(0, 0));
    want[0] = C(kInv2, 0);
    want[4] = C(kInv2, 0);
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}
