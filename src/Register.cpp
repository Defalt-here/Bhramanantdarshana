/**
 * @file Register.cpp
 * @brief Implementation of the multi-qubit @c Register class.
 *
 * Companion translation unit for @ref Register.h. The two-step pattern used
 * by @c Collapse() and @c MeasureWithoutCollapse() — build a running
 * cumulative probability, then walk it once with a single uniform draw —
 * is the standard textbook sampler for a categorical distribution.
 */

#include "Register.h"

#include <cassert>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>

#include "Conventions.h"
#include "RNG.h"

Register::Register(int n) : bits(n) {
    assert(n >= 1 && n <= qsim::kMaxQubits && "Register size out of range");
    int size = 1 << n;
    val.assign(static_cast<std::size_t>(size), std::complex<double>(0.0, 0.0));
    val[0] = std::complex<double>(1.0, 0.0);
    Normalise();
}

Register::Register(int n, const std::map<std::string, std::complex<double>>& initStates)
    : bits(n) {
    assert(n >= 1 && n <= qsim::kMaxQubits && "Register size out of range");
    int size = 1 << n;
    val.assign(static_cast<std::size_t>(size), std::complex<double>(0.0, 0.0));

    for (const auto& [bitstring, amplitude] : initStates) {
        assert(static_cast<int>(bitstring.length()) == n
               && "Bitstring length must match register size");
        int index = std::stoi(bitstring, nullptr, 2);
        assert(index < size && "Index out of bounds for register size");
        val[index] = amplitude;
    }

    Normalise();
}

double Register::MagnitudeSquareSum() const {
    double r = 0.0;
    for (const auto& a : val) r += std::norm(a);
    return r;
}

double Register::GetProbab(int i) const {
    return std::norm(val[i]);
}

std::complex<double> Register::FindInnerProduct(const Register& other) const {
    assert(val.size() == other.val.size());
    std::complex<double> ip(0.0, 0.0);
    for (std::size_t i = 0; i < val.size(); ++i) {
        ip += std::conj(val[i]) * other.val[i];
    }
    return ip;
}

void Register::Print() const {
    std::cout << "|ψ⟩ = ";
    bool first = true;

    for (std::size_t i = 0; i < val.size(); ++i) {
        if (std::abs(val[i]) < 1e-6) continue;
        if (!first) std::cout << " + ";
        first = false;

        std::cout << val[i] << "|";
        // Big-endian: print bit (bits-1) first so qubit 0 appears leftmost.
        for (int j = bits - 1; j >= 0; --j) {
            std::cout << ((i >> j) & 1);
        }
        std::cout << "⟩";
    }

    std::cout << std::endl;
}

std::string Register::Collapse() {
    double r = qsim::rng::uniform_double();

    double cumulative = 0.0;
    std::size_t collapsed = val.size() - 1;  // fall back to last index
    for (std::size_t i = 0; i < val.size(); ++i) {
        cumulative += std::norm(val[i]);
        if (r < cumulative) { collapsed = i; break; }
    }

    for (std::size_t i = 0; i < val.size(); ++i) {
        val[i] = (i == collapsed) ? std::complex<double>(1.0, 0.0)
                                  : std::complex<double>(0.0, 0.0);
    }

    std::string result;
    result.reserve(static_cast<std::size_t>(bits));
    for (int j = bits - 1; j >= 0; --j) {
        result += static_cast<char>('0' + ((collapsed >> j) & 1));
    }
    return result;
}

std::string Register::MeasureWithoutCollapse() const {
    double r = qsim::rng::uniform_double();

    double cumulative = 0.0;
    std::size_t chosen = val.size() - 1;
    for (std::size_t i = 0; i < val.size(); ++i) {
        cumulative += std::norm(val[i]);
        if (r < cumulative) { chosen = i; break; }
    }

    std::string result;
    result.reserve(static_cast<std::size_t>(bits));
    for (int j = bits - 1; j >= 0; --j) {
        result += static_cast<char>('0' + ((chosen >> j) & 1));
    }
    return result;
}

void Register::WriteMeasurementsCSV(const std::string& filename, int n_samples) const {
    std::ofstream out(filename);
    out << "outcome\n";
    for (int i = 0; i < n_samples; ++i) {
        out << MeasureWithoutCollapse() << "\n";
    }
}

void Register::ToJson(std::ostream& os) const {
    // Use full double precision so a round-trip through JSON loses no bits.
    os << std::setprecision(17);
    os << "{\n";
    os << "  \"bits\": " << bits << ",\n";
    os << "  \"amplitudes\": [\n";
    for (std::size_t i = 0; i < val.size(); ++i) {
        os << "    [" << val[i].real() << ", " << val[i].imag() << "]";
        if (i + 1 < val.size()) os << ",";
        os << "\n";
    }
    os << "  ]\n";
    os << "}\n";
}

void Register::Normalise() {
    double mag2 = MagnitudeSquareSum();
    if (mag2 == 0.0) return;  // a zero vector cannot be normalised
    double s = std::sqrt(mag2);
    for (auto& a : val) a /= s;
}
