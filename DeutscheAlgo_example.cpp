/**
 * @file DeutscheAlgo_example.cpp
 * @brief Implementation of Deutsch's Algorithm - First quantum algorithm to show quantum advantage
 * 
 * This file demonstrates Deutsch's algorithm, which was historically significant as the first
 * quantum algorithm to provide an exponential speedup over classical computation for a specific problem.
 * 
 * Problem Statement:
 * Given a black-box function f: {0,1} → {0,1}, determine if f is:
 * - Constant: f(0) = f(1) (both outputs are the same)
 * - Balanced: f(0) ≠ f(1) (outputs are different)
 * 
 * Classical Solution: Requires 2 function evaluations (worst case)
 * Quantum Solution: Requires only 1 function evaluation using quantum parallelism
 * 
 * Algorithm Overview:
 * 1. Initialize qubits in superposition using Hadamard gates
 * 2. Apply quantum oracle that encodes the function f
 * 3. Use quantum interference to extract global information
 * 4. Measure to determine function type with 100% certainty
 * 
 * Quantum Advantage:
 * - Classical: O(2) function calls needed
 * - Quantum: O(1) function call needed
 * - Demonstrates quantum parallelism and interference
 * 
 * Historical Significance:
 * - First quantum algorithm (David Deutsch, 1985)
 * - Proof of concept for quantum computational advantage
 * - Foundation for Deutsch-Jozsa and other quantum algorithms
 * 
 * @author Your Name
 * @date 2025
 */

#include <iostream>
#include <complex>
#include <cmath>
#include <vector>

#include "RegisterGates_cl.cpp"

#define COMPLEX std::complex

/**
 * @enum OracleType
 * @brief Enumeration of possible function types for Deutsch's algorithm
 * 
 * Represents the four possible Boolean functions f: {0,1} → {0,1}:
 * 
 * Truth Tables:
 * Constant0: f(0)=0, f(1)=0  (always returns 0)
 * Constant1: f(0)=1, f(1)=1  (always returns 1)
 * Identity:  f(0)=0, f(1)=1  (returns input unchanged)
 * Not:       f(0)=1, f(1)=0  (returns negated input)
 * 
 * Classification:
 * - Constant functions: Constant0, Constant1 (same output for all inputs)
 * - Balanced functions: Identity, Not (different outputs for different inputs)
 * 
 * The goal of Deutsch's algorithm is to distinguish between constant
 * and balanced functions using only one quantum function evaluation.
 */
enum class OracleType{
    Constant0,  ///< f(x) = 0 for all x (constant function)
    Constant1,  ///< f(x) = 1 for all x (constant function)
    Identity,   ///< f(x) = x (balanced function)
    Not         ///< f(x) = ¬x (balanced function)
};

/**
 * @class DeutschOracle
 * @brief Quantum oracle implementation for Deutsch's algorithm
 * 
 * A quantum oracle is a black-box unitary operation that encodes a classical function
 * into a quantum circuit. For Deutsch's algorithm, we need an oracle that implements:
 * 
 * U_f |x⟩|y⟩ = |x⟩|y ⊕ f(x)⟩
 * 
 * where:
 * - x is the input qubit (0 or 1)
 * - y is the ancilla qubit (initially |1⟩ for phase kickback)
 * - f(x) is the function we want to evaluate
 * - ⊕ is XOR (addition modulo 2)
 * 
 * Phase Kickback Mechanism:
 * When y = 1, the transformation becomes:
 * U_f |x⟩|1⟩ = |x⟩|1 ⊕ f(x)⟩ = (-1)^f(x) |x⟩|1⟩
 * 
 * This encodes f(x) as a phase factor, allowing quantum interference
 * to reveal global properties of the function.
 * 
 * Implementation Details:
 * - Uses 2-qubit register: |x⟩|y⟩
 * - Applies controlled operations based on function type
 * - Preserves quantum superposition throughout
 * - Enables parallel evaluation of f(0) and f(1)
 */
class DeutschOracle{
    OracleType type;  ///< Type of function this oracle implements
    
public:
    /**
     * @brief Constructor for Deutsch oracle
     * @param t Type of Boolean function to implement
     */
    DeutschOracle(OracleType t) : type(t){}
    
    /**
     * @brief Apply the quantum oracle to the register
     * @param reg 2-qubit quantum register in state |x⟩|y⟩
     * 
     * Implements the unitary transformation U_f |x⟩|y⟩ = |x⟩|y ⊕ f(x)⟩
     * 
     * Algorithm:
     * 1. Iterate through all possible computational basis states
     * 2. For each state |xy⟩, extract x (input) and y (ancilla)
     * 3. Compute f(x) based on the oracle type
     * 4. Apply XOR: new_y = y ⊕ f(x)
     * 5. Swap amplitudes to implement the transformation
     * 
     * State Evolution:
     * - Input: superposition over all |x⟩|y⟩ states
     * - Output: superposition where each amplitude is transformed according to f
     * - Preserves quantum coherence for interference effects
     * 
     * Bit Encoding (for 2-qubit register):
     * - State index i: bit representation |x⟩|y⟩
     * - x = (i >> 1) & 1: extract input qubit (MSB)
     * - y = i & 1: extract ancilla qubit (LSB)
     * - j = (x << 1) | new_y: construct new state index
     */
    void Apply(Register &reg) const{
        int size = reg.val.size();  // Total number of basis states (2^2 = 4)

        // Process each computational basis state
        for(int i = 0; i < size; ++i){
            // Extract qubit values from state index
            int x = (i >> 1) & 1;       // Input qubit: 0 or 1
            int y = i & 1;              // Ancilla qubit: 0 or 1
            
            // Evaluate function f(x) based on oracle type
            int fx = 0;
            switch (type) {
                case OracleType::Constant0: fx = 0; break;      // f(x) = 0
                case OracleType::Constant1: fx = 1; break;      // f(x) = 1
                case OracleType::Identity:  fx = x; break;      // f(x) = x
                case OracleType::Not:       fx = !x; break;     // f(x) = ¬x
            }
            
            // Apply oracle transformation: y → y ⊕ f(x)
            int newY = y ^ fx;                    // XOR operation
            int j = (x << 1) | newY;              // Construct new state index
            
            // Swap amplitudes to implement the unitary transformation
            // This effectively maps |i⟩ → |j⟩ in the computational basis
            if (i != j) {  // Only swap if states are different
                std::swap(reg.val[i], reg.val[j]);
            }
        }
    }
};
/**
 * @brief Execute Deutsch's Algorithm to determine if a function is constant or balanced
 * @param isConstant Expected result (true if function should be constant, false if balanced)
 * 
 * This function implements the complete Deutsch algorithm, demonstrating quantum
 * computational advantage through quantum parallelism and interference.
 * 
 * Algorithm Steps:
 * 1. State Preparation: Initialize |01⟩ (input=0, ancilla=1)
 * 2. Superposition Creation: Apply H⊗H to create equal superposition
 * 3. Oracle Query: Apply U_f to evaluate function on superposition
 * 4. Interference: Apply H to first qubit to create interference
 * 5. Measurement: Measure first qubit to determine function type
 * 
 * Quantum Circuit:
 * |0⟩ ──[H]── ●── [H] ──■ (measure)
 *              │           
 * |1⟩ ──[H]── ⊕ ─────────
 *           U_f
 * 
 * Mathematical Analysis:
 * After step 2: (|00⟩ - |01⟩ + |10⟩ - |11⟩)/2
 * After step 3: Function-dependent superposition with encoded f(0), f(1)
 * After step 4: Interference creates different final states based on function type
 * 
 * Result Interpretation:
 * - Constant function: First qubit measures to |0⟩ with probability 1
 * - Balanced function: First qubit measures to |1⟩ with probability 1
 * 
 * Quantum Advantage:
 * - Classical: Must evaluate f(0) AND f(1) to determine type
 * - Quantum: Single oracle query determines type with certainty
 * - Speedup: 2× reduction in function evaluations
 * 
 * @note This implementation uses a fixed oracle type for demonstration.
 *       In practice, the oracle type would be unknown (black box).
 */
void RunDeutschAlgorithm(bool isConstant) {
    std::cout << "\n=== Deutsch's Algorithm Execution ===" << std::endl;
    std::cout << "Testing " << (isConstant ? "CONSTANT" : "BALANCED") << " function" << std::endl;
    
    // Step 1: Initialize quantum register in computational basis state |01⟩
    // Qubit 0 (input): |0⟩
    // Qubit 1 (ancilla): |1⟩ (required for phase kickback mechanism)
    Register reg(2, {
        {"01", std::complex<double>(1.0, 0.0)}
    });
    std::cout << "\nStep 1 - Initial state |01⟩:" << std::endl;
    reg.Print();

    // Step 2: Apply Hadamard gates to both qubits
    // Creates superposition: (|00⟩ - |01⟩ + |10⟩ - |11⟩)/2
    // This enables quantum parallelism - both f(0) and f(1) will be evaluated simultaneously
    HadamardR H;
    H.ApplyToSingle(reg, 0);  // H on input qubit
    H.ApplyToSingle(reg, 1);  // H on ancilla qubit
    std::cout << "\nStep 2 - After applying H⊗H (superposition created):" << std::endl;
    reg.Print();

    // Step 3: Apply the quantum oracle U_f
    // For demonstration, using Constant0 - in practice this would be unknown
    DeutschOracle oracle(isConstant ? OracleType::Constant0 : OracleType::Identity);
    oracle.Apply(reg);
    std::cout << "\nStep 3 - After oracle application:" << std::endl;
    reg.Print();

    // Step 4: Apply Hadamard to the first qubit only
    // This creates interference between the |0⟩ and |1⟩ components
    // The interference pattern depends on whether f is constant or balanced
    H.ApplyToSingle(reg, 0);
    std::cout << "\nStep 4 - After final Hadamard on input qubit:" << std::endl;
    reg.Print();

    // Step 5: Measure the first qubit to determine function type
    // Constant function: Always measures 0
    // Balanced function: Always measures 1
    std::string measurement = reg.MeasureWithoutCollapse();
    int result = measurement[0] - '0';  // Convert char to int
    
    std::cout << "\nStep 5 - Measurement Results:" << std::endl;
    std::cout << "Full measurement: " << measurement << std::endl;
    std::cout << "First qubit: " << result << std::endl;

    // Interpret the results
    std::cout << "\n=== Algorithm Analysis ===" << std::endl;
    std::cout << "Function is " << (isConstant ? "constant" : "balanced") << std::endl;
    std::cout << "Measured first qubit: " << result << std::endl;
    std::cout << "Expected result: " << (isConstant ? "0" : "1") << std::endl;

    // Verify correctness
    bool correct = (isConstant && result == 0) || (!isConstant && result == 1);
    if (correct) {
        std::cout << "✅ Algorithm SUCCESS: Correctly identified function type!" << std::endl;
    } else {
        std::cout << "❌ Algorithm FAILURE: Incorrect function identification!" << std::endl;
    }
    
    // Theoretical explanation
    std::cout << "\n=== Quantum Advantage Demonstrated ===" << std::endl;
    std::cout << "• Classical approach: 2 function evaluations needed (worst case)" << std::endl;
    std::cout << "• Quantum approach: 1 function evaluation with 100% certainty" << std::endl;
    std::cout << "• Speedup factor: 2× reduction in queries" << std::endl;
    std::cout << "• Key principle: Quantum parallelism + interference" << std::endl;
}

/**
 * @brief Demonstrate all four possible Boolean functions with Deutsch's algorithm
 * 
 * This function tests all four possible Boolean functions that can exist
 * for a single-bit input, showing how the algorithm correctly identifies
 * constant vs. balanced functions in each case.
 * 
 * Educational Value:
 * - Shows algorithm works for all possible functions
 * - Demonstrates the deterministic nature of quantum measurement
 * - Illustrates the theoretical foundations of quantum advantage
 */
void DemonstrateAllFunctions() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "COMPREHENSIVE DEUTSCH ALGORITHM DEMONSTRATION" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Test all four possible Boolean functions
    struct FunctionTest {
        OracleType type;
        std::string name;
        bool isConstant;
        std::string description;
    };
    
    std::vector<FunctionTest> functions = {
        {OracleType::Constant0, "Constant0", true, "f(x) = 0 for all x"},
        {OracleType::Constant1, "Constant1", true, "f(x) = 1 for all x"},
        {OracleType::Identity, "Identity", false, "f(x) = x"},
        {OracleType::Not, "NOT", false, "f(x) = ¬x"}
    };
    
    for (const auto& func : functions) {
        std::cout << "\n" << std::string(40, '-') << std::endl;
        std::cout << "Testing Function: " << func.name << std::endl;
        std::cout << "Description: " << func.description << std::endl;
        std::cout << "Type: " << (func.isConstant ? "CONSTANT" : "BALANCED") << std::endl;
        std::cout << std::string(40, '-') << std::endl;
        
        // Run algorithm with specific oracle
        Register reg(2, {{"01", std::complex<double>(1.0, 0.0)}});
        
        // Apply algorithm steps
        HadamardR H;
        H.ApplyToSingle(reg, 0);
        H.ApplyToSingle(reg, 1);
        
        DeutschOracle oracle(func.type);
        oracle.Apply(reg);
        
        H.ApplyToSingle(reg, 0);
        
        // Measure and analyze
        std::string measurement = reg.MeasureWithoutCollapse();
        int result = measurement[0] - '0';
        
        std::cout << "Measurement result: " << result << std::endl;
        std::cout << "Expected for " << (func.isConstant ? "constant" : "balanced") 
                  << " function: " << (func.isConstant ? "0" : "1") << std::endl;
        
        bool correct = (func.isConstant && result == 0) || (!func.isConstant && result == 1);
        std::cout << (correct ? "✅ CORRECT" : "❌ INCORRECT") << " identification!" << std::endl;
    }
}

/**
 * @brief Display theoretical background and significance of Deutsch's algorithm
 * 
 * Provides educational context about the algorithm's importance in
 * quantum computing history and its theoretical implications.
 */
void DisplayTheoreticalBackground() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "THEORETICAL BACKGROUND: DEUTSCH'S ALGORITHM" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::cout << "\n📚 HISTORICAL SIGNIFICANCE:" << std::endl;
    std::cout << "• First quantum algorithm (David Deutsch, 1985)" << std::endl;
    std::cout << "• First proof of quantum computational advantage" << std::endl;
    std::cout << "• Foundation for Deutsch-Jozsa algorithm" << std::endl;
    std::cout << "• Inspired development of Shor's and Grover's algorithms" << std::endl;
    
    std::cout << "\n🎯 PROBLEM DEFINITION:" << std::endl;
    std::cout << "• Given: Black-box function f: {0,1} → {0,1}" << std::endl;
    std::cout << "• Goal: Determine if f is constant or balanced" << std::endl;
    std::cout << "• Constant: f(0) = f(1) (same output for all inputs)" << std::endl;
    std::cout << "• Balanced: f(0) ≠ f(1) (different outputs)" << std::endl;
    
    std::cout << "\n⚡ QUANTUM ADVANTAGE:" << std::endl;
    std::cout << "• Classical complexity: O(2) function evaluations (worst case)" << std::endl;
    std::cout << "• Quantum complexity: O(1) function evaluation" << std::endl;
    std::cout << "• Speedup: 2× reduction in queries" << std::endl;
    std::cout << "• Success probability: 100% (deterministic)" << std::endl;
    
    std::cout << "\n🔬 KEY QUANTUM CONCEPTS:" << std::endl;
    std::cout << "• Quantum Parallelism: Evaluate f(0) and f(1) simultaneously" << std::endl;
    std::cout << "• Quantum Interference: Extract global function properties" << std::endl;
    std::cout << "• Phase Kickback: Encode function output as quantum phase" << std::endl;
    std::cout << "• Superposition: Enable parallel computation paths" << std::endl;
    
    std::cout << "\n🏗️ ALGORITHM STRUCTURE:" << std::endl;
    std::cout << "1. Prepare initial state |01⟩" << std::endl;
    std::cout << "2. Create superposition with Hadamard gates" << std::endl;
    std::cout << "3. Apply quantum oracle U_f" << std::endl;
    std::cout << "4. Interfere amplitudes with final Hadamard" << std::endl;
    std::cout << "5. Measure to extract function property" << std::endl;
    
    std::cout << "\n🌐 BROADER IMPLICATIONS:" << std::endl;
    std::cout << "• Proof that quantum computers can outperform classical ones" << std::endl;
    std::cout << "• Foundation for query complexity theory" << std::endl;
    std::cout << "• Inspiration for modern quantum algorithms" << std::endl;
    std::cout << "• Bridge between theoretical and practical quantum computing" << std::endl;
}
