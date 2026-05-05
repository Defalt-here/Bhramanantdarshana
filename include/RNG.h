#pragma once

#include <cstdint>
#include <random>

/**
 * @file RNG.h
 * @brief Project-wide random-number generation.
 *
 * Every call site in this codebase that needs randomness — measurement
 * collapse, statistical benchmarks, randomised tests — funnels through the
 * @c qsim::rng namespace below, so a single @c qsim::rng::seed() call is
 * sufficient to make a whole demo or test reproducible.
 *
 * The shared engine is a @c std::mt19937_64. It is constructed lazily on first
 * use; if the caller never calls @c seed(), a non-deterministic 64-bit seed
 * is drawn from @c std::random_device. Thread-safety is *not* provided —
 * the simulator is single-threaded by design.
 */
namespace qsim::rng {

/**
 * @brief Deterministically seed the shared engine.
 * @param s 64-bit seed value.
 *
 * Use this at the top of @c main() in tests, demos, and benchmarks to make
 * the run reproducible. Any subsequent draws (uniform_double, engine().*) use
 * the new seed.
 */
void seed(std::uint64_t s);

/**
 * @brief Generate a non-deterministic 64-bit seed from @c std::random_device.
 *
 * Useful when you want fresh randomness but still want to log the seed for
 * later replay: read this once, log it, then pass it back to @c seed().
 */
std::uint64_t default_seed();

/**
 * @brief Sample a uniform real in [0, 1) from the shared engine.
 *
 * This is the primary entry point for measurement-collapse sampling.
 */
double uniform_double();

/**
 * @brief Direct access to the shared engine.
 *
 * Provided for callers that need a non-uniform distribution (for example, a
 * @c std::uniform_int_distribution to pick a random qubit index).
 */
std::mt19937_64& engine();

}  // namespace qsim::rng
