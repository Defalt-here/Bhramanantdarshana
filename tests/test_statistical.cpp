/**
 * @file test_statistical.cpp
 * @brief χ² goodness-of-fit checks at the 5 % significance level.
 *
 * For four canonical circuits we draw @c kSamples = 10 000 measurement
 * outcomes and compare the empirical histogram to the analytic Born-rule
 * distribution via a chi-squared statistic
 * @code
 *   χ² = Σᵢ (Oᵢ - Eᵢ)² / Eᵢ
 * @endcode
 * where the sum is over outcomes with non-trivial expected probability.
 * The test passes if χ² is below the critical value at α = 0.05 for the
 * appropriate degrees of freedom (df = number of outcomes − 1).
 *
 * Critical values used (α = 0.05):
 *   df = 1 : χ²_crit ≈ 3.841
 *   df = 3 : χ²_crit ≈ 7.815
 *   df = 7 : χ²_crit ≈ 14.067
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

namespace {

constexpr int kSamples = 10000;

// Empirical histogram of `kSamples` measurements.
std::map<std::string, int> sample(const Register& reg, int n) {
    std::map<std::string, int> counts;
    for (int i = 0; i < n; ++i) ++counts[reg.MeasureWithoutCollapse()];
    return counts;
}

double chi2(const std::map<std::string, int>& observed,
            const std::map<std::string, double>& expectedProb,
            int n) {
    double x2 = 0.0;
    for (const auto& [outcome, p] : expectedProb) {
        const double E = p * n;
        const double O = observed.count(outcome) ? observed.at(outcome) : 0;
        if (E > 0.0) x2 += (O - E) * (O - E) / E;
    }
    return x2;
}

}  // namespace

TEST(Statistical, SingleHadamardChi2) {
    qsim::rng::seed(20260505);
    Register r(1); HadamardR H;
    H.Apply(r);                        // 50/50 between |0⟩ and |1⟩
    auto obs = sample(r, kSamples);
    std::map<std::string, double> exp = {{"0", 0.5}, {"1", 0.5}};
    const double x2 = chi2(obs, exp, kSamples);
    EXPECT_TRUE(x2 < 3.841);           // df = 1, α = 0.05
}

TEST(Statistical, BellPhiPlusChi2) {
    qsim::rng::seed(20260505);
    Register r(2); HadamardR H; CNOTGateR CNOT;
    H.ApplyToSingle(r, 0);
    CNOT.ApplyToPair(r, 0, 1);         // (|00⟩+|11⟩)/√2
    auto obs = sample(r, kSamples);
    std::map<std::string, double> exp = {{"00", 0.5}, {"11", 0.5}};
    const double x2 = chi2(obs, exp, kSamples);
    EXPECT_TRUE(x2 < 3.841);           // only 2 non-zero outcomes → df = 1
}

TEST(Statistical, GHZChi2) {
    qsim::rng::seed(20260505);
    Register r(3); HadamardR H; CNOTGateR CNOT;
    H.ApplyToSingle(r, 0);
    CNOT.ApplyToPair(r, 0, 1);
    CNOT.ApplyToPair(r, 1, 2);         // (|000⟩+|111⟩)/√2
    auto obs = sample(r, kSamples);
    std::map<std::string, double> exp = {{"000", 0.5}, {"111", 0.5}};
    const double x2 = chi2(obs, exp, kSamples);
    EXPECT_TRUE(x2 < 3.841);           // df = 1
}

TEST(Statistical, DeutschConstantHasDeterministicInputBitChi2) {
    qsim::rng::seed(20260505);
    // Build the post-Deutsch register for f = constant-0; the input qubit
    // should be deterministically 0. Empirically we still draw 10 000
    // samples to confirm it.
    Register reg(2, {{"01", C(1, 0)}});
    HadamardR H;
    H.ApplyToSingle(reg, 0);
    H.ApplyToSingle(reg, 1);
    // constant-0 oracle is identity — skip.
    H.ApplyToSingle(reg, 0);

    int firstBitIsOne = 0;
    for (int i = 0; i < kSamples; ++i) {
        if (reg.MeasureWithoutCollapse()[0] == '1') ++firstBitIsOne;
    }
    // Expected count is 0; observed should also be exactly 0 → x² is
    // undefined (E=0), so we just assert the empirical count.
    EXPECT_EQ(firstBitIsOne, 0);
}
