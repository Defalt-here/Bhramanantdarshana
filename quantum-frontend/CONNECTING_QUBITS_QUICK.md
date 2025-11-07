# Connecting Qubits - Quick Start Guide

## 🎯 Quick Answer

To connect two qubits using CNOT or SWAP gates:

1. **Drag** a CNOT or SWAP gate to the first qubit (control)
2. **Click** on a second qubit in the same time step (target)
3. See the purple connection line appear
4. **Run** simulation to see entanglement!

## 🎮 Interactive Example

### Create a Bell State in 3 Steps:

1. Drag **H** (blue) to q[0], Step 1
2. Drag **CNOT** (purple) to q[0], Step 2  
3. Click on **q[1]** in Step 2
4. Click **▶ Run Simulation**

Result: Qubits 0 and 1 are now entangled! 🔗

## 📋 Visual Cues

- **Yellow pulsing** = Click here to complete connection
- **Purple dashed line** = Qubits are connected
- **🔗 Entangled badge** = Qubits share quantum state

## ❓ Common Issues

**Q: Nothing happens when I drop a CNOT gate?**  
A: Look for the yellow prompt - you need to click another qubit to finish the connection.

**Q: Can't click on other qubits?**  
A: Make sure you're clicking on the **same time step** (same column) as the first gate.

**Q: How do I remove a connection?**  
A: Click on either gate involved in the connection.

## 🚀 Try These

- **Bell State**: H on q[0], then CNOT between q[0] and q[1]
- **State Swap**: Put different gates on q[0] and q[1], then SWAP them
- **GHZ State**: H on q[0], CNOT to q[1], CNOT to q[2]

See **HOW_TO_CONNECT_QUBITS.md** for detailed instructions!
