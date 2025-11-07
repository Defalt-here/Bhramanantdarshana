'use client';

import { useDrag } from 'react-dnd';
import { Gate } from '@/types/quantum';

const gates: Gate[] = [
  { type: 'H', name: 'Hadamard', color: 'bg-blue-200 border-2 border-black text-black', description: 'Creates superposition' },
  { type: 'X', name: 'Pauli-X', color: 'bg-red-200 border-2 border-black text-black', description: 'Quantum NOT gate' },
  { type: 'Y', name: 'Pauli-Y', color: 'bg-yellow-200 border-2 border-black text-black', description: 'Rotation around Y-axis' },
  { type: 'Z', name: 'Pauli-Z', color: 'bg-green-200 border-2 border-black text-black', description: 'Phase flip gate' },
  { type: 'CNOT', name: 'CNOT', color: 'bg-purple-200 border-2 border-black text-black', description: 'Controlled-NOT (2-qubit)' },
  { type: 'SWAP', name: 'SWAP', color: 'bg-pink-200 border-2 border-black text-black', description: 'Swap two qubits' },
  { type: 'M', name: 'Measure', color: 'bg-orange-200 border-2 border-black text-black', description: 'Measurement gate' },
];

function DraggableGate({ gate }: { gate: Gate }) {
  const [{ isDragging }, drag] = useDrag(() => ({
    type: 'GATE',
    item: gate,
    collect: (monitor) => ({
      isDragging: !!monitor.isDragging(),
    }),
  }));

  return (
    <div
      ref={(node) => { if (node) drag(node); }}
      className={`${gate.color} p-4 rounded-none cursor-move ${isDragging ? 'opacity-60' : 'opacity-100'}`}
    >
      <div className="text-black font-extrabold text-xl text-center mb-1">{gate.type}</div>
      <div className="text-black text-xs text-center">{gate.name}</div>
    </div>
  );
}

export default function GateLibrary() {
  return (
    <div className="bg-white border-2 border-black rounded-none p-6">
      <h2 className="text-2xl font-extrabold text-black mb-4">Quantum Gates</h2>
      <div className="space-y-4">
        {gates.map((gate) => (
          <div key={gate.type}>
            <DraggableGate gate={gate} />
            <p className="text-zinc-700 text-xs mt-1 px-1">{gate.description}</p>
          </div>
        ))}
      </div>
      <div className="mt-8 p-4 bg-zinc-100 border-2 border-black rounded-none">
        <h3 className="text-black font-bold mb-2">How to Use:</h3>
        <ul className="text-zinc-800 text-sm space-y-1">
          <li>• Drag gates to circuit</li>
          <li>• Click gate to remove</li>
          <li>• For CNOT/SWAP: drag to control qubit, then click target</li>
          <li>• Run simulation to see results</li>
        </ul>
      </div>
      <div className="mt-4 p-4 bg-zinc-100 border-2 border-black rounded-none">
        <h3 className="text-black font-bold mb-2 text-sm">Two-Qubit Gates:</h3>
        <p className="text-zinc-800 text-xs">
          CNOT and SWAP require two qubits. After dragging to the circuit, 
          the gate will prompt you to select a second qubit to connect.
        </p>
      </div>
    </div>
  );
}
