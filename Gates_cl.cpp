/**
 * @file Gates_cl.cpp
 * @brief Implementation of fundamental single-qubit quantum gates
 * 
 * This file implements the most important single-qubit quantum gates used in
 * quantum computing and quantum information processing. Each gate is implemented
 * as a unitary transformation that preserves the normalization of quantum states.
 * 
 * Quantum Gates Implemented:
 * - Identity: Does nothing (useful for timing and benchmarking)
 * - Hadamard: Creates superposition states (basis for many quantum algorithms)
 * - Pauli Gates (X, Y, Z): Fundamental rotations around Bloch sphere axes
 * - S Gate: Quarter turn around Z-axis (phase gate)
 * - T Gate: Eighth turn around Z-axis (π/8 gate)
 * 
 * Mathematical Foundation:
 * Each gate corresponds to a 2×2 unitary matrix acting on the qubit state:
 * |ψ'⟩ = U|ψ⟩ where U†U = I (unitary condition)
 * 
 * Bloch Sphere Representation:
 * - X gates: Rotations around X-axis
 * - Y gates: Rotations around Y-axis  
 * - Z gates: Rotations around Z-axis
 * - Hadamard: Rotation around (X+Z)/√2 axis
 * 
 * @author Your Name
 * @date 2025
 */

#include <iostream>
#include <complex>
#include <cmath>
#ifndef PI
#define PI 3.14159265358979323846  ///< Mathematical constant π for phase calculations
#endif
#include "Qubits_cl.cpp"
#include <fstream>
#include <vector>
/**
 * @class QGates
 * @brief Abstract base class for all single-qubit quantum gate operations
 * 
 * This class defines the interface for quantum gate operations on individual qubits.
 * All concrete quantum gates must inherit from this class and implement the apply() method.
 * 
 * Design Pattern: Strategy Pattern
 * - Allows different gate operations to be used interchangeably
 * - Enables composition of quantum circuits
 * - Facilitates gate optimization and transformation
 * 
 * Mathematical Interface:
 * Each gate represents a unitary transformation U: ℂ² → ℂ²
 * Input: |ψ⟩ = α|0⟩ + β|1⟩
 * Output: |ψ'⟩ = U|ψ⟩
 */
class QGates
{
    /**
     * @brief Apply the quantum gate to a qubit
     * @param q Input qubit state to transform
     * @return New qubit state after gate application
     * 
     * Pure virtual function that must be implemented by all concrete gate classes.
     * Represents the fundamental quantum gate operation |ψ'⟩ = U|ψ⟩.
     */
    virtual Qubit apply(const Qubit &q) const = 0;
};
/**
 * @class Identity
 * @brief Identity quantum gate (I gate)
 * 
 * The identity gate leaves the qubit state unchanged. While seemingly trivial,
 * it serves important purposes in quantum computing:
 * - Placeholder in quantum circuits for timing alignment
 * - Benchmarking and performance testing
 * - Circuit optimization and gate scheduling
 * - Representing "no operation" in conditional quantum circuits
 * 
 * Matrix Representation:
 * I = |1 0|
 *     |0 1|
 * 
 * Transformations:
 * |0⟩ → |0⟩
 * |1⟩ → |1⟩
 * α|0⟩ + β|1⟩ → α|0⟩ + β|1⟩
 * 
 * Physical Interpretation:
 * - No rotation on the Bloch sphere
 * - Preserves all quantum properties (amplitude, phase, entanglement)
 * - Represents the "wait" operation in quantum hardware
 */
class Identity : public QGates
{
public:
    /**
     * @brief Apply identity transformation to qubit
     * @param q Input qubit state
     * @return Identical qubit state (unchanged)
     * 
     * Implementation simply copies the input amplitudes without modification.
     * This preserves both the computational basis amplitudes and any relative phases.
     */
    Qubit apply(const Qubit &q) const override
    {
        return Qubit(q.val[0], q.val[1]);  // Return identical state: I|ψ⟩ = |ψ⟩
    }
};
/**
 * @class Hadamard
 * @brief Hadamard quantum gate (H gate) - The superposition creator
 * 
 * The Hadamard gate is one of the most important gates in quantum computing.
 * It creates equal superposition states and is essential for quantum parallelism.
 * 
 * Matrix Representation:
 * H = (1/√2) * |1  1 |
 *              |1 -1|
 * 
 * Transformations:
 * |0⟩ → (|0⟩ + |1⟩)/√2  (creates equal superposition)
 * |1⟩ → (|0⟩ - |1⟩)/√2  (creates equal superposition with phase)
 * 
 * Key Properties:
 * - Self-inverse: H² = I (applying twice returns to original state)
 * - Creates maximum superposition from basis states
 * - Essential for quantum algorithms (Deutsch, Grover, Shor, etc.)
 * - Changes computational basis to Hadamard basis: {|+⟩, |-⟩}
 * 
 * Physical Interpretation:
 * - Rotates qubit by π around the (X+Z)/√2 axis on Bloch sphere
 * - Converts Z-basis measurement to X-basis measurement
 * - Fundamental building block for quantum Fourier transform
 * 
 * Applications:
 * - Quantum algorithm initialization
 * - Creating Bell states and GHZ states
 * - Quantum random number generation
 * - Quantum key distribution protocols
 */
class Hadamard : public QGates
{
public:
    /**
     * @brief Apply Hadamard transformation to qubit
     * @param q Input qubit state |ψ⟩ = α|0⟩ + β|1⟩
     * @return Transformed qubit state H|ψ⟩ = (α+β)|0⟩/√2 + (α-β)|1⟩/√2
     * 
     * Algorithm:
     * 1. Extract current amplitudes α (for |0⟩) and β (for |1⟩)
     * 2. Apply Hadamard matrix multiplication:
     *    new_α = (α + β)/√2
     *    new_β = (α - β)/√2
     * 3. Return new qubit with transformed amplitudes
     * 
     * Mathematical derivation:
     * H|ψ⟩ = H(α|0⟩ + β|1⟩) = α·H|0⟩ + β·H|1⟩
     *      = α·(|0⟩+|1⟩)/√2 + β·(|0⟩-|1⟩)/√2
     *      = (α+β)|0⟩/√2 + (α-β)|1⟩/√2
     */
    Qubit apply(const Qubit &q) const override
    {
        const double invSqrt2 = 1.0 / std::sqrt(2.0);  // Normalization factor 1/√2
        
        // Extract current amplitudes
        std::complex<double> a = q.ampliA;              // Amplitude for |0⟩ state
        std::complex<double> b = q.ampliB;              // Amplitude for |1⟩ state
        
        // Apply Hadamard transformation matrix
        std::complex<double> new0 = invSqrt2 * (a + b); // New amplitude for |0⟩
        std::complex<double> new1 = invSqrt2 * (a - b); // New amplitude for |1⟩
        
        return Qubit(new0, new1);
    }
};
/**
 * @class PauliX
 * @brief Pauli-X quantum gate (X gate, NOT gate, bit-flip gate)
 * 
 * The Pauli-X gate is the quantum analog of the classical NOT gate.
 * It flips the computational basis states of a qubit.
 * 
 * Matrix Representation:
 * X = |0 1|
 *     |1 0|
 * 
 * Transformations:
 * |0⟩ → |1⟩  (bit flip)
 * |1⟩ → |0⟩  (bit flip)
 * α|0⟩ + β|1⟩ → β|0⟩ + α|1⟩  (amplitude swap)
 * 
 * Key Properties:
 * - Self-inverse: X² = I
 * - Part of Pauli group: {I, X, Y, Z}
 * - Anticommutes with Z: XZ = -ZX
 * - Commutes with Y: XY = iZ
 * 
 * Physical Interpretation:
 * - Rotation by π around X-axis on Bloch sphere
 * - Flips qubit between north and south poles
 * - Equivalent to classical bit flip for computational basis states
 * - Preserves superposition structure while swapping amplitudes
 * 
 * Applications:
 * - Quantum error correction (bit-flip errors)
 * - State preparation and initialization
 * - Implementing classical logic in quantum circuits
 * - Building controlled gates (CNOT when controlled)
 */
class PauliX : public QGates
{
public:
    /**
     * @brief Apply Pauli-X (NOT) transformation to qubit
     * @param q Input qubit state |ψ⟩ = α|0⟩ + β|1⟩
     * @return Bit-flipped qubit state X|ψ⟩ = β|0⟩ + α|1⟩
     * 
     * Implementation simply swaps the amplitudes for |0⟩ and |1⟩ states.
     * This preserves the quantum superposition while performing the bit flip.
     * 
     * Mathematical derivation:
     * X|ψ⟩ = X(α|0⟩ + β|1⟩) = α·X|0⟩ + β·X|1⟩
     *      = α|1⟩ + β|0⟩ = β|0⟩ + α|1⟩
     */
    Qubit apply(const Qubit &q) const override
    {
        // Swap amplitudes: |0⟩ amplitude becomes |1⟩ amplitude and vice versa
        return Qubit(q.ampliB, q.ampliA);  // X|ψ⟩: α|0⟩ + β|1⟩ → β|0⟩ + α|1⟩
    }
};
/**
 * @class PauliY
 * @brief Pauli-Y quantum gate (Y gate, combined bit-flip and phase-flip)
 * 
 * The Pauli-Y gate combines both bit-flip and phase-flip operations.
 * It's equivalent to applying X followed by Z (or Z followed by X with a sign).
 * 
 * Matrix Representation:
 * Y = |0 -i|
 *     |i  0|
 * 
 * Transformations:
 * |0⟩ → i|1⟩    (bit flip with +i phase)
 * |1⟩ → -i|0⟩   (bit flip with -i phase)
 * α|0⟩ + β|1⟩ → -iβ|0⟩ + iα|1⟩
 * 
 * Key Properties:
 * - Self-inverse: Y² = I
 * - Part of Pauli group: {I, X, Y, Z}
 * - Anticommutes with both X and Z: YX = -XY, YZ = -ZY
 * - Relationship: Y = iXZ = -iZX
 * 
 * Physical Interpretation:
 * - Rotation by π around Y-axis on Bloch sphere
 * - Combines amplitude swap (like X) with complex phase factors
 * - Maps |+⟩ → |-⟩ and |-⟩ → |+⟩ (Hadamard basis flip)
 * - Creates pure imaginary amplitudes from real inputs
 * 
 * Applications:
 * - Quantum error correction (combined bit and phase flip errors)
 * - Implementing arbitrary single-qubit rotations
 * - Building universal gate sets with X and Z
 * - Creating specific phase relationships in quantum algorithms
 */
class PauliY : public QGates
{
public:
    /**
     * @brief Apply Pauli-Y transformation to qubit
     * @param q Input qubit state |ψ⟩ = α|0⟩ + β|1⟩
     * @return Transformed qubit state Y|ψ⟩ = -iβ|0⟩ + iα|1⟩
     * 
     * The transformation swaps amplitudes (like X gate) but also applies
     * complex phase factors: +i to the new |1⟩ amplitude and -i to the new |0⟩ amplitude.
     * 
     * Mathematical derivation:
     * Y|ψ⟩ = Y(α|0⟩ + β|1⟩) = α·Y|0⟩ + β·Y|1⟩
     *      = α·(i|1⟩) + β·(-i|0⟩) = -iβ|0⟩ + iα|1⟩
     */
    Qubit apply(const Qubit &q) const override
    {
        std::complex<double> i(0, 1);  // Complex number i = √(-1)
        
        // Apply Y gate transformation: swap amplitudes and apply phase factors
        return Qubit(-i * q.ampliB,    // New |0⟩ amplitude: -i * β
                     i * q.ampliA);    // New |1⟩ amplitude: +i * α
    }
};
/**
 * @class PauliZ
 * @brief Pauli-Z quantum gate (Z gate, phase-flip gate)
 * 
 * The Pauli-Z gate applies a phase flip to the |1⟩ state while leaving |0⟩ unchanged.
 * It's the "phase-flip" analog of the X gate's "bit-flip" operation.
 * 
 * Matrix Representation:
 * Z = |1  0|
 *     |0 -1|
 * 
 * Transformations:
 * |0⟩ → |0⟩     (unchanged)
 * |1⟩ → -|1⟩    (phase flip)
 * α|0⟩ + β|1⟩ → α|0⟩ - β|1⟩
 * 
 * Key Properties:
 * - Self-inverse: Z² = I
 * - Part of Pauli group: {I, X, Y, Z}
 * - Anticommutes with X and Y: ZX = -XZ, ZY = -YZ
 * - Diagonal matrix (preserves computational basis states)
 * 
 * Physical Interpretation:
 * - Rotation by π around Z-axis on Bloch sphere
 * - Flips between |+⟩ and |-⟩ states (Hadamard basis)
 * - Preserves probabilities: |α|² and |β|² unchanged
 * - Only affects relative phase between |0⟩ and |1⟩ components
 * 
 * Applications:
 * - Quantum error correction (phase-flip errors)
 * - Implementing phase differences in quantum algorithms
 * - Building conditional phase gates
 * - Quantum Fourier transform components
 * - Creating interference effects in quantum circuits
 */
class PauliZ : public QGates
{
public:
    /**
     * @brief Apply Pauli-Z (phase-flip) transformation to qubit
     * @param q Input qubit state |ψ⟩ = α|0⟩ + β|1⟩
     * @return Phase-flipped qubit state Z|ψ⟩ = α|0⟩ - β|1⟩
     * 
     * The Z gate leaves the |0⟩ amplitude unchanged but multiplies the |1⟩ amplitude by -1.
     * This creates a relative phase of π between the two computational basis components.
     * 
     * Mathematical derivation:
     * Z|ψ⟩ = Z(α|0⟩ + β|1⟩) = α·Z|0⟩ + β·Z|1⟩
     *      = α|0⟩ + β·(-|1⟩) = α|0⟩ - β|1⟩
     */
    Qubit apply(const Qubit &q) const override
    {
        // Keep |0⟩ amplitude unchanged, negate |1⟩ amplitude
        return Qubit(q.ampliA,      // |0⟩ amplitude: α → α
                    -q.ampliB);     // |1⟩ amplitude: β → -β
    }
};
/**
 * @class SGate
 * @brief S quantum gate (Phase gate, P gate, √Z gate)
 * 
 * The S gate applies a quarter turn (π/2) phase rotation around the Z-axis.
 * It's the square root of the Z gate: S² = Z.
 * 
 * Matrix Representation:
 * S = |1 0|
 *     |0 i|
 * 
 * Transformations:
 * |0⟩ → |0⟩     (unchanged)
 * |1⟩ → i|1⟩    (adds π/2 phase)
 * α|0⟩ + β|1⟩ → α|0⟩ + iβ|1⟩
 * 
 * Key Properties:
 * - S² = Z (square root of Z gate)
 * - S⁴ = I (returns to identity after 4 applications)
 * - Commutes with Z: SZ = ZS
 * - Dagger: S† = S³ (inverse requires 3 more applications)
 * 
 * Physical Interpretation:
 * - Rotation by π/2 around Z-axis on Bloch sphere
 * - Converts |+⟩ to (|0⟩ + i|1⟩)/√2 (adds phase to superposition)
 * - Preserves computational basis probabilities
 * - Creates 90° phase relationship between basis states
 * 
 * Applications:
 * - Building arbitrary single-qubit rotations
 * - Quantum Fourier transform implementation
 * - Phase manipulation in quantum algorithms
 * - Constructing controlled phase gates
 * - Creating specific interference patterns
 */
class SGate : public QGates
{
public:
    /**
     * @brief Apply S gate (quarter Z-rotation) to qubit
     * @param q Input qubit state |ψ⟩ = α|0⟩ + β|1⟩
     * @return Phase-rotated qubit state S|ψ⟩ = α|0⟩ + iβ|1⟩
     * 
     * The S gate leaves the |0⟩ amplitude unchanged but multiplies the |1⟩ amplitude by i,
     * adding a π/2 phase shift to the |1⟩ component.
     * 
     * Mathematical derivation:
     * S|ψ⟩ = S(α|0⟩ + β|1⟩) = α·S|0⟩ + β·S|1⟩
     *      = α|0⟩ + β·(i|1⟩) = α|0⟩ + iβ|1⟩
     */
    Qubit apply(const Qubit &q) const override
    {
        std::complex<double> i(0, 1);  // Complex number i = √(-1)
        
        // Keep |0⟩ amplitude unchanged, multiply |1⟩ amplitude by i
        return Qubit(q.ampliA,          // |0⟩ amplitude: α → α
                    i * q.ampliB);      // |1⟩ amplitude: β → iβ
    }
};

/**
 * @class TGate
 * @brief T quantum gate (π/8 gate, T gate, eighth-turn gate)
 * 
 * The T gate applies an eighth turn (π/4) phase rotation around the Z-axis.
 * It's the square root of the S gate: T² = S, and T⁴ = Z.
 * 
 * Matrix Representation:
 * T = |1  0    |
 *     |0  e^(iπ/4)|
 * 
 * where e^(iπ/4) = cos(π/4) + i·sin(π/4) = (1+i)/√2
 * 
 * Transformations:
 * |0⟩ → |0⟩                    (unchanged)
 * |1⟩ → e^(iπ/4)|1⟩            (adds π/4 phase)
 * α|0⟩ + β|1⟩ → α|0⟩ + βe^(iπ/4)|1⟩
 * 
 * Key Properties:
 * - T² = S (square root of S gate)
 * - T⁴ = Z (fourth root of Z gate)
 * - T⁸ = I (returns to identity after 8 applications)
 * - Universal: {H, T} forms a universal gate set (with measurement)
 * - Fault-tolerant: Can be implemented with magic state distillation
 * 
 * Physical Interpretation:
 * - Rotation by π/4 around Z-axis on Bloch sphere
 * - Provides fine-grained phase control
 * - Creates 45° phase relationship between basis states
 * - Essential for arbitrary single-qubit rotations
 * 
 * Applications:
 * - Universal quantum computation (with Hadamard)
 * - Fault-tolerant quantum computing protocols
 * - Quantum Fourier transform high-precision implementations
 * - Shor's algorithm factoring components
 * - Building arbitrary phase rotations via Solovay-Kitaev theorem
 */
class TGate : public QGates
{
public:
    /**
     * @brief Apply T gate (π/8 rotation) to qubit
     * @param q Input qubit state |ψ⟩ = α|0⟩ + β|1⟩
     * @return Phase-rotated qubit state T|ψ⟩ = α|0⟩ + βe^(iπ/4)|1⟩
     * 
     * The T gate leaves the |0⟩ amplitude unchanged but multiplies the |1⟩ amplitude
     * by e^(iπ/4) = cos(π/4) + i·sin(π/4), adding a π/4 phase shift.
     * 
     * Mathematical derivation:
     * T|ψ⟩ = T(α|0⟩ + β|1⟩) = α·T|0⟩ + β·T|1⟩
     *      = α|0⟩ + β·(e^(iπ/4)|1⟩) = α|0⟩ + βe^(iπ/4)|1⟩
     * 
     * Phase calculation:
     * e^(iπ/4) = cos(π/4) + i·sin(π/4) = √2/2 + i·√2/2 = (1+i)/√2
     */
    Qubit apply(const Qubit &q) const override
    {
        // Calculate phase factor e^(iπ/4) = cos(π/4) + i·sin(π/4)
        std::complex<double> phase(std::cos(PI / 4), std::sin(PI / 4));
        
        // Keep |0⟩ amplitude unchanged, multiply |1⟩ amplitude by e^(iπ/4)
        return Qubit(q.ampliA,          // |0⟩ amplitude: α → α
                    phase * q.ampliB);  // |1⟩ amplitude: β → βe^(iπ/4)
    }
};
