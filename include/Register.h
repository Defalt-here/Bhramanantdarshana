#pragma once

#include <complex>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

/**
 * @class Register
 * @brief Multi-qubit pure state stored as a dense complex amplitude vector.
 *
 * A quantum register of @c n qubits lives in a 2ⁿ-dimensional Hilbert space.
 * The state is the column vector
 * @code
 *   |ψ⟩ = Σᵢ αᵢ |i⟩
 * @endcode
 * where @c αᵢ is the amplitude of basis state @c |i⟩ and @c Σ|αᵢ|² = 1.
 *
 * @section indexing Indexing convention
 *
 * Big-endian: qubit 0 is the most-significant bit of @c i. The basis state
 * @c |abc⟩ for a 3-qubit register (qubit 0 = a, qubit 1 = b, qubit 2 = c)
 * sits at @c val[4a + 2b + c]. See @ref Conventions.h for the full
 * worked example.
 *
 * @section gate_compat Gate compatibility
 *
 * Single-qubit gates in the @c RGates hierarchy locate their target qubit
 * via the bitmask @c (1 << (bits - 1 - qubitIndex)). Two-qubit gates pair
 * the corresponding control and target masks. As long as the gate stays
 * within this convention, it composes consistently with @c Print(),
 * @c Collapse(), and the bit-string constructor below.
 */
class Register {
public:
    /// Number of qubits in this register.
    int bits;

    /// Dense state vector of length 2^@c bits.
    std::vector<std::complex<double>> val;

    /**
     * @brief Build an n-qubit register in the all-zeros state |00…0⟩.
     * @param n Number of qubits, with @c 1 <= n <= @c qsim::kMaxQubits.
     *
     * @complexity Time @c O(2^n), space @c O(2^n).
     */
    explicit Register(int n);

    /**
     * @brief Build an n-qubit register with arbitrary initial amplitudes.
     * @param n Number of qubits.
     * @param initStates Map from bit-string (length n) to complex amplitude.
     *
     * Bit-strings are interpreted in big-endian order: the leftmost
     * character is qubit 0. The state is auto-normalised after assignment.
     *
     * @code
     * Register reg(2, {{"00", {0.6, 0.0}}, {"11", {0.8, 0.0}}});
     * // → 0.6|00⟩ + 0.8|11⟩, then normalised to unit length.
     * @endcode
     *
     * @complexity Time @c O(2^n + |initStates|), space @c O(2^n).
     */
    Register(int n, const std::map<std::string, std::complex<double>>& initStates);

    /**
     * @brief Sum of squared magnitudes of every amplitude.
     * @return Σᵢ |αᵢ|², which equals 1 for a properly normalised state.
     * @complexity Time @c O(2^n), space @c O(1).
     */
    double MagnitudeSquareSum() const;

    /**
     * @brief Born-rule probability of measuring basis state @c |i⟩.
     * @param i State-vector index, with @c 0 <= i < 2^@c bits.
     * @return @c P(i) = |αᵢ|².
     * @complexity Time @c O(1).
     */
    double GetProbab(int i) const;

    /**
     * @brief Quantum inner product ⟨this|other⟩.
     * @param other Another register of the same size.
     * @return @c Σᵢ conj(αᵢ) · α'ᵢ.
     *
     * Taken by @c const reference: copying a register can be expensive for
     * larger @c n, and the inner product is purely a read.
     *
     * @complexity Time @c O(2^n), space @c O(1).
     */
    std::complex<double> FindInnerProduct(const Register& other) const;

    /**
     * @brief Print the state in Dirac notation.
     *
     * Output format: @c |ψ⟩ = α₀|00…0⟩ + α₁|00…1⟩ + …, with terms whose
     * amplitude is below @c 1e-6 suppressed. Bit-strings are big-endian.
     *
     * @complexity Time @c O(2^n).
     */
    void Print() const;

    /**
     * @brief Destructive measurement in the computational basis.
     * @return Big-endian bit-string of the outcome (length @c bits).
     *
     * Samples an index @c k from the Born distribution, then collapses the
     * state to @c |k⟩ (sets that amplitude to 1, all others to 0). Repeated
     * calls after collapse always return the same outcome.
     *
     * @complexity Time @c O(2^n), space @c O(1).
     */
    std::string Collapse();

    /**
     * @brief Sample a measurement outcome without altering the state.
     * @return Big-endian bit-string of the outcome (length @c bits).
     *
     * Useful for collecting measurement statistics without destroying the
     * superposition. The state vector is unchanged.
     *
     * @complexity Time @c O(2^n), space @c O(1).
     */
    std::string MeasureWithoutCollapse() const;

    /**
     * @brief Write @p n_samples Born-rule measurements to a single-column CSV.
     * @param filename Path to the output file (created or overwritten).
     * @param n_samples Number of independent samples to draw.
     *
     * The CSV has the header line @c outcome and one bit-string per row.
     * Sampling uses @c MeasureWithoutCollapse, so the register state is
     * unchanged; this method is therefore @c const.
     *
     * @complexity Time @c O(n_samples · 2^n), space @c O(1).
     */
    void WriteMeasurementsCSV(const std::string& filename, int n_samples) const;

    /**
     * @brief Serialise the register state as JSON onto @p os.
     * @param os Output stream.
     *
     * Schema:
     * @code
     * {
     *   "bits": <n>,
     *   "amplitudes": [
     *     [re, im],
     *     [re, im],
     *     ...
     *   ]
     * }
     * @endcode
     * The @c amplitudes array has @c 2^n entries indexed in big-endian
     * order: entry @c i is the amplitude of basis state @c |i⟩ (qubit 0 is
     * the most-significant bit of @c i).
     *
     * @complexity Time @c O(2^n).
     */
    void ToJson(std::ostream& os) const;

private:
    /// Rescale @c val so that @c MagnitudeSquareSum() == 1.
    void Normalise();
};
