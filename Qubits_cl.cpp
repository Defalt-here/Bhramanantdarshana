#include <iostream>
#include <complex>

class Qubit {
public:
    std::complex<double> ampliA;
    std::complex<double> ampliB;
    std::complex<double> val[2];
    Qubit(std::complex<double> a, std::complex<double> b) { //A Qubit can exist in 2 states at once (0 or 1) hence a and b define the amplitude of the Qubit object for 0 and 1
        val[0] = a;
        val[1] = b;
        Normalise();
        ampliA = val[0];
        ampliB = val[1];
    }
    void Print() const {
        std::cout << "|ψ⟩ = ";
        
        bool firstPrinted = false;
    
        if (val[0] != std::complex<double>(0,0)) {
            printComplex(val[0]);
            std::cout << "|0⟩";
            firstPrinted = true;
        }
    
        if (val[1] != std::complex<double>(0,0)) {
            if (firstPrinted) std::cout << " + ";
            printComplex(val[1]);
            std::cout << "|1⟩";
            firstPrinted = true;
        }
    
        if (!firstPrinted) {
            std::cout << "0";
        }
    
        std::cout << std::endl;
    }
    double MagnitudeSquareSum() const {
        return std::norm(val[0]) + std::norm(val[1]);
    }
    double GetProbabilityOf0(){
        return std::norm(val[0]);
    }
    double GetProbabilityOf1(){
        return std::norm(val[1]);
    }
    int MeasureWithoutCollapse(){
        EnsureRandomSeeded();
        double prob0 = std::norm(ampliA);
        double randNum = static_cast<double>(rand()) / RAND_MAX;
        return (randNum < prob0) ? 0 : 1;
    }
    bool Collapse() {
        EnsureRandomSeeded();
        double prob0 = std::norm(ampliA);
        double randNum = static_cast<double>(rand()) / RAND_MAX;
    
        if (randNum < prob0) {
            // Collapse to |0⟩
            ampliA,val[0] = {1.0, 0.0};
            ampliB,val[1] = {0.0, 0.0};
            return false;  // measured 0
        } else {
            // Collapse to |1⟩
            ampliA,val[0] = {0.0, 0.0};
            ampliB,val[1] = {1.0, 0.0};
            return true;  // measured 1
        }
    }
    
    std::complex<double> FindInnerProduct(const Qubit& other) const {
        return std::conj(val[0]) * other.val[0] + std::conj(val[1]) * other.val[1];
    }
    private:
    void EnsureRandomSeeded() {
        static bool seeded = [] {
            std::srand(static_cast<unsigned>(time(0)));
            return true;
        }();
    }
    void Normalise(){
        double magnitudeSquared = MagnitudeSquareSum();
        val[0] /= sqrt(magnitudeSquared);
        val[1] /= sqrt(magnitudeSquared);
    }
    void printComplex(const std::complex<double>& c) const {
        double re = c.real();
        double im = c.imag();
    
        if (re == 0 && im == 0) {
            std::cout << "0";
            return;
        }
    
        bool printed = false;
    
        if (re != 0) {
            std::cout << re;
            printed = true;
        }
    
        if (im != 0) {
            if (im > 0 && printed) std::cout << "+";
            else if (im < 0) std::cout << "-";
    
            if (std::abs(im) != 1)
                std::cout << std::abs(im);
    
            std::cout << "i";
        }
    }
    
};
