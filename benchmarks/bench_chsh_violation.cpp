/**
 * @file bench_chsh_violation.cpp
 * @brief Empirically measure the CHSH inequality violation on a Bell state.
 *
 * The CHSH quantity is
 * @code
 *   S = E(a, b) - E(a, b') + E(a', b) + E(a', b'),
 * @endcode
 * where Alice picks one of two measurement axes {a, a'}, Bob picks one of
 * {b, b'}, and @c E(·,·) is the empirical correlation of the ±1 outcomes.
 * Local hidden-variable theories are bounded by @c |S| ≤ 2; quantum
 * mechanics on the Bell state @c |Φ⁺⟩ saturates @c |S| = 2√2 ≈ 2.828
 * for the optimal angle choice
 *
 *   Alice: a = 0 (Z basis),    a' = π/2 (X basis);
 *   Bob:   b = π/4,            b' = 3π/4.
 *
 * (Choosing Alice and Bob both from {Z, X} alone gives S = 2 exactly — the
 * classical bound is not exceeded. This benchmark therefore uses the
 * rotated Bob axes that demonstrate genuine non-locality. See
 * @c BACKEND_DEVIATIONS.md for the rationale.)
 *
 * Output: benchmarks/results/bench_chsh_violation.csv
 *   columns: basis_combo, n_samples, correlation, S_value
 *
 * The first four rows hold the four (a/a', b/b') correlations; the fifth
 * row reports the assembled S value.
 */
#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>
#include <string>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

namespace {

constexpr int kSamples = 10000;

/// Build |Φ⁺⟩ = (|00⟩ + |11⟩)/√2 in a fresh 2-qubit register.
Register makeBell() {
    Register r(2);
    HadamardR H;
    CNOTGateR CNOT;
    H.ApplyToSingle(r, 0);
    CNOT.ApplyToPair(r, 0, 1);
    return r;
}

/// Empirical correlation of (±1)·(±1) outcomes for the CHSH test, with
/// Alice measuring at angle @p thetaA and Bob at angle @p thetaB
/// (angles measured around the Y-axis of the Bloch sphere away from Z).
double correlation(double thetaA, double thetaB) {
    int sum = 0;
    for (int i = 0; i < kSamples; ++i) {
        Register r = makeBell();
        // Rotate so the desired measurement axis aligns with Z, then measure.
        if (thetaA != 0.0) RyR(-thetaA).ApplyToSingle(r, 0);
        if (thetaB != 0.0) RyR(-thetaB).ApplyToSingle(r, 1);
        const std::string out = r.Collapse();
        const int aBit = out[0] - '0';
        const int bBit = out[1] - '0';
        const int aSign = aBit ? -1 : 1;
        const int bSign = bBit ? -1 : 1;
        sum += aSign * bSign;
    }
    return static_cast<double>(sum) / kSamples;
}

}  // namespace

int main() {
    qsim::rng::seed(42);

    constexpr double kPi = 3.14159265358979323846;
    const double a   = 0.0;             // Z
    const double aP  = kPi / 2.0;       // X
    const double b   = kPi / 4.0;
    const double bP  = 3.0 * kPi / 4.0;

    const double Eab   = correlation(a,  b);
    const double EabP  = correlation(a,  bP);
    const double EaPb  = correlation(aP, b);
    const double EaPbP = correlation(aP, bP);
    const double S     = Eab - EabP + EaPb + EaPbP;

    std::ofstream csv("benchmarks/results/bench_chsh_violation.csv");
    csv << "basis_combo,n_samples,correlation,S_value\n";
    csv << "a=Z   b=pi/4,"  << kSamples << "," << Eab   << ",\n";
    csv << "a=Z   b=3pi/4," << kSamples << "," << EabP  << ",\n";
    csv << "a=X   b=pi/4,"  << kSamples << "," << EaPb  << ",\n";
    csv << "a=X   b=3pi/4," << kSamples << "," << EaPbP << ",\n";
    csv << "S,"             << (4 * kSamples) << ",,"   << S << "\n";

    std::cout << "CHSH on |Φ⁺⟩ (n=" << kSamples << " per correlation)\n"
              << "  E(Z,   π/4)   = " << Eab   << "  (theory: " << std::cos(a   - b)  << ")\n"
              << "  E(Z,   3π/4)  = " << EabP  << "  (theory: " << std::cos(a   - bP) << ")\n"
              << "  E(X,   π/4)   = " << EaPb  << "  (theory: " << std::cos(aP  - b)  << ")\n"
              << "  E(X,   3π/4)  = " << EaPbP << "  (theory: " << std::cos(aP  - bP) << ")\n"
              << "  S             = " << S     << "  (theory: " << 2 * std::sqrt(2.0)
              << ", classical bound 2)\n";
    std::cout << "Wrote benchmarks/results/bench_chsh_violation.csv\n";
    return 0;
}
