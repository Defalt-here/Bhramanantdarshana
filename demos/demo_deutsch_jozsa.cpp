/**
 * @file demo_deutsch_jozsa.cpp
 * @brief Deutsch–Jozsa algorithm — the n-qubit generalisation of Deutsch.
 *
 * Problem. Given a black-box function f : {0,1}^n → {0,1} that is *promised*
 * to be either constant (same output for every input) or balanced (output
 * 0 on half the inputs and 1 on the other half), decide which.
 *
 *   Classical worst case: 2^(n-1) + 1 queries.
 *   Quantum:               1 query.
 *
 * Circuit (n input qubits + 1 ancilla = n+1 total, big-endian):
 * @code
 *   |0⟩^n ── H^⊗n ──┐         ┌── H^⊗n ── M^⊗n
 *                   │         │
 *                   │   U_f   │
 *                   │         │
 *   |1⟩   ── H ─────┘         └─────────────────
 * @endcode
 *
 * If the n input qubits all measure 0 the function is constant; otherwise
 * it is balanced.
 *
 * The demo exercises four oracle types:
 *   1. constant-0           — f(x) = 0
 *   2. constant-1           — f(x) = 1
 *   3. balanced-by-parity   — f(x) = parity(x)         (XOR of all input bits)
 *   4. balanced-by-first-bit — f(x) = bit-at(qubit 0)  (input MSB)
 *
 * Default n = 4 input qubits (5 total). Pass an integer command-line
 * argument to override.
 */

#include <complex>
#include <cstdlib>
#include <iostream>
#include <string>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

enum class DJOracle {
    Constant0,
    Constant1,
    BalancedParity,
    BalancedFirstBit,
};

namespace {

/// Compute f(x) for an n-bit input @p x under the chosen oracle.
int evalF(int x, int n, DJOracle which) {
    switch (which) {
        case DJOracle::Constant0: return 0;
        case DJOracle::Constant1: return 1;
        case DJOracle::BalancedParity: {
            int p = 0;
            for (int b = 0; b < n; ++b) p ^= (x >> b) & 1;
            return p;
        }
        case DJOracle::BalancedFirstBit:
            // Big-endian: qubit 0 of the n-input register is the MSB,
            // i.e. bit (n-1) of the integer x.
            return (x >> (n - 1)) & 1;
    }
    return 0;
}

/// Apply U_f to the (n + 1)-qubit register: qubit 0..n-1 input, qubit n ancilla.
void applyOracle(Register& reg, int n, DJOracle which) {
    const int size = static_cast<int>(reg.val.size());
    const int ancillaMask = 1;                 // ancilla is the LSB (qubit n)
    const int xMask       = (1 << n) - 1;      // bits [n-1 .. 0] hold x in BE order
    for (int i = 0; i < size; ++i) {
        const int x = (i >> 1) & xMask;        // shift past the ancilla bit
        const int y =  i & ancillaMask;
        const int fx = evalF(x, n, which);
        const int j = (i & ~ancillaMask) | (y ^ fx);
        if (i < j) std::swap(reg.val[i], reg.val[j]);
    }
}

const char* oracleName(DJOracle o) {
    switch (o) {
        case DJOracle::Constant0:        return "constant-0";
        case DJOracle::Constant1:        return "constant-1";
        case DJOracle::BalancedParity:   return "balanced (parity)";
        case DJOracle::BalancedFirstBit: return "balanced (first input bit)";
    }
    return "?";
}

bool isConstant(DJOracle o) {
    return o == DJOracle::Constant0 || o == DJOracle::Constant1;
}

void run(int n, DJOracle which) {
    std::cout << "\n--- n=" << n << ",  oracle: " << oracleName(which) << " ---\n";

    const int total = n + 1;

    // |0...0⟩|1⟩: ancilla qubit (the rightmost / LSB position in BE)
    // starts in |1⟩, all input qubits in |0⟩.
    std::string initBits(total, '0');
    initBits.back() = '1';
    Register reg(total, {{initBits, std::complex<double>(1.0, 0.0)}});

    HadamardR H;
    for (int q = 0; q < total; ++q) H.ApplyToSingle(reg, q);

    applyOracle(reg, n, which);

    for (int q = 0; q < n; ++q) H.ApplyToSingle(reg, q);

    const std::string outcome = reg.MeasureWithoutCollapse();
    // Inspect the n input qubits (the leftmost n characters in big-endian).
    const std::string inputBits = outcome.substr(0, n);
    bool allZero = true;
    for (char c : inputBits) if (c != '0') { allZero = false; break; }

    std::cout << "Measured " << outcome
              << "  (input bits = " << inputBits << ")\n";
    std::cout << "Verdict: " << (allZero ? "CONSTANT" : "BALANCED")
              << "  (truth: " << (isConstant(which) ? "CONSTANT" : "BALANCED") << ")\n";
    std::cout << "Classical worst case for n=" << n
              << ": " << ((1 << (n - 1)) + 1) << " queries; quantum: 1 query.\n";
}

}  // namespace

int main(int argc, char** argv) {
    qsim::rng::seed(42);

    int n = 4;
    if (argc >= 2) n = std::atoi(argv[1]);
    if (n < 1 || n + 1 > qsim::kMaxQubits) {
        std::cerr << "Need 1 ≤ n ≤ " << (qsim::kMaxQubits - 1) << "\n";
        return 1;
    }

    std::cout << "Deutsch–Jozsa: deciding constant vs balanced for "
                 "f : {0,1}^" << n << " → {0,1} with one quantum query.\n";

    run(n, DJOracle::Constant0);
    run(n, DJOracle::Constant1);
    run(n, DJOracle::BalancedParity);
    run(n, DJOracle::BalancedFirstBit);
    return 0;
}
