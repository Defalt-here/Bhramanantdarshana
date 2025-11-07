'use client';

import { useDrop } from 'react-dnd';
import { Gate, Connection } from '@/types/quantum';

interface CircuitSlotProps {
  qubitIndex: number;
  stepIndex: number;
  gate: Gate | null;
  isPending: boolean;
  isConnected: boolean;
  onAddGate: (gate: Gate, qubitIndex: number, stepIndex: number) => void;
  onRemoveGate: (qubitIndex: number, stepIndex: number) => void;
  onQubitClick: (qubitIndex: number, stepIndex: number) => void;
}

function CircuitSlot({ qubitIndex, stepIndex, gate, isPending, isConnected, onAddGate, onRemoveGate, onQubitClick }: CircuitSlotProps) {
  const [{ isOver }, drop] = useDrop(() => ({
    accept: 'GATE',
    drop: (item: Gate) => onAddGate(item, qubitIndex, stepIndex),
    collect: (monitor) => ({
      isOver: !!monitor.isOver(),
    }),
  }));

  const handleClick = () => {
    if (gate) {
      onRemoveGate(qubitIndex, stepIndex);
    } else if (isPending) {
      onQubitClick(qubitIndex, stepIndex);
    }
  };

  return (
    <div
      ref={(node: HTMLDivElement | null) => { if (node) drop(node); }}
      onClick={handleClick}
      className={`
        w-20 h-20 border-2 border-black rounded-none flex items-center justify-center
        cursor-pointer relative bg-white
        ${isPending ? 'bg-yellow-100' : ''}
        ${isOver ? 'outline outline-2 outline-yellow-500' : ''}
        ${isConnected ? 'outline outline-2 outline-blue-600' : ''}
      `}
    >
      {gate && (
        <div className={`${gate.color} w-full h-full rounded-none flex items-center justify-center font-extrabold text-xl relative z-10`}> 
          {gate.type === 'CNOT' ? '⊕' : gate.type === 'SWAP' ? '×' : gate.type}
        </div>
      )}
      {isPending && !gate && (
        <div className="text-yellow-700 text-[10px] text-center font-bold">
          CLICK
        </div>
      )}
    </div>
  );
}

interface CircuitGridWithConnectionsProps {
  numQubits: number;
  circuit: (Gate | null)[][];
  connections: Connection[];
  pendingConnection: { gate: Gate; qubit: number; step: number } | null;
  onAddGate: (gate: Gate, qubitIndex: number, stepIndex: number) => void;
  onRemoveGate: (qubitIndex: number, stepIndex: number) => void;
  onQubitClick: (qubitIndex: number, stepIndex: number) => void;
}

export default function CircuitGridWithConnections({ 
  numQubits, 
  circuit, 
  connections, 
  pendingConnection,
  onAddGate, 
  onRemoveGate,
  onQubitClick 
}: CircuitGridWithConnectionsProps) {
  
  const isSlotConnected = (qubitIndex: number, stepIndex: number) => {
    return connections.some(conn => 
      conn.stepIndex === stepIndex && 
      (conn.controlQubit === qubitIndex || conn.targetQubit === qubitIndex)
    );
  };

  const isSlotPending = (qubitIndex: number, stepIndex: number) => {
    return pendingConnection !== null && 
           pendingConnection.step === stepIndex && 
           qubitIndex !== pendingConnection.qubit;
  };

  return (
    <div className="overflow-x-auto">
      <div className="inline-block min-w-full relative">
        {/* Column headers */}
        <div className="flex mb-2 text-black">
          <div className="w-24 flex items-center justify-center font-bold">
            Qubit
          </div>
          {[...Array(8)].map((_, i) => (
            <div key={i} className="w-20 ml-2 text-center text-xs">
              {i + 1}
            </div>
          ))}
        </div>

        {/* Circuit rows */}
        <div className="relative">
          {[...Array(numQubits)].map((_, qubitIndex) => (
            <div key={qubitIndex} className="flex items-center mb-4 relative">
              {/* Qubit label */}
              <div className="w-24 flex items-center justify-center z-10">
                <div className="border-2 border-black bg-zinc-100 text-black font-extrabold px-3 py-1">
                  q[{qubitIndex}]
                </div>
              </div>
              {/* Gate slots */}
              <div className="flex gap-2 relative z-10 pb-2 border-b-2 border-black">
                {circuit[qubitIndex].map((gate, stepIndex) => (
                  <CircuitSlot
                    key={stepIndex}
                    qubitIndex={qubitIndex}
                    stepIndex={stepIndex}
                    gate={gate}
                    isPending={isSlotPending(qubitIndex, stepIndex)}
                    isConnected={isSlotConnected(qubitIndex, stepIndex)}
                    onAddGate={onAddGate}
                    onRemoveGate={onRemoveGate}
                    onQubitClick={onQubitClick}
                  />
                ))}
              </div>
            </div>
          ))}

          {/* Draw connection lines */}
          <svg className="absolute top-0 left-0 w-full h-full pointer-events-none z-0">
            {connections.map((conn, idx) => {
              const x = 24 + 6 + conn.stepIndex * 88 + 40; // Calculate x position
              const y1 = conn.controlQubit * 96 + 48; // Control qubit y position
              const y2 = conn.targetQubit * 96 + 48; // Target qubit y position
              
              return (
                <g key={idx}>
                  {/* Connection line */}
                  <line
                    x1={x}
                    y1={y1}
                    x2={x}
                    y2={y2}
                    stroke="#000000"
                    strokeWidth="3"
                  />
                  {/* Control dot */}
                  <circle
                    cx={x}
                    cy={y1}
                    r="5"
                    fill="#000000"
                    stroke="#000000"
                    strokeWidth="1"
                  />
                  {/* Target indicator */}
                  {conn.type === 'CNOT' && (
                    <circle
                      cx={x}
                      cy={y2}
                      r="10"
                      fill="none"
                      stroke="#000000"
                      strokeWidth="3"
                    />
                  )}
                  {conn.type === 'SWAP' && (
                    <>
                      <circle
                        cx={x}
                        cy={y2}
                        r="5"
                        fill="#000000"
                        stroke="#000000"
                        strokeWidth="1"
                      />
                    </>
                  )}
                </g>
              );
            })}
          </svg>
        </div>
      </div>
    </div>
  );
}
