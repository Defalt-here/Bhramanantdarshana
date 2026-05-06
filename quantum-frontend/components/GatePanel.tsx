'use client';

import { useDrag } from 'react-dnd';
import { GateType } from '@/types/circuit';
import { GATE_INFO, GateInfo } from '@/lib/gateInfo';
import { DragItem } from './CircuitCanvas';
import { useState } from 'react';

interface DraggableGateProps {
  gateType: GateType;
  info: GateInfo;
  theta?: number;
  onHover: (g: GateType | null) => void;
}

function DraggableGate({ gateType, info, theta, onHover }: DraggableGateProps) {
  const [{ isDragging }, dragRef] = useDrag<DragItem, void, { isDragging: boolean }>(() => ({
    type: 'QGATE',
    item: { type: gateType, theta },
    collect: m => ({ isDragging: m.isDragging() }),
  }), [gateType, theta]);

  return (
    <div
      ref={(node: HTMLDivElement | null) => { if (node) dragRef(node); }}
      onMouseEnter={() => onHover(gateType)}
      onMouseLeave={() => onHover(null)}
      className={`gate-chip ${info.chipBg} ${info.chipText} cursor-grab`}
      style={{ opacity: isDragging ? 0.4 : 1, width: 44, height: 44, fontSize: '0.75rem' }}
      title={`Drag to place ${info.name}`}
    >
      {info.symbol}
    </div>
  );
}

interface RotationGateProps {
  gateType: GateType;
  info: GateInfo;
  onHover: (g: GateType | null) => void;
}

function RotationGate({ gateType, info, onHover }: RotationGateProps) {
  const [theta, setTheta] = useState(1.5708); // π/2 default
  const [{ isDragging }, dragRef] = useDrag<DragItem, void, { isDragging: boolean }>(() => ({
    type: 'QGATE',
    item: { type: gateType, theta },
    collect: m => ({ isDragging: m.isDragging() }),
  }), [theta]);

  const piLabel = (t: number) => {
    if (Math.abs(t - Math.PI) < 0.01) return 'π';
    if (Math.abs(t - Math.PI / 2) < 0.01) return 'π/2';
    if (Math.abs(t - Math.PI / 4) < 0.01) return 'π/4';
    return t.toFixed(2);
  };

  return (
    <div className="flex items-center gap-2">
      <div
        ref={(node: HTMLDivElement | null) => { if (node) dragRef(node); }}
        onMouseEnter={() => onHover(gateType)}
        onMouseLeave={() => onHover(null)}
        className={`gate-chip ${info.chipBg} ${info.chipText} cursor-grab flex-shrink-0`}
        style={{ opacity: isDragging ? 0.4 : 1, width: 44, height: 44, fontSize: '0.65rem' }}
        title={`Drag ${info.name} (θ=${piLabel(theta)})`}
      >
        {info.symbol}<br />{piLabel(theta)}
      </div>
      <input
        type="range"
        min={0} max={6.283} step={0.1}
        value={theta}
        onChange={e => setTheta(parseFloat(e.target.value))}
        className="flex-1 h-1 accent-violet-500"
        style={{ accentColor: 'var(--accent)' }}
        title="Rotation angle θ"
      />
    </div>
  );
}

interface GatePanelProps {
  onHover: (gateType: GateType | null) => void;
}

const SECTION: { label: string; types: GateType[] }[] = [
  { label: 'Pauli + Phase', types: ['H', 'X', 'Y', 'Z', 'S', 'T'] },
  { label: 'Rotations', types: ['RX', 'RY', 'RZ'] },
  { label: 'Two-qubit', types: ['CNOT', 'CZ', 'SWAP'] },
];

export default function GatePanel({ onHover }: GatePanelProps) {
  return (
    <div
      className="flex flex-col gap-5 p-4 rounded-xl thin-scroll overflow-y-auto"
      style={{ background: 'var(--surface)', border: '1px solid var(--border)' }}
    >
      <p className="text-xs font-semibold uppercase tracking-widest" style={{ color: 'var(--text-muted)' }}>
        Gate Library
      </p>

      {SECTION.map(({ label, types }) => (
        <div key={label}>
          <p className="text-[10px] uppercase tracking-wider mb-2" style={{ color: 'var(--text-muted)' }}>
            {label}
          </p>
          <div className="flex flex-col gap-2">
            {types.map(t => {
              const info = GATE_INFO[t];
              return info.hasAngle ? (
                <RotationGate key={t} gateType={t} info={info} onHover={onHover} />
              ) : (
                <div key={t} className="flex items-center gap-3">
                  <DraggableGate gateType={t} info={info} onHover={onHover} />
                  <span className="text-xs" style={{ color: 'var(--text-muted)' }}>{info.name}</span>
                </div>
              );
            })}
          </div>
        </div>
      ))}

      <div className="mt-2 p-3 rounded-lg text-[10px] leading-relaxed"
           style={{ background: 'var(--elevated)', color: 'var(--text-muted)' }}>
        <strong style={{ color: 'var(--text)' }}>Usage</strong><br />
        Drag any gate onto the circuit grid. For two-qubit gates (CNOT/CZ/SWAP) the first click sets the target, then click another qubit in the same column to set the control. Click a placed gate to remove it.
      </div>
    </div>
  );
}
