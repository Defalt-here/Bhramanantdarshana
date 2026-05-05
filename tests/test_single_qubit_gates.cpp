/**
 * @file test_single_qubit_gates.cpp
 * @brief Tests for the single-qubit @c QGates classes acting on @c Qubit.
 *
 * Coverage: each of H, X, Y, Z, S, T applied to |0⟩ and |1⟩ (12 tests),
 * plus H² = I and S² = Z identities (2 tests).
 */
#include <cmath>
#include <complex>

#include "Conventions.h"
#include "Gates.h"
#include "Qubits.h"
#include "test_harness.h"

using C = std::complex<double>;
constexpr double kInv2 = 0.7071067811865476;  // 1/√2

TEST(QGates, HadamardOnZeroProducesPlus) {
    Hadamard H;
    Qubit q(1, 0);
    Qubit r = H.apply(q);
    EXPECT_NEAR_COMPLEX(r.val[0], C(kInv2, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(kInv2, 0), 1e-12);
}

TEST(QGates, HadamardOnOneProducesMinus) {
    Hadamard H;
    Qubit q(0, 1);
    Qubit r = H.apply(q);
    EXPECT_NEAR_COMPLEX(r.val[0], C( kInv2, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(-kInv2, 0), 1e-12);
}

TEST(QGates, XOnZeroFlipsToOne) {
    PauliX X;
    Qubit r = X.apply(Qubit(1, 0));
    EXPECT_NEAR_COMPLEX(r.val[0], C(0, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(1, 0), 1e-12);
}

TEST(QGates, XOnOneFlipsToZero) {
    PauliX X;
    Qubit r = X.apply(Qubit(0, 1));
    EXPECT_NEAR_COMPLEX(r.val[0], C(1, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(0, 0), 1e-12);
}

TEST(QGates, YOnZeroProducesIOne) {
    PauliY Y;
    Qubit r = Y.apply(Qubit(1, 0));
    EXPECT_NEAR_COMPLEX(r.val[0], C(0, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(0, 1), 1e-12);
}

TEST(QGates, YOnOneProducesNegIZero) {
    PauliY Y;
    Qubit r = Y.apply(Qubit(0, 1));
    EXPECT_NEAR_COMPLEX(r.val[0], C(0, -1), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(0,  0), 1e-12);
}

TEST(QGates, ZOnZeroIsIdentity) {
    PauliZ Z;
    Qubit r = Z.apply(Qubit(1, 0));
    EXPECT_NEAR_COMPLEX(r.val[0], C(1, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(0, 0), 1e-12);
}

TEST(QGates, ZOnOneFlipsSign) {
    PauliZ Z;
    Qubit r = Z.apply(Qubit(0, 1));
    EXPECT_NEAR_COMPLEX(r.val[0], C( 0, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(-1, 0), 1e-12);
}

TEST(QGates, SOnZeroIsIdentity) {
    SGate S;
    Qubit r = S.apply(Qubit(1, 0));
    EXPECT_NEAR_COMPLEX(r.val[0], C(1, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(0, 0), 1e-12);
}

TEST(QGates, SOnOnePicksUpIPhase) {
    SGate S;
    Qubit r = S.apply(Qubit(0, 1));
    EXPECT_NEAR_COMPLEX(r.val[0], C(0, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(0, 1), 1e-12);
}

TEST(QGates, TOnZeroIsIdentity) {
    TGate T;
    Qubit r = T.apply(Qubit(1, 0));
    EXPECT_NEAR_COMPLEX(r.val[0], C(1, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(0, 0), 1e-12);
}

TEST(QGates, TOnOnePicksUpEighthRotation) {
    TGate T;
    Qubit r = T.apply(Qubit(0, 1));
    // exp(iπ/4) = (1+i)/√2.
    EXPECT_NEAR_COMPLEX(r.val[0], C(0, 0), 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], C(kInv2, kInv2), 1e-12);
}

TEST(QGates, HSquaredIsIdentity) {
    Hadamard H;
    Qubit q(0.6, 0.8);
    Qubit r = H.apply(H.apply(q));
    EXPECT_NEAR_COMPLEX(r.val[0], q.val[0], 1e-12);
    EXPECT_NEAR_COMPLEX(r.val[1], q.val[1], 1e-12);
}

TEST(QGates, SSquaredEqualsZ) {
    SGate S;
    PauliZ Z;
    Qubit q(C(0.6, 0.0), C(0.0, 0.8));
    Qubit ssq = S.apply(S.apply(q));
    Qubit z   = Z.apply(q);
    EXPECT_NEAR_COMPLEX(ssq.val[0], z.val[0], 1e-12);
    EXPECT_NEAR_COMPLEX(ssq.val[1], z.val[1], 1e-12);
}
