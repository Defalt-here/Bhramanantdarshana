/**
 * @file RNG.cpp
 * @brief Implementation of the shared random-number generator.
 *
 * The engine lives in a function-local @c static so it is constructed
 * exactly once on first use — well-defined under C++11's "magic statics"
 * rule. Reseeding via @c seed(s) goes through the same accessor, which
 * keeps the engine identity stable for the life of the program.
 */

#include "RNG.h"

#include <random>

namespace qsim::rng {

namespace {

/// Returns a reference to the single shared engine instance.
/// Lazily constructed on first call.
std::mt19937_64& shared_engine() {
    static std::mt19937_64 eng(default_seed());
    return eng;
}

}  // namespace

std::uint64_t default_seed() {
    std::random_device rd;
    // std::random_device::result_type is unsigned int (typically 32 bits),
    // so we splice two draws together to fill a 64-bit seed.
    return (static_cast<std::uint64_t>(rd()) << 32)
         ^ static_cast<std::uint64_t>(rd());
}

void seed(std::uint64_t s) {
    shared_engine().seed(s);
}

std::mt19937_64& engine() {
    return shared_engine();
}

double uniform_double() {
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(shared_engine());
}

}  // namespace qsim::rng
