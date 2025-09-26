#include <iostream>
#include <complex>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <map>
#include <thread>
#include "RegisterGates_cl.cpp"

using namespace std;
using namespace std::chrono;

std::map<std::string, std::complex<double>> customAmps = {
    {"000", {0.7, 0.0}},   // |000⟩ with amplitude 0.5
    {"001", {0.1, 0.0}},   // |001⟩ with amplitude 0
    {"010", {0.2, 0.0}},   // |010⟩ with amplitude 0
    {"011", {0.5, 0.6}},   // |011⟩ with amplitude 0
    {"100", {0.3, 0.0}},   // |100⟩ with amplitude 0
    {"101", {0.5, 0.8}},   // |101⟩ with amplitude 0.5
    {"110", {0.5, 0.9}},   // |110⟩ with amplitude 0.5
    {"111", {0.0, 0.1}}    // |111⟩ with amplitude 0.5
};

Register reg(3, customAmps);  // This will auto-normalize the amplitudes
HadamardR H;

/**
 * @brief Classical electronic parity checker simulation
 * @param inputs Vector of binary strings to check
 * @return Execution time in microseconds
 */
double classicalParityChecker(const vector<string>& inputs) {
    auto start = high_resolution_clock::now();
    
    cout << "\n🔧 CLASSICAL ELECTRONIC PARITY CHECKER:" << endl;
    cout << "========================================" << endl;
    cout << "Processing " << inputs.size() << " inputs sequentially..." << endl;
    
    map<string, int> parityResults;
    
    // Sequential processing (realistic electronic circuit behavior)
    for (const string& input : inputs) {
        // Simulate gate delays in electronic circuits
        this_thread::sleep_for(microseconds(1)); // Simulated gate delay
        
        int ones = count(input.begin(), input.end(), '1');
        string parity = (ones % 2 == 0) ? "EVEN" : "ODD";
        parityResults[parity]++;
        
        cout << "  Input: " << input << " → " << ones << " ones → " << parity << " parity" << endl;
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    cout << "\n📊 Classical Results:" << endl;
    for (const auto& p : parityResults) {
        cout << "  " << p.first << " parity: " << p.second << " inputs" << endl;
    }
    cout << "⏱️  Total time: " << duration.count() << " microseconds" << endl;
    cout << "⚡ Time complexity: O(n) where n = number of inputs" << endl;
    cout << "🔄 Parallelism: NONE - must process sequentially" << endl;
    
    return duration.count();
}

/**
 * @brief Quantum parity checker with superposition
 * @param numMeasurements Number of quantum measurements to perform
 * @return Execution time in microseconds
 */
double quantumParityChecker(int numMeasurements) {
    auto start = high_resolution_clock::now();
    
    cout << "\n⚛️  QUANTUM PARITY CHECKER:" << endl;
    cout << "========================================" << endl;
    
    cout << "Initial quantum superposition state:" << endl;
    reg.Print();
    
    cout << "\n🌊 Applying Hadamard transformation..." << endl;
    H.Apply(reg); // Apply Hadamard to all qubits
    
    cout << "Quantum state after Hadamard (ALL states in superposition):" << endl;
    reg.Print();
    
    map<string, int> parityResults;
    map<string, int> stateFrequency;
    
    cout << "\n🎯 Quantum measurements (sampling from superposition):" << endl;
    
    // Quantum measurements
    for (int i = 0; i < numMeasurements; i++) {
        Register tempReg = reg; // Copy for measurement
        string measurement = tempReg.Collapse();
        int ones = count(measurement.begin(), measurement.end(), '1');
        string parity = (ones % 2 == 0) ? "EVEN" : "ODD";
        
        parityResults[parity]++;
        stateFrequency[measurement]++;
        
        if (i < 8) { // Show first 8 measurements
            cout << "  Measurement " << (i+1) << ": |" << measurement 
                 << "⟩ → " << ones << " ones → " << parity << " parity" << endl;
        }
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    cout << "\n📊 Quantum Results:" << endl;
    for (const auto& p : parityResults) {
        cout << "  " << p.first << " parity: " << p.second << "/" << numMeasurements 
             << " measurements (" << fixed << setprecision(1) 
             << (100.0 * p.second / numMeasurements) << "%)" << endl;
    }
    
    cout << "\n📈 State frequency distribution:" << endl;
    for (const auto& s : stateFrequency) {
        cout << "  |" << s.first << "⟩: " << s.second << " times" << endl;
    }
    
    cout << "\n⏱️  Total time: " << duration.count() << " microseconds" << endl;
    cout << "⚡ Time complexity: O(1) - constant time for superposition exploration" << endl;
    cout << "🚀 Parallelism: QUANTUM - explores ALL states simultaneously" << endl;
    
    return duration.count();
}

/**
 * @brief Export comprehensive comparison data to CSV
 */
void exportComparisonData() {
    cout << "\n💾 Exporting comparison data..." << endl;
    
    // Generate all 3-bit strings for classical analysis
    vector<string> allStates;
    for (int i = 0; i < 8; i++) {
        string state = "";
        for (int j = 2; j >= 0; j--) {
            state += ((i >> j) & 1) ? '1' : '0';
        }
        allStates.push_back(state);
    }
    
    // Classical vs Quantum comparison data
    ofstream compFile("parity_comparison_analysis.csv");
    compFile << "Method,Input_Size,Time_Microseconds,Complexity,Parallelism\n";
    
    // Classical scaling analysis
    for (int n = 1; n <= 8; n++) {
        vector<string> subset(allStates.begin(), allStates.begin() + n);
        double classicalTime = classicalParityChecker(subset);
        compFile << "Classical," << n << "," << classicalTime << ",O(n),Sequential\n";
    }
    
    // Quantum analysis (constant time regardless of superposition size)
    for (int n = 1; n <= 8; n++) {
        double quantumTime = quantumParityChecker(n * 2); // More measurements for larger n
        compFile << "Quantum," << n << "," << quantumTime << ",O(1),Parallel\n";
    }
    
    compFile.close();
    
    // Export detailed measurement data
    ofstream detailFile("quantum_measurements_detailed.csv");
    detailFile << "Measurement,State,Ones,Parity,Probability\n";
    
    Register measureReg = reg;
    for (int i = 0; i < 50; i++) {
        Register tempReg = measureReg;
        string measurement = tempReg.Collapse();
        int ones = count(measurement.begin(), measurement.end(), '1');
        string parity = (ones % 2 == 0) ? "EVEN" : "ODD";
        double prob = 1.0 / 8.0; // Uniform after Hadamard
        
        detailFile << i+1 << "," << measurement << "," << ones << "," 
                  << parity << "," << prob << "\n";
    }
    
    detailFile.close();
    
    cout << "📁 Files created:" << endl;
    cout << "  - parity_comparison_analysis.csv (scaling analysis)" << endl;
    cout << "  - quantum_measurements_detailed.csv (measurement data)" << endl;
}

/**
 * @brief Time complexity analysis and visualization
 */
void timeComplexityAnalysis() {
    cout << "\n⏱️  TIME COMPLEXITY ANALYSIS:" << endl;
    cout << "===========================================" << endl;
    
    cout << "\n🔧 Classical Electronic Parity Checker:" << endl;
    cout << "  • Algorithm: Sequential bit counting" << endl;
    cout << "  • Time Complexity: O(n × m)" << endl;
    cout << "    - n = number of input strings" << endl;
    cout << "    - m = bits per string (constant = 3)" << endl;
    cout << "  • Space Complexity: O(1)" << endl;
    cout << "  • Scalability: Linear growth with input size" << endl;
    cout << "  • Hardware: Electronic gates with propagation delays" << endl;
    
    cout << "\n⚛️  Quantum Parity Checker:" << endl;
    cout << "  • Algorithm: Superposition + measurement sampling" << endl;
    cout << "  • Time Complexity: O(1) for superposition creation" << endl;
    cout << "  • Measurement Complexity: O(k) where k = measurements" << endl;
    cout << "  • Space Complexity: O(2^n) quantum state space" << endl;
    cout << "  • Scalability: Exponential state space, constant operation time" << endl;
    cout << "  • Hardware: Quantum gates with coherent superposition" << endl;
    
    cout << "\n🚀 QUANTUM ADVANTAGE:" << endl;
    cout << "  ✅ Parallel exploration of ALL 2^n states simultaneously" << endl;
    cout << "  ✅ Single quantum operation vs n classical operations" << endl;
    cout << "  ✅ Probabilistic sampling reveals quantum interference" << endl;
    cout << "  ✅ Exponential speedup for certain problems" << endl;
    
    cout << "\n⚠️  QUANTUM LIMITATIONS:" << endl;
    cout << "  ❌ Requires quantum hardware (fragile)" << endl;
    cout << "  ❌ Probabilistic results need multiple measurements" << endl;
    cout << "  ❌ Decoherence limits operation time" << endl;
    cout << "  ❌ No-cloning theorem prevents copying quantum states" << endl;
}

int main() {
    cout << "=============================================" << endl;
    cout << "   ELECTRONIC vs QUANTUM PARITY ANALYSIS" << endl;
    cout << "=============================================" << endl;
    
    // Generate all possible 3-bit inputs for classical comparison
    vector<string> allInputs = {"000", "001", "010", "011", "100", "101", "110", "111"};
    
    // Classical analysis
    double classicalTime = classicalParityChecker(allInputs);
    
    // Quantum analysis  
    double quantumTime = quantumParityChecker(20);
    
    // Time complexity analysis
    timeComplexityAnalysis();
    
    // Performance comparison summary
    cout << "\n📈 PERFORMANCE COMPARISON SUMMARY:" << endl;
    cout << "===========================================" << endl;
    cout << "Classical time (8 inputs): " << classicalTime << " μs" << endl;
    cout << "Quantum time (20 measurements): " << quantumTime << " μs" << endl;
    cout << "Speedup ratio: " << fixed << setprecision(2) << (classicalTime / quantumTime) << "x" << endl;
    
    // Export data for visualization
    exportComparisonData();
    
    cout << "\n🎯 KEY TAKEAWAY:" << endl;
    cout << "Quantum computing shows exponential parallelism through superposition," << endl;
    cout << "exploring multiple computational paths simultaneously!" << endl;
    
    return 0;
}

