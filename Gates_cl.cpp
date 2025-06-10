#include <iostream>
#include <complex>
#include <cmath>
#ifndef PI
#define PI 3.14159265358979323846
#endif
#include "Qubits_cl.cpp"
#include <fstream>
#include <vector>
class QGates
{
    virtual Qubit apply(const Qubit &q) const = 0;
};
class Identity : public QGates
{
public:
    Qubit apply(const Qubit &q) const override
    {
        return Qubit(q.val[0], q.val[1]);
    }
};
class Hadamard : public QGates
{
public:
    Qubit apply(const Qubit &q) const override
    {
        const double invSqrt2 = 1.0 / std::sqrt(2.0);
        std::complex<double> a = q.ampliA;
        std::complex<double> b = q.ampliB;
        std::complex<double> new0 = invSqrt2 * (a + b);
        std::complex<double> new1 = invSqrt2 * (a - b);
        return Qubit(new0, new1);
    }
};
class PauliX : public QGates
{
public:
    Qubit apply(const Qubit &q) const override
    {
        return Qubit(q.ampliB, q.ampliA);
    }
};
class PauliY : public QGates
{
public:
    Qubit apply(const Qubit &q) const override
    {
        std::complex<double> i(0, 1);
        return Qubit(-i * q.ampliB, i * q.ampliA);
    }
};
class PauliZ : public QGates
{
public:
    Qubit apply(const Qubit &q) const override
    {
        return Qubit(q.ampliA, -q.ampliB);
    }
};
class SGate : public QGates
{
public:
    Qubit apply(const Qubit &q) const override
    {
        std::complex<double> i(0, 1);
        return Qubit(q.ampliA, i * q.ampliB);
    }
};

class TGate : public QGates
{
public:
    Qubit apply(const Qubit &q) const override
    {
        std::complex<double> phase(std::cos(PI / 4), std::sin(PI / 4));
        return Qubit(q.ampliA, phase * q.ampliB);
    }
};
