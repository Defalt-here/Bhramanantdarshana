/**
 * @file Qubits.cpp
 * @brief Implementation of the single-qubit @c Qubit class.
 *
 * Companion translation unit for @ref Qubits.h. Behaviour is documented on
 * the declarations there; this file only adds inline notes where the
 * implementation needs them.
 */

#include "Qubits.h"

#include <cmath>
#include <complex>
#include <iostream>

#include "RNG.h"

Qubit::Qubit(std::complex<double> a, std::complex<double> b) {
    val[0] = a;
    val[1] = b;
    Normalise();
    ampliA = val[0];
    ampliB = val[1];
}

void Qubit::Print() const {
    std::cout << "|ψ⟩ = ";
    bool firstPrinted = false;

    if (val[0] != std::complex<double>(0, 0)) {
        printComplex(val[0]);
        std::cout << "|0⟩";
        firstPrinted = true;
    }

    if (val[1] != std::complex<double>(0, 0)) {
        if (firstPrinted) std::cout << " + ";
        printComplex(val[1]);
        std::cout << "|1⟩";
        firstPrinted = true;
    }

    if (!firstPrinted) std::cout << "0";

    std::cout << std::endl;
}

double Qubit::MagnitudeSquareSum() const {
    return std::norm(val[0]) + std::norm(val[1]);
}

double Qubit::GetProbabilityOf0() const {
    return std::norm(val[0]);
}

double Qubit::GetProbabilityOf1() const {
    return std::norm(val[1]);
}

int Qubit::MeasureWithoutCollapse() const {
    double prob0 = std::norm(ampliA);
    double r = qsim::rng::uniform_double();
    return (r < prob0) ? 0 : 1;
}

bool Qubit::Collapse() {
    double prob0 = std::norm(ampliA);
    double r = qsim::rng::uniform_double();

    if (r < prob0) {
        // Collapse to |0⟩. The original code wrote
        //   ampliA,val[0] = {1.0, 0.0};
        // which is a comma-operator expression that only assigns val[0] and
        // discards ampliA. Each member is now assigned explicitly.
        val[0] = std::complex<double>(1.0, 0.0);
        val[1] = std::complex<double>(0.0, 0.0);
        ampliA = val[0];
        ampliB = val[1];
        return false;
    } else {
        val[0] = std::complex<double>(0.0, 0.0);
        val[1] = std::complex<double>(1.0, 0.0);
        ampliA = val[0];
        ampliB = val[1];
        return true;
    }
}

std::complex<double> Qubit::FindInnerProduct(const Qubit& other) const {
    return std::conj(val[0]) * other.val[0] + std::conj(val[1]) * other.val[1];
}

void Qubit::Normalise() {
    double mag2 = MagnitudeSquareSum();
    double s = std::sqrt(mag2);
    val[0] /= s;
    val[1] /= s;
}

void Qubit::printComplex(const std::complex<double>& c) const {
    double re = c.real();
    double im = c.imag();

    if (re == 0 && im == 0) {
        std::cout << "0";
        return;
    }

    bool printed = false;

    if (re != 0) {
        std::cout << re;
        printed = true;
    }

    if (im != 0) {
        if (im > 0 && printed) std::cout << "+";
        else if (im < 0)       std::cout << "-";

        if (std::abs(im) != 1) std::cout << std::abs(im);

        std::cout << "i";
    }
}
