/**
 * @file test_properties.cpp
 * @brief Property-based identity checks across many random states.
 *
 * For each unitary identity (e.g. H² = I), draw 100 pseudo-random unit
 * states from a fixed seed, apply the LHS sequence, and assert the result
 * matches the RHS within @c qsim::kAmplitudeTolerance.
 *
 * Norm preservation under arbitrary gate sequences is checked separately
 * with 1000 random gate applications on a 4-qubit register.
 */
#include <cmath>
#include <complex>
#include <random>
#include <vector>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"
#include "test_harness.h"

using C = std::complex<double>;
using Vec = std::vector<C>;

namespace {

constexpr int kSamples = 100;

// Draw a random pure 1-qubit state under a fixed PRNG.
Register randomQubitState(std::mt19937_64& gen) {
    std::uniform_real_distribution<double> u(-1.0, 1.0);
    Register r(1, {{"0", C(u(gen), u(gen))}, {"1", C(u(gen), u(gen))}});
    return r;
}

bool stateMatches(const Register& a, const Register& b) {
    if (a.val.size() != b.val.size()) return false;
    for (std::size_t i = 0; i < a.val.size(); ++i)
        if (std::abs(a.val[i] - b.val[i]) > 1e-10) return false;
    return true;
}

}  // namespace

TEST(Property, HSquaredIsIdentity) {
    std::mt19937_64 gen(12345);
    HadamardR H;
    int matched = 0;
    for (int t = 0; t < kSamples; ++t) {
        Register r = randomQubitState(gen);
        Register snap = r;
        H.Apply(r); H.Apply(r);
        if (stateMatches(r, snap)) ++matched;
    }
    EXPECT_EQ(matched, kSamples);
}

TEST(Property, XSquaredIsIdentity) {
    std::mt19937_64 gen(12345);
    XGateR X;
    int matched = 0;
    for (int t = 0; t < kSamples; ++t) {
        Register r = randomQubitState(gen);
        Register snap = r;
        X.Apply(r); X.Apply(r);
        if (stateMatches(r, snap)) ++matched;
    }
    EXPECT_EQ(matched, kSamples);
}

TEST(Property, YSquaredIsIdentity) {
    std::mt19937_64 gen(12345);
    YGateR Y;
    int matched = 0;
    for (int t = 0; t < kSamples; ++t) {
        Register r = randomQubitState(gen);
        Register snap = r;
        Y.Apply(r); Y.Apply(r);
        if (stateMatches(r, snap)) ++matched;
    }
    EXPECT_EQ(matched, kSamples);
}

TEST(Property, ZSquaredIsIdentity) {
    std::mt19937_64 gen(12345);
    ZGateR Z;
    int matched = 0;
    for (int t = 0; t < kSamples; ++t) {
        Register r = randomQubitState(gen);
        Register snap = r;
        Z.Apply(r); Z.Apply(r);
        if (stateMatches(r, snap)) ++matched;
    }
    EXPECT_EQ(matched, kSamples);
}

TEST(Property, SSquaredEqualsZ) {
    std::mt19937_64 gen(12345);
    SGateR S; ZGateR Z;
    int matched = 0;
    for (int t = 0; t < kSamples; ++t) {
        Register a = randomQubitState(gen);
        Register b = a;                         // independent copy
        S.Apply(a); S.Apply(a);                 // S²
        Z.Apply(b);                             // Z
        if (stateMatches(a, b)) ++matched;
    }
    EXPECT_EQ(matched, kSamples);
}

TEST(Property, NormPreservedBy1000RandomGateApplications) {
    qsim::rng::seed(424242);
    std::mt19937_64 gen(7654321);
    std::uniform_int_distribution<int> gateChoice(0, 6);

    Register r(4);
    HadamardR H; XGateR X; YGateR Y; ZGateR Z;
    SGateR S; TGateR T;
    CNOTGateR CNOT;
    std::uniform_int_distribution<int> qubit(0, 3);

    for (int step = 0; step < 1000; ++step) {
        const int g = gateChoice(gen);
        const int q = qubit(gen);
        switch (g) {
            case 0: H.ApplyToSingle(r, q); break;
            case 1: X.ApplyToSingle(r, q); break;
            case 2: Y.ApplyToSingle(r, q); break;
            case 3: Z.ApplyToSingle(r, q); break;
            case 4: S.ApplyToSingle(r, q); break;
            case 5: T.ApplyToSingle(r, q); break;
            case 6: {
                int q2 = qubit(gen);
                if (q2 == q) q2 = (q + 1) % 4;
                CNOT.ApplyToPair(r, q, q2);
                break;
            }
        }
    }

    EXPECT_NEAR(r.MagnitudeSquareSum(), 1.0, qsim::kNormTolerance);
}
