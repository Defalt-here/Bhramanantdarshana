// Core circuit and simulation types shared across the application.

export type SingleGateType = 'H' | 'X' | 'Y' | 'Z' | 'S' | 'T' | 'RX' | 'RY' | 'RZ';
export type TwoGateType = 'CNOT' | 'CZ' | 'SWAP';
export type ThreeGateType = 'TOFFOLI';
export type GateType = SingleGateType | TwoGateType | ThreeGateType;

export interface PlacedGate {
  id: string;
  type: GateType;
  qubit: number;      // target qubit index
  step: number;       // time-step column
  theta?: number;     // radians – used by RX, RY, RZ
  control?: number;   // first control – used by CNOT, CZ, SWAP, TOFFOLI
  control2?: number;  // second control – TOFFOLI only
}

export interface AmplitudeEntry {
  basisState: string;  // e.g. "01", "101"
  real: number;
  imag: number;
  probability: number; // |re|² + |im|²
}

export interface SimulationResult {
  bits: number;
  rawAmplitudes: [number, number][];
  entries: AmplitudeEntry[];   // non-trivial (prob > 1e-6) only
  norm: number;
}

export type InfoContext =
  | { kind: 'gate'; gateType: GateType }
  | { kind: 'demo'; demoId: string }
  | { kind: 'none' };
