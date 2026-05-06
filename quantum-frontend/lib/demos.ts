export interface DemoSpec {
  id: string;
  name: string;
  category: 'entanglement' | 'algorithm' | 'gate-demo' | 'misc';
  tagline: string;
  description: string;
  algorithmInfo: string;
  classicalVsQuantum?: string;
  numQubits: number;
  qsimProgram: string;
  expectedState: string;
  icon: string;
}

export const DEMOS: DemoSpec[] = [
  // ---- Entanglement -------------------------------------------------------
  {
    id: 'bell_phi_plus',
    name: '|Φ⁺⟩ Bell State',
    category: 'entanglement',
    icon: '🔗',
    tagline: 'Maximum two-qubit entanglement',
    description:
      'The canonical Bell pair (|00⟩ + |11⟩)/√2. Measuring one qubit instantly ' +
      'determines the other, no matter how far apart they are.',
    algorithmInfo:
      'Apply Hadamard to qubit 0 to create a superposition, then CNOT ' +
      '(control 0, target 1) to entangle. The resulting state cannot be ' +
      'written as a product of individual qubit states — a defining feature ' +
      'of entanglement.',
    numQubits: 2,
    qsimProgram: 'qubits 2\nH 0\nCNOT 0 1',
    expectedState: '(|00⟩ + |11⟩) / √2',
  },
  {
    id: 'bell_psi_plus',
    name: '|Ψ⁺⟩ Bell State',
    category: 'entanglement',
    icon: '🔗',
    tagline: 'Anti-correlated Bell pair',
    description:
      'The Bell pair (|01⟩ + |10⟩)/√2. Measuring one qubit gives the opposite ' +
      'result for the other.',
    algorithmInfo:
      'Start by flipping qubit 1 with X (so initial state is |01⟩), then ' +
      'apply H on qubit 0 and CNOT. The X before H changes which Bell state ' +
      'we land on.',
    numQubits: 2,
    qsimProgram: 'qubits 2\nX 1\nH 0\nCNOT 0 1',
    expectedState: '(|01⟩ + |10⟩) / √2',
  },
  {
    id: 'ghz',
    name: '3-Qubit GHZ State',
    category: 'entanglement',
    icon: '🌐',
    tagline: 'Multi-party entanglement across 3 qubits',
    description:
      'The Greenberger–Horne–Zeilinger state (|000⟩ + |111⟩)/√2. All three ' +
      'qubits are correlated: measuring any one qubit collapses all three ' +
      'simultaneously.',
    algorithmInfo:
      'Hadamard on qubit 0 creates the superposition seed. The two CNOT ' +
      'gates spread the entanglement to qubits 1 and 2 in a chain. The ' +
      'same circuit can be built with CZ gates using the identity ' +
      'CNOT = H·CZ·H.',
    numQubits: 3,
    qsimProgram: 'qubits 3\nH 0\nCNOT 0 1\nCNOT 1 2',
    expectedState: '(|000⟩ + |111⟩) / √2',
  },
  // ---- Algorithms ---------------------------------------------------------
  {
    id: 'deutsch_constant',
    name: "Deutsch's Algorithm  (constant-0)",
    category: 'algorithm',
    icon: '⚡',
    tagline: 'Constant vs balanced in one query',
    description:
      "Decides whether f: {0,1}→{0,1} is constant or balanced using a single " +
      "quantum oracle call. A constant function always returns 0 or always " +
      "returns 1; a balanced function returns 0 for one input and 1 for the other.",
    algorithmInfo:
      'Circuit: prepare |0⟩|1⟩, apply H⊗H, apply the oracle, apply H on ' +
      'qubit 0, measure. For a constant oracle, qubit 0 measures 0 with ' +
      'certainty. For a balanced oracle, it measures 1 with certainty. ' +
      'Classically you need 2 oracle calls; quantum needs 1.',
    classicalVsQuantum: 'Classical: 2 queries worst-case.  Quantum: 1 query — always.',
    numQubits: 2,
    qsimProgram:
      '# Deutsch: constant-0 oracle  (oracle does nothing — f(x) = 0)\n' +
      'qubits 2\n' +
      'X 1\n' +     // ancilla starts in |1⟩
      'H 0\n' +
      'H 1\n' +
      '# constant-0 oracle = identity (no gates)\n' +
      'H 0',
    expectedState: 'qubit 0 in |0⟩  →  function is CONSTANT',
  },
  {
    id: 'deutsch_balanced',
    name: "Deutsch's Algorithm  (balanced/identity)",
    category: 'algorithm',
    icon: '⚡',
    tagline: 'Detects balanced oracle in one shot',
    description:
      "Same circuit, different oracle: f(x) = x (identity). The final " +
      "measurement now reveals qubit 0 in |1⟩ with 100% probability.",
    algorithmInfo:
      'The identity oracle is a CNOT with qubit 0 as control and qubit 1 ' +
      'as target. Phase kickback imprints a -1 sign on the |1⟩ branch of ' +
      'qubit 0 (the ancilla is in |−⟩ which is a -1 eigenstate of X), ' +
      'shifting qubit 0 from |+⟩ to |−⟩. The final H converts that to |1⟩.',
    classicalVsQuantum: 'Classical: 2 queries.  Quantum: 1 query.',
    numQubits: 2,
    qsimProgram:
      '# Deutsch: identity oracle  (f(x) = x)\n' +
      'qubits 2\n' +
      'X 1\n' +
      'H 0\n' +
      'H 1\n' +
      'CNOT 0 1\n' +  // oracle
      'H 0',
    expectedState: 'qubit 0 in |1⟩  →  function is BALANCED',
  },
  {
    id: 'bernstein_vazirani',
    name: 'Bernstein–Vazirani  (s = 101)',
    category: 'algorithm',
    icon: '🔍',
    tagline: 'Recover a hidden bitstring in one query',
    description:
      'Recovers the hidden n-bit string s from f(x) = s·x (mod 2) in a ' +
      'single quantum query. Classically you need n queries (one per bit).',
    algorithmInfo:
      'Four input qubits + one ancilla. The oracle implements f(x) = s·x ' +
      'by CNOTing each set bit of s (positions 0 and 2 for s = 101) into ' +
      'the ancilla. One H layer before and after recovers s from the ' +
      'input register.',
    classicalVsQuantum: 'Classical: n queries.  Quantum: 1 query.',
    numQubits: 5,
    qsimProgram:
      '# Bernstein-Vazirani: s = 1011 (4-bit), 5 qubits total (4 input + 1 ancilla)\n' +
      'qubits 5\n' +
      'X 4\n' +        // ancilla starts |1⟩
      'H 0\nH 1\nH 2\nH 3\nH 4\n' +
      '# oracle: CNOT from bits 0,1,3 (set bits of 1011)\n' +
      'CNOT 0 4\nCNOT 1 4\nCNOT 3 4\n' +
      'H 0\nH 1\nH 2\nH 3',
    expectedState: 'input qubits measure to 1011  (the hidden string)',
  },
  {
    id: 'grover',
    name: "Grover's Search  (marks |101⟩)",
    category: 'algorithm',
    icon: '🔎',
    tagline: 'Quadratic quantum speedup over classical search',
    description:
      'Finds the marked state |101⟩ out of 8 possibilities using 2 ' +
      'Grover iterations. Classical search needs up to 8 lookups; ' +
      'Grover converges in √8 ≈ 2.8 iterations.',
    algorithmInfo:
      'Two-step iteration: oracle phase-flip (−1 to |101⟩) + diffusion ' +
      '(inversion about the mean amplitude). Each step amplifies the ' +
      'marked state by 2|marked⟩ while suppressing all others. After 2 ' +
      'optimal iterations, |101⟩ has ≈ 94% probability.',
    classicalVsQuantum: 'Classical: O(N).  Grover: O(√N) — quadratic speedup.',
    numQubits: 3,
    qsimProgram:
      '# Grover search, marked state |101> (n=3, N=8, k=2 optimal iters)\n' +
      'qubits 3\n' +
      'H 0\nH 1\nH 2\n' +
      '# === Iteration 1 ===\n' +
      '# Oracle: phase-flip |101>  =>  X q1, then CCZ(0,1,2) via H-Toffoli-H trick\n' +
      'X 1\n' +
      'H 2\nTOFFOLI 0 1 2\nH 2\n' +
      'X 1\n' +
      '# Diffusion: H all, flip all but |000>, H all\n' +
      'H 0\nH 1\nH 2\n' +
      'X 0\nX 1\nX 2\n' +
      'H 2\nTOFFOLI 0 1 2\nH 2\n' +
      'X 0\nX 1\nX 2\n' +
      'H 0\nH 1\nH 2\n' +
      '# === Iteration 2 ===\n' +
      'X 1\n' +
      'H 2\nTOFFOLI 0 1 2\nH 2\n' +
      'X 1\n' +
      'H 0\nH 1\nH 2\n' +
      'X 0\nX 1\nX 2\n' +
      'H 2\nTOFFOLI 0 1 2\nH 2\n' +
      'X 0\nX 1\nX 2\n' +
      'H 0\nH 1\nH 2',
    expectedState: '|101⟩ with ≈ 94% probability',
  },
  // ---- Gate demos ---------------------------------------------------------
  {
    id: 'phase_kickback',
    name: 'Phase Kickback',
    category: 'gate-demo',
    icon: '🪃',
    tagline: 'The mechanism behind every Deutsch-style oracle',
    description:
      'Shows how a phase eigenvalue "kicks back" onto the control qubit ' +
      'when the target is in an eigenstate. The target qubit is unchanged ' +
      'while the control acquires the phase.',
    algorithmInfo:
      'Start with q1 = |−⟩ (X then H), q0 = |+⟩ (H). The −1 eigenvalue ' +
      'of Pauli-X on |−⟩ kicks back onto q0, converting it from |+⟩ to ' +
      '|−⟩. This is the core trick behind Deutsch, Bernstein-Vazirani, ' +
      'and Grover oracles.',
    numQubits: 2,
    qsimProgram:
      '# Phase kickback: control q0 in |+>, target q1 in |->\n' +
      'qubits 2\n' +
      'X 1\nH 1\n' +   // q1 = |->
      'H 0\n' +         // q0 = |+>
      'CNOT 0 1',      // kickback: q0 goes to |->
    expectedState: '|−⟩|−⟩  =  (|0⟩−|1⟩)/√2 ⊗ (|0⟩−|1⟩)/√2',
  },
];

export function getDemoById(id: string): DemoSpec | undefined {
  return DEMOS.find(d => d.id === id);
}
