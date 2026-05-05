#pragma once

#include <cassert>

#include "Register.h"

/**
 * @file RegisterGates.h
 * @brief Gates that act directly on a multi-qubit @c Register.
 *
 * The @c RGates hierarchy applies a unitary to a chosen subset of qubits in
 * an n-qubit register without ever materialising a per-qubit @c Qubit
 * object. Single-qubit gates use the bit-pairing pattern: for each state
 * index @c i where the target qubit bit is 0, pair it with
 * @c j = i | mask, then apply the 2×2 matrix to the
 * @c (val[i], val[j]) amplitude pair. Two- and three-qubit gates use the
 * same idea but with multiple bitmasks.
 *
 * @section indexing Indexing
 *
 * Qubit indices are big-endian (qubit 0 = MSB). The bitmask used inside
 * @c ApplyToSingle is therefore
 * @code
 *   mask = 1 << (reg.bits - 1 - qubitIndex)
 * @endcode
 * See @ref Conventions.h.
 *
 * @section arity Arity-specific entry points
 *
 * Rather than introducing a separate @c R2Gates / @c R3Gates hierarchy, the
 * single @c RGates base advertises four virtual entry points:
 *   - @c Apply(Register&) — broadcast a single-qubit gate to every qubit;
 *   - @c ApplyToSingle(Register&, int) — apply a single-qubit gate to one qubit;
 *   - @c ApplyToPair(Register&, int, int) — apply a two-qubit gate;
 *   - @c ApplyToTriple(Register&, int, int, int) — apply a three-qubit gate
 *     (currently only @c ToffoliGateR uses this).
 *
 * Each method has a default body that asserts. A concrete gate overrides
 * only the entry points whose arity it implements; calling a wrong-arity
 * method on it fails loudly rather than silently doing nothing. The single
 * unified hierarchy keeps the demos and tests free of base-class branching.
 */

/**
 * @class RGates
 * @brief Abstract base for register-level gate operations.
 *
 * Concrete derived classes override the entry points whose arity they
 * implement. Other entry points fall through to a default-asserting body
 * inherited from this base.
 */
class RGates {
public:
    virtual ~RGates() = default;

    /**
     * @brief Apply a single-qubit gate to every qubit in the register, in order.
     * @param reg Register to mutate in place.
     *
     * Default body asserts. Override in single-qubit gate classes.
     */
    virtual void Apply(Register& reg) const {
        (void)reg;
        assert(false && "Apply(Register&) not implemented for this gate");
    }

    /**
     * @brief Apply a single-qubit gate to one chosen qubit of the register.
     * @param reg Register to mutate in place.
     * @param qubitIndex Big-endian index of the target qubit
     *        (0 = leftmost / MSB, @c reg.bits - 1 = rightmost / LSB).
     *
     * Default body asserts. Override in single-qubit gate classes.
     */
    virtual void ApplyToSingle(Register& reg, int qubitIndex) const {
        (void)reg; (void)qubitIndex;
        assert(false && "ApplyToSingle not implemented for this gate");
    }

    /**
     * @brief Apply a two-qubit gate.
     * @param reg Register to mutate in place.
     * @param control Big-endian index of the control qubit.
     * @param target Big-endian index of the target qubit. Must differ from @p control.
     *
     * Default body asserts. Override in two-qubit gate classes such as
     * @c CNOTGateR, @c CZGateR, @c SWAPGateR.
     */
    virtual void ApplyToPair(Register& reg, int control, int target) const {
        (void)reg; (void)control; (void)target;
        assert(false && "ApplyToPair not implemented for this gate");
    }

    /**
     * @brief Apply a three-qubit gate.
     * @param reg Register to mutate in place.
     * @param c1 Big-endian index of the first control qubit.
     * @param c2 Big-endian index of the second control qubit.
     * @param target Big-endian index of the target qubit. All three must differ.
     *
     * Default body asserts. Override in three-qubit gate classes such as
     * @c ToffoliGateR.
     */
    virtual void ApplyToTriple(Register& reg, int c1, int c2, int target) const {
        (void)reg; (void)c1; (void)c2; (void)target;
        assert(false && "ApplyToTriple not implemented for this gate");
    }
};

/**
 * @class HadamardR
 * @brief Hadamard gate on a register qubit.
 *
 * Matrix and identities: see @c Hadamard in @ref Gates.h. Bit-pairing
 * implementation: for each (i, j = i | mask) pair where the target bit
 * is 0, set
 * @code
 *   val[i] = (a + b) / √2
 *   val[j] = (a - b) / √2
 * @endcode
 * with @c a = old val[i], @c b = old val[j].
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class HadamardR : public RGates {
public:
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
};

/**
 * @class XGateR
 * @brief Pauli-X gate on a register qubit.
 *
 * Matrix and identities: see @c PauliX in @ref Gates.h. Implementation:
 * for each i with i < (i ^ mask), swap @c val[i] and @c val[i ^ mask].
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class XGateR : public RGates {
public:
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
};

/**
 * @class IdentityR
 * @brief Identity gate on a register qubit (no-op).
 *
 * @verbatim
 * I = | 1  0 |
 *     | 0  1 |
 * @endverbatim
 *
 * Action:
 *   |0⟩ → |0⟩,
 *   |1⟩ → |1⟩,
 *   α|0⟩ + β|1⟩ → α|0⟩ + β|1⟩.
 *
 * Identity: @c I = I (trivially), and @c I·U = U·I = U for any other gate.
 *
 * Useful as a placeholder in circuit builders, in tests that need a
 * known-noop, and as the @c θ = 0 limit of every rotation gate.
 *
 * @complexity Time @c O(1), space @c O(1).
 */
class IdentityR : public RGates {
public:
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
};

/**
 * @class YGateR
 * @brief Pauli-Y gate on a register qubit.
 *
 * @verbatim
 * Y = | 0  -i |
 *     | i   0 |
 * @endverbatim
 *
 * Action:
 *   |0⟩ → i|1⟩,
 *   |1⟩ → -i|0⟩,
 *   α|0⟩ + β|1⟩ → -iβ|0⟩ + iα|1⟩.
 *
 * Identities: @c Y² = I, @c Y = iXZ, @c YX = -XY, @c YZ = -ZY.
 *
 * Bit-pairing: for each (i, j = i | mask) pair with target bit = 0,
 *   new val[i] = -i · old val[j],
 *   new val[j] =  i · old val[i].
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class YGateR : public RGates {
public:
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
};

/**
 * @class ZGateR
 * @brief Pauli-Z gate on a register qubit (phase flip).
 *
 * @verbatim
 * Z = | 1   0 |
 *     | 0  -1 |
 * @endverbatim
 *
 * Action:
 *   |0⟩ → |0⟩,
 *   |1⟩ → -|1⟩,
 *   α|0⟩ + β|1⟩ → α|0⟩ - β|1⟩.
 *
 * Identities: @c Z² = I, @c HZH = X, @c ZX = -XZ.
 *
 * Z is diagonal in the computational basis. The implementation iterates
 * the state vector once and multiplies @c val[i] by @c -1 whenever the
 * target qubit bit is set in @c i.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class ZGateR : public RGates {
public:
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
};

/**
 * @class SGateR
 * @brief S gate on a register qubit (π/2 phase / √Z).
 *
 * @verbatim
 * S = | 1  0 |
 *     | 0  i |
 * @endverbatim
 *
 * Action:
 *   |0⟩ → |0⟩,
 *   |1⟩ → i|1⟩,
 *   α|0⟩ + β|1⟩ → α|0⟩ + iβ|1⟩.
 *
 * Identities: @c S² = Z, @c S⁴ = I, @c S† = S³.
 *
 * Diagonal. The implementation multiplies @c val[i] by @c i whenever the
 * target qubit bit is set in @c i.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class SGateR : public RGates {
public:
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
};

/**
 * @class TGateR
 * @brief T gate on a register qubit (π/4 phase / ⁴√Z).
 *
 * @verbatim
 * T = | 1   0      |
 *     | 0   e^(iπ/4) |
 * @endverbatim
 *
 * Action:
 *   |0⟩ → |0⟩,
 *   |1⟩ → e^(iπ/4) |1⟩,
 *   α|0⟩ + β|1⟩ → α|0⟩ + e^(iπ/4) β|1⟩.
 *
 * Identities: @c T² = S, @c T⁴ = Z, @c T⁸ = I.
 *
 * Diagonal. The implementation multiplies @c val[i] by @c e^(iπ/4) whenever
 * the target qubit bit is set in @c i.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class TGateR : public RGates {
public:
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
};

/**
 * @class RxR
 * @brief Continuous rotation about the X-axis of the Bloch sphere by angle θ.
 *
 * @verbatim
 *               | cos(θ/2)     -i·sin(θ/2) |
 *  Rx(θ) =      |                            |
 *               | -i·sin(θ/2)   cos(θ/2)    |
 * @endverbatim
 *
 * Action:
 *   |0⟩ → cos(θ/2)|0⟩ - i·sin(θ/2)|1⟩,
 *   |1⟩ → -i·sin(θ/2)|0⟩ + cos(θ/2)|1⟩,
 *   α|0⟩ + β|1⟩ → (cα - i·s·β)|0⟩ + (-i·s·α + c·β)|1⟩,
 *     where c = cos(θ/2), s = sin(θ/2).
 *
 * Identity: @c Rx(π) = -i·X (a global phase from the Pauli-X gate).
 * Special cases: @c Rx(0) = I, @c Rx(2π) = -I.
 *
 * Bit-pairing implementation, identical in shape to Hadamard but with a
 * different 2×2 matrix.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class RxR : public RGates {
public:
    /**
     * @brief Construct an Rx gate with rotation angle θ (radians).
     * @param theta Rotation angle in radians; stored verbatim.
     */
    explicit RxR(double theta);
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
private:
    double theta_;
};

/**
 * @class RyR
 * @brief Continuous rotation about the Y-axis of the Bloch sphere by angle θ.
 *
 * @verbatim
 *               | cos(θ/2)   -sin(θ/2) |
 *  Ry(θ) =      |                       |
 *               | sin(θ/2)    cos(θ/2) |
 * @endverbatim
 *
 * Action:
 *   |0⟩ → cos(θ/2)|0⟩ + sin(θ/2)|1⟩,
 *   |1⟩ → -sin(θ/2)|0⟩ + cos(θ/2)|1⟩,
 *   α|0⟩ + β|1⟩ → (c·α - s·β)|0⟩ + (s·α + c·β)|1⟩,
 *     where c = cos(θ/2), s = sin(θ/2).
 *
 * Identity: @c Ry(π) = -i·Y.  Real-valued matrix — useful for preparing
 * arbitrary real superpositions without phase.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class RyR : public RGates {
public:
    /**
     * @brief Construct an Ry gate with rotation angle θ (radians).
     * @param theta Rotation angle in radians; stored verbatim.
     */
    explicit RyR(double theta);
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
private:
    double theta_;
};

/**
 * @class RzR
 * @brief Continuous rotation about the Z-axis of the Bloch sphere by angle θ.
 *
 * @verbatim
 *               | e^(-iθ/2)   0          |
 *  Rz(θ) =      |                         |
 *               | 0           e^(+iθ/2)  |
 * @endverbatim
 *
 * Action:
 *   |0⟩ → e^(-iθ/2)|0⟩,
 *   |1⟩ → e^(+iθ/2)|1⟩,
 *   α|0⟩ + β|1⟩ → e^(-iθ/2)·α|0⟩ + e^(+iθ/2)·β|1⟩.
 *
 * Identities: @c Rz(π) = -i·Z, @c Rz(π/2) = e^(-iπ/4) · S.  Diagonal — only
 * affects relative phase between |0⟩ and |1⟩, never measurement
 * probabilities.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class RzR : public RGates {
public:
    /**
     * @brief Construct an Rz gate with rotation angle θ (radians).
     * @param theta Rotation angle in radians; stored verbatim.
     */
    explicit RzR(double theta);
    void Apply(Register& reg) const override;
    void ApplyToSingle(Register& reg, int qubitIndex) const override;
private:
    double theta_;
};

// ===========================================================================
//                          TWO-QUBIT GATES
// ===========================================================================

/**
 * @class CNOTGateR
 * @brief Controlled-NOT gate (CNOT, CX). Two-qubit primitive of entanglement.
 *
 * @verbatim
 * Basis order: |c=0,t=0⟩, |c=0,t=1⟩, |c=1,t=0⟩, |c=1,t=1⟩.
 *
 *           | 1  0  0  0 |
 *  CNOT  =  | 0  1  0  0 |
 *           | 0  0  0  1 |
 *           | 0  0  1  0 |
 * @endverbatim
 *
 * Action on the four computational basis states:
 *   |00⟩ → |00⟩
 *   |01⟩ → |01⟩
 *   |10⟩ → |11⟩    (control=1, target flipped)
 *   |11⟩ → |10⟩    (control=1, target flipped)
 *
 * @section super Superposition control — what CNOT really does
 *
 * It is tempting to read CNOT as "if the classical control bit is 1, flip
 * the target". That picture is wrong: a quantum gate is a linear operator
 * on amplitudes, not a branch on a bit.
 *
 * Concretely, with the control qubit in superposition and the target in
 * |0⟩:
 * @code
 *   (α|0⟩ + β|1⟩) ⊗ |0⟩  =  α|00⟩ + β|10⟩
 *   ── CNOT(control, target) ──>
 *                            α|00⟩ + β|11⟩
 * @endcode
 * The output is the entangled Bell-like state α|00⟩ + β|11⟩ — neither
 * qubit by itself has a definite value, yet measuring one perfectly
 * predicts the other. Setting α = β = 1/√2 gives the canonical Bell pair
 * @c |Φ⁺⟩ = (|00⟩ + |11⟩)/√2.
 *
 * @section impl Implementation
 *
 * For each state index @c i with the control bit set and the target bit
 * clear, swap @c val[i] with @c val[i ^ targetMask]. Each pair is processed
 * exactly once.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class CNOTGateR : public RGates {
public:
    /**
     * @copydoc RGates::ApplyToPair
     *
     * Applies CNOT with the chosen control and target qubits.
     */
    void ApplyToPair(Register& reg, int control, int target) const override;
};

/**
 * @class CZGateR
 * @brief Controlled-Z gate (CZ). Phase-flips the |11⟩ component.
 *
 * @verbatim
 * Basis order: |c=0,t=0⟩, |c=0,t=1⟩, |c=1,t=0⟩, |c=1,t=1⟩.
 *
 *           | 1  0  0   0 |
 *   CZ   =  | 0  1  0   0 |
 *           | 0  0  1   0 |
 *           | 0  0  0  -1 |
 * @endverbatim
 *
 * Action on the four computational basis states:
 *   |00⟩ →  |00⟩
 *   |01⟩ →  |01⟩
 *   |10⟩ →  |10⟩
 *   |11⟩ → -|11⟩    (the only phase that changes)
 *
 * CZ is symmetric in control and target (the matrix is the same under
 * @c c ↔ t swap). The names "control" and "target" are kept here only for
 * interface parity with @c CNOTGateR.
 *
 * Useful identity: @c CNOT(c,t) = (I⊗H) · CZ(c,t) · (I⊗H), so CZ + Hadamard
 * gives an alternative way to assemble Bell states (see @c demo_ghz.cpp's
 * H+CZ+CZ variant).
 *
 * @section super Superposition control
 *
 * As with @c CNOTGateR, CZ is a linear operator; if the control is in
 * superposition the gate produces entanglement on amplitudes, not a
 * conditional branch. With the target in |+⟩ = (|0⟩+|1⟩)/√2:
 * @code
 *   (α|0⟩ + β|1⟩) ⊗ |+⟩  ── CZ ──>  α|0⟩|+⟩ + β|1⟩|−⟩,
 * @endcode
 * because CZ leaves the target in |+⟩ for c=0 and flips it to |−⟩ for c=1.
 *
 * @section impl Implementation
 *
 * For each state index @c i with both control and target bits set, multiply
 * @c val[i] by @c -1.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class CZGateR : public RGates {
public:
    /**
     * @copydoc RGates::ApplyToPair
     *
     * Applies CZ with the chosen control and target qubits. CZ is symmetric
     * in its arguments, so the labels "control" and "target" are
     * interchangeable.
     */
    void ApplyToPair(Register& reg, int control, int target) const override;
};

/**
 * @class SWAPGateR
 * @brief SWAP gate. Exchanges the contents of two qubits.
 *
 * @verbatim
 * Basis order: |q1=0,q2=0⟩, |q1=0,q2=1⟩, |q1=1,q2=0⟩, |q1=1,q2=1⟩.
 *
 *           | 1  0  0  0 |
 *  SWAP  =  | 0  0  1  0 |
 *           | 0  1  0  0 |
 *           | 0  0  0  1 |
 * @endverbatim
 *
 * Action on the four computational basis states:
 *   |00⟩ → |00⟩
 *   |01⟩ → |10⟩
 *   |10⟩ → |01⟩
 *   |11⟩ → |11⟩
 *
 * Like @c CZGateR, SWAP is symmetric in its two arguments; the parameter
 * names follow the @c CNOTGateR template only for consistency.
 *
 * @section super Superposition swap
 *
 * For arbitrary single-qubit states |ψ⟩ and |φ⟩, SWAP transports the whole
 * states across the two register slots:
 * @code
 *   |ψ⟩ ⊗ |φ⟩  ── SWAP ──>  |φ⟩ ⊗ |ψ⟩.
 * @endcode
 * Concretely, with q1 = α|0⟩+β|1⟩ and q2 = |0⟩:
 *   @c α|00⟩ + β|10⟩  →  α|00⟩ + β|01⟩.
 *
 * Identity: @c SWAP = CNOT(c,t) · CNOT(t,c) · CNOT(c,t) — three CNOTs
 * suffice on hardware that lacks a native SWAP.
 *
 * @section impl Implementation
 *
 * For each state index @c i where the two target bits differ, swap
 * @c val[i] with its partner @c val[i ^ controlMask ^ targetMask]. Each
 * pair is processed exactly once.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class SWAPGateR : public RGates {
public:
    /**
     * @copydoc RGates::ApplyToPair
     *
     * Applies SWAP with the chosen pair of qubits. SWAP is symmetric, so
     * the labels "control" and "target" are interchangeable.
     */
    void ApplyToPair(Register& reg, int control, int target) const override;
};

// ===========================================================================
//                         THREE-QUBIT GATES
// ===========================================================================

/**
 * @class ToffoliGateR
 * @brief Toffoli gate (CCNOT). Three-qubit doubly-controlled NOT.
 *
 * @verbatim
 * Basis order: |c1, c2, t⟩ ranging over the 8 combinations of three bits.
 *
 *               | 1 0 0 0 0 0 0 0 |
 *               | 0 1 0 0 0 0 0 0 |
 *               | 0 0 1 0 0 0 0 0 |
 *  Toffoli  =   | 0 0 0 1 0 0 0 0 |
 *               | 0 0 0 0 1 0 0 0 |
 *               | 0 0 0 0 0 1 0 0 |
 *               | 0 0 0 0 0 0 0 1 |
 *               | 0 0 0 0 0 0 1 0 |
 * @endverbatim
 *
 * Action: |c1, c2, t⟩ → |c1, c2, t ⊕ (c1 ∧ c2)⟩.  Equivalently:
 *   |000⟩ → |000⟩       |100⟩ → |100⟩
 *   |001⟩ → |001⟩       |101⟩ → |101⟩
 *   |010⟩ → |010⟩       |110⟩ → |111⟩
 *   |011⟩ → |011⟩       |111⟩ → |110⟩
 *
 * The Toffoli gate is universal for classical reversible computation —
 * AND, OR and NOT can all be implemented with Toffolis and ancilla qubits —
 * which is why it shows up in quantum implementations of classical
 * subroutines (e.g. modular addition inside Shor's algorithm).
 *
 * @section super Superposition controls
 *
 * Both controls may be in superposition; Toffoli is linear and acts on
 * amplitudes. If c1 and c2 are each in (|0⟩+|1⟩)/√2 and t = |0⟩:
 * @code
 *   ((|0⟩+|1⟩)/√2) ⊗ ((|0⟩+|1⟩)/√2) ⊗ |0⟩
 *     = (|000⟩ + |010⟩ + |100⟩ + |110⟩) / 2
 *   ── Toffoli ──>
 *     (|000⟩ + |010⟩ + |100⟩ + |111⟩) / 2,
 * @endcode
 * which is GHZ-like (the |111⟩ branch picks up the conditional bit-flip).
 *
 * @section impl Implementation
 *
 * For each state index @c i where both control bits are set and the target
 * bit is clear, swap @c val[i] with @c val[i ^ targetMask]. Each pair is
 * processed exactly once.
 *
 * @section choice Design choice
 *
 * @c ToffoliGateR overrides @c RGates::ApplyToTriple rather than
 * introducing a separate @c R3Gates base class. The single hierarchy keeps
 * polymorphism uniform across all gate arities; the trade-off is that
 * single- and two-qubit gates inherit a default-asserting
 * @c ApplyToTriple, which is the desired failure mode anyway.
 *
 * @complexity Time @c O(2^n), space @c O(1).
 */
class ToffoliGateR : public RGates {
public:
    /**
     * @copydoc RGates::ApplyToTriple
     *
     * Applies Toffoli (CCNOT) with the chosen control and target qubits.
     */
    void ApplyToTriple(Register& reg, int c1, int c2, int target) const override;
};
