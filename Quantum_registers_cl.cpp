/**
 * @file Quantum_registers_cl.cpp
 * @brief Implementation of quantum register for multi-qubit quantum state manipulation
 * 
 * This file implements a quantum register class that represents and manipulates
 * quantum states using the state vector formalism. The implementation supports:
 * - Arbitrary quantum state initialization
 * - Quantum state normalization and probability calculations
 * - Quantum measurement with and without state collapse
 * - Inner product calculations for quantum states
 * 
 * Mathematical Foundation:
 * A quantum register with n qubits exists in a 2^n-dimensional Hilbert space.
 * Each state |ψ⟩ is represented as: |ψ⟩ = Σᵢ αᵢ|i⟩
 * where αᵢ are complex probability amplitudes and Σᵢ|αᵢ|² = 1
 * 
 * State Vector Representation:
 * - Index i corresponds to binary representation of basis state |i⟩
 * - For 3 qubits: |000⟩→index 0, |001⟩→index 1, |010⟩→index 2, etc.
 * - Amplitude val[i] represents the coefficient of basis state |i⟩
 * 
 * @author Your Name
 * @date 2025
 */

#include <iostream>
#include <complex>
#include <vector>
#include <cassert>
#include <map>
#include <fstream>
#include <random>
#include <string>
#include <bitset>

#define COMPLEX std::complex
/**
 * @class Register
 * @brief Quantum register implementation for multi-qubit quantum state representation
 * 
 * This class implements a quantum register using the state vector formalism,
 * where quantum states are represented as vectors in a complex Hilbert space.
 * The class supports quantum state manipulation, measurement, and analysis.
 * 
 * Key Features:
 * - State vector representation with complex amplitudes
 * - Automatic normalization of quantum states
 * - Probabilistic quantum measurement simulation
 * - Quantum state analysis and visualization
 * - Support for custom initial state preparation
 * 
 * Memory Layout:
 * For n qubits, val[i] stores the amplitude of basis state |binary(i)⟩
 * Example for 2 qubits: val[0]=|00⟩, val[1]=|01⟩, val[2]=|10⟩, val[3]=|11⟩
 */
class Register
{
public:
    int bits;                                    ///< Number of qubits in the register
    std::vector<std::complex<double>> val;       ///< State vector storing probability amplitudes
    /**
     * @brief Default constructor creating quantum register in |00...0⟩ state
     * @param n Number of qubits in the register
     * 
     * Initializes a quantum register with n qubits in the computational basis
     * state |00...0⟩. This is the most common initial state for quantum algorithms.
     * 
     * State Created: |ψ⟩ = |00...0⟩ (all qubits in |0⟩ state)
     * Probability Amplitude: val[0] = 1+0i, val[i>0] = 0+0i
     * 
     * @complexity Time: O(2^n), Space: O(2^n)
     */
    Register(int n) : bits(n)
    {
        int resizeFactor = 1 << n;                          // Calculate 2^n states
        val.resize(resizeFactor, COMPLEX<double>(0.0f, 0.0f)); // Initialize all to zero
        val[0] = COMPLEX(1.0f, 0.0f);                       // Set |00...0⟩ amplitude to 1
        Normalise();                                        // Ensure normalization
    }
    /**
     * @brief Constructor with custom initial state specification
     * @param n Number of qubits in the register
     * @param initStates Map from binary strings to complex amplitudes
     * 
     * Creates a quantum register with arbitrary initial superposition state.
     * Allows specification of custom probability amplitudes for each basis state.
     * 
     * @param initStates Key-value pairs where:
     *                   - Key: Binary string representation of basis state (e.g., "101")
     *                   - Value: Complex probability amplitude for that state
     * 
     * Example Usage:
     * std::map<std::string, std::complex<double>> states = {
     *     {"00", {0.6, 0.0}},    // |00⟩ with amplitude 0.6
     *     {"11", {0.8, 0.0}}     // |11⟩ with amplitude 0.8
     * };
     * Register reg(2, states);    // Creates superposition 0.6|00⟩ + 0.8|11⟩
     * 
     * @note The state is automatically normalized after initialization
     * @complexity Time: O(2^n + m), Space: O(2^n) where m = initStates.size()
     */
    Register(int n, const std::map<std::string, std::complex<double>> &initStates) : bits(n)
    {
        int size = 1 << n;                                  // Calculate 2^n states
        val.resize(size, COMPLEX<double>(0.0, 0.0));        // Initialize all amplitudes to zero

        // Set amplitudes for specified basis states
        for (const auto &[bitstring, amplitude] : initStates)
        {
            // Validate input format
            assert(static_cast<int>(bitstring.length()) == n && "Bitstring length must match register size");
            
            // Convert binary string to state index
            int index = std::stoi(bitstring, nullptr, 2);    // Binary to integer conversion
            assert(index < size && "Index out of bounds for register size");
            
            val[index] = amplitude;                          // Set amplitude for this basis state
        }

        Normalise();                                         // Normalize the quantum state
    }

    /**
     * @brief Calculate sum of squared magnitudes of all amplitudes
     * @return Sum of |αᵢ|² for all amplitudes αᵢ
     * 
     * Computes the normalization factor for the quantum state.
     * For a properly normalized quantum state, this should equal 1.0.
     * 
     * Mathematical Formula: Σᵢ |αᵢ|² where αᵢ = val[i]
     * 
     * Used for:
     * - State normalization verification
     * - Probability conservation checking
     * - Quantum state validation
     * 
     * @complexity Time: O(2^n), Space: O(1)
     */
    double MagnitudeSquareSum() const
    {
        double result = 0.0;
        for (const auto &i : val)
        {
            result += std::norm(i);                         // |α|² = α*α* (squared magnitude)
        }
        return result;
    }
    /**
     * @brief Get measurement probability for a specific basis state
     * @param i Index of the basis state (0 to 2^n - 1)
     * @return Probability of measuring the quantum system in state |i⟩
     * 
     * Calculates the Born rule probability P(i) = |αᵢ|² for measuring
     * the quantum register in computational basis state |i⟩.
     * 
     * The index i corresponds to the binary representation:
     * - i=0 → |00...0⟩, i=1 → |00...1⟩, i=2 → |00...10⟩, etc.
     * 
     * Example: For 2-qubit register
     * - GetProbab(0) returns P(|00⟩)
     * - GetProbab(3) returns P(|11⟩)
     * 
     * @complexity Time: O(1), Space: O(1)
     */
    double GetProbab(int i)
    {
        return std::norm(val[i]);                           // Born rule: P(i) = |αᵢ|²
    }
    /**
     * @brief Calculate inner product between two quantum states
     * @param other Another quantum register to compute inner product with
     * @return Complex number representing ⟨ψ|φ⟩ where |φ⟩ is other state
     * 
     * Computes the quantum mechanical inner product between two quantum states.
     * This is fundamental for:
     * - Measuring quantum state overlap
     * - Computing transition amplitudes
     * - Calculating fidelity between states
     * - Implementing quantum algorithms
     * 
     * Mathematical Formula: ⟨ψ|φ⟩ = Σᵢ ψᵢ* φᵢ
     * where ψᵢ* is the complex conjugate of amplitude ψᵢ
     * 
     * Properties:
     * - ⟨ψ|ψ⟩ = ||ψ||² (normalization)
     * - ⟨ψ|φ⟩ = ⟨φ|ψ⟩* (conjugate symmetry)
     * - |⟨ψ|φ⟩|² = probability of measuring |ψ⟩ when starting from |φ⟩
     * 
     * @complexity Time: O(2^n), Space: O(1)
     */
    COMPLEX<double> FindInnerProduct(Register other) const
    {
        assert(val.size() == other.val.size());
        COMPLEX<double> IP = COMPLEX<double>(0.0, 0.0);
        for (size_t i = 0; i < val.size(); i++)
        {
            IP += std::conj(val[i]) * other.val[i];         // ⟨ψ|φ⟩ = Σᵢ ψᵢ* φᵢ
        }
        return IP;
    }
    /**
     * @brief Print quantum state in Dirac notation
     * 
     * Displays the quantum state in standard quantum mechanical notation:
     * |ψ⟩ = α₀|00⟩ + α₁|01⟩ + α₂|10⟩ + α₃|11⟩ + ...
     * 
     * Features:
     * - Displays only non-zero amplitude terms (threshold: 1e-6)
     * - Shows complex amplitudes in (real, imaginary) format
     * - Uses binary representation for basis states
     * - Proper mathematical formatting with + signs
     * 
     * Example Output:
     * |ψ⟩ = (0.707,0)|00⟩ + (0.707,0)|11⟩
     * 
     * This represents the Bell state (|00⟩ + |11⟩)/√2
     * 
     * @complexity Time: O(2^n), Space: O(1)
     */
    void Print() const
    {
        std::cout << "|ψ⟩ = ";
        int n = std::log2(val.size());                      // Number of qubits
        bool first = true;

        // Iterate through all basis states
        for (size_t i = 0; i < val.size(); ++i)
        {
            // Skip terms with negligible amplitude
            if (std::abs(val[i]) < 1e-6)
                continue;

            // Add + sign between terms (except first term)
            if (!first)
                std::cout << " + ";
            first = false;

            // Print amplitude in (real, imaginary) format
            std::cout << "" << val[i] << "|";

            // Print basis state in binary representation
            for (int j = n - 1; j >= 0; --j)
                std::cout << ((i >> j) & 1);                // Extract j-th bit of i
            
            std::cout << "⟩";                               // Close ket notation
        }

        std::cout << std::endl;
    }
    /**
     * @brief Perform quantum measurement with state collapse
     * @return Binary string representation of measured outcome
     * 
     * Simulates quantum measurement according to Born's rule, causing
     * the quantum state to collapse to a definite computational basis state.
     * 
     * Algorithm:
     * 1. Calculate cumulative probability distribution from |αᵢ|²
     * 2. Generate random number r ∈ [0,1)
     * 3. Find first index i where cumulative_prob[i] > r
     * 4. Collapse state: set val[i] = 1, val[j≠i] = 0
     * 5. Return binary representation of measured state i
     * 
     * Physical Interpretation:
     * - Models the irreversible process of quantum measurement
     * - Probability P(outcome = i) = |αᵢ|² (Born rule)
     * - After measurement, system is in definite state |i⟩
     * - Subsequent measurements will always yield the same result
     * 
     * Example: Bell state (|00⟩ + |11⟩)/√2
     * - 50% chance of measuring "00", 50% chance of measuring "11"
     * - After measurement, state becomes either |00⟩ or |11⟩
     * 
     * @note This function modifies the quantum state (destructive measurement)
     * @complexity Time: O(2^n), Space: O(2^n) for cumulative array
     */
    std::string Collapse()
    {
        EnsureRandomSeeded();
        
        // Build cumulative probability distribution
        std::vector<double> cumulative;
        double total = 0.0;
        for (const auto &amp : val)
        {
            total += std::norm(amp);                        // Add |αᵢ|² to cumulative sum
            cumulative.push_back(total);
        }

        // Generate random number for measurement outcome
        double r = static_cast<double>(rand()) / RAND_MAX;

        // Find measured state using cumulative distribution
        size_t collapsedIndex = 0;
        for (; collapsedIndex < cumulative.size(); ++collapsedIndex)
        {
            if (r < cumulative[collapsedIndex])
                break;
        }

        // Collapse the state vector to measured outcome
        for (size_t i = 0; i < val.size(); ++i)
        {
            val[i] = (i == collapsedIndex) ? COMPLEX(1.0, 0.0) : COMPLEX(0.0, 0.0);
        }

        // Convert measured state index to binary string
        std::string result;
        for (int j = bits - 1; j >= 0; --j)
        {
            result += std::to_string((collapsedIndex >> j) & 1);
        }
        return result;
    }
    /**
     * @brief Perform quantum measurement without state collapse
     * @return Binary string representation of measurement outcome
     * 
     * Simulates a "weak measurement" or sampling from the quantum state
     * without destroying the superposition. The quantum state remains
     * unchanged after this operation.
     * 
     * Use Cases:
     * - Statistical analysis of quantum states
     * - Multiple sampling from same quantum distribution
     * - Testing measurement probabilities without state destruction
     * - Quantum state tomography simulations
     * 
     * Difference from Collapse():
     * - Same probabilistic sampling algorithm
     * - State vector remains unchanged (non-destructive)
     * - Can be called multiple times on same state
     * - Useful for statistical analysis
     * 
     * Algorithm:
     * 1. Build cumulative probability distribution from |αᵢ|²
     * 2. Sample random outcome according to Born rule
     * 3. Return measurement result as binary string
     * 4. Preserve original quantum state
     * 
     * Example Usage:
     * Register reg(2);  // |00⟩ state
     * // Apply gates to create superposition...
     * for(int i = 0; i < 1000; i++) {
     *     std::string outcome = reg.MeasureWithoutCollapse();
     *     // Collect statistics...
     * }
     * // reg still contains original superposition state
     * 
     * @note This function does NOT modify the quantum state
     * @complexity Time: O(2^n), Space: O(2^n) for cumulative array
     */
    std::string MeasureWithoutCollapse()
    {
        EnsureRandomSeeded();
        
        // Build cumulative probability distribution (same as Collapse)
        std::vector<double> cumulative;
        double total = 0.0;
        for (const auto &amp : val)
        {
            total += std::norm(amp);                        // Add |αᵢ|² to cumulative sum
            cumulative.push_back(total);
        }

        // Generate random number for measurement outcome
        double r = static_cast<double>(rand()) / RAND_MAX;

        // Find measured state using cumulative distribution
        size_t measuredIndex = 0;
        for (; measuredIndex < cumulative.size(); ++measuredIndex)
        {
            if (r < cumulative[measuredIndex])
                break;
        }

        // Convert measured state index to binary string
        // Note: We do NOT modify val[] - state remains unchanged
        std::string result;
        for (int j = bits - 1; j >= 0; --j)
        {
            result += std::to_string((measuredIndex >> j) & 1);
        }
        return result;
    }

private:
    /**
     * @brief Normalize the quantum state to ensure probability conservation
     * @return The original magnitude squared sum before normalization
     * 
     * Ensures that the quantum state satisfies the normalization condition:
     * Σᵢ |αᵢ|² = 1
     * 
     * This is crucial for:
     * - Maintaining probability conservation in quantum mechanics
     * - Ensuring valid quantum states after gate operations
     * - Preventing numerical errors from accumulating
     * 
     * Algorithm:
     * 1. Calculate total probability: S = Σᵢ |αᵢ|²
     * 2. Divide each amplitude by √S: αᵢ → αᵢ/√S
     * 3. After normalization: Σᵢ |αᵢ/√S|² = S/S = 1
     * 
     * Mathematical Justification:
     * The Born rule requires Σᵢ P(i) = Σᵢ |αᵢ|² = 1 for a valid quantum state.
     * Normalization preserves relative phases and amplitudes while ensuring
     * the total probability equals 1.
     * 
     * @complexity Time: O(2^n), Space: O(1)
     */
    double Normalise()
    {
        double magnitudeSquareSum = MagnitudeSquareSum();
        for (auto &i : val)
        {
            i /= sqrt(magnitudeSquareSum);                  // Normalize: αᵢ → αᵢ/√Σⱼ|αⱼ|²
        }
        return magnitudeSquareSum;
    }

    /**
     * @brief Ensure random number generator is properly seeded
     * 
     * Uses a static variable to ensure the random number generator is
     * seeded exactly once during program execution. This prevents
     * predictable random sequences that could occur if srand() is
     * called multiple times with the same seed.
     * 
     * Implementation Details:
     * - Static lambda ensures single execution
     * - Seeds with current time for unpredictability
     * - Thread-safe initialization (C++11 guarantees)
     * 
     * Note: For cryptographic applications or high-quality randomness,
     * consider using std::random_device and std::mt19937 instead of rand().
     */
    void EnsureRandomSeeded()
    {
        static bool seeded = []
        {
            std::srand(static_cast<unsigned>(time(0)));     // Seed with current time
            return true;
        }();
    }
};

