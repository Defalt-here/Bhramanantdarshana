export interface Gate {
  type: 'H' | 'X' | 'Y' | 'Z' | 'CNOT' | 'SWAP' | 'M';
  name: string;
  color: string;
  description?: string;
  // For two-qubit gates
  controlQubit?: number;
  targetQubit?: number;
}

export interface CircuitState {
  alpha: number;  // Amplitude for |0⟩
  beta: number;   // Amplitude for |1⟩
  measured: boolean;
  entangled?: boolean;
  entangledWith?: number[];
}

export interface QubitRegister {
  id: number;
  state: CircuitState;
}

export interface Connection {
  controlQubit: number;
  targetQubit: number;
  stepIndex: number;
  type: 'CNOT' | 'SWAP';
}
