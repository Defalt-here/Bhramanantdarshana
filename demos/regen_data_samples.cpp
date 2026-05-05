/**
 * @file regen_data_samples.cpp
 * @brief One-shot helper that regenerates the headline sample CSVs in
 *        @c data/ using the consolidated @c WriteMeasurementsCSV format
 *        (single @c outcome column).
 *
 * Files regenerated:
 *   - data/coin_flips.csv             (100 samples,  1-qubit Hadamard)
 *   - data/bell_500_samples.csv       (500 samples,  Bell |Φ⁺⟩)
 *   - data/ghz_500_samples.csv        (500 samples,  3-qubit GHZ)
 *
 * Reproducible: seeded with @c qsim::rng::seed(42) at the top of @c main.
 */
#include <iostream>

#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

int main() {
    qsim::rng::seed(42);

    {
        // Coin flip: 1-qubit register, single Hadamard.
        Register r(1);
        HadamardR H;
        H.Apply(r);
        r.WriteMeasurementsCSV("data/coin_flips.csv", 100);
        std::cout << "Wrote data/coin_flips.csv\n";
    }
    {
        // Bell |Φ⁺⟩ = (|00⟩+|11⟩)/√2.
        Register r(2);
        HadamardR H;
        CNOTGateR CNOT;
        H.ApplyToSingle(r, 0);
        CNOT.ApplyToPair(r, 0, 1);
        r.WriteMeasurementsCSV("data/bell_500_samples.csv", 500);
        std::cout << "Wrote data/bell_500_samples.csv\n";
    }
    {
        // 3-qubit GHZ.
        Register r(3);
        HadamardR H;
        CNOTGateR CNOT;
        H.ApplyToSingle(r, 0);
        CNOT.ApplyToPair(r, 0, 1);
        CNOT.ApplyToPair(r, 1, 2);
        r.WriteMeasurementsCSV("data/ghz_500_samples.csv", 500);
        std::cout << "Wrote data/ghz_500_samples.csv\n";
    }

    return 0;
}
