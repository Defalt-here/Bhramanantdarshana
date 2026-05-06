import { GateType } from '@/types/circuit';

export interface GateInfo {
  name: string;
  symbol: string;
  /** Tailwind class for the gate chip background. */
  chipBg: string;
  /** Text color class. */
  chipText: string;
  description: string;
  matrixLines: string[];  // ASCII matrix, one string per row
  on0: string;            // action on |0⟩
  on1: string;            // action on |1⟩
  identity: string;       // key algebraic identity
  category: 'single' | 'rotation' | 'two-qubit' | 'three-qubit';
  hasAngle?: boolean;
  numQubits: number;
}

export const GATE_INFO: Record<GateType, GateInfo> = {
  H: {
    name: 'Hadamard',
    symbol: 'H',
    chipBg: 'bg-violet-600',
    chipText: 'text-white',
    description:
      'Creates an equal superposition. The starting point of nearly every quantum algorithm — it transforms a definite |0⟩ or |1⟩ into a quantum coin-flip.',
    matrixLines: ['1/√2 · | 1   1 |', '       | 1  -1 |'],
    on0: '→ (|0⟩ + |1⟩)/√2  =  |+⟩',
    on1: '→ (|0⟩ − |1⟩)/√2  =  |−⟩',
    identity: 'H² = I,   HXH = Z,   HZH = X',
    category: 'single',
    numQubits: 1,
  },
  X: {
    name: 'Pauli-X  (bit flip)',
    symbol: 'X',
    chipBg: 'bg-red-500',
    chipText: 'text-white',
    description:
      'The quantum NOT gate. Swaps the amplitudes of |0⟩ and |1⟩, rotating 180° around the X-axis of the Bloch sphere.',
    matrixLines: ['| 0  1 |', '| 1  0 |'],
    on0: '→ |1⟩',
    on1: '→ |0⟩',
    identity: 'X² = I',
    category: 'single',
    numQubits: 1,
  },
  Y: {
    name: 'Pauli-Y',
    symbol: 'Y',
    chipBg: 'bg-pink-500',
    chipText: 'text-white',
    description:
      'Combines a bit-flip with a phase-flip. Rotates 180° around the Y-axis. Produces purely imaginary amplitudes from real inputs.',
    matrixLines: ['|  0  -i |', '|  i   0 |'],
    on0: '→ i|1⟩',
    on1: '→ -i|0⟩',
    identity: 'Y² = I,   Y = iXZ',
    category: 'single',
    numQubits: 1,
  },
  Z: {
    name: 'Pauli-Z  (phase flip)',
    symbol: 'Z',
    chipBg: 'bg-yellow-400',
    chipText: 'text-black',
    description:
      'Flips the sign of the |1⟩ amplitude only. Diagonal — never changes measurement probabilities, only the relative phase.',
    matrixLines: ['| 1   0 |', '| 0  -1 |'],
    on0: '→ |0⟩  (unchanged)',
    on1: '→ -|1⟩',
    identity: 'Z² = I,   HZH = X',
    category: 'single',
    numQubits: 1,
  },
  S: {
    name: 'S gate  (√Z)',
    symbol: 'S',
    chipBg: 'bg-sky-500',
    chipText: 'text-white',
    description:
      'Applies a π/2 phase rotation around the Z-axis. The square root of Z: S² = Z.',
    matrixLines: ['| 1  0 |', '| 0  i |'],
    on0: '→ |0⟩',
    on1: '→ i|1⟩',
    identity: 'S² = Z,   S⁴ = I',
    category: 'single',
    numQubits: 1,
  },
  T: {
    name: 'T gate  (⁴√Z)',
    symbol: 'T',
    chipBg: 'bg-orange-500',
    chipText: 'text-white',
    description:
      'Applies a π/4 phase rotation. Together with H, forms a universal gate set for quantum computation.',
    matrixLines: ['| 1   0         |', '| 0   e^(iπ/4) |'],
    on0: '→ |0⟩',
    on1: '→ e^(iπ/4)|1⟩',
    identity: 'T² = S,   T⁴ = Z,   T⁸ = I',
    category: 'single',
    numQubits: 1,
  },
  RX: {
    name: 'Rx(θ)  rotation',
    symbol: 'Rx',
    chipBg: 'bg-purple-600',
    chipText: 'text-white',
    description:
      'Rotates the Bloch sphere by angle θ around the X-axis. Rx(π) = −i·X.',
    matrixLines: [
      '| cos(θ/2)     -i·sin(θ/2) |',
      '| -i·sin(θ/2)   cos(θ/2)   |',
    ],
    on0: '→ cos(θ/2)|0⟩ − i·sin(θ/2)|1⟩',
    on1: '→ −i·sin(θ/2)|0⟩ + cos(θ/2)|1⟩',
    identity: 'Rx(π) = −i·X,   Rx(0) = I',
    category: 'rotation',
    hasAngle: true,
    numQubits: 1,
  },
  RY: {
    name: 'Ry(θ)  rotation',
    symbol: 'Ry',
    chipBg: 'bg-fuchsia-600',
    chipText: 'text-white',
    description:
      'Rotates the Bloch sphere by angle θ around the Y-axis. Real-valued matrix — useful for preparing arbitrary superpositions without phase.',
    matrixLines: [
      '| cos(θ/2)  -sin(θ/2) |',
      '| sin(θ/2)   cos(θ/2) |',
    ],
    on0: '→ cos(θ/2)|0⟩ + sin(θ/2)|1⟩',
    on1: '→ −sin(θ/2)|0⟩ + cos(θ/2)|1⟩',
    identity: 'Ry(π) = −i·Y,   Ry(π/2)|0⟩ = (|0⟩+|1⟩)/√2',
    category: 'rotation',
    hasAngle: true,
    numQubits: 1,
  },
  RZ: {
    name: 'Rz(θ)  rotation',
    symbol: 'Rz',
    chipBg: 'bg-amber-600',
    chipText: 'text-white',
    description:
      'Rotates the Bloch sphere by angle θ around the Z-axis. Diagonal — only affects relative phase between |0⟩ and |1⟩.',
    matrixLines: [
      '| e^(−iθ/2)   0         |',
      '| 0           e^(+iθ/2) |',
    ],
    on0: '→ e^(−iθ/2)|0⟩',
    on1: '→ e^(+iθ/2)|1⟩',
    identity: 'Rz(π) = −i·Z,   Rz(π/2) ≈ e^(-iπ/4)·S',
    category: 'rotation',
    hasAngle: true,
    numQubits: 1,
  },
  CNOT: {
    name: 'CNOT  (controlled-X)',
    symbol: '⊕',
    chipBg: 'bg-emerald-500',
    chipText: 'text-white',
    description:
      'The primary entangling gate. Flips the target qubit iff the control is |1⟩. When the control is in superposition it does NOT branch classically — it entangles the two qubits.',
    matrixLines: [
      '| 1  0  0  0 |',
      '| 0  1  0  0 |',
      '| 0  0  0  1 |',
      '| 0  0  1  0 |',
    ],
    on0: '|00⟩→|00⟩  |10⟩→|11⟩',
    on1: '|01⟩→|01⟩  |11⟩→|10⟩',
    identity:
      '(α|0⟩+β|1⟩)⊗|0⟩  ─CNOT→  α|00⟩+β|11⟩  (Bell state when α=β=1/√2)',
    category: 'two-qubit',
    numQubits: 2,
  },
  CZ: {
    name: 'CZ  (controlled-Z)',
    symbol: 'Z•',
    chipBg: 'bg-teal-500',
    chipText: 'text-white',
    description:
      'Flips the phase of |11⟩ only. Symmetric in its two qubits. Equivalent to CNOT with H on the target: CZ = (I⊗H)·CNOT·(I⊗H).',
    matrixLines: [
      '| 1  0  0   0 |',
      '| 0  1  0   0 |',
      '| 0  0  1   0 |',
      '| 0  0  0  -1 |',
    ],
    on0: '|00⟩→|00⟩  |10⟩→|10⟩',
    on1: '|01⟩→|01⟩  |11⟩→-|11⟩',
    identity: 'CZ = (I⊗H)·CNOT·(I⊗H)',
    category: 'two-qubit',
    numQubits: 2,
  },
  SWAP: {
    name: 'SWAP',
    symbol: '↔',
    chipBg: 'bg-cyan-500',
    chipText: 'text-white',
    description:
      'Exchanges the full quantum state of two qubits, including any superposition. Equivalent to three sequential CNOTs.',
    matrixLines: [
      '| 1  0  0  0 |',
      '| 0  0  1  0 |',
      '| 0  1  0  0 |',
      '| 0  0  0  1 |',
    ],
    on0: '|01⟩→|10⟩',
    on1: '|10⟩→|01⟩',
    identity: 'SWAP = CNOT(c,t)·CNOT(t,c)·CNOT(c,t)',
    category: 'two-qubit',
    numQubits: 2,
  },
  TOFFOLI: {
    name: 'Toffoli  (CCNOT)',
    symbol: '⊕⊕',
    chipBg: 'bg-lime-500',
    chipText: 'text-black',
    description:
      'Three-qubit doubly-controlled NOT. Flips the target only when both controls are |1⟩. Universal for classical reversible computation — used inside quantum implementations of arithmetic and search oracles.',
    matrixLines: [
      '|c₁,c₂,t⟩ → |c₁,c₂, t ⊕ (c₁ ∧ c₂)⟩',
      '8×8 identity except the last 2×2 block is X',
    ],
    on0: '|110⟩ → |111⟩',
    on1: '|111⟩ → |110⟩',
    identity: 'Toffoli is universal for classical reversible logic',
    category: 'three-qubit',
    numQubits: 3,
  },
};
