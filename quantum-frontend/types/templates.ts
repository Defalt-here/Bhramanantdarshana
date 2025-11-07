import { Gate, Connection } from './quantum';

export interface CircuitTemplate {
  id: string;
  name: string;
  category: 'algorithm' | 'gate-demo' | 'entanglement' | 'measurement';
  description: string;
  numQubits: number;
  circuit: (Gate | null)[][];
  connections: Connection[];
  icon: string;
  explanation: string;
}

export const quantumTemplates: CircuitTemplate[] = [
  {
    id: 'bell-state',
    name: 'Bell State',
    category: 'entanglement',
    description: 'Create maximal entanglement between two qubits',
    numQubits: 2,
    circuit: [
      [
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        { type: 'CNOT', name: 'CNOT', color: 'bg-purple-500' },
        null, null, null, null, null, null
      ],
      [
        null,
        { type: 'CNOT', name: 'CNOT', color: 'bg-purple-500' },
        null, null, null, null, null, null
      ]
    ],
    connections: [
      { controlQubit: 0, targetQubit: 1, stepIndex: 1, type: 'CNOT' }
    ],
    icon: '🔗',
    explanation: 'The Bell state (|00⟩ + |11⟩)/√2 is the simplest example of quantum entanglement. First, we apply Hadamard to q[0] to create superposition, then CNOT entangles q[0] and q[1]. Measuring one qubit instantly determines the other!'
  },
  {
    id: 'ghz-state',
    name: 'GHZ State (3-Qubit)',
    category: 'entanglement',
    description: 'Greenberger-Horne-Zeilinger state with 3 qubits',
    numQubits: 3,
    circuit: [
      [
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        { type: 'CNOT', name: 'CNOT', color: 'bg-purple-500' },
        { type: 'CNOT', name: 'CNOT', color: 'bg-purple-500' },
        null, null, null, null, null
      ],
      [
        null,
        { type: 'CNOT', name: 'CNOT', color: 'bg-purple-500' },
        null, null, null, null, null, null
      ],
      [
        null, null,
        { type: 'CNOT', name: 'CNOT', color: 'bg-purple-500' },
        null, null, null, null, null
      ]
    ],
    connections: [
      { controlQubit: 0, targetQubit: 1, stepIndex: 1, type: 'CNOT' },
      { controlQubit: 0, targetQubit: 2, stepIndex: 2, type: 'CNOT' }
    ],
    icon: '🌐',
    explanation: 'GHZ state (|000⟩ + |111⟩)/√2 demonstrates three-way entanglement. All qubits are maximally entangled - measuring any one collapses all three to the same value!'
  },
  {
    id: 'quantum-not',
    name: 'Quantum NOT (X Gate)',
    category: 'gate-demo',
    description: 'Flip qubit from |0⟩ to |1⟩',
    numQubits: 1,
    circuit: [
      [
        { type: 'X', name: 'Pauli-X', color: 'bg-red-500' },
        null, null, null, null, null, null, null
      ]
    ],
    connections: [],
    icon: '🔄',
    explanation: 'The Pauli-X gate is the quantum equivalent of a classical NOT gate. It flips |0⟩ to |1⟩ and vice versa. Starting from |0⟩, after X gate you get |1⟩ with 100% probability.'
  },
  {
    id: 'superposition',
    name: 'Superposition (Hadamard)',
    category: 'gate-demo',
    description: 'Create equal superposition of |0⟩ and |1⟩',
    numQubits: 1,
    circuit: [
      [
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        null, null, null, null, null, null, null
      ]
    ],
    connections: [],
    icon: '⚛️',
    explanation: 'Hadamard gate creates superposition! Starting from |0⟩, it produces (|0⟩ + |1⟩)/√2, meaning 50% chance of measuring 0 or 1. This is the quantum "both states at once" phenomenon!'
  },
  {
    id: 'phase-flip',
    name: 'Phase Flip (Z Gate)',
    category: 'gate-demo',
    description: 'Add phase to |1⟩ state',
    numQubits: 1,
    circuit: [
      [
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        { type: 'Z', name: 'Pauli-Z', color: 'bg-green-500' },
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        null, null, null, null, null
      ]
    ],
    connections: [],
    icon: '🌊',
    explanation: 'Z gate adds a phase flip to |1⟩. Combined with Hadamard gates (H-Z-H), it effectively performs a bit flip in the superposition basis, demonstrating quantum phase!'
  },
  {
    id: 'deutsch-simple',
    name: 'Deutsch Algorithm (Constant)',
    category: 'algorithm',
    description: 'Determine if function is constant or balanced',
    numQubits: 2,
    circuit: [
      [
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        null,
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        { type: 'M', name: 'Measure', color: 'bg-orange-500' },
        null, null, null, null
      ],
      [
        { type: 'X', name: 'Pauli-X', color: 'bg-red-500' },
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        null, null, null, null, null, null
      ]
    ],
    connections: [],
    icon: '🎯',
    explanation: 'Simplified Deutsch algorithm! The first quantum algorithm showing quantum advantage. It determines if a black-box function is constant or balanced in just ONE query, while classical computers need TWO.'
  },
  {
    id: 'swap-demo',
    name: 'State Swap',
    category: 'gate-demo',
    description: 'Exchange states between two qubits',
    numQubits: 2,
    circuit: [
      [
        { type: 'X', name: 'Pauli-X', color: 'bg-red-500' },
        { type: 'SWAP', name: 'SWAP', color: 'bg-pink-500' },
        null, null, null, null, null, null
      ],
      [
        null,
        { type: 'SWAP', name: 'SWAP', color: 'bg-pink-500' },
        null, null, null, null, null, null
      ]
    ],
    connections: [
      { controlQubit: 0, targetQubit: 1, stepIndex: 1, type: 'SWAP' }
    ],
    icon: '↔️',
    explanation: 'SWAP gate exchanges quantum states between qubits. Here q[0] starts as |1⟩ (after X), q[1] as |0⟩. After SWAP, q[0] becomes |0⟩ and q[1] becomes |1⟩!'
  },
  {
    id: 'measurement-demo',
    name: 'Measurement Collapse',
    category: 'measurement',
    description: 'See superposition collapse upon measurement',
    numQubits: 1,
    circuit: [
      [
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        { type: 'M', name: 'Measure', color: 'bg-orange-500' },
        null, null, null, null, null, null
      ]
    ],
    connections: [],
    icon: '📏',
    explanation: 'Quantum measurement collapses superposition! After Hadamard, the qubit is in (|0⟩ + |1⟩)/√2. Measurement randomly collapses it to either |0⟩ or |1⟩ with 50% probability each.'
  },
  {
    id: 'entangle-measure',
    name: 'Entanglement + Measurement',
    category: 'measurement',
    description: 'Measure entangled qubits to see correlation',
    numQubits: 2,
    circuit: [
      [
        { type: 'H', name: 'Hadamard', color: 'bg-blue-500' },
        { type: 'CNOT', name: 'CNOT', color: 'bg-purple-500' },
        { type: 'M', name: 'Measure', color: 'bg-orange-500' },
        null, null, null, null, null
      ],
      [
        null,
        { type: 'CNOT', name: 'CNOT', color: 'bg-purple-500' },
        { type: 'M', name: 'Measure', color: 'bg-orange-500' },
        null, null, null, null, null
      ]
    ],
    connections: [
      { controlQubit: 0, targetQubit: 1, stepIndex: 1, type: 'CNOT' }
    ],
    icon: '🎲',
    explanation: 'Entangled qubits show perfect correlation! Both measurements always give the same result (both 0 or both 1), even though each individual outcome is random. This is "spooky action at a distance"!'
  },
  {
    id: 'three-gates',
    name: 'Three Pauli Gates',
    category: 'gate-demo',
    description: 'X, Y, Z gates in sequence',
    numQubits: 1,
    circuit: [
      [
        { type: 'X', name: 'Pauli-X', color: 'bg-red-500' },
        { type: 'Y', name: 'Pauli-Y', color: 'bg-yellow-500' },
        { type: 'Z', name: 'Pauli-Z', color: 'bg-green-500' },
        null, null, null, null, null
      ]
    ],
    connections: [],
    icon: '🎨',
    explanation: 'The three Pauli gates (X, Y, Z) are fundamental quantum operations representing rotations on the Bloch sphere. X flips, Y rotates around Y-axis, Z adds phase. Together they can create any single-qubit rotation!'
  }
];
