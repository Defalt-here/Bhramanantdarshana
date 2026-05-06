import { PlacedGate, GateType, AmplitudeEntry, SimulationResult } from '@/types/circuit';

/** Convert a structured circuit into a .qsim program string. */
export function buildQsimProgram(numQubits: number, gates: PlacedGate[]): string {
  const lines: string[] = [`qubits ${numQubits}`];
  const sorted = [...gates].sort((a, b) =>
    a.step !== b.step ? a.step - b.step : a.qubit - b.qubit
  );
  for (const g of sorted) {
    const q = g.qubit;
    const theta = (g.theta ?? Math.PI / 2).toFixed(6);
    const c = g.control ?? 0;
    const c2 = g.control2 ?? 0;
    switch (g.type) {
      case 'H':    lines.push(`H ${q}`); break;
      case 'X':    lines.push(`X ${q}`); break;
      case 'Y':    lines.push(`Y ${q}`); break;
      case 'Z':    lines.push(`Z ${q}`); break;
      case 'S':    lines.push(`S ${q}`); break;
      case 'T':    lines.push(`T ${q}`); break;
      case 'RX':   lines.push(`RX ${q} ${theta}`); break;
      case 'RY':   lines.push(`RY ${q} ${theta}`); break;
      case 'RZ':   lines.push(`RZ ${q} ${theta}`); break;
      case 'CNOT': if (g.control != null) lines.push(`CNOT ${c} ${q}`); break;
      case 'CZ':   if (g.control != null) lines.push(`CZ ${c} ${q}`); break;
      case 'SWAP': if (g.control != null) lines.push(`SWAP ${c} ${q}`); break;
      case 'TOFFOLI':
        if (g.control != null && g.control2 != null)
          lines.push(`TOFFOLI ${c} ${c2} ${q}`);
        break;
    }
  }
  return lines.join('\n');
}

/**
 * Parse a .qsim program back into a structured circuit. Independent gates
 * (acting on disjoint qubits) are packed into the earliest column where
 * their qubits are free, so e.g. `H 0; H 1; H 2` collapse onto one step.
 */
export function parseQsim(program: string): {
  numQubits: number;
  gates: PlacedGate[];
  warnings: string[];
} {
  const rawLines = program
    .split('\n')
    .map(l => l.trim())
    .filter(l => l && !l.startsWith('#'));

  let numQubits = 0;
  const gates: PlacedGate[] = [];
  const warnings: string[] = [];
  const stepUsage = new Map<number, Set<number>>();
  let counter = 0;

  const findStep = (qubits: number[]): number => {
    for (let s = 0; s < 2000; s++) {
      const used = stepUsage.get(s);
      const conflict = used ? qubits.some(q => used.has(q)) : false;
      if (!conflict) {
        if (!stepUsage.has(s)) stepUsage.set(s, new Set());
        const set = stepUsage.get(s)!;
        for (const q of qubits) set.add(q);
        return s;
      }
    }
    return 0;
  };

  const pushGate = (
    type: GateType,
    qubit: number,
    extras: { theta?: number; control?: number; control2?: number } = {}
  ) => {
    const involved = [qubit];
    if (extras.control  != null) involved.push(extras.control);
    if (extras.control2 != null) involved.push(extras.control2);
    const step = findStep(involved);
    gates.push({
      id: `loaded_${counter++}_${Date.now()}`,
      type,
      qubit,
      step,
      ...extras,
    });
  };

  for (const raw of rawLines) {
    const tokens = raw.split(/\s+/);
    const op = tokens[0].toUpperCase();
    try {
      switch (op) {
        case 'QUBITS':
          numQubits = parseInt(tokens[1], 10);
          break;
        case 'H': case 'X': case 'Y': case 'Z': case 'S': case 'T':
          pushGate(op as GateType, parseInt(tokens[1], 10));
          break;
        case 'RX': case 'RY': case 'RZ':
          pushGate(op as GateType, parseInt(tokens[1], 10), {
            theta: parseFloat(tokens[2]),
          });
          break;
        case 'CNOT': case 'CZ': case 'SWAP':
          pushGate(op as GateType, parseInt(tokens[2], 10), {
            control: parseInt(tokens[1], 10),
          });
          break;
        case 'TOFFOLI':
          pushGate('TOFFOLI', parseInt(tokens[3], 10), {
            control:  parseInt(tokens[1], 10),
            control2: parseInt(tokens[2], 10),
          });
          break;
        case 'PRINT': case 'MEASURE_ALL': case 'OUTPUT_STATE':
          break;  // IO ops not relevant for display
        default:
          warnings.push(`Unrecognized op: ${op}`);
      }
    } catch {
      warnings.push(`Failed to parse line: ${raw}`);
    }
  }
  return { numQubits, gates, warnings };
}

/** Parse the raw amplitude array returned by the API into report-friendly entries. */
export function parseSimResult(
  bits: number,
  amplitudes: [number, number][]
): SimulationResult {
  const entries: AmplitudeEntry[] = [];
  let norm = 0;
  amplitudes.forEach(([re, im], i) => {
    const prob = re * re + im * im;
    norm += prob;
    if (prob > 1e-6) {
      entries.push({ basisState: i.toString(2).padStart(bits, '0'), real: re, imag: im, probability: prob });
    }
  });
  entries.sort((a, b) => b.probability - a.probability);
  return { bits, rawAmplitudes: amplitudes, entries, norm };
}

/** Format a complex number for Dirac notation. */
export function fmtComplex(re: number, im: number): string {
  const r = +re.toFixed(4);
  const i = +im.toFixed(4);
  if (i === 0) return `${r}`;
  if (r === 0) return `${i}i`;
  return `(${r}${i >= 0 ? '+' : ''}${i}i)`;
}

/** Build a Dirac-notation string for the top amplitudes. */
export function toDirac(entries: AmplitudeEntry[]): string {
  return entries
    .slice(0, 8)
    .map(e => `${fmtComplex(e.real, e.imag)}|${e.basisState}⟩`)
    .join(' + ');
}

/** Call the /api/simulate endpoint with a .qsim program string. */
export async function simulate(program: string): Promise<SimulationResult> {
  const res = await fetch('/api/simulate', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ program }),
  });
  const data = await res.json() as
    | { ok: true; bits: number; amplitudes: [number, number][] }
    | { ok: false; error: string };
  if (!data.ok) throw new Error(data.error);
  return parseSimResult(data.bits, data.amplitudes);
}
