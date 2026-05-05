/**
 * @file demo_grover.cpp
 * @brief Three-qubit Grover search.
 *
 * Grover's algorithm finds a marked element of an unsorted database of N
 * items in O(√N) oracle queries — a quadratic speedup over the classical
 * O(N) bound. For N = 8 (3 qubits), the optimal number of Grover iterations
 * is
 * @code
 *   k* = floor(π/4 · √N) = floor(π/4 · √8) = 2.
 * @endcode
 *
 * Each Grover iteration consists of two reflections:
 *
 *   1. Oracle reflection — flip the sign of the marked basis state's
 *      amplitude. For a marked computational state @c m, this is just
 *      @c val[m] *= -1.
 *
 *   2. Diffusion (inversion about the average) — implemented as
 *      @c H^⊗n · (2|0⟩⟨0| − I) · H^⊗n, i.e. apply H on every qubit, flip
 *      the sign of every @c val[i] except @c val[0], then apply H on every
 *      qubit again.
 *
 * The demo runs k = 0, 1, …, 6 iterations of Grover with the marked state
 * (default @c |101⟩, configurable as a 3-bit binary command-line argument)
 * and, for each k, samples 1000 measurement outcomes and reports the
 * empirical success rate. Results go to @c data/grover_iterations.csv.
 *
 * Theoretical success probability after k iterations is
 *   P(k) = sin²((2k + 1) · θ),  where sin(θ) = √(1/8).
 */

#include <cmath>
#include <complex>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

namespace {

constexpr int kQubits = 3;
constexpr int kN      = 1 << kQubits;
constexpr int kSamplesPerK = 1000;
constexpr int kMaxIter = 6;

bool validBinary3(const std::string& s) {
    if (s.size() != static_cast<std::size_t>(kQubits)) return false;
    for (char c : s) if (c != '0' && c != '1') return false;
    return true;
}

int indexFromBitString(const std::string& s) {
    int v = 0;
    for (char c : s) v = (v << 1) | (c == '1' ? 1 : 0);
    return v;
}

/// Mark-state phase oracle: flip the amplitude of |m⟩.
void oracleFlip(Register& reg, int markedIndex) {
    reg.val[markedIndex] = -reg.val[markedIndex];
}

/// Diffusion reflection 2|s⟩⟨s| − I where |s⟩ is the equal superposition.
/// Implemented via H · (2|0⟩⟨0| − I) · H.
void diffuse(Register& reg) {
    HadamardR H;
    H.Apply(reg);
    // 2|0⟩⟨0| − I: keep val[0], negate all other amplitudes.
    for (std::size_t i = 1; i < reg.val.size(); ++i) reg.val[i] = -reg.val[i];
    H.Apply(reg);
}

/// Build a 3-qubit equal superposition and run @p k Grover iterations
/// against the marked state @p m.
Register buildAndRun(int markedIndex, int k) {
    Register reg(kQubits);
    HadamardR H;
    H.Apply(reg);                            // |s⟩ = uniform superposition
    for (int it = 0; it < k; ++it) {
        oracleFlip(reg, markedIndex);
        diffuse(reg);
    }
    return reg;
}

double theoreticalP(int k) {
    const double sinTheta = std::sqrt(1.0 / static_cast<double>(kN));
    const double theta    = std::asin(sinTheta);
    const double s        = std::sin((2 * k + 1) * theta);
    return s * s;
}

}  // namespace

int main(int argc, char** argv) {
    qsim::rng::seed(42);

    std::string mark = (argc >= 2) ? argv[1] : "101";
    if (!validBinary3(mark)) {
        std::cerr << "marked state must be a 3-bit binary string, e.g. 101\n";
        return 1;
    }
    const int markedIndex = indexFromBitString(mark);
    const int kOptimal    = static_cast<int>(std::floor(M_PI / 4.0 * std::sqrt(kN)));

    std::cout << "Grover's algorithm: marked state |" << mark
              << "⟩ (index " << markedIndex << ")\n"
              << "N = " << kN << ",  optimal iterations = " << kOptimal << "\n\n";

    // Run with the optimal number of iterations and print the resulting state.
    Register optimal = buildAndRun(markedIndex, kOptimal);
    std::cout << "State after " << kOptimal << " Grover iterations:\n  ";
    optimal.Print();
    const double pOptimal = std::norm(optimal.val[markedIndex]);
    std::cout << "P(measure marked) = " << pOptimal << "\n";

    // Sweep iteration count, sampling kSamplesPerK times each. This is the
    // textbook plot showing P(k) following sin²((2k+1)θ) until k* ≈ 2.
    std::ofstream csv("data/grover_iterations.csv");
    csv << "iterations,success_count,success_probability,theoretical_probability\n";
    std::cout << "\nIter | success / " << kSamplesPerK
              << " | empirical | theoretical\n"
              << "-----+--------------+-----------+------------\n";
    for (int k = 0; k <= kMaxIter; ++k) {
        Register reg = buildAndRun(markedIndex, k);
        int hits = 0;
        for (int t = 0; t < kSamplesPerK; ++t) {
            std::string out = reg.MeasureWithoutCollapse();
            if (indexFromBitString(out) == markedIndex) ++hits;
        }
        const double empirical   = static_cast<double>(hits) / kSamplesPerK;
        const double theoretical = theoreticalP(k);
        csv << k << "," << hits << "," << empirical << "," << theoretical << "\n";
        std::cout << " " << k
                  << "   |   " << hits
                  << " / " << kSamplesPerK
                  << "   |  " << empirical
                  << "  |  " << theoretical << "\n";
    }
    std::cout << "\nWrote sweep to data/grover_iterations.csv\n";
    return 0;
}
