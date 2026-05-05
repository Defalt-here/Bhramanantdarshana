/**
 * @file demo_bell.cpp
 * @brief Construct the four Bell states and sample 1000 outcomes from each.
 *
 * The four Bell states are the maximally-entangled basis of two qubits:
 *
 *   |Φ⁺⟩ = (|00⟩ + |11⟩)/√2,
 *   |Φ⁻⟩ = (|00⟩ − |11⟩)/√2,
 *   |Ψ⁺⟩ = (|01⟩ + |10⟩)/√2,
 *   |Ψ⁻⟩ = (|01⟩ − |10⟩)/√2.
 *
 * Each is prepared by the same two-step circuit — Hadamard on qubit 0
 * followed by CNOT with qubit 0 as control and qubit 1 as target — applied
 * to a different two-qubit basis state:
 *
 *   |00⟩ → |Φ⁺⟩
 *   |01⟩ → |Ψ⁺⟩
 *   |10⟩ → |Φ⁻⟩
 *   |11⟩ → |Ψ⁻⟩
 *
 * For each Bell state this demo prints the prepared state vector in Dirac
 * notation, then samples 1000 measurement outcomes (without collapse) into
 *   data/bell_<name>_samples.csv
 * with a single @c outcome column. The expected histograms:
 *   |Φ⁺⟩, |Φ⁻⟩ — only @c 00 and @c 11 outcomes, each ≈ 50 %;
 *   |Ψ⁺⟩, |Ψ⁻⟩ — only @c 01 and @c 10 outcomes, each ≈ 50 %.
 * Born-rule probabilities depend on |amplitude|² alone, so the relative
 * sign distinguishing Φ⁺/Φ⁻ (or Ψ⁺/Ψ⁻) is invisible to a single-basis
 * measurement. CHSH-style measurements in different bases can recover it
 * (see @c bench_chsh_violation.cpp).
 */

#include <iostream>
#include <string>

#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

namespace {

struct BellSpec {
    const char* name;        ///< filename-safe name, e.g. "phi_plus"
    const char* dirac;       ///< pretty Dirac label, e.g. "|Φ⁺⟩"
    const char* initState;   ///< 2-qubit big-endian initial bit-string
};

void prepareAndSample(const BellSpec& spec, int nSamples) {
    using std::cout;
    using std::endl;

    cout << "\n==== Preparing " << spec.dirac << " ("
         << spec.name << ") ====\n";

    Register reg(2, {{spec.initState, std::complex<double>(1.0, 0.0)}});
    cout << "Initial computational state |" << spec.initState << "⟩:\n  ";
    reg.Print();

    HadamardR H;
    CNOTGateR CNOT;

    H.ApplyToSingle(reg, 0);
    cout << "After H on qubit 0:\n  ";
    reg.Print();

    CNOT.ApplyToPair(reg, 0, 1);
    cout << "After CNOT(0,1) — final Bell state:\n  ";
    reg.Print();

    const std::string csvPath = std::string("data/bell_") + spec.name + "_samples.csv";
    reg.WriteMeasurementsCSV(csvPath, nSamples);
    cout << "Wrote " << nSamples << " samples to " << csvPath << "\n";
}

}  // namespace

int main() {
    qsim::rng::seed(42);

    std::cout << "Bell-state demo: preparing the four maximally-entangled\n"
              << "two-qubit states and sampling 1000 outcomes from each.\n";

    const BellSpec specs[] = {
        {"phi_plus",  "|Φ⁺⟩", "00"},
        {"psi_plus",  "|Ψ⁺⟩", "01"},
        {"phi_minus", "|Φ⁻⟩", "10"},
        {"psi_minus", "|Ψ⁻⟩", "11"},
    };

    constexpr int kSamples = 1000;
    for (const auto& s : specs) {
        prepareAndSample(s, kSamples);
    }

    std::cout << "\nAll Bell states prepared. CSVs in data/bell_*_samples.csv\n";
    return 0;
}
