#include <iostream>
#include <complex>
#include <cmath>
#include <vector>

#include "Quantum_registers_cl.cpp"

#define COMPLEX std::complex
class RGates{
public:
    // virtual void apply(Register& reg, int qubitIndex) const = 0;
    virtual void Apply(Register& reg) const = 0;
    virtual void ApplyToSingle(Register& reg, int qubitIndex) const = 0;
    virtual ~RGates() = default;
};
class HadamardR : public RGates{
public:
    void ApplyToSingle(Register& reg, int qubitIndex) const override{
        int size = reg.val.size();
        int mask = 1 << qubitIndex;

        for(int i = 0; i < size; ++i){
            if(!(i&mask)){
                int j = i | mask;

                COMPLEX<double> a = reg.val[i];
                COMPLEX<double> b = reg.val[j];
                const double invSqrt2 = 1.0f/std::sqrt(2.0f);
                reg.val[i] = (a+b)*invSqrt2;
                reg.val[j] = (a-b)*invSqrt2;
            }
        }
    }
    void Apply(Register& reg) const override {
        int numQubits = reg.bits;  // Get number of qubits
        for(int i = 0; i < numQubits; ++i) {   // Iterate over qubits, not states
            ApplyToSingle(reg, i);
        }
    }
};
class XGateR : public RGates {
    public:
        void ApplyToSingle(Register& reg, int qubitIndex) const override {
            int size = reg.val.size();
            int mask = 1 << qubitIndex;
            for(int i = 0; i < size; ++i){
                int j = i ^ mask;
                if(i < j){
                    std::swap(reg.val[i],reg.val[j]);
                }
            }
        }
        void Apply(Register& reg) const override {
            int numQubits = reg.bits;
            for(int i = 0; i < numQubits; ++i){
                ApplyToSingle(reg, i);
            }
        }
};
