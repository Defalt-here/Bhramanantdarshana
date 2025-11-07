# 🌌 Quantum Circuit Simulator

An interactive, educational web application for learning quantum computing concepts through visual circuit building and simulation.

![Quantum Circuit Simulator](https://img.shields.io/badge/Quantum-Computing-blueviolet)
![Next.js](https://img.shields.io/badge/Next.js-16-black)
![TypeScript](https://img.shields.io/badge/TypeScript-5-blue)
![Tailwind CSS](https://img.shields.io/badge/Tailwind-CSS-38bdf8)

## ✨ Features

### 🎯 Interactive Circuit Building
- **Drag & Drop Interface**: Intuitive gate placement on quantum circuits
- **Multiple Quantum Gates**: H (Hadamard), X, Y, Z (Pauli gates), CNOT, and Measurement
- **Flexible Circuit Design**: Support for 1-5 qubits with 8 time steps each

### 📊 Real-Time Visualization
- **Bloch Sphere Representation**: Visual representation of qubit states
- **Probability Bars**: Live probability distribution for |0⟩ and |1⟩ states
- **State Vector Display**: Mathematical notation of quantum states
- **Measurement Indication**: Visual feedback when qubits are measured

### 🎓 Educational Features
- Gate descriptions and explanations
- Step-by-step circuit execution
- Superposition and entanglement visualization
- Interactive learning experience
- **10+ Pre-made Circuit Templates** demonstrating quantum algorithms and concepts

## 🚀 Getting Started

### Prerequisites
- Node.js 18.x or higher
- npm or yarn package manager

### Installation

1. Navigate to the quantum-frontend directory:
```bash
cd quantum-frontend
```

2. Install dependencies:
```bash
npm install
```

3. Run the development server:
```bash
npm run dev
```

4. Open your browser and navigate to:
```
http://localhost:3000
```

## 🎮 How to Use

### Building a Circuit

1. **Select Number of Qubits**: Use the dropdown to choose 1-5 qubits
2. **Drag Gates**: Drag quantum gates from the library on the left
3. **Drop on Circuit**: Drop gates onto the circuit grid slots
4. **Remove Gates**: Click on a placed gate to remove it
5. **Run Simulation**: Click "▶ Run Simulation" to execute the circuit
6. **View Results**: See qubit states update in the right panel
7. **Reset**: Click "🔄 Reset" to clear the circuit

### Using Pre-made Templates

Click the **"📋 Circuit Templates"** button (bottom-right) to access 10+ pre-built circuits:

#### 🔗 Entanglement Templates
- **Bell State**: Create maximal two-qubit entanglement
- **GHZ State**: Three-way entanglement demonstration
- **Entanglement + Measurement**: See quantum correlation in action

#### 🎯 Algorithm Templates
- **Deutsch Algorithm**: First quantum algorithm showing advantage over classical

#### ⚛️ Gate Demonstrations
- **Quantum NOT**: X gate bit flip
- **Superposition**: Hadamard gate creating equal superposition
- **Phase Flip**: Z gate demonstrating quantum phase
- **Three Pauli Gates**: X, Y, Z gates in sequence
- **State Swap**: Exchange quantum states between qubits

#### 📏 Measurement Templates
- **Measurement Collapse**: Observe superposition collapse
- **Entangled Measurement**: See correlated measurement results

Each template includes:
- Pre-configured circuit with proper gate placement
- Automatic qubit connections (for CNOT/SWAP)
- Detailed explanation of what the circuit demonstrates
- Category-based filtering (Algorithms, Entanglement, Gates, Measurement)

### Available Quantum Gates

#### **H (Hadamard Gate)** - Blue
Creates superposition: transforms |0⟩ into (|0⟩ + |1⟩)/√2

#### **X (Pauli-X / NOT Gate)** - Red
Quantum NOT: flips |0⟩ to |1⟩ and vice versa

#### **Y (Pauli-Y Gate)** - Yellow
Rotation around Y-axis of the Bloch sphere

#### **Z (Pauli-Z / Phase Gate)** - Green
Adds a phase flip to |1⟩ state

#### **CNOT (Controlled-NOT)** - Purple
Entangles two qubits (controlled operation)

#### **M (Measurement)** - Orange
Collapses superposition to classical |0⟩ or |1⟩

## 🏗️ Project Structure

```
quantum-frontend/
├── app/
│   ├── layout.tsx          # Root layout with DnD provider
│   ├── page.tsx            # Main page component
│   ├── DndProvider.tsx     # Drag and drop context
│   └── globals.css         # Global styles
├── components/
│   ├── QuantumCircuitBuilder.tsx  # Main circuit builder logic
│   ├── GateLibrary.tsx            # Draggable gate components
│   ├── CircuitGrid.tsx            # Circuit grid with drop zones
│   └── QubitVisualizer.tsx        # Qubit state visualization
├── types/
│   └── quantum.ts          # TypeScript type definitions
└── package.json
```

## 🎨 Design Highlights

- **Modern UI**: Glassmorphism design with gradient backgrounds
- **Responsive Layout**: Works on desktop and tablet devices
- **Smooth Animations**: Transitions for gate placement and state updates
- **Color-Coded Gates**: Each gate type has a distinct color for easy identification
- **Visual Feedback**: Hover effects, drag states, and measurement indicators

## 🧮 Quantum Simulation

The simulator implements simplified quantum mechanics:
- **Superposition**: Gates like Hadamard create quantum superposition
- **State Evolution**: Each gate transforms qubit amplitudes
- **Measurement**: Probabilistic collapse to classical states
- **Visualization**: Bloch sphere representation of single-qubit states

### Limitations
This is an educational tool with simplified physics:
- Single-qubit gates only (CNOT is visual but not fully functional)
- No phase relationships displayed
- Simplified probability calculations
- No error correction or noise modeling

## 🛠️ Technology Stack

- **Framework**: [Next.js 16](https://nextjs.org/) (App Router)
- **Language**: [TypeScript](https://www.typescriptlang.org/)
- **Styling**: [Tailwind CSS](https://tailwindcss.com/)
- **Drag & Drop**: [react-dnd](https://react-dnd.github.io/react-dnd/)
- **Graphics**: SVG for Bloch sphere visualization

## 📚 Learning Resources

To learn more about quantum computing:
- [IBM Quantum Learning](https://learning.quantum.ibm.com/)
- [Qiskit Textbook](https://qiskit.org/textbook/)
- [Microsoft Quantum Documentation](https://docs.microsoft.com/quantum/)

## 🤝 Contributing

This is an educational project. Feel free to:
- Add more quantum gates (T, S, Rx, Ry, Rz)
- Implement multi-qubit entanglement visualization
- Add circuit export/import functionality
- Create tutorial modes or guided lessons
- Improve the quantum simulation accuracy

## 📝 License

This project is created for educational purposes as part of the Bhramanantdarshana quantum computing project.

## 🎓 About

This frontend is designed to complement the C++ quantum computing SDK, providing an accessible visual interface for students and enthusiasts to learn quantum computing concepts through interactive experimentation.

---

**Built with ❤️ for quantum education**
