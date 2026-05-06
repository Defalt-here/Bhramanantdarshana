'use client';

import { useDrop } from 'react-dnd';
import { PlacedGate, GateType } from '@/types/circuit';
import { GATE_INFO } from '@/lib/gateInfo';
import { useRef, useCallback, useId } from 'react';

const CELL_W = 56;
const CELL_H = 56;
const LABEL_W = 64;

export interface DragItem { type: GateType; theta?: number; }

interface DropCellProps {
  qubit: number;
  step: number;
  gate: PlacedGate | undefined;
  isPendingTarget: boolean;
  onDrop: (item: DragItem, qubit: number, step: number) => void;
  onRemove: (id: string) => void;
  onClick: () => void;
}

function DropCell({ qubit, step, gate, isPendingTarget, onDrop, onRemove, onClick }: DropCellProps) {
  const [{ isOver }, dropRef] = useDrop<DragItem, void, { isOver: boolean }>(() => ({
    accept: 'QGATE',
    drop: (item) => onDrop(item, qubit, step),
    collect: m => ({ isOver: m.isOver() }),
  }), [onDrop, qubit, step]);

  const handleClick = () => {
    if (gate) onRemove(gate.id);
    else if (isPendingTarget) onClick();
  };

  const info = gate ? GATE_INFO[gate.type] : null;

  return (
    <div
      ref={(node: HTMLDivElement | null) => { if (node) dropRef(node); }}
      onClick={handleClick}
      className={`drop-cell${isOver ? ' over' : ''}${isPendingTarget ? ' pending-target' : ''}`}
      title={gate ? `Click to remove ${GATE_INFO[gate.type].name}` : isPendingTarget ? 'Click to set control here' : ''}
    >
      {gate && info ? (
        <div className={`gate-chip ${info.chipBg} ${info.chipText}`}>
          {gate.type === 'CNOT' ? '⊕' :
           gate.type === 'SWAP' ? '↔' :
           gate.type === 'CZ' ? 'Z•' :
           gate.theta !== undefined ? `${gate.type.slice(1)}(${(gate.theta).toFixed(1)})` :
           gate.type}
          {/* control marker shown on the control qubit side */}
          {gate.control != null && gate.qubit === qubit && (
            <span className="absolute -top-1 -right-1 w-3 h-3 rounded-full bg-white opacity-70 text-black text-[7px] flex items-center justify-center">t</span>
          )}
        </div>
      ) : isPendingTarget ? (
        <div className="w-8 h-8 rounded-full border-2 border-dashed flex items-center justify-center"
             style={{ borderColor: 'rgba(245,158,11,0.7)', color: 'rgba(245,158,11,0.9)', fontSize: 9 }}>
          ctrl
        </div>
      ) : null}
    </div>
  );
}

interface CircuitCanvasProps {
  numQubits: number;
  numSteps: number;
  gates: PlacedGate[];
  pendingGate: { type: GateType; step: number; qubit: number } | null;
  onDrop: (item: DragItem, qubit: number, step: number) => void;
  onRemove: (id: string) => void;
  onSetControl: (qubit: number, step: number) => void;
}

export default function CircuitCanvas({
  numQubits, numSteps, gates, pendingGate, onDrop, onRemove, onSetControl
}: CircuitCanvasProps) {
  const canvasRef = useRef<HTMLDivElement>(null);

  const getGate = useCallback(
    (q: number, s: number) => gates.find(g => g.qubit === q && g.step === s),
    [gates]
  );

  const totalW = LABEL_W + numSteps * CELL_W;
  const totalH = numQubits * CELL_H;

  const twoQubitGates = gates.filter(g =>
    (g.type === 'CNOT' || g.type === 'CZ' || g.type === 'SWAP') && g.control != null
  );
  const toffoliGates = gates.filter(g =>
    g.type === 'TOFFOLI' && g.control != null && g.control2 != null
  );

  return (
    <div className="overflow-x-auto thin-scroll pb-2">
      <div ref={canvasRef} style={{ minWidth: totalW + 16 }}>
        {/* Step headers */}
        <div className="flex mb-1" style={{ paddingLeft: LABEL_W }}>
          {Array.from({ length: numSteps }, (_, s) => (
            <div key={s} className="text-center text-[10px]"
                 style={{ width: CELL_W, color: 'var(--text-muted)' }}>
              {s + 1}
            </div>
          ))}
        </div>

        {/* Qubit rows + SVG overlay */}
        <div className="relative">
          {Array.from({ length: numQubits }, (_, q) => (
            <div key={q} className="flex items-center" style={{ height: CELL_H }}>
              {/* Qubit label */}
              <div
                className="flex items-center justify-center text-xs font-mono font-semibold flex-shrink-0"
                style={{ width: LABEL_W, color: 'var(--accent-lit)' }}
              >
                |q{q}⟩
              </div>

              {/* Wire + cells */}
              <div className="relative flex">
                <div className="qubit-wire" />
                {Array.from({ length: numSteps }, (_, s) => {
                  const g = getGate(q, s);
                  const isControlHere =
                    twoQubitGates.some(tg => tg.control === q && tg.step === s) ||
                    toffoliGates.some(tg => (tg.control === q || tg.control2 === q) && tg.step === s);
                  const isPendingTarget =
                    pendingGate !== null &&
                    pendingGate.step === s &&
                    q !== pendingGate.qubit;

                  return (
                    <div key={s} style={{ position: 'relative', zIndex: 10 }}>
                      <DropCell
                        qubit={q} step={s}
                        gate={g}
                        isPendingTarget={isPendingTarget}
                        onDrop={onDrop}
                        onRemove={onRemove}
                        onClick={() => onSetControl(q, s)}
                      />
                      {/* Control dot for two-qubit gates */}
                      {isControlHere && !g && (
                        <div
                          style={{ position: 'absolute', top: '50%', left: '50%',
                            transform: 'translate(-50%, -50%)',
                            width: 12, height: 12, borderRadius: '50%',
                            background: '#a78bfa', zIndex: 20, pointerEvents: 'none' }}
                        />
                      )}
                    </div>
                  );
                })}
              </div>
            </div>
          ))}

          {/* SVG vertical connections for 2-qubit gates */}
          <svg
            style={{ position: 'absolute', top: 0, left: 0,
              width: LABEL_W + numSteps * CELL_W, height: totalH,
              pointerEvents: 'none', zIndex: 5 }}
          >
            {twoQubitGates.map((g) => {
              if (g.control == null) return null;
              const x = LABEL_W + g.step * CELL_W + CELL_W / 2;
              const y1 = g.control * CELL_H + CELL_H / 2;
              const y2 = g.qubit * CELL_H + CELL_H / 2;
              const color = GATE_INFO[g.type].chipBg.includes('emerald')
                ? '#10b981' : GATE_INFO[g.type].chipBg.includes('teal')
                ? '#14b8a6' : '#06b6d4';
              return (
                <g key={g.id}>
                  <line x1={x} y1={y1} x2={x} y2={y2} stroke={color} strokeWidth={2} />
                  <circle cx={x} cy={y1} r={5} fill={color} />
                </g>
              );
            })}

            {/* Three-qubit Toffoli connections */}
            {toffoliGates.map(g => {
              if (g.control == null || g.control2 == null) return null;
              const x = LABEL_W + g.step * CELL_W + CELL_W / 2;
              const ys = [g.control, g.control2, g.qubit].map(q => q * CELL_H + CELL_H / 2);
              const yMin = Math.min(...ys);
              const yMax = Math.max(...ys);
              const color = '#84cc16';  // lime-500
              return (
                <g key={g.id}>
                  <line x1={x} y1={yMin} x2={x} y2={yMax} stroke={color} strokeWidth={2} />
                  <circle cx={x} cy={ys[0]} r={5} fill={color} />
                  <circle cx={x} cy={ys[1]} r={5} fill={color} />
                </g>
              );
            })}

            {/* Pending gate dashed line */}
            {pendingGate && (
              <line
                x1={LABEL_W + pendingGate.step * CELL_W + CELL_W / 2}
                y1={pendingGate.qubit * CELL_H + CELL_H / 2}
                x2={LABEL_W + pendingGate.step * CELL_W + CELL_W / 2}
                y2={pendingGate.qubit * CELL_H + CELL_H / 2 + 30}
                stroke="rgba(245,158,11,0.6)" strokeWidth={2}
                strokeDasharray="4 3"
              />
            )}
          </svg>
        </div>
      </div>
    </div>
  );
}
