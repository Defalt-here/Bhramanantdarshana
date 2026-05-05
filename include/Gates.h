#pragma once

#include "Qubits.h"

/**
 * @file Gates.h
 * @brief Single-qubit textbook gate classes that act on @c Qubit objects.
 *
 * These classes represent the standard 2×2 unitaries in the Pauli + phase
 * gate set: I, H, X, Y, Z, S, T. Each gate's @c apply() takes a @c Qubit
 * by const reference and returns a fresh @c Qubit holding the transformed
 * state.
 *
 * For multi-qubit work (where bit-pairing on the dense state vector is
 * cheaper than constructing intermediate @c Qubit objects), use the
 * register-level @c RGates family in @ref RegisterGates.h instead.
 */

/**
 * @class QGates
 * @brief Abstract base for all single-qubit gate operations on @c Qubit.
 *
 * Concrete derived classes implement @c apply() with a specific 2×2
 * unitary. The Strategy-pattern interface lets callers compose any
 * sequence of gates without caring about the underlying matrix.
 */
class QGates {
public:
    virtual ~QGates() = default;

    /**
     * @brief Apply the gate's unitary to a qubit.
     * @param q Input state.
     * @return New state @c U|q⟩.
     */
    virtual Qubit apply(const Qubit& q) const = 0;
};

/**
 * @class Identity
 * @brief Identity gate.
 *
 * @verbatim
 * I = | 1  0 |
 *     | 0  1 |
 * @endverbatim
 *
 * Action: |0⟩ → |0⟩, |1⟩ → |1⟩, α|0⟩+β|1⟩ → α|0⟩+β|1⟩.
 * Identity: I² = I. Time/space @c O(1).
 */
class Identity : public QGates {
public:
    Qubit apply(const Qubit& q) const override;
};

/**
 * @class Hadamard
 * @brief Hadamard gate — superposition creator.
 *
 * @verbatim
 * H = (1/√2) | 1   1 |
 *            | 1  -1 |
 * @endverbatim
 *
 * Action: |0⟩ → (|0⟩+|1⟩)/√2, |1⟩ → (|0⟩−|1⟩)/√2,
 *         α|0⟩+β|1⟩ → (α+β)/√2 · |0⟩ + (α−β)/√2 · |1⟩.
 * Identity: H² = I. Time/space @c O(1).
 */
class Hadamard : public QGates {
public:
    Qubit apply(const Qubit& q) const override;
};

/**
 * @class PauliX
 * @brief Pauli-X (bit flip / NOT).
 *
 * @verbatim
 * X = | 0  1 |
 *     | 1  0 |
 * @endverbatim
 *
 * Action: |0⟩ → |1⟩, |1⟩ → |0⟩, α|0⟩+β|1⟩ → β|0⟩+α|1⟩.
 * Identity: X² = I. Time/space @c O(1).
 */
class PauliX : public QGates {
public:
    Qubit apply(const Qubit& q) const override;
};

/**
 * @class PauliY
 * @brief Pauli-Y (bit + phase flip).
 *
 * @verbatim
 * Y = | 0  -i |
 *     | i   0 |
 * @endverbatim
 *
 * Action: |0⟩ → i|1⟩, |1⟩ → -i|0⟩, α|0⟩+β|1⟩ → -iβ|0⟩ + iα|1⟩.
 * Identity: Y² = I. Time/space @c O(1).
 */
class PauliY : public QGates {
public:
    Qubit apply(const Qubit& q) const override;
};

/**
 * @class PauliZ
 * @brief Pauli-Z (phase flip).
 *
 * @verbatim
 * Z = | 1   0 |
 *     | 0  -1 |
 * @endverbatim
 *
 * Action: |0⟩ → |0⟩, |1⟩ → -|1⟩, α|0⟩+β|1⟩ → α|0⟩ - β|1⟩.
 * Identity: Z² = I. Time/space @c O(1).
 */
class PauliZ : public QGates {
public:
    Qubit apply(const Qubit& q) const override;
};

/**
 * @class SGate
 * @brief S gate (π/2 phase / √Z).
 *
 * @verbatim
 * S = | 1  0 |
 *     | 0  i |
 * @endverbatim
 *
 * Action: |0⟩ → |0⟩, |1⟩ → i|1⟩, α|0⟩+β|1⟩ → α|0⟩ + iβ|1⟩.
 * Identities: S² = Z, S⁴ = I. Time/space @c O(1).
 */
class SGate : public QGates {
public:
    Qubit apply(const Qubit& q) const override;
};

/**
 * @class TGate
 * @brief T gate (π/4 phase / ⁴√Z).
 *
 * @verbatim
 * T = | 1   0      |
 *     | 0   e^(iπ/4) |
 * @endverbatim
 *
 * Action: |0⟩ → |0⟩, |1⟩ → e^(iπ/4) |1⟩.
 * Identities: T² = S, T⁴ = Z, T⁸ = I. Time/space @c O(1).
 */
class TGate : public QGates {
public:
    Qubit apply(const Qubit& q) const override;
};
