/**
 * @file bench_norm_drift.cpp
 * @brief Track |Σ|αᵢ|² − 1| over a long random circuit on 5 qubits.
 *
 * Apply 10 000 random gate operations (uniform over the implemented gate
 * set) to a 5-qubit register, recording the deviation from unit norm every
 * 100 steps. In exact arithmetic each gate is unitary and the deviation is
 * 0; in floating-point arithmetic we expect the deviation to creep upward
 * but stay in the order of @c 1e-12 across thousands of gates.
 *
 * Output: benchmarks/results/bench_norm_drift.csv
 *   columns: step, deviation
 */
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>

#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

constexpr int kSteps         = 10000;
constexpr int kSampleEvery   = 100;
constexpr int kNQubits       = 5;

int main() {
    qsim::rng::seed(42);
    std::ofstream csv("benchmarks/results/bench_norm_drift.csv");
    csv << "step,deviation\n";

    Register r(kNQubits);

    HadamardR H; XGateR X; YGateR Y; ZGateR Z; SGateR S; TGateR T;
    CNOTGateR CNOT; CZGateR CZ; SWAPGateR SW;

    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int> gateChoice(0, 8);
    std::uniform_int_distribution<int> qpick(0, kNQubits - 1);

    for (int step = 1; step <= kSteps; ++step) {
        const int q = qpick(rng);
        switch (gateChoice(rng)) {
            case 0: H.ApplyToSingle(r, q); break;
            case 1: X.ApplyToSingle(r, q); break;
            case 2: Y.ApplyToSingle(r, q); break;
            case 3: Z.ApplyToSingle(r, q); break;
            case 4: S.ApplyToSingle(r, q); break;
            case 5: T.ApplyToSingle(r, q); break;
            case 6: case 7: case 8: {
                int c = qpick(rng), t = qpick(rng);
                if (c == t) t = (c + 1) % kNQubits;
                if      (gateChoice(rng) % 3 == 0) CNOT.ApplyToPair(r, c, t);
                else if (gateChoice(rng) % 3 == 1) CZ.ApplyToPair(r, c, t);
                else                                SW.ApplyToPair(r, c, t);
                break;
            }
        }
        if (step % kSampleEvery == 0) {
            const double dev = std::abs(r.MagnitudeSquareSum() - 1.0);
            csv << step << "," << dev << "\n";
        }
    }

    csv.flush();
    std::cout << "Wrote benchmarks/results/bench_norm_drift.csv\n";
    return 0;
}
