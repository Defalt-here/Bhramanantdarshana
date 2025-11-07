'use client';

import { useState, useCallback } from 'react';
import GateLibrary from './GateLibrary';
import CircuitGridWithConnections from './CircuitGridWithConnections';
import QubitVisualizer from './QubitVisualizer';
import TemplateGallery from './TemplateGallery';
import Toast from './Toast';
import { Gate, CircuitState, Connection } from '@/types/quantum';
import { CircuitTemplate } from '@/types/templates';

export default function QuantumCircuitBuilder() {
  const [numQubits, setNumQubits] = useState(3);
  const [circuit, setCircuit] = useState<(Gate | null)[][]>(
    Array(3).fill(null).map(() => Array(8).fill(null))
  );
  const [connections, setConnections] = useState<Connection[]>([]);
  const [qubitStates, setQubitStates] = useState<CircuitState[]>(
    Array(3).fill({ alpha: 1, beta: 0, measured: false, entangled: false, entangledWith: [] })
  );
  const [pendingConnection, setPendingConnection] = useState<{
    gate: Gate;
    qubit: number;
    step: number;
  } | null>(null);
  const [toastMessage, setToastMessage] = useState<{ message: string; icon: string } | null>(null);
  const [currentTemplate, setCurrentTemplate] = useState<CircuitTemplate | null>(null);

  const handleNumQubitsChange = (num: number) => {
    setNumQubits(num);
    setCircuit(Array(num).fill(null).map(() => Array(8).fill(null)));
    setQubitStates(Array(num).fill({ alpha: 1, beta: 0, measured: false, entangled: false, entangledWith: [] }));
    setConnections([]);
    setPendingConnection(null);
  };

  const addGateToCircuit = useCallback((gate: Gate, qubitIndex: number, stepIndex: number) => {
    // Check if this is a two-qubit gate
    if (gate.type === 'CNOT' || gate.type === 'SWAP') {
      setPendingConnection({ gate, qubit: qubitIndex, step: stepIndex });
      // Add the gate to the circuit
      setCircuit(prev => {
        const newCircuit = prev.map(row => [...row]);
        newCircuit[qubitIndex][stepIndex] = gate;
        return newCircuit;
      });
    } else {
      // Single-qubit gate
      setCircuit(prev => {
        const newCircuit = prev.map(row => [...row]);
        newCircuit[qubitIndex][stepIndex] = gate;
        return newCircuit;
      });
    }
  }, []);

  const handleQubitClick = useCallback((qubitIndex: number, stepIndex: number) => {
    if (pendingConnection && 
        qubitIndex !== pendingConnection.qubit && 
        stepIndex === pendingConnection.step) {
      // Create the connection
      const newConnection: Connection = {
        controlQubit: pendingConnection.qubit,
        targetQubit: qubitIndex,
        stepIndex: pendingConnection.step,
        type: pendingConnection.gate.type as 'CNOT' | 'SWAP'
      };
      
      setConnections(prev => [...prev, newConnection]);
      
      // Add the gate to the target qubit as well
      setCircuit(prev => {
        const newCircuit = prev.map(row => [...row]);
        newCircuit[qubitIndex][stepIndex] = pendingConnection.gate;
        return newCircuit;
      });
      
      setPendingConnection(null);
    }
  }, [pendingConnection]);

  const removeGateFromCircuit = useCallback((qubitIndex: number, stepIndex: number) => {
    setCircuit(prev => {
      const newCircuit = prev.map(row => [...row]);
      newCircuit[qubitIndex][stepIndex] = null;
      return newCircuit;
    });
    
    // Remove any connections involving this position
    setConnections(prev => 
      prev.filter(conn => 
        !(conn.stepIndex === stepIndex && 
          (conn.controlQubit === qubitIndex || conn.targetQubit === qubitIndex))
      )
    );
    
    // Cancel pending connection if it matches
    if (pendingConnection?.qubit === qubitIndex && pendingConnection?.step === stepIndex) {
      setPendingConnection(null);
    }
  }, [pendingConnection]);

  const simulateCircuit = () => {
    const newStates = qubitStates.map(s => ({ ...s }));
    
    // Process each time step
    for (let step = 0; step < 8; step++) {
      // First apply single-qubit gates
      circuit.forEach((qubitGates, qubitIndex) => {
        const gate = qubitGates[step];
        if (!gate) return;
        
        // Skip two-qubit gates here, we'll handle them separately
        if (gate.type === 'CNOT' || gate.type === 'SWAP') return;
        
        let state = newStates[qubitIndex];
        
        switch (gate.type) {
          case 'H':
            state.alpha = (state.alpha + state.beta) / Math.sqrt(2);
            state.beta = (state.alpha - state.beta) / Math.sqrt(2);
            break;
          case 'X':
            [state.alpha, state.beta] = [state.beta, state.alpha];
            break;
          case 'Y':
            [state.alpha, state.beta] = [-state.beta, state.alpha];
            break;
          case 'Z':
            state.beta = -state.beta;
            break;
          case 'M':
            const prob = state.beta * state.beta;
            const collapsed = Math.random() < prob ? 1 : 0;
            state.alpha = collapsed === 0 ? 1 : 0;
            state.beta = collapsed === 1 ? 1 : 0;
            state.measured = true;
            break;
        }
      });
      
      // Then apply two-qubit gates
      connections
        .filter(conn => conn.stepIndex === step)
        .forEach(conn => {
          const control = newStates[conn.controlQubit];
          const target = newStates[conn.targetQubit];
          
          if (conn.type === 'CNOT') {
            // CNOT: if control is |1⟩, flip target
            if (control.beta > 0.5) {
              [target.alpha, target.beta] = [target.beta, target.alpha];
            }
            // Mark as entangled
            control.entangled = true;
            target.entangled = true;
            control.entangledWith = [conn.targetQubit];
            target.entangledWith = [conn.controlQubit];
          } else if (conn.type === 'SWAP') {
            // SWAP: exchange states
            [newStates[conn.controlQubit], newStates[conn.targetQubit]] = 
              [newStates[conn.targetQubit], newStates[conn.controlQubit]];
          }
        });
    }
    
    setQubitStates(newStates);
  };

  const resetCircuit = () => {
    setCircuit(Array(numQubits).fill(null).map(() => Array(8).fill(null)));
    setQubitStates(Array(numQubits).fill({ alpha: 1, beta: 0, measured: false, entangled: false, entangledWith: [] }));
    setConnections([]);
    setPendingConnection(null);
    setCurrentTemplate(null);
  };

  const loadTemplate = useCallback((template: CircuitTemplate) => {
    // Set the number of qubits first
    setNumQubits(template.numQubits);
    
    // Load the circuit
    setCircuit(template.circuit.map(row => [...row]));
    
    // Load connections
    setConnections([...template.connections]);
    
    // Reset qubit states
    setQubitStates(Array(template.numQubits).fill({ alpha: 1, beta: 0, measured: false, entangled: false, entangledWith: [] }));
    
    // Clear pending connection
    setPendingConnection(null);
    
    // Set current template for info display
    setCurrentTemplate(template);
    
    // Show notification
    setToastMessage({ 
      message: `Loaded: ${template.name}`, 
      icon: template.icon 
    });
  }, []);

  return (
  <div className="grid grid-cols-1 lg:grid-cols-4 gap-6">
      {/* Gate Library */}
      <div className="lg:col-span-1">
        <GateLibrary />
      </div>

      {/* Circuit Builder */}
      <div className="lg:col-span-2">
        <div className="bg-white border-2 border-black p-6 rounded-none">
          <div className="flex justify-between items-center mb-4">
            <h2 className="text-2xl font-extrabold text-black">Circuit Builder</h2>
            <div className="flex items-center gap-4">
              <label className="text-black font-semibold text-sm">
                Qubits:
                <select 
                  value={numQubits}
                  onChange={(e) => handleNumQubitsChange(parseInt(e.target.value))}
                  className="ml-2 bg-zinc-200 border-2 border-black text-black px-2 py-1 rounded-none focus:outline-none"
                >
                  {[1, 2, 3, 4, 5].map(n => (
                    <option key={n} value={n}>{n}</option>
                  ))}
                </select>
              </label>
            </div>
          </div>

          {pendingConnection && (
            <div className="mb-4 p-3 bg-yellow-200 border-2 border-black">
              <p className="text-black text-xs font-bold tracking-wide">
                PENDING: click another qubit in column {pendingConnection.step + 1} to finish {pendingConnection.gate.type}
              </p>
            </div>
          )}

          {currentTemplate && (
            <div className="mb-4 p-4 bg-zinc-100 border-2 border-black">
              <div className="flex items-start justify-between mb-2">
                <div className="flex items-center gap-2">
                  <span className="text-xl">{currentTemplate.icon}</span>
                  <h3 className="text-lg font-extrabold">{currentTemplate.name}</h3>
                </div>
                <button
                  onClick={() => setCurrentTemplate(null)}
                  className="text-black hover:bg-black hover:text-white px-2"
                >
                  ✕
                </button>
              </div>
              <p className="text-zinc-800 text-xs leading-relaxed">
                {currentTemplate.explanation}
              </p>
            </div>
          )}

          <CircuitGridWithConnections
            numQubits={numQubits}
            circuit={circuit}
            connections={connections}
            pendingConnection={pendingConnection}
            onAddGate={addGateToCircuit}
            onRemoveGate={removeGateFromCircuit}
            onQubitClick={handleQubitClick}
          />

          <div className="flex gap-4 mt-6">
            <button
              onClick={simulateCircuit}
              className="flex-1 bg-black text-white font-bold py-3 px-4 border-2 border-black rounded-none hover:bg-white hover:text-black transition-colors"
            >
              RUN ▶
            </button>
            <button
              onClick={resetCircuit}
              className="bg-white text-black font-bold py-3 px-4 border-2 border-black rounded-none hover:bg-red-500 hover:text-white transition-colors"
            >
              RESET ✕
            </button>
          </div>
        </div>
      </div>

      {/* Qubit Visualizer */}
      <div className="lg:col-span-1">
        <QubitVisualizer qubitStates={qubitStates} />
      </div>

      {/* Template Gallery (Floating Button) */}
      <TemplateGallery onLoadTemplate={loadTemplate} />

      {/* Toast Notification */}
      {toastMessage && (
        <Toast
          message={toastMessage.message}
          icon={toastMessage.icon}
          onClose={() => setToastMessage(null)}
        />
      )}
    </div>
  );
}
