/**
 * @file RegisterGates_cl.cpp
 * @brief Implementation of quantum gates for multi-qubit quantum registers
 * 
 * This file implements quantum gate operations on quantum registers, including:
 * - Hadamard gates for creating superposition states
 * - Pauli-X gates for qubit flipping operations
 * 
 * The implementation uses a state vector representation where a quantum register
 * with n qubits is represented by a vector of 2^n complex amplitudes.
 * 
 * Mathematical Foundation:
 * - Each quantum state |ψ⟩ = Σ αᵢ|i⟩ where αᵢ are complex amplitudes
 * - Gate operations are unitary transformations preserving probability conservation
 * - Multi-qubit gates operate on tensor product spaces
 * 
 * @author Your Name
 * @date 2025
 */

#include <iostream>
#include <complex>
#include <cmath>
#include <vector>

#include "Quantum_registers_cl.cpp"

#define COMPLEX std::complex
/**
 * @class RGates
 * @brief Abstract base class for quantum gate operations on registers
 * 
 * This class defines the interface for all quantum gate operations that can be
 * applied to multi-qubit quantum registers. It provides two types of operations:
 * 1. Apply to all qubits in the register
 * 2. Apply to a specific single qubit
 * 
 * Design Pattern: Template Method Pattern
 * - Concrete gates implement specific gate operations
 * - Common interface ensures consistency across different gate types
 */
class RGates{
public:
    /**
     * @brief Apply the quantum gate to all qubits in the register
     * @param reg Reference to the quantum register to operate on
     * 
     * This method applies the gate operation to every qubit in the register.
     * For example, applying Hadamard to all qubits creates maximum superposition.
     */
    virtual void Apply(Register& reg) const = 0;
    
    /**
     * @brief Apply the quantum gate to a specific qubit
     * @param reg Reference to the quantum register to operate on
     * @param qubitIndex Zero-based index of the target qubit (0 = leftmost qubit)
     * 
     * This method applies the gate operation to only the specified qubit,
     * leaving other qubits unchanged. Essential for controlled operations.
     */
    virtual void ApplyToSingle(Register& reg, int qubitIndex) const = 0;
    
    /**
     * @brief Virtual destructor for proper cleanup of derived classes
     */
    virtual ~RGates() = default;
};
/**
 * @class HadamardR
 * @brief Implementation of the Hadamard quantum gate for registers
 * 
 * The Hadamard gate is fundamental in quantum computing, creating superposition
 * by transforming basis states according to the matrix:
 * H = (1/√2) * |1  1 |
 *              |1 -1|
 * 
 * Transformations:
 * |0⟩ → (|0⟩ + |1⟩)/√2  (creates equal superposition)
 * |1⟩ → (|0⟩ - |1⟩)/√2  (creates equal superposition with phase)
 * 
 * Physical Meaning:
 * - Rotates qubit state by π around X+Z axis on Bloch sphere
 * - Essential for quantum algorithms (Deutsch, Grover, etc.)
 * - Creates quantum parallelism when applied to multiple qubits
 */
class HadamardR : public RGates{
public:
    /**
     * @brief Apply Hadamard gate to a specific qubit in the register
     * @param reg Reference to the quantum register
     * @param qubitIndex Index of the target qubit (0-based, 0 = rightmost)
     * 
     * Algorithm:
     * 1. Create bitmask to identify target qubit position
     * 2. For each pair of states that differ only in target qubit:
     *    - Extract amplitudes a (qubit=0) and b (qubit=1)
     *    - Apply transformation: new_a = (a+b)/√2, new_b = (a-b)/√2
     * 3. Update register with transformed amplitudes
     * 
     * Time Complexity: O(2^n) where n is number of qubits
     * Space Complexity: O(1) additional space
     */
    void ApplyToSingle(Register& reg, int qubitIndex) const override{
        int size = reg.val.size();           // Total number of quantum states (2^n)
        int mask = 1 << qubitIndex;          // Bitmask for target qubit position

        // Iterate through all quantum states
        for(int i = 0; i < size; ++i){
            // Process only states where target qubit is 0 (to avoid double processing)
            if(!(i&mask)){
                int j = i | mask;            // Corresponding state with target qubit = 1

                // Extract current amplitudes for the state pair
                COMPLEX<double> a = reg.val[i];  // Amplitude when target qubit = 0
                COMPLEX<double> b = reg.val[j];  // Amplitude when target qubit = 1
                
                // Hadamard transformation matrix normalization factor
                const double invSqrt2 = 1.0f/std::sqrt(2.0f);
                
                // Apply Hadamard transformation
                reg.val[i] = (a+b)*invSqrt2;     // New amplitude for |...0...⟩
                reg.val[j] = (a-b)*invSqrt2;     // New amplitude for |...1...⟩
            }
        }
    }
    
    /**
     * @brief Apply Hadamard gate to all qubits in the register
     * @param reg Reference to the quantum register
     * 
     * Creates maximum superposition state where all 2^n basis states have
     * equal probability amplitudes. For n qubits, each state gets amplitude 1/√(2^n).
     * 
     * Example: 2-qubit register |00⟩ becomes (|00⟩ + |01⟩ + |10⟩ + |11⟩)/2
     */
    void Apply(Register& reg) const override {
        int numQubits = reg.bits;            // Get number of qubits in register
        
        // Apply Hadamard to each qubit sequentially
        for(int i = 0; i < numQubits; ++i) {
            ApplyToSingle(reg, i);           // Transform qubit i
        }
    }
};
/**
 * @class XGateR
 * @brief Implementation of the Pauli-X (NOT) quantum gate for registers
 * 
 * The Pauli-X gate is the quantum equivalent of the classical NOT gate.
 * It flips the state of a qubit according to the matrix:
 * X = |0 1|
 *     |1 0|
 * 
 * Transformations:
 * |0⟩ → |1⟩  (bit flip)
 * |1⟩ → |0⟩  (bit flip)
 * 
 * Physical Meaning:
 * - Rotates qubit state by π around X-axis on Bloch sphere
 * - Implements quantum bit flip operation
 * - Fundamental building block for quantum circuits
 * - Part of Pauli group {I, X, Y, Z}
 */
class XGateR : public RGates {
    public:
        /**
         * @brief Apply Pauli-X gate to a specific qubit in the register
         * @param reg Reference to the quantum register
         * @param qubitIndex Index of the target qubit (0-based)
         * 
         * Algorithm:
         * 1. Create bitmask to identify target qubit position
         * 2. For each quantum state, find its bit-flipped partner
         * 3. Swap amplitudes between paired states
         * 4. Process each pair only once to avoid double-swapping
         * 
         * Example: For qubit 0 in 2-qubit system:
         * |00⟩ ↔ |01⟩, |10⟩ ↔ |11⟩
         * 
         * Time Complexity: O(2^n) where n is number of qubits
         * Space Complexity: O(1) additional space
         */
        void ApplyToSingle(Register& reg, int qubitIndex) const override {
            int size = reg.val.size();           // Total number of quantum states
            int mask = 1 << qubitIndex;          // Bitmask for target qubit
            
            // Iterate through all quantum states
            for(int i = 0; i < size; ++i){
                int j = i ^ mask;                // XOR to flip target qubit bit
                
                // Swap amplitudes only once per pair (avoid double-swapping)
                if(i < j){
                    std::swap(reg.val[i], reg.val[j]);
                }
            }
        }
        
        /**
         * @brief Apply Pauli-X gate to all qubits in the register
         * @param reg Reference to the quantum register
         * 
         * Flips all qubits in the register simultaneously.
         * Example: |000⟩ → |111⟩, |101⟩ → |010⟩
         * 
         * Note: Applying X to all qubits is equivalent to applying
         * the tensor product X⊗X⊗...⊗X
         */
        void Apply(Register& reg) const override {
            int numQubits = reg.bits;            // Get number of qubits
            
            // Apply X gate to each qubit sequentially
            for(int i = 0; i < numQubits; ++i){
                ApplyToSingle(reg, i);           // Flip qubit i
            }
        }
};

/**
 * @brief Main function demonstrating quantum gate operations
 * 
 * This example shows:
 * 1. Creating a quantum register with custom initial amplitudes
 * 2. Applying Hadamard gates to create superposition
 * 3. Applying Pauli-X gates for qubit flipping
 * 4. Observing quantum state evolution
 * 
 * Educational Value:
 * - Demonstrates quantum superposition principles
 * - Shows how gates transform probability amplitudes
 * - Illustrates single-qubit vs. multi-qubit operations
 */
// int main() {
//     using COMPLEXD = std::complex<double>;

//     // Initialize a 2-qubit register with custom amplitudes
//     // Note: These amplitudes should be normalized for physical validity
//     std::map<std::string, COMPLEXD> amps = {
//         {"00", {0.5, 0.0}},   // |00⟩ state with amplitude 0.5+0i
//         {"01", {0.5, 0.0}},   // |01⟩ state with amplitude 0.5+0i  
//         {"10", {0.5, 0.0}},   // |10⟩ state with amplitude 0.5+0i
//         {"11", {0.5, 0.0}}    // |11⟩ state with amplitude 0.5+0i
//     };

//     // Create quantum register with specified initial state
//     Register reg(2, amps);
//     std::cout << "Initial quantum state:" << std::endl;
//     reg.Print();

//     // Apply Hadamard gate to all qubits
//     // This creates maximum superposition: each basis state gets equal amplitude
//     HadamardR hadamard;
//     hadamard.Apply(reg);
//     std::cout << "\nAfter applying Hadamard gate to all qubits:" << std::endl;
//     reg.Print();

//     // Apply X gate to first qubit only (qubit index 0)
//     // This flips the first qubit while preserving superposition structure
//     XGateR xgate;
//     xgate.ApplyToSingle(reg, 0);
//     std::cout << "\nAfter applying X gate to qubit 0:" << std::endl;
//     reg.Print();

//     // Demonstrate measurement (optional - uncomment to see probabilistic results)
//     /*
//     std::cout << "\nMeasurement results (10 samples):" << std::endl;
//     for(int i = 0; i < 10; i++) {
//         Register temp_reg = reg;  // Copy for non-destructive measurement
//         std::string result = temp_reg.Collapse();
//         std::cout << "Measurement " << i+1 << ": " << result << std::endl;
//     }
//     */

//     return 0;
// }
