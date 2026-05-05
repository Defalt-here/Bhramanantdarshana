/**
 * @file test_measurement.cpp
 * @brief Tests for the measurement protocol: collapse, peek, Born-rule
 *        empirical distribution, sticky measurements.
 */
#include <cmath>
#include <complex>
#include <map>
#include <string>
#include <vector>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"
#include "test_harness.h"

using C = std::complex<double>;
using Vec = std::vector<C>;

TEST(Measurement, CollapseOnBasisStateIsDeterministic) {
    qsim::rng::seed(99);
    Register r(2, {{"01", C(1, 0)}});
    auto out = r.Collapse();
    EXPECT_EQ(out, std::string("01"));
    Vec want(4, C(0, 0)); want[1] = C(1, 0);
    EXPECT_STATE_EQ(r, want, qsim::kAmplitudeTolerance);
}

TEST(Measurement, PeekDoesNotMutateState) {
    qsim::rng::seed(99);
    Register r(2);
    HadamardR H;
    H.Apply(r);                 // |++⟩ — uniform amplitude 0.5
    Vec snapshot = r.val;
    for (int i = 0; i < 100; ++i) (void)r.MeasureWithoutCollapse();
    EXPECT_STATE_EQ(r, snapshot, qsim::kAmplitudeTolerance);
}

TEST(Measurement, RepeatedCollapsesAreSticky) {
    qsim::rng::seed(7);
    Register r(3);
    HadamardR H;
    H.Apply(r);
    auto first = r.Collapse();
    for (int i = 0; i < 10; ++i) {
        auto again = r.Collapse();
        EXPECT_EQ(again, first);
    }
}

TEST(Measurement, BornProbabilitiesMatchEmpirical) {
    // Bell-like superposition with α=0.6 on |00⟩ and β=0.8 on |11⟩.
    // Born predicts P(00) = 0.36, P(11) = 0.64, others = 0.
    qsim::rng::seed(2024);
    Register r(2, {{"00", C(0.6, 0)}, {"11", C(0.8, 0)}});
    constexpr int kSamples = 10000;
    std::map<std::string, int> counts;
    for (int i = 0; i < kSamples; ++i) ++counts[r.MeasureWithoutCollapse()];
    EXPECT_EQ(counts["01"], 0);
    EXPECT_EQ(counts["10"], 0);
    const double p00 = counts["00"] / static_cast<double>(kSamples);
    const double p11 = counts["11"] / static_cast<double>(kSamples);
    // 3-σ tolerance for a binomial with n=10000, p≈0.36: σ ≈ 0.0048.
    EXPECT_NEAR(p00, 0.36, 0.02);
    EXPECT_NEAR(p11, 0.64, 0.02);
}

TEST(Measurement, PeekThenCollapseDistributesIdentically) {
    // After many peeks the state is unchanged; the next Collapse should
    // still draw an outcome consistent with the original Born distribution.
    qsim::rng::seed(2025);
    Register r(2, {{"00", C(0.6, 0)}, {"11", C(0.8, 0)}});
    for (int i = 0; i < 50; ++i) (void)r.MeasureWithoutCollapse();
    auto out = r.Collapse();
    EXPECT_TRUE(out == "00" || out == "11");
}
