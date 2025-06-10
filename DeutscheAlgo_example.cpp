#include <iostream>
#include <complex>
#include <cmath>
#include <vector>

#include "RegisterGates_cl.cpp"

#define COMPLEX std::complex

enum class OracleType{
    Constant0,
    Constant1,
    Identity,
    Not
};

class DeutschOracle{
    OracleType type;
public:
    DeutschOracle(OracleType t) : type(t){}
    void Apply(Register &reg) const{
        int size = reg.val.size();

        for(int i = 0; i < size; ++i){
            int x = (i >> 1) & 1;  // qubit 0
            int y = i & 1; 
            int fx = 0;
            switch (type) {
                case OracleType::Constant0: fx = 0; break;
                case OracleType::Constant1: fx = 1; break;
                case OracleType::Identity:  fx = x; break;
                case OracleType::Not:       fx = !x; break;
            }
            int newY = y ^ fx;
            int j = (x << 1) | newY;

            std::swap(reg.val[i], reg.val[j]);  // apply U_f
        }
    }
};
void RunDeutschAlgorithm(bool isConstant) {
    // Step 1: Initialize register with |01⟩
    Register reg(2, {
        {"01", std::complex<double>(1.0, 0.0)}
    });

    // Step 2: Apply Hadamard to both qubits
    HadamardR H;
    H.ApplyToSingle(reg, 0);
    H.ApplyToSingle(reg, 1);

    // Step 3: Apply the oracle (constant or balanced)
    DeutschOracle oracle(OracleType::Constant0);
    oracle.Apply(reg);

    // Step 4: Apply Hadamard again to the first qubit
    H.ApplyToSingle(reg, 0);

    // Step 5: Measure the first qubit
    int result = reg.MeasureWithoutCollapse()[0]; // Only the first qubit is relevant

    std::cout << "Function is " << (isConstant ? "constant" : "balanced")
              << ", Measured: " << result << std::endl;

    if ((isConstant && result == 0) || (!isConstant && result == 1)) {
        std::cout << "✅ Correct" << std::endl;
    } else {
        std::cout << "❌ Incorrect" << std::endl;
    }
}