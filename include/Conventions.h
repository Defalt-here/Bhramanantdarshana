#pragma once

/**
 * @file Conventions.h
 * @brief Project-wide indexing convention and tolerance constants.
 *
 * @section indexing Big-endian qubit indexing
 *
 * Qubit 0 is the **most-significant bit** of the state-vector index. For an
 * n-qubit register, qubit @c q (with @c 0 <= q < n) lives in bit position
 * @c (n - 1 - q) of the index. The bitmask that selects qubit @c q is
 * therefore @c (1 << (n - 1 - q)).
 *
 * @subsection example Worked example for n = 3
 *
 * The basis state @c |abc⟩ — where qubit 0 = a, qubit 1 = b, qubit 2 = c —
 * is stored at state-vector index @c i = 4a + 2b + c.
 *
 * @code
 *   |000⟩ → 0       |100⟩ → 4
 *   |001⟩ → 1       |101⟩ → 5
 *   |010⟩ → 2       |110⟩ → 6
 *   |011⟩ → 3       |111⟩ → 7
 * @endcode
 *
 * To extract the value of qubit @c q from a state index @c i:
 * @code
 *   bit_q = (i >> (n - 1 - q)) & 1
 * @endcode
 *
 * Every gate, every demo, every test, and every CSV/JSON output in this
 * project obeys this convention. The leftmost character of any printed
 * bit-string is qubit 0.
 *
 * @section tolerances Floating-point tolerances
 *
 * Quantum operations on a finite-precision state vector accumulate small
 * round-off errors. Two project-wide tolerances pin what counts as "equal":
 * @c kNormTolerance for whole-state normalisation checks, and
 * @c kAmplitudeTolerance for per-amplitude equality.
 */

namespace qsim {

/// Tolerance for normalisation checks: a state is considered normalised when
/// |Σ|αᵢ|² − 1| < @c kNormTolerance.
inline constexpr double kNormTolerance = 1e-9;

/// Per-amplitude tolerance for state-vector equality. Two state vectors are
/// considered equal when @c |a_i - b_i| < @c kAmplitudeTolerance for every i.
inline constexpr double kAmplitudeTolerance = 1e-10;

/// Hard cap on register size to guard against accidentally allocating a 2^N
/// state vector that does not fit in memory. 2^14 amplitudes ≈ 256 KB at
/// 16 bytes per std::complex<double>.
inline constexpr int kMaxQubits = 14;

}  // namespace qsim
