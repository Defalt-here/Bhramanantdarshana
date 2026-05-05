#pragma once

#include <complex>

/**
 * @class Qubit
 * @brief Single-qubit pure state |ψ⟩ = α|0⟩ + β|1⟩.
 *
 * A qubit is the smallest unit of quantum information. Unlike a classical
 * bit, it can exist in a continuous superposition of |0⟩ and |1⟩, with the
 * weights given by complex amplitudes α (for |0⟩) and β (for |1⟩) subject
 * to the normalisation condition |α|² + |β|² = 1.
 *
 * @section storage Storage layout
 * - @c val[0] holds α, the amplitude of |0⟩.
 * - @c val[1] holds β, the amplitude of |1⟩.
 * - @c ampliA and @c ampliB are kept in sync with @c val[0] and @c val[1] as
 *   convenience aliases used by the gate classes in @ref Gates.h.
 *
 * @section measurement Measurement
 * Per Born's rule, measuring in the computational basis yields outcome 0
 * with probability |α|² and outcome 1 with probability |β|². Two flavours
 * are provided:
 * - @c Collapse() — destructive: forces the state into the measured outcome.
 * - @c MeasureWithoutCollapse() — non-destructive sampling.
 *
 * @section relation_to_register Relation to @ref Register
 * @c Qubit is a self-contained single-qubit type used by the textbook
 * @c QGates classes. For multi-qubit work, use the @c Register class plus
 * the @c RGates family — those operate on the full state vector via
 * bit-pairing and never materialise individual @c Qubit objects.
 */
class Qubit {
public:
    /// Amplitude of the |0⟩ component (kept in sync with @c val[0]).
    std::complex<double> ampliA;
    /// Amplitude of the |1⟩ component (kept in sync with @c val[1]).
    std::complex<double> ampliB;
    /// Raw amplitude pair: @c val[0] = α, @c val[1] = β.
    std::complex<double> val[2];

    /**
     * @brief Build a qubit from raw amplitudes; auto-normalises.
     * @param a Amplitude of |0⟩ (need not be normalised).
     * @param b Amplitude of |1⟩ (need not be normalised).
     *
     * The constructor scales @c a and @c b so that |a|² + |b|² = 1, then
     * stores the result. If both inputs are zero, behaviour is undefined
     * (a non-state).
     */
    Qubit(std::complex<double> a, std::complex<double> b);

    /**
     * @brief Print the state in Dirac notation.
     *
     * Output format: @c |ψ⟩ = α|0⟩ + β|1⟩, with zero terms suppressed.
     */
    void Print() const;

    /**
     * @brief Sum of squared magnitudes of the amplitudes.
     * @return |α|² + |β|², which equals 1 for a properly normalised qubit.
     */
    double MagnitudeSquareSum() const;

    /// Born-rule probability of measuring 0.
    double GetProbabilityOf0() const;
    /// Born-rule probability of measuring 1.
    double GetProbabilityOf1() const;

    /**
     * @brief Sample a measurement outcome without altering the state.
     * @return 0 or 1, drawn from the Born distribution.
     */
    int MeasureWithoutCollapse() const;

    /**
     * @brief Destructive measurement: collapses the state to the outcome.
     * @return @c false if the outcome is 0, @c true if the outcome is 1.
     *
     * After this call, the qubit is in either |0⟩ or |1⟩ depending on the
     * outcome. Subsequent measurements are deterministic.
     */
    bool Collapse();

    /**
     * @brief Quantum inner product ⟨ψ|φ⟩ between this qubit and @p other.
     * @param other The other qubit to compare against.
     * @return ⟨ψ|φ⟩ = α*·α' + β*·β', where the asterisk denotes complex
     *         conjugation.
     */
    std::complex<double> FindInnerProduct(const Qubit& other) const;

private:
    /// Rescale @c val[] so that the state is unit-norm.
    void Normalise();

    /// Helper used by @c Print() to render a single complex number compactly.
    void printComplex(const std::complex<double>& c) const;
};
