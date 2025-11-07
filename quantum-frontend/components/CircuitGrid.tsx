'use client';

import { useDrop } from 'react-dnd';
import { Gate } from '@/types/quantum';

interface CircuitSlotProps {
  qubitIndex: number;
  stepIndex: number;
  gate: Gate | null;
  onAddGate: (gate: Gate, qubitIndex: number, stepIndex: number) => void;
  onRemoveGate: (qubitIndex: number, stepIndex: number) => void;
}

function CircuitSlot({ qubitIndex, stepIndex, gate, onAddGate, onRemoveGate }: CircuitSlotProps) {
  const [{ isOver }, drop] = useDrop(() => ({
    accept: 'GATE',
    drop: (item: Gate) => onAddGate(item, qubitIndex, stepIndex),
    collect: (monitor) => ({
      isOver: !!monitor.isOver(),
    }),
  }));

  return (
    <div
      ref={drop}
      onClick={() => gate && onRemoveGate(qubitIndex, stepIndex)}
      className={`
        w-20 h-20 border-2 border-dashed rounded-lg flex items-center justify-center
        transition-all duration-200 cursor-pointer
        ${isOver ? 'border-yellow-400 bg-yellow-400/20 scale-105' : 'border-purple-400/40 bg-purple-900/20'}
        ${gate ? 'hover:bg-red-500/30' : 'hover:bg-purple-500/30'}
      `}
    >
      {gate && (
        <div className={`${gate.color} w-full h-full rounded flex items-center justify-center text-white font-bold text-xl shadow-lg`}>
          {gate.type}
        </div>
      )}
    </div>
  );
}

interface CircuitGridProps {
  numQubits: number;
  circuit: (Gate | null)[][];
  onAddGate: (gate: Gate, qubitIndex: number, stepIndex: number) => void;
  onRemoveGate: (qubitIndex: number, stepIndex: number) => void;
}

export default function CircuitGrid({ numQubits, circuit, onAddGate, onRemoveGate }: CircuitGridProps) {
  return (
    <div className="overflow-x-auto">
      <div className="inline-block min-w-full">
        {/* Column headers */}
        <div className="flex mb-2">
          <div className="w-24 flex items-center justify-center text-white font-semibold">
            Qubit
          </div>
          {[...Array(8)].map((_, i) => (
            <div key={i} className="w-20 ml-2 text-center text-purple-300 text-sm">
              Step {i + 1}
            </div>
          ))}
        </div>

        {/* Circuit rows */}
        {[...Array(numQubits)].map((_, qubitIndex) => (
          <div key={qubitIndex} className="flex items-center mb-4">
            {/* Qubit label */}
            <div className="w-24 flex items-center justify-center">
              <div className="bg-gradient-to-r from-purple-600 to-blue-600 text-white font-bold px-4 py-2 rounded-lg">
                q[{qubitIndex}]
              </div>
            </div>
            
            {/* Wire line */}
            <div className="flex-1 h-0.5 bg-purple-400/50 absolute ml-24" style={{ width: 'calc(100% - 6rem)' }} />
            
            {/* Gate slots */}
            <div className="flex gap-2 relative z-10">
              {circuit[qubitIndex].map((gate, stepIndex) => (
                <CircuitSlot
                  key={stepIndex}
                  qubitIndex={qubitIndex}
                  stepIndex={stepIndex}
                  gate={gate}
                  onAddGate={onAddGate}
                  onRemoveGate={onRemoveGate}
                />
              ))}
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
