# How to Connect Two Qubits

## Overview
Two-qubit gates allow you to create **entanglement** and perform **controlled operations** between qubits. This is a fundamental concept in quantum computing that enables quantum algorithms like Deutsch-Jozsa, Grover's, and Shor's algorithms.

## Available Two-Qubit Gates

### 🟣 CNOT (Controlled-NOT)
- **Purpose**: Creates entanglement between qubits
- **Behavior**: Flips the target qubit only if the control qubit is |1⟩
- **Symbol**: ⊕ (XOR symbol)
- **Use Case**: Creating Bell states, quantum teleportation

### 🌸 SWAP
- **Purpose**: Exchanges the quantum states of two qubits
- **Behavior**: Swaps q[i] ↔ q[j]
- **Symbol**: × (cross symbol)
- **Use Case**: Moving quantum information, qubit routing

## Step-by-Step Guide

### Method 1: Drag and Click (Interactive)

1. **Drag the Gate**
   - From the gate library (left panel), drag a CNOT or SWAP gate
   - Drop it on the desired qubit wire at a specific time step
   - This becomes the **control qubit**

2. **Click to Connect**
   - After dropping, you'll see a yellow prompt: "Click on another qubit to complete the connection"
   - The available slots in the same time step will pulse with a yellow glow
   - Click on a different qubit in the **same time step** to select the **target qubit**

3. **Visual Confirmation**
   - A purple dashed line will appear connecting the two qubits
   - A control dot (●) appears on the control qubit
   - A target symbol (⊕ or ×) appears on the target qubit

4. **Remove Connection**
   - Click on either gate to remove the entire connection
   - Both gates and the connection line will be cleared

### Example: Creating a Bell State

**Goal**: Create the maximally entangled state (|00⟩ + |11⟩)/√2

```
Step 1: Hadamard on q[0]
Step 2: CNOT with control=q[0], target=q[1]
```

**Instructions**:
1. Drag **H (Hadamard)** gate to q[0], Step 1
2. Drag **CNOT** gate to q[0], Step 2
3. Click on q[1] in Step 2 to complete the connection
4. Click "▶ Run Simulation"
5. Observe both qubits show entanglement in the visualizer

## Visual Indicators

| Indicator | Meaning |
|-----------|---------|
| 🟡 Yellow pulse | This slot is awaiting your click to complete the connection |
| 🟣 Purple dashed line | Active connection between qubits |
| ● Purple dot | Control qubit marker |
| ⊕ Circle | CNOT target |
| × Cross | SWAP participant |
| 🔗 Badge | Qubit is entangled (shown in visualizer) |

## Important Rules

✅ **Do**:
- Connect qubits in the **same time step**
- Use CNOT to create entanglement
- Remove gates by clicking on them
- Experiment with different combinations

❌ **Don't**:
- Try to connect qubits in different time steps (won't work)
- Try to connect a qubit to itself (won't work)
- Forget to complete the connection (circuit won't simulate correctly)

## Example Circuits

### 1. Bell State (Entanglement)
```
q[0]: ──H──●──
           │
q[1]: ─────⊕──
```
Creates maximal entanglement

### 2. Swap States
```
q[0]: ──X──×──
           │
q[1]: ──H──×──
```
Swaps superposition from q[1] to q[0]

### 3. Multiple Entanglement
```
q[0]: ──H──●─────●──
           │     │
q[1]: ─────⊕──●──┼──
              │  │
q[2]: ────────⊕──⊕──
```
Creates GHZ state across 3 qubits

## Understanding the Visualization

After running simulation with connected qubits:

- **Entangled Badge**: Shows which qubits are entangled together
- **Bloch Sphere**: May not fully represent entangled states (limitation of single-qubit visualization)
- **Probabilities**: Show the marginal probabilities for each qubit

## Tips for Learning

1. **Start Simple**: Begin with just 2 qubits and a CNOT gate
2. **Observe**: Watch how entanglement creates correlations
3. **Experiment**: Try H + CNOT, X + CNOT, different combinations
4. **Multiple Gates**: Create chains of entanglement
5. **Measurement**: Add M gates after entanglement to see quantum correlation

## Technical Notes

### Simulation Limitations
This is an **educational simulator** with simplified physics:
- Entanglement is approximated for visualization
- Multi-qubit state space is simplified
- Phase relationships are not fully modeled
- Perfect for learning concepts, not for accurate quantum computation

### Next Steps
- Learn about Bell states and quantum teleportation
- Explore the Deutsch-Jozsa algorithm
- Study quantum error correction basics
- Understand how CNOT gates create quantum parallelism

---

**Happy Quantum Computing! 🌌**
