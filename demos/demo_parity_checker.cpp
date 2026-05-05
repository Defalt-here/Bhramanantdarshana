/**
 * @file demo_parity_checker.cpp
 * @brief Side-by-side parity check: classical sequential scan vs quantum
 *        superposition sampling on a 3-bit input space.
 *
 * The quantum side prepares an arbitrary 3-qubit superposition (auto-
 * normalised by the @c Register constructor), applies a Hadamard layer to
 * spread the amplitudes, then samples measurement outcomes; for each
 * outcome it reports the parity (count of 1-bits, mod 2).
 *
 * The classical side iterates the eight 3-bit strings in turn, with a
 * small synthetic delay per "gate" to crudely model an electronic parity
 * checker. The two are not strictly comparable — the quantum side gets one
 * sample per measurement, and many samples are needed to read off the
 * parity distribution — but the timing and parallelism contrast is the
 * pedagogical point.
 *
 * This demo is a port of the original @c Parity\ Checker.cpp from the root
 * of the repository, adjusted to use the new project layout, the
 * @c qsim::rng RNG, and a deterministic seed so the measurement results
 * are reproducible.
 */

#include <algorithm>
#include <chrono>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::duration_cast;

namespace {

const std::map<std::string, std::complex<double>>& customAmps() {
    static const std::map<std::string, std::complex<double>> a = {
        {"000", {0.7, 0.0}},
        {"001", {0.1, 0.0}},
        {"010", {0.2, 0.0}},
        {"011", {0.5, 0.6}},
        {"100", {0.3, 0.0}},
        {"101", {0.5, 0.8}},
        {"110", {0.5, 0.9}},
        {"111", {0.0, 0.1}},
    };
    return a;
}

double classicalParityChecker(const vector<string>& inputs) {
    auto start = high_resolution_clock::now();
    cout << "\n=== CLASSICAL PARITY CHECKER ===\n";
    cout << "Processing " << inputs.size() << " inputs sequentially...\n";

    map<string, int> parityResults;
    for (const string& input : inputs) {
        std::this_thread::sleep_for(microseconds(1));   // synthetic gate delay
        int ones = static_cast<int>(std::count(input.begin(), input.end(), '1'));
        string parity = (ones % 2 == 0) ? "EVEN" : "ODD";
        parityResults[parity]++;
        cout << "  Input: " << input << " → " << ones << " ones → " << parity << "\n";
    }
    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);

    cout << "\nClassical results:\n";
    for (const auto& p : parityResults) {
        cout << "  " << p.first << ": " << p.second << " inputs\n";
    }
    cout << "Total time: " << duration.count() << " µs\n";
    return static_cast<double>(duration.count());
}

double quantumParityChecker(int numMeasurements) {
    auto start = high_resolution_clock::now();
    cout << "\n=== QUANTUM PARITY CHECKER ===\n";

    Register reg(3, customAmps());
    cout << "Initial superposition (auto-normalised):\n  ";
    reg.Print();

    HadamardR H;
    H.Apply(reg);
    cout << "After H ⊗ H ⊗ H:\n  ";
    reg.Print();

    map<string, int> parityResults;
    map<string, int> stateFrequency;
    cout << "\nQuantum samples:\n";
    for (int i = 0; i < numMeasurements; ++i) {
        Register tmp = reg;                       // copy for destructive collapse
        string outcome = tmp.Collapse();
        int ones = static_cast<int>(std::count(outcome.begin(), outcome.end(), '1'));
        string parity = (ones % 2 == 0) ? "EVEN" : "ODD";
        parityResults[parity]++;
        stateFrequency[outcome]++;
        if (i < 8) {
            cout << "  Measurement " << (i + 1) << ": |" << outcome << "⟩ → "
                 << ones << " ones → " << parity << "\n";
        }
    }
    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);

    cout << "\nParity histogram:\n";
    for (const auto& p : parityResults) {
        cout << "  " << p.first << ": " << p.second << " / " << numMeasurements
             << "  (" << std::fixed << std::setprecision(1)
             << (100.0 * p.second / numMeasurements) << " %)\n";
    }
    cout << "\nState frequency:\n";
    for (const auto& s : stateFrequency) {
        cout << "  |" << s.first << "⟩: " << s.second << "\n";
    }
    cout << "Total time: " << duration.count() << " µs\n";
    return static_cast<double>(duration.count());
}

}  // namespace

int main() {
    qsim::rng::seed(42);

    cout << "Electronic-vs-quantum parity demo (3-bit inputs).\n";

    vector<string> allInputs = {"000", "001", "010", "011", "100", "101", "110", "111"};
    double classicalTime = classicalParityChecker(allInputs);
    double quantumTime   = quantumParityChecker(20);

    cout << "\nSummary\n"
         << "  Classical (8 inputs, sequential): " << classicalTime << " µs\n"
         << "  Quantum (20 samples):              " << quantumTime  << " µs\n";
    return 0;
}
