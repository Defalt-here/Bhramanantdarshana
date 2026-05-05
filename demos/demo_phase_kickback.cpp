/**
 * @file demo_phase_kickback.cpp
 * @brief Phase kickback — the mechanism behind Deutsch-style algorithms.
 *
 * "Phase kickback" is the surprising fact that, when a controlled gate
 * acts on a target that is in an eigenstate of the gate's underlying
 * operation, the phase eigenvalue is "kicked back" onto the *control*
 * qubit even though the target's state is left unchanged.
 *
 * The textbook two-qubit example uses CNOT with the target in
 *   @c |−⟩ = (|0⟩ − |1⟩)/√2,
 * which is a −1-eigenstate of the Pauli-X gate that CNOT applies to its
 * target. Pre-kickback the control is in |+⟩; post-kickback the control
 * is in |−⟩.
 *
 * Circuit (qubit 0 = control, qubit 1 = target):
 * @code
 *   |0⟩ ── H ──────────●──────  (after CNOT, this qubit is in |−⟩)
 *                      │
 *   |0⟩ ── X ── H ─────⊕──────  (still in |−⟩)
 * @endcode
 *
 * State trace.
 *   Start         |00⟩
 *   X on q1       |01⟩
 *   H on q1       |0⟩ ⊗ (|0⟩−|1⟩)/√2          (= |0⟩|−⟩)
 *   H on q0      (|0⟩+|1⟩)/√2 ⊗ (|0⟩−|1⟩)/√2 (= |+⟩|−⟩)
 *   CNOT(q0,q1)  (|0⟩−|1⟩)/√2 ⊗ (|0⟩−|1⟩)/√2 (= |−⟩|−⟩)
 *
 * The control flipped from |+⟩ to |−⟩ — a phase change — while the
 * target's reduced density matrix is identical before and after CNOT.
 * Deutsch and Bernstein–Vazirani both work by arranging an oracle that
 * is a controlled X conditioned on f(x); the (-1)^f(x) eigenvalue is
 * kicked back onto the input register, where a final Hadamard layer
 * extracts global information about f.
 */

#include <complex>
#include <iostream>

#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

int main() {
    qsim::rng::seed(42);

    std::cout << "Phase-kickback demo on two qubits.\n"
              << "Plan: prepare control in |+⟩ and target in |−⟩, apply CNOT.\n"
              << "Outcome: control is now |−⟩, target is unchanged.\n";

    Register reg(2);
    std::cout << "\nInitial |00⟩:\n  ";
    reg.Print();

    XGateR X;
    HadamardR H;
    CNOTGateR CNOT;

    X.ApplyToSingle(reg, 1);
    std::cout << "After X on qubit 1:\n  ";
    reg.Print();

    H.ApplyToSingle(reg, 1);
    std::cout << "After H on qubit 1   (target now in |−⟩):\n  ";
    reg.Print();

    H.ApplyToSingle(reg, 0);
    std::cout << "After H on qubit 0   (control in |+⟩, joint state |+⟩|−⟩):\n  ";
    reg.Print();

    CNOT.ApplyToPair(reg, 0, 1);
    std::cout << "After CNOT(0,1)      (joint state |−⟩|−⟩ — kickback!):\n  ";
    reg.Print();

    std::cout << "\nNotice: the *target*'s amplitudes (qubit 1) describe the\n"
                 "same |−⟩ state both before and after the CNOT. The control\n"
                 "qubit is what changed — it picked up the eigenvalue\n"
                 "(-1) from acting CNOT on the |−⟩ eigenvector of X.\n"
                 "This is the phase-kickback trick that powers Deutsch's,\n"
                 "Deutsch–Jozsa's, Bernstein–Vazirani's, and Grover's oracles.\n";
    return 0;
}
