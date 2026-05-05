/**
 * @file demo_bernstein_vazirani.cpp
 * @brief Bernstein–Vazirani algorithm — recover a hidden bit-string s
 *        from f(x) = s · x (mod 2) in a single quantum query.
 *
 * Problem. An n-bit hidden string @c s is encoded inside an oracle that
 * computes the dot product
 * @code
 *   f(x) = (s · x) mod 2 = Σ_k s_k · x_k  (mod 2).
 * @endcode
 * Classically each query reveals at most one bit of @c s, so we need n
 * queries. Quantumly, one query suffices: after the algorithm runs and we
 * measure the n input qubits, the bit-string we read *is* @c s.
 *
 * Circuit. Identical in shape to Deutsch–Jozsa:
 *   1. Prepare |0⟩^n |1⟩.
 *   2. Apply H on every qubit.
 *   3. Apply U_f.
 *   4. Apply H on the n input qubits.
 *   5. Measure the input qubits — outcome equals @c s.
 *
 * The hidden string is given on the command line as a binary literal,
 * e.g. @c ./demo_bernstein_vazirani 1011. Default is @c "1011" if no
 * argument is supplied. Big-endian convention: the leftmost character of
 * @c s is bit 0, which sits in qubit 0 (the MSB of the input register).
 */

#include <complex>
#include <cstring>
#include <iostream>
#include <string>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

namespace {

bool validBinaryString(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) if (c != '0' && c != '1') return false;
    return true;
}

/// Apply U_f for f(x) = s · x (mod 2) to an (n+1)-qubit register.
/// @p sBits is the n-bit string in big-endian order (sBits[0] = qubit 0 = MSB of x).
void applyOracle(Register& reg, int n, const std::string& sBits) {
    const int size = static_cast<int>(reg.val.size());
    const int ancillaMask = 1;
    const int xMask = (1 << n) - 1;

    // Pre-compute s as an integer with the same MSB-first convention.
    int sInt = 0;
    for (char c : sBits) sInt = (sInt << 1) | (c == '1' ? 1 : 0);

    for (int i = 0; i < size; ++i) {
        const int x = (i >> 1) & xMask;
        const int y = i & ancillaMask;
        // f(x) = popcount(s & x) mod 2.
        int xs = x & sInt;
        int fx = 0;
        while (xs) { fx ^= 1; xs &= (xs - 1); }
        const int j = (i & ~ancillaMask) | (y ^ fx);
        if (i < j) std::swap(reg.val[i], reg.val[j]);
    }
}

}  // namespace

int main(int argc, char** argv) {
    qsim::rng::seed(42);

    std::string s = (argc >= 2) ? argv[1] : "1011";
    if (!validBinaryString(s)) {
        std::cerr << "hidden string must be non-empty and contain only 0/1\n";
        return 1;
    }
    const int n = static_cast<int>(s.size());
    if (n + 1 > qsim::kMaxQubits) {
        std::cerr << "hidden string too long: max n = "
                  << (qsim::kMaxQubits - 1) << "\n";
        return 1;
    }

    std::cout << "Bernstein–Vazirani: recovering n=" << n
              << "-bit string s = " << s << " in one quantum query.\n";

    const int total = n + 1;
    std::string initBits(total, '0');
    initBits.back() = '1';
    Register reg(total, {{initBits, std::complex<double>(1.0, 0.0)}});
    std::cout << "\nInitial |0...01⟩:\n  ";
    reg.Print();

    HadamardR H;
    for (int q = 0; q < total; ++q) H.ApplyToSingle(reg, q);
    std::cout << "After H on every qubit:\n  ";
    reg.Print();

    applyOracle(reg, n, s);
    std::cout << "After U_f:\n  ";
    reg.Print();

    for (int q = 0; q < n; ++q) H.ApplyToSingle(reg, q);
    std::cout << "After H on the n input qubits:\n  ";
    reg.Print();

    const std::string outcome = reg.MeasureWithoutCollapse();
    const std::string recovered = outcome.substr(0, n);
    std::cout << "\nMeasured: " << outcome
              << "  → recovered s = " << recovered
              << "  (target = " << s << ")  "
              << (recovered == s ? "✓" : "✗") << "\n";
    std::cout << "Classical worst case: " << n
              << " queries; quantum: 1 query.\n";
    return recovered == s ? 0 : 1;
}
