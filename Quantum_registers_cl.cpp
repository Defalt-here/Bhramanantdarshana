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
class Register
{
public:
    int bits;
    std::vector<std::complex<double>> val;
    Register(int n) : bits(n)
    {
        int resizeFactor = 1 << n;
        val.resize(resizeFactor, COMPLEX<double>(0.0f, 0.0f));
        val[0] = COMPLEX(1.0f, 0.0f);
        Normalise();
    }
    Register(int n, const std::map<std::string, std::complex<double>> &initStates) : bits(n)
    {
        int size = 1 << n;
        val.resize(size, COMPLEX<double>(0.0, 0.0));

        for (const auto &[bitstring, amplitude] : initStates)
        {
            assert(static_cast<int>(bitstring.length()) == n && "Bitstring length must match register size");
            int index = std::stoi(bitstring, nullptr, 2);
            assert(index < size && "Index out of bounds for register size");
            val[index] = amplitude;
        }

        Normalise();
    }

    double MagnitudeSquareSum() const
    {
        double result = 0.0;
        for (const auto &i : val)
        {
            result += std::norm(i);
        }
        return result;
    }
    double GetProbab(int i)
    {
        return std::norm(val[i]);
    }
    COMPLEX<double> FindInnerProduct(Register other) const
    {
        assert(val.size() == other.val.size());
        COMPLEX<double> IP = COMPLEX<double>(0.0, 0.0);
        for (size_t i = 0; i < val.size(); i++)
        {
            IP += std::conj(val[i]) * other.val[i];
        }
        return IP;
    }
    void Print() const
    {
        std::cout << "|ψ⟩ = ";
        int n = std::log2(val.size());
        bool first = true;

        for (size_t i = 0; i < val.size(); ++i)
        {
            if (std::abs(val[i]) < 1e-6)
                continue;

            if (!first)
                std::cout << " + ";
            first = false;

            std::cout << "" << val[i] << "|";

            for (int j = n - 1; j >= 0; --j)
                std::cout << ((i >> j) & 1);
        }

        std::cout << std::endl;
    }
    std::string Collapse()
    {
        EnsureRandomSeeded();
        std::vector<double> cumulative;
        double total = 0.0;
        for (const auto &amp : val)
        {
            total += std::norm(amp);
            cumulative.push_back(total);
        }

        double r = static_cast<double>(rand()) / RAND_MAX;

        size_t collapsedIndex = 0;
        for (; collapsedIndex < cumulative.size(); ++collapsedIndex)
        {
            if (r < cumulative[collapsedIndex])
                break;
        }

        // Collapse the state vector
        for (size_t i = 0; i < val.size(); ++i)
        {
            val[i] = (i == collapsedIndex) ? COMPLEX(1.0, 0.0) : COMPLEX(0.0, 0.0);
        }

        std::string result;
        // Format result with leading zeros to match number of bits
        for (int j = bits - 1; j >= 0; --j)
        {
            result += std::to_string((collapsedIndex >> j) & 1);
        }
        return result;
    }
    std::string MeasureWithoutCollapse()
    {
        EnsureRandomSeeded();
        std::vector<double> cumulative;
        double total = 0.0;
        for (const auto &amp : val)
        {
            total += std::norm(amp);
            cumulative.push_back(total);
        }

        double r = static_cast<double>(rand()) / RAND_MAX;

        size_t measuredIndex = 0;
        for (; measuredIndex < cumulative.size(); ++measuredIndex)
        {
            if (r < cumulative[measuredIndex])
                break;
        }

        std::string result;
        // Format result with leading zeros to match number of bits
        for (int j = bits - 1; j >= 0; --j)
        {
            result += std::to_string((measuredIndex >> j) & 1);
        }
        return result;
    }

private:
    double Normalise()
    {
        double magnitudeSquareSum = MagnitudeSquareSum();
        for (auto &i : val)
        {
            i /= sqrt(magnitudeSquareSum);
        }
        return magnitudeSquareSum;
    }

    void EnsureRandomSeeded()
    {
        static bool seeded = []
        {
            std::srand(static_cast<unsigned>(time(0)));
            return true;
        }();
    }
};

