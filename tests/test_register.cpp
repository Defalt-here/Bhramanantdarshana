/**
 * @file test_register.cpp
 * @brief Tests for the @c Register class: construction, normalisation,
 *        probabilities, inner product, print, and measurement.
 */
#include <complex>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Conventions.h"
#include "Register.h"
#include "RNG.h"
#include "test_harness.h"

using C = std::complex<double>;
using Vec = std::vector<C>;

TEST(Register, DefaultConstructorMakesAllZerosState) {
    Register r(3);
    EXPECT_EQ(static_cast<std::size_t>(r.val.size()), static_cast<std::size_t>(8));
    EXPECT_NEAR_COMPLEX(r.val[0], C(1, 0), qsim::kAmplitudeTolerance);
    for (std::size_t i = 1; i < r.val.size(); ++i)
        EXPECT_NEAR_COMPLEX(r.val[i], C(0, 0), qsim::kAmplitudeTolerance);
}

TEST(Register, BitstringConstructorPlacesAmplitudesBigEndian) {
    // qubit 0 = MSB → "10" maps to index 4*1 + 2*0 = 2 in a 2-qubit register.
    Register r(2, {{"10", C(1, 0)}});
    EXPECT_NEAR_COMPLEX(r.val[2], C(1, 0), qsim::kAmplitudeTolerance);
    EXPECT_NEAR_COMPLEX(r.val[0], C(0, 0), qsim::kAmplitudeTolerance);
    EXPECT_NEAR_COMPLEX(r.val[1], C(0, 0), qsim::kAmplitudeTolerance);
    EXPECT_NEAR_COMPLEX(r.val[3], C(0, 0), qsim::kAmplitudeTolerance);
}

TEST(Register, ConstructorAutoNormalises) {
    Register r(2, {{"00", C(3, 0)}, {"11", C(4, 0)}});
    // Pre-normalisation magnitudes were 9 + 16 = 25.
    EXPECT_NEAR(r.MagnitudeSquareSum(), 1.0, qsim::kNormTolerance);
    EXPECT_NEAR_COMPLEX(r.val[0], C(0.6, 0), qsim::kAmplitudeTolerance);
    EXPECT_NEAR_COMPLEX(r.val[3], C(0.8, 0), qsim::kAmplitudeTolerance);
}

TEST(Register, GetProbabReturnsBornRule) {
    Register r(1, {{"0", C(0.6, 0)}, {"1", C(0.8, 0)}});
    EXPECT_NEAR(r.GetProbab(0), 0.36, 1e-12);
    EXPECT_NEAR(r.GetProbab(1), 0.64, 1e-12);
}

TEST(Register, FindInnerProductOnIdenticalStates) {
    Register a(3), b(3);
    auto ip = a.FindInnerProduct(b);
    EXPECT_NEAR_COMPLEX(ip, C(1, 0), qsim::kAmplitudeTolerance);
}

TEST(Register, FindInnerProductOnOrthogonalStates) {
    Register a(2, {{"00", C(1, 0)}});
    Register b(2, {{"11", C(1, 0)}});
    auto ip = a.FindInnerProduct(b);
    EXPECT_NEAR_COMPLEX(ip, C(0, 0), qsim::kAmplitudeTolerance);
}

TEST(Register, PrintProducesNonEmptyDiracString) {
    Register r(2, {{"01", C(0.6, 0)}, {"10", C(0.8, 0)}});
    std::ostringstream os;
    auto* old = std::cout.rdbuf(os.rdbuf());
    r.Print();
    std::cout.rdbuf(old);
    const std::string s = os.str();
    EXPECT_TRUE(s.find("|01⟩") != std::string::npos);
    EXPECT_TRUE(s.find("|10⟩") != std::string::npos);
}

TEST(Register, CollapseOnBasisStateIsDeterministic) {
    qsim::rng::seed(2026);
    // Already a single basis state |11⟩ — Collapse must return "11".
    Register r(2, {{"11", C(1, 0)}});
    std::string out = r.Collapse();
    EXPECT_EQ(out, std::string("11"));
    // After collapse, val[3] = 1, others 0.
    Vec expected(4, C(0, 0));
    expected[3] = C(1, 0);
    EXPECT_STATE_EQ(r, expected, qsim::kAmplitudeTolerance);
}

TEST(Register, MeasureWithoutCollapseDoesNotMutateState) {
    qsim::rng::seed(2026);
    Register r(3, {{"010", C(0.6, 0)}, {"110", C(0.8, 0)}});
    Vec snapshot = r.val;
    for (int i = 0; i < 100; ++i) (void)r.MeasureWithoutCollapse();
    EXPECT_STATE_EQ(r, snapshot, qsim::kAmplitudeTolerance);
}
