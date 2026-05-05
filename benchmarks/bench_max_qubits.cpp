/**
 * @file bench_max_qubits.cpp
 * @brief Construction time, memory, and full Hadamard-layer time
 *        across register sizes that approach @c qsim::kMaxQubits.
 *
 * For each n in {8, 10, 12, 13, 14}, we
 *   - time the construction of an n-qubit Register (i.e. allocating and
 *     zero-filling 2^n complex amplitudes plus initialising val[0] = 1),
 *   - record the resulting memory footprint in megabytes
 *     (= 16 · 2^n / 1e6 — a complex<double> is 16 bytes),
 *   - time one full layer of Hadamards (one @c H per qubit, applied
 *     sequentially via @c HadamardR::Apply).
 *
 * Output: benchmarks/results/bench_max_qubits.csv
 *   columns: qubits, memory_mb, construct_ms, hadamard_ms
 */
#include <chrono>
#include <fstream>
#include <iostream>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

using clock_type = std::chrono::steady_clock;

int main() {
    qsim::rng::seed(42);
    std::ofstream csv("benchmarks/results/bench_max_qubits.csv");
    csv << "qubits,memory_mb,construct_ms,hadamard_ms\n";

    const int sizes[] = {8, 10, 12, 13, 14};
    for (int n : sizes) {
        if (n > qsim::kMaxQubits) continue;
        const double memMb = 16.0 * static_cast<double>(1u << n) / 1.0e6;

        auto t0 = clock_type::now();
        Register r(n);
        auto t1 = clock_type::now();
        const double constructMs =
            std::chrono::duration<double, std::milli>(t1 - t0).count();

        HadamardR H;
        auto t2 = clock_type::now();
        H.Apply(r);
        auto t3 = clock_type::now();
        const double hadamardMs =
            std::chrono::duration<double, std::milli>(t3 - t2).count();

        csv << n << "," << memMb << "," << constructMs << "," << hadamardMs << "\n";
        std::cout << "n=" << n
                  << "  mem=" << memMb << " MB"
                  << "  construct=" << constructMs << " ms"
                  << "  H_layer=" << hadamardMs << " ms\n";
    }
    csv.flush();
    std::cout << "Wrote benchmarks/results/bench_max_qubits.csv\n";
    return 0;
}
