'use client';

import { useState, useCallback } from 'react';
import GateLibrary from './GateLibrary';
import CircuitGrid from './CircuitGrid';
import QubitVisualizer from './QubitVisualizer';
import { Gate, CircuitState } from '@/types/quantum';

export default function QuantumCircuitBuilder() {
  const [numQubits, setNumQubits] = useState(3);
  const [circuit, setCircuit] = useState<Gate[][]>(
    Array(3).fill(null).map(() => Array(8).fill(null))
  );
  const [qubitStates, setQubitStates] = useState<CircuitState[]>(
    Array(3).fill({ alpha: 1, beta: 0, measured: false })
  );

  const handleNumQubitsChange = (num: number) => {
    setNumQubits(num);
    setCircuit(Array(num).fill(null).map(() => Array(8).fill(null)));
    setQubitStates(Array(num).fill({ alpha: 1, beta: 0, measured: false }));
  };

  const addGateToCircuit = useCallback((gate: Gate, qubitIndex: number, stepIndex: number) => {
    setCircuit(prev => {
      const newCircuit = prev.map(row => [...row]);
      newCircuit[qubitIndex][stepIndex] = gate;
      return newCircuit;
    });
  }, []);

  const removeGateFromCircuit = useCallback((qubitIndex: number, stepIndex: number) => {
    setCircuit(prev => {
      const newCircuit = prev.map(row => [...row]);
      newCircuit[qubitIndex][stepIndex] = null;
      return newCircuit;
    });
  }, []);

  const simulateCircuit = () => {
    // Simple simulation logic for educational purposes
    const newStates = [...qubitStates];
    
    circuit.forEach((qubitGates, qubitIndex) => {
      let state = { alpha: 1, beta: 0, measured: false };
      
      qubitGates.forEach(gate => {
        if (!gate) return;
        
        switch (gate.type) {
          case 'H':
            // Hadamard creates superposition
            state = { alpha: 0.707, beta: 0.707, measured: false };
            break;
          case 'X':
            // Pauli-X flips the state
            [state.alpha, state.beta] = [state.beta, state.alpha];
            break;
          case 'Y':
            // Pauli-Y
            [state.alpha, state.beta] = [-state.beta, state.alpha];
            break;
          case 'Z':
            // Pauli-Z adds phase
            state.beta = -state.beta;
            break;
          case 'M':
            // Measurement collapses to |0⟩ or |1⟩
            const prob = state.beta * state.beta;
            const collapsed = Math.random() < prob ? 1 : 0;
            state = collapsed === 0 
              ? { alpha: 1, beta: 0, measured: true }
              : { alpha: 0, beta: 1, measured: true };
            break;
        }
      });
      
      newStates[qubitIndex] = state;
    });
    
    setQubitStates(newStates);
  };

  const resetCircuit = () => {
    setCircuit(Array(numQubits).fill(null).map(() => Array(8).fill(null)));
    setQubitStates(Array(numQubits).fill({ alpha: 1, beta: 0, measured: false }));
  };

  return (
    <div className="grid grid-cols-1 lg:grid-cols-4 gap-6">
      {/* Gate Library */}
      <div className="lg:col-span-1">
        <GateLibrary />
      </div>

      {/* Circuit Builder */}
      <div className="lg:col-span-2">
        <div className="bg-white/10 backdrop-blur-lg rounded-xl p-6 shadow-2xl border border-purple-500/30">
          <div className="flex justify-between items-center mb-6">
            <h2 className="text-2xl font-bold text-white">Circuit Builder</h2>
            <div className="flex items-center gap-4">
              <label className="text-white">
                Qubits:
                <select 
                  value={numQubits}
                  onChange={(e) => handleNumQubitsChange(parseInt(e.target.value))}
                  className="ml-2 bg-purple-700 text-white px-3 py-1 rounded-lg"
                >
                  {[1, 2, 3, 4, 5].map(n => (
                    <option key={n} value={n}>{n}</option>
                  ))}
                </select>
              </label>
            </div>
          </div>

          <CircuitGrid
            numQubits={numQubits}
            circuit={circuit}
            onAddGate={addGateToCircuit}
            onRemoveGate={removeGateFromCircuit}
          />

          <div className="flex gap-4 mt-6">
            <button
              onClick={simulateCircuit}
              className="flex-1 bg-gradient-to-r from-purple-600 to-blue-600 hover:from-purple-700 hover:to-blue-700 text-white font-bold py-3 px-6 rounded-lg transition-all duration-200 shadow-lg hover:shadow-xl"
            >
              ▶ Run Simulation
            </button>
            <button
              onClick={resetCircuit}
              className="bg-red-600 hover:bg-red-700 text-white font-bold py-3 px-6 rounded-lg transition-all duration-200 shadow-lg hover:shadow-xl"
            >
              🔄 Reset
            </button>
          </div>
        </div>
      </div>

      {/* Qubit Visualizer */}
      <div className="lg:col-span-1">
        <QubitVisualizer qubitStates={qubitStates} />
      </div>
    </div>
  );
}
