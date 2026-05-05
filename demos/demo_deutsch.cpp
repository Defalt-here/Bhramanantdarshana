/**
 * @file demo_deutsch.cpp
 * @brief Deutsch's algorithm — the first quantum algorithm to beat classical.
 *
 * Problem. Given a black-box function f : {0,1} → {0,1}, decide whether
 * @c f is constant (f(0) = f(1)) or balanced (f(0) ≠ f(1)). Classically
 * this requires two queries in the worst case; Deutsch's algorithm uses
 * exactly one.
 *
 * Circuit (qubits in big-endian order: qubit 0 = input, qubit 1 = ancilla):
 * @code
 *   |0⟩ ── H ──┐         ┌── H ── M
 *              │         │
 *              │   U_f   │
 *              │         │
 *   |1⟩ ── H ──┘         └────────
 * @endcode
 *
 * State trace.
 *   1. Prepare |0⟩|1⟩.
 *   2. Apply H on both qubits → ½(|0⟩+|1⟩)(|0⟩−|1⟩).
 *   3. Apply U_f, which encodes f via phase kickback so that the input
 *      qubit picks up the factor (-1)^f(x) for each branch x.
 *   4. Apply H on the input qubit only.
 *   5. Measure the input qubit. The outcome is 0 iff f is constant and 1
 *      iff f is balanced — deterministically, in a single oracle query.
 *
 * The oracle implements U_f |x⟩|y⟩ = |x⟩|y ⊕ f(x)⟩. Because the project
 * uses big-endian indexing (qubit 0 = MSB of the state-index), bit
 * extraction inside the oracle uses @c (i >> 1) & 1 for x and @c i & 1
 * for y on the 2-qubit register.
 */

#include <complex>
#include <iostream>
#include <string>
#include <vector>

#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

/// The four single-bit Boolean functions we can reach with a 1-bit input.
enum class OracleType {
    Constant0,  ///< f(x) = 0 — constant
    Constant1,  ///< f(x) = 1 — constant
    Identity,   ///< f(x) = x — balanced
    Not         ///< f(x) = ¬x — balanced
};

/// Apply U_f for the chosen oracle type to a 2-qubit register.
static void applyOracle(Register& reg, OracleType type) {
    const int size = static_cast<int>(reg.val.size());
    for (int i = 0; i < size; ++i) {
        const int x = (i >> 1) & 1;          // qubit 0 (MSB) is the input
        const int y =  i       & 1;          // qubit 1 (LSB) is the ancilla
        int fx = 0;
        switch (type) {
            case OracleType::Constant0: fx = 0;     break;
            case OracleType::Constant1: fx = 1;     break;
            case OracleType::Identity:  fx = x;     break;
            case OracleType::Not:       fx = 1 - x; break;
        }
        const int j = (x << 1) | (y ^ fx);
        if (i < j) std::swap(reg.val[i], reg.val[j]);
    }
}

/// Run Deutsch's algorithm for one oracle and report the input-qubit outcome.
static int runDeutsch(OracleType type, const std::string& label) {
    std::cout << "\n--- Oracle: " << label << " ---\n";

    // Step 1: prepare |0⟩|1⟩.
    Register reg(2, {{"01", std::complex<double>(1.0, 0.0)}});
    std::cout << "Initial |01⟩:\n  ";
    reg.Print();

    // Step 2: H on both qubits.
    HadamardR H;
    H.ApplyToSingle(reg, 0);
    H.ApplyToSingle(reg, 1);
    std::cout << "After H ⊗ H:\n  ";
    reg.Print();

    // Step 3: oracle.
    applyOracle(reg, type);
    std::cout << "After U_f:\n  ";
    reg.Print();

    // Step 4: H on the input qubit only.
    H.ApplyToSingle(reg, 0);
    std::cout << "After final H on qubit 0:\n  ";
    reg.Print();

    // Step 5: read the input qubit (without collapse so we can re-use reg).
    const std::string outcome = reg.MeasureWithoutCollapse();
    const int firstBit = outcome[0] - '0';
    std::cout << "Measurement: " << outcome
              << "  → input qubit = " << firstBit
              << " ("
              << (firstBit == 0 ? "constant" : "balanced")
              << ")\n";
    return firstBit;
}

int main() {
    qsim::rng::seed(42);

    std::cout << "Deutsch's algorithm: distinguishing constant vs balanced\n"
              << "f : {0,1} → {0,1} in a single quantum oracle query.\n";

    struct Case {
        OracleType type;
        const char* label;
        int        expected;  ///< 0 for constant, 1 for balanced
    };
    const Case cases[] = {
        {OracleType::Constant0, "Constant0  f(x) = 0",   0},
        {OracleType::Constant1, "Constant1  f(x) = 1",   0},
        {OracleType::Identity,  "Identity   f(x) = x",   1},
        {OracleType::Not,       "NOT        f(x) = ¬x",  1},
    };

    int wrong = 0;
    for (const auto& c : cases) {
        int got = runDeutsch(c.type, c.label);
        if (got != c.expected) ++wrong;
    }

    std::cout << "\nSummary: " << (4 - wrong) << "/4 oracle types classified correctly.\n";
    return wrong;
}
