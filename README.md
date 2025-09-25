<h1>Bhramanantdarshana</h1>
<h4>(Name still evolving – suggestions welcome!)</h4>

<p>
<strong>Bhramanantdarshana</strong> is a lightweight, didactic <strong>C++ quantum register & gate simulator</strong> built from first principles (no external quantum SDKs). It focuses on clarity of state evolution, measurement mechanics, and foundational algorithms (currently: <em>Deutsch's Algorithm</em>). The project aims to strike a balance between educational transparency and a path toward extensibility (multi‑qubit gates, oracles, and visualization hooks).
</p>

---

## 🚀 Quick Glance
| Aspect | Status |
|--------|--------|
| Core State Representation | Complex amplitude vector (2^n) |
| Measurement | Collapsing & non-collapsing binary string outputs |
| Gates Implemented | Identity, H, X, Y, Z, S, T + register-level variants |
| Algorithm Demo | Deutsch’s Algorithm (constant vs balanced) |
| Output Export | CSV measurement sampling |
| Visualization | Python matplotlib + seaborn palette overlay (bar + line) |
| Docs | Rich inline Doxygen-style comments |

---

## 📌 Why This Exists
Most quantum “tutorial” simulators either hide details behind abstractions or overwhelm newcomers with framework overhead. This project shows:

* How a quantum register is just a normalized complex vector.
* How single-qubit gates act via pairwise amplitude mixing (bit masks & toggles).
* How measurement converts amplitudes → probabilities → sampled bitstrings.
* How algorithms (Deutsch) exploit superposition to reduce oracle calls.

If you can read C++ and basic linear algebra, you can understand and extend this code.

---

## 🧠 Core Concepts Modeled
* State Vector: `std::vector<std::complex<double>>` of length 2^n.
* Normalization: Sum of squared magnitudes forced to 1.
* Gates: Bitwise pairing using masks (Hadamard) or permutation (X gate).
* Measurement: Cumulative distribution over |amplitude|²; returns n-bit string.
* Non-Destructive Peek: Sampling without collapse for statistics gathering.

---

## 🔧 Features
* Clean measurement API returning exactly `n` bits.
* CSV export of repeated collapses for empirical distributions.
* Python plotting script (`plotter.py`) producing bar + line overlay & annotations.
* Extensive commentary explaining mathematical intent (great for learning / teaching).
* Modular gate architecture (single-qubit + register-level wrappers).

---

## 🏁 Quick Start
1. Compile (example with MinGW / g++):
	```bash
	g++ -std=c++17 Quantum_registers_cl.cpp RegisterGates_cl.cpp DeutscheAlgo_example.cpp -o deutsch.exe
	```
2. Run the Deutsch example:
	```bash
	./deutsch.exe
	```
3. (Optional) Generate measurement dataset (ensure your `main` loop writes CSV) then visualize:
	```bash
	python plotter.py
	```

> On Windows (PowerShell) adjust paths as needed. The repository already includes an example CSV (`collapse_measurements.csv`).

---

## 📂 Project Structure
| File | Purpose |
|------|---------|
| `Quantum_registers_cl.cpp` | Core quantum register (state vector, normalization, measurement). |
| `RegisterGates_cl.cpp` | Register-wide gate application (HadamardR, XGateR, etc.). |
| `Gates_cl.cpp` | Single-qubit gate definitions & theory (H, Pauli, phase gates). |
| `DeutscheAlgo_example.cpp` | Implementation + commentary of Deutsch’s Algorithm. |
| `plotter.py` | Visualization of measurement frequency distribution. |
| `collapse_measurements.csv` | Example measurement dataset. |
| `register_measurement_3qubit.csv` | Example 3‑qubit register measurement log. |
| `coin_flips.csv` | Auxiliary probability demo output. |

---

## 🔍 Example: Measurement Logic (Conceptual)
1. Compute probabilities: p(i) = |ψᵢ|².
2. Form cumulative array: c[i] = Σ_{k ≤ i} p(k).
3. Draw uniform r ∈ [0,1) and binary search for first c[i] ≥ r.
4. Collapse: Zero out all amplitudes except chosen index; renormalize to 1.
5. Return binary string of measured index (MSB = qubit 0 by convention here).

---

## 🧪 Deutsch’s Algorithm
Implemented oracles: constant-0, constant-1, identity (balanced), NOT (balanced). With a single oracle evaluation the algorithm distinguishes constant vs balanced—showcasing quantum advantage over classical two-query requirement.

---

## 📊 Visualization
`plotter.py` renders:
* Bars: Empirical frequency of each observed basis state.
* Line: Same frequencies connected to show distribution profile.
* Annotations: Raw counts atop bars.
Legend placement is adjustable (currently anchored left outside plot area).

---

## 🛣️ Roadmap
| Stage | Planned Enhancements |
|-------|-----------------------|
| Short Term | Replace `rand()` with `std::mt19937`; add unit tests; seed configurability. |
| Medium | Implement multi-qubit entangling gates (CNOT, CZ); add generic matrix-apply path. |
| Medium | Add state dumping to JSON + richer Python visualization (Bloch vectors for single qubits). |
| Long | Introduce simple circuit builder DSL; add Grover / Bernstein–Vazirani demos. |
| Long | Optional parallelization for large (n>15) experiments. |

---

## 🎓 Learning Outcomes
Reading this code should help you internalize:
* How linear algebra underpins qubit logic.
* The practical difference between collapse vs statistical sampling.
* How gate locality (single-qubit) induces structured state index pairing.
* Why normalization and numerical stability matter.
* How an algorithm encodes a decision property into phase/amplitude structure.

---

## 🤝 Contributing
Ideas, critiques, and naming suggestions welcome. Keep additions minimalistic and pedagogical—avoid over-engineering early.

---

## 📄 License
Currently unspecified. Until a license is added, treat this as “all rights reserved” for external redistribution. A permissive license (MIT / Apache-2.0) may be added soon.

---

<sub>Original minimal README text retained in spirit, now expanded for clarity.</sub>
