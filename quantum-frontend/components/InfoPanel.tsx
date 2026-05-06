'use client';

import { GateType, InfoContext } from '@/types/circuit';
import { GATE_INFO } from '@/lib/gateInfo';
import { getDemoById } from '@/lib/demos';

interface InfoPanelProps {
  context: InfoContext;
}

export default function InfoPanel({ context }: InfoPanelProps) {
  const containerStyle = {
    background: 'var(--surface)',
    border: '1px solid var(--border)',
  };

  if (context.kind === 'none') {
    return (
      <div className="p-5 rounded-xl text-center text-xs leading-relaxed"
           style={{ ...containerStyle, color: 'var(--text-muted)' }}>
        <p className="text-2xl mb-2">🔬</p>
        Hover a gate in the library or select a demo to see details here.
      </div>
    );
  }

  if (context.kind === 'gate') {
    const info = GATE_INFO[context.gateType];
    return (
      <div className="rounded-xl overflow-hidden" style={containerStyle}>
        {/* Title bar */}
        <div className="flex items-center gap-3 px-4 py-3"
             style={{ borderBottom: '1px solid var(--border)' }}>
          <div className={`gate-chip ${info.chipBg} ${info.chipText} cursor-default`}
               style={{ width: 40, height: 40, fontSize: '0.8rem' }}>
            {info.symbol}
          </div>
          <div>
            <p className="text-sm font-semibold" style={{ color: 'var(--text)' }}>{info.name}</p>
            <p className="text-[10px]" style={{ color: 'var(--text-muted)' }}>
              {info.numQubits}-qubit · {info.category}
            </p>
          </div>
        </div>

        <div className="px-4 py-4 space-y-4">
          {/* Description */}
          <p className="text-xs leading-relaxed" style={{ color: 'var(--text-muted)' }}>
            {info.description}
          </p>

          {/* Matrix */}
          <div>
            <p className="text-[10px] uppercase tracking-wider mb-1" style={{ color: 'var(--text-muted)' }}>
              Unitary Matrix
            </p>
            <pre className="text-xs font-mono p-3 rounded-lg"
                 style={{ background: 'var(--elevated)', color: 'var(--accent-lit)', overflowX: 'auto' }}>
              {info.matrixLines.join('\n')}
            </pre>
          </div>

          {/* Actions */}
          <div>
            <p className="text-[10px] uppercase tracking-wider mb-1" style={{ color: 'var(--text-muted)' }}>
              Action on basis states
            </p>
            <div className="space-y-1">
              <div className="flex items-center gap-2 text-xs font-mono">
                <span style={{ color: 'var(--text-muted)' }}>|0⟩</span>
                <span style={{ color: 'var(--text)' }}>{info.on0}</span>
              </div>
              <div className="flex items-center gap-2 text-xs font-mono">
                <span style={{ color: 'var(--text-muted)' }}>|1⟩</span>
                <span style={{ color: 'var(--text)' }}>{info.on1}</span>
              </div>
            </div>
          </div>

          {/* Identity */}
          <div className="p-3 rounded-lg" style={{ background: 'var(--elevated)' }}>
            <p className="text-[10px] uppercase tracking-wider mb-1" style={{ color: 'var(--text-muted)' }}>
              Key Identity
            </p>
            <p className="text-xs font-mono" style={{ color: 'var(--accent-lit)' }}>
              {info.identity}
            </p>
          </div>
        </div>
      </div>
    );
  }

  // Demo context
  const demo = getDemoById(context.demoId);
  if (!demo) return null;

  const catColor: Record<string, string> = {
    algorithm: '#7c3aed',
    entanglement: '#0891b2',
    'gate-demo': '#b45309',
    misc: '#374151',
  };

  return (
    <div className="rounded-xl overflow-hidden" style={containerStyle}>
      <div className="px-4 py-3" style={{ borderBottom: '1px solid var(--border)' }}>
        <div className="flex items-center gap-2 mb-1">
          <span className="text-xl">{demo.icon}</span>
          <p className="text-sm font-semibold" style={{ color: 'var(--text)' }}>{demo.name}</p>
          <span className="ml-auto text-[10px] px-2 py-0.5 rounded-full"
                style={{ background: catColor[demo.category] ?? '#374151', color: '#fff' }}>
            {demo.category}
          </span>
        </div>
        <p className="text-xs" style={{ color: 'var(--text-muted)' }}>{demo.tagline}</p>
      </div>

      <div className="px-4 py-4 space-y-4">
        <p className="text-xs leading-relaxed" style={{ color: 'var(--text-muted)' }}>
          {demo.description}
        </p>

        <div>
          <p className="text-[10px] uppercase tracking-wider mb-1" style={{ color: 'var(--text-muted)' }}>
            How it works
          </p>
          <p className="text-xs leading-relaxed" style={{ color: 'var(--text)' }}>
            {demo.algorithmInfo}
          </p>
        </div>

        {demo.classicalVsQuantum && (
          <div className="p-3 rounded-lg" style={{ background: 'var(--elevated)' }}>
            <p className="text-[10px] uppercase tracking-wider mb-1" style={{ color: 'var(--text-muted)' }}>
              Classical vs Quantum
            </p>
            <p className="text-xs font-mono" style={{ color: 'var(--accent-lit)' }}>
              {demo.classicalVsQuantum}
            </p>
          </div>
        )}

        <div>
          <p className="text-[10px] uppercase tracking-wider mb-1" style={{ color: 'var(--text-muted)' }}>
            Expected state
          </p>
          <p className="text-xs font-mono p-2 rounded" style={{ background: 'var(--elevated)', color: '#86efac' }}>
            {demo.expectedState}
          </p>
        </div>
      </div>
    </div>
  );
}
