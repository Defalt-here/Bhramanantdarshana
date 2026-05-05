/**
 * @file qsim_cli.cpp
 * @brief Tiny command-line interpreter for quantum-circuit descriptions.
 *
 * Reads a circuit, line by line, from stdin in the @c .qsim grammar
 * documented in @c cli/PROTOCOL.md. Each line is one of:
 *
 *   qubits N                — declare register size (must come first)
 *   H q                     — single-qubit gate on qubit q
 *   X q | Y q | Z q | S q | T q
 *   RX q theta              — rotation gate on qubit q
 *   RY q theta
 *   RZ q theta
 *   CNOT c t                — two-qubit controlled gate
 *   CZ   c t
 *   SWAP c t
 *   TOFFOLI c1 c2 t         — three-qubit controlled gate
 *   print                   — Print state in Dirac notation (stderr)
 *   measure_all N file.csv  — N samples (without collapse) → CSV
 *   output_state file.json  — full state vector → JSON
 *
 * Empty lines and lines whose first non-whitespace character is @c # are
 * ignored. The program exits with status 0 on success, non-zero on any
 * parse or semantic error (with a diagnostic on stderr).
 */
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Conventions.h"
#include "Register.h"
#include "RegisterGates.h"
#include "RNG.h"

namespace {

void usage(std::ostream& os) {
    os << "qsim_cli reads a .qsim circuit description from stdin and\n"
          "executes it against the in-process simulator.\n"
          "See cli/PROTOCOL.md for the complete grammar.\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc >= 2) {
        std::string a = argv[1];
        if (a == "-h" || a == "--help") { usage(std::cout); return 0; }
    }
    qsim::rng::seed(42);

    std::unique_ptr<Register> reg;
    HadamardR H; XGateR X; YGateR Y; ZGateR Z; SGateR S; TGateR T;
    CNOTGateR CNOT; CZGateR CZ; SWAPGateR SW; ToffoliGateR Tf;

    int lineNo = 0;
    std::string line;
    while (std::getline(std::cin, line)) {
        ++lineNo;

        // Trim leading whitespace for the comment / blank check.
        std::size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        if (line[start] == '#')          continue;

        std::istringstream iss(line);
        std::string op;
        iss >> op;
        try {
            if (op == "qubits") {
                int n;
                if (!(iss >> n) || n < 1 || n > qsim::kMaxQubits)
                    throw std::runtime_error("invalid qubits N");
                reg = std::make_unique<Register>(n);
                continue;
            }
            if (!reg) throw std::runtime_error("expected `qubits N` first");

            auto needSingle = [&](RGates& gate) {
                int q;
                if (!(iss >> q)) throw std::runtime_error("expected qubit index");
                gate.ApplyToSingle(*reg, q);
            };
            auto needRotation = [&](char which) {
                int q; double theta;
                if (!(iss >> q >> theta)) throw std::runtime_error("expected q theta");
                if (which == 'X') RxR(theta).ApplyToSingle(*reg, q);
                if (which == 'Y') RyR(theta).ApplyToSingle(*reg, q);
                if (which == 'Z') RzR(theta).ApplyToSingle(*reg, q);
            };
            auto needPair = [&](RGates& gate) {
                int c, t;
                if (!(iss >> c >> t)) throw std::runtime_error("expected c t");
                gate.ApplyToPair(*reg, c, t);
            };

            if      (op == "H")    needSingle(H);
            else if (op == "X")    needSingle(X);
            else if (op == "Y")    needSingle(Y);
            else if (op == "Z")    needSingle(Z);
            else if (op == "S")    needSingle(S);
            else if (op == "T")    needSingle(T);
            else if (op == "RX")   needRotation('X');
            else if (op == "RY")   needRotation('Y');
            else if (op == "RZ")   needRotation('Z');
            else if (op == "CNOT") needPair(CNOT);
            else if (op == "CZ")   needPair(CZ);
            else if (op == "SWAP") needPair(SW);
            else if (op == "TOFFOLI") {
                int c1, c2, t;
                if (!(iss >> c1 >> c2 >> t)) throw std::runtime_error("expected c1 c2 t");
                Tf.ApplyToTriple(*reg, c1, c2, t);
            }
            else if (op == "print") {
                reg->Print();
            }
            else if (op == "measure_all") {
                int n; std::string path;
                if (!(iss >> n >> path)) throw std::runtime_error("expected N file.csv");
                reg->WriteMeasurementsCSV(path, n);
            }
            else if (op == "output_state") {
                std::string path;
                if (!(iss >> path)) throw std::runtime_error("expected file.json");
                std::ofstream out(path);
                if (!out) throw std::runtime_error("cannot open " + path);
                reg->ToJson(out);
            }
            else {
                throw std::runtime_error("unknown command: " + op);
            }
        } catch (const std::exception& e) {
            std::cerr << "error at line " << lineNo << ": " << e.what() << "\n"
                      << "  > " << line << "\n";
            return 1;
        }
    }
    return 0;
}
