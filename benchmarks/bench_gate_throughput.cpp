/**
 * @file bench_gate_throughput.cpp
 * @brief Wall-clock throughput of each gate across register sizes 1..12.
 *
 * For every (n, gate) combination we apply the gate 1000 times to a freshly
 * prepared all-zeros n-qubit register, picking a random target qubit each
 * time. Two-qubit gates additionally pick a random control distinct from
 * the target.
 *
 * Output: benchmarks/results/bench_gate_throughput.csv
 *   columns: qubits, gate, total_time_ms, time_per_gate_us
 */
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

using clock_type = std::chrono::steady_clock;
constexpr int kReps = 1000;

template <class GateFn>
double timeMs(GateFn fn) {
    auto t0 = clock_type::now();
    fn();
    auto t1 = clock_type::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main() {
    qsim::rng::seed(42);
    std::ofstream csv("benchmarks/results/bench_gate_throughput.csv");
    csv << "qubits,gate,total_time_ms,time_per_gate_us\n";

    HadamardR H; XGateR X; YGateR Y; ZGateR Z; SGateR S; TGateR T;
    CNOTGateR CNOT; CZGateR CZ; SWAPGateR SW;

    std::mt19937_64 rng(42);
    std::vector<std::string> singleNames = {"H", "X", "Y", "Z", "S", "T"};

    for (int n = 1; n <= 12; ++n) {
        std::uniform_int_distribution<int> qpick(0, n - 1);

        // single-qubit gates
        for (const auto& name : singleNames) {
            Register r(n);
            std::vector<int> qs(kReps);
            for (auto& q : qs) q = qpick(rng);
            const double ms = timeMs([&]{
                for (int i = 0; i < kReps; ++i) {
                    if      (name == "H") H.ApplyToSingle(r, qs[i]);
                    else if (name == "X") X.ApplyToSingle(r, qs[i]);
                    else if (name == "Y") Y.ApplyToSingle(r, qs[i]);
                    else if (name == "Z") Z.ApplyToSingle(r, qs[i]);
                    else if (name == "S") S.ApplyToSingle(r, qs[i]);
                    else if (name == "T") T.ApplyToSingle(r, qs[i]);
                }
            });
            csv << n << "," << name << "," << ms << "," << (ms * 1000.0 / kReps) << "\n";
        }

        // two-qubit gates require n >= 2
        if (n >= 2) {
            std::vector<std::string> twoNames = {"CNOT", "CZ", "SWAP"};
            for (const auto& name : twoNames) {
                Register r(n);
                std::vector<std::pair<int,int>> ps(kReps);
                for (auto& p : ps) {
                    int c = qpick(rng), t = qpick(rng);
                    if (c == t) t = (c + 1) % n;
                    p = {c, t};
                }
                const double ms = timeMs([&]{
                    for (int i = 0; i < kReps; ++i) {
                        if      (name == "CNOT") CNOT.ApplyToPair(r, ps[i].first, ps[i].second);
                        else if (name == "CZ")   CZ.ApplyToPair(r,   ps[i].first, ps[i].second);
                        else if (name == "SWAP") SW.ApplyToPair(r,   ps[i].first, ps[i].second);
                    }
                });
                csv << n << "," << name << "," << ms << "," << (ms * 1000.0 / kReps) << "\n";
            }
        }
    }

    csv.flush();
    std::cout << "Wrote benchmarks/results/bench_gate_throughput.csv\n";
    return 0;
}
