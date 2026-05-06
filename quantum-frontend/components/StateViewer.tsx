'use client';

import { SimulationResult } from '@/types/circuit';
import { toDirac, fmtComplex } from '@/lib/circuitUtils';

interface StateViewerProps {
  result: SimulationResult | null;
  loading: boolean;
  error: string | null;
}

export default function StateViewer({ result, loading, error }: StateViewerProps) {
  if (loading) {
    return (
      <div className="flex items-center gap-3 p-6 rounded-xl"
           style={{ background: 'var(--surface)', border: '1px solid var(--border)' }}>
        <div className="w-4 h-4 rounded-full border-2 border-t-transparent animate-spin"
             style={{ borderColor: 'var(--accent)' }} />
        <span className="text-sm" style={{ color: 'var(--text-muted)' }}>Running on C++ backend…</span>
      </div>
    );
  }

  if (error) {
    return (
      <div className="p-4 rounded-xl text-sm"
           style={{ background: '#1a0a0a', border: '1px solid #7f1d1d', color: '#fca5a5' }}>
        <strong>Backend error: </strong>{error}
      </div>
    );
  }

  if (!result) {
    return (
      <div className="p-6 rounded-xl text-sm text-center"
           style={{ background: 'var(--surface)', border: '1px dashed var(--border)', color: 'var(--text-muted)' }}>
        Run the circuit to see the state vector from the C++ simulator.
      </div>
    );
  }

  const normOk = Math.abs(result.norm - 1) < 1e-6;

  return (
    <div className="rounded-xl overflow-hidden"
         style={{ background: 'var(--surface)', border: '1px solid var(--border)' }}>
      {/* Header */}
      <div className="flex items-center justify-between px-4 py-3"
           style={{ borderBottom: '1px solid var(--border)' }}>
        <span className="text-xs font-semibold uppercase tracking-widest" style={{ color: 'var(--text-muted)' }}>
          State Vector  ·  {result.bits} qubits  ·  {result.entries.length} non-zero amplitudes
        </span>
        <span className={`text-[10px] px-2 py-0.5 rounded-full ${normOk ? '' : 'opacity-70'}`}
              style={{ background: normOk ? '#14532d' : '#7f1d1d', color: normOk ? '#86efac' : '#fca5a5' }}>
          ‖ψ‖ = {result.norm.toFixed(8)}
        </span>
      </div>

      {/* Dirac notation */}
      <div className="px-4 py-3 font-mono text-sm" style={{ color: 'var(--accent-lit)', borderBottom: '1px solid var(--border)' }}>
        |ψ⟩ = {toDirac(result.entries)}
      </div>

      {/* Amplitude table */}
      <div className="px-4 py-3 space-y-2">
        {result.entries.map(e => {
          const pct = (e.probability * 100).toFixed(1);
          const barW = `${Math.round(e.probability * 100)}%`;
          return (
            <div key={e.basisState} className="space-y-1">
              <div className="flex items-center justify-between text-xs font-mono">
                <span style={{ color: 'var(--text)' }}>|{e.basisState}⟩</span>
                <span style={{ color: 'var(--text-muted)' }}>
                  {fmtComplex(e.real, e.imag)}
                </span>
                <span className="font-semibold" style={{ color: 'var(--accent-lit)' }}>{pct}%</span>
              </div>
              <div className="h-2 rounded-full" style={{ background: 'var(--elevated)' }}>
                <div className="prob-bar h-2 rounded-full" style={{ width: barW }} />
              </div>
            </div>
          );
        })}
      </div>
    </div>
  );
}
