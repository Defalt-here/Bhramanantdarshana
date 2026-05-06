'use client';

import { useState, useCallback } from 'react';
import { HTML5Backend } from 'react-dnd-html5-backend';
import { DndProvider } from 'react-dnd';
import CircuitCanvas, { DragItem } from './CircuitCanvas';
import GatePanel from './GatePanel';
import DemoGallery from './DemoGallery';
import InfoPanel from './InfoPanel';
import StateViewer from './StateViewer';
import { PlacedGate, GateType, SimulationResult, InfoContext } from '@/types/circuit';
import { GATE_INFO } from '@/lib/gateInfo';
import { DemoSpec } from '@/lib/demos';
import { buildQsimProgram, parseQsim, simulate } from '@/lib/circuitUtils';

const NUM_STEPS = 24;

export default function QuantumPlayground() {
  // --- Circuit state ---
  const [numQubits, setNumQubits] = useState(3);
  const [gates, setGates] = useState<PlacedGate[]>([]);
  const [pendingGate, setPendingGate] = useState<{ type: GateType; step: number; qubit: number } | null>(null);
  const makeId = () => `g${Date.now()}_${Math.random().toString(36).slice(2)}`;

  // --- Simulation ---
  const [result, setResult] = useState<SimulationResult | null>(null);
  const [loading, setLoading] = useState(false);
  const [simError, setSimError] = useState<string | null>(null);

  // --- Info panel ---
  const [infoCtx, setInfoCtx] = useState<InfoContext>({ kind: 'none' });
  const [activeDemo, setActiveDemo] = useState<string | null>(null);

  // --- Toolbar state ---
  const [toast, setToast] = useState<string | null>(null);

  const showToast = (msg: string) => {
    setToast(msg);
    setTimeout(() => setToast(null), 2500);
  };

  // -- Gate drop handler --
  // Uses functional setState so it never closes over stale `gates`. (Previously
  // each drop saw the initial empty `gates` array and wiped any prior placement.)
  const handleDrop = useCallback((item: DragItem, qubit: number, step: number) => {
    const info = GATE_INFO[item.type];
    const id = makeId();
    setGates(prev => {
      const filtered = prev.filter(g => g.qubit !== qubit || g.step !== step);
      return [...filtered, { id, type: item.type, qubit, step, theta: item.theta }];
    });
    if (info.numQubits === 2) {
      setPendingGate({ type: item.type, step, qubit });
    } else {
      setPendingGate(null);
    }
  }, []);

  const handleSetControl = useCallback((qubit: number, step: number) => {
    setPendingGate(prevPending => {
      if (!prevPending || step !== prevPending.step || qubit === prevPending.qubit) {
        return prevPending;
      }
      setGates(prev => prev.map(g =>
        g.qubit === prevPending.qubit && g.step === prevPending.step
          ? { ...g, control: qubit }
          : g
      ));
      return null;
    });
  }, []);

  const handleRemove = useCallback((id: string) => {
    setGates(prev => prev.filter(g => g.id !== id));
    setPendingGate(null);
  }, []);

  // -- Run circuit --
  const runCircuit = async (program?: string) => {
    const prog = program ?? buildQsimProgram(numQubits, gates);
    if (!prog.trim()) { showToast('Circuit is empty'); return; }
    setLoading(true);
    setSimError(null);
    setResult(null);
    try {
      const r = await simulate(prog);
      setResult(r);
    } catch (e) {
      setSimError(String(e));
    } finally {
      setLoading(false);
    }
  };

  // -- Demo actions --
  const loadDemo = (demo: DemoSpec) => {
    const parsed = parseQsim(demo.qsimProgram);
    setNumQubits(Math.max(parsed.numQubits, demo.numQubits));
    setGates(parsed.gates);
    setPendingGate(null);
    setResult(null);
    setSimError(null);
    setActiveDemo(demo.id);
    setInfoCtx({ kind: 'demo', demoId: demo.id });
    const note = parsed.warnings.length
      ? ` (${parsed.warnings.length} warning${parsed.warnings.length === 1 ? '' : 's'})`
      : '';
    showToast(`Loaded ${demo.name} · ${parsed.gates.length} gates${note}`);
  };

  const runDemo = (demo: DemoSpec) => {
    setActiveDemo(demo.id);
    setInfoCtx({ kind: 'demo', demoId: demo.id });
    runCircuit(demo.qsimProgram);
  };

  const clearCircuit = () => {
    setGates([]);
    setPendingGate(null);
    setResult(null);
    setSimError(null);
  };

  const exportQsim = () => {
    const prog = buildQsimProgram(numQubits, gates);
    navigator.clipboard.writeText(prog).then(() => showToast('Copied .qsim to clipboard'));
  };

  const handleGateHover = (g: GateType | null) => {
    if (g) setInfoCtx({ kind: 'gate', gateType: g });
    else if (infoCtx.kind === 'gate') setInfoCtx({ kind: 'none' });
  };

  const handleDemoHover = (id: string | null) => {
    if (id) setInfoCtx({ kind: 'demo', demoId: id });
    else if (infoCtx.kind === 'demo') {
      setInfoCtx(activeDemo ? { kind: 'demo', demoId: activeDemo } : { kind: 'none' });
    }
  };

  return (
    <DndProvider backend={HTML5Backend}>
      <div
        className="grid h-[calc(100vh-52px)]"
        style={{ gridTemplateColumns: '240px 1fr 300px', gap: 0 }}
      >
        {/* ── Left sidebar ─────────────────────────────────────────── */}
        <div className="flex flex-col gap-4 p-3 overflow-y-auto thin-scroll"
             style={{ borderRight: '1px solid var(--border)' }}>
          <GatePanel onHover={handleGateHover} />
          <DemoGallery
            onLoad={loadDemo}
            onRun={runDemo}
            onHover={handleDemoHover}
            activeId={activeDemo}
          />
        </div>

        {/* ── Main circuit area ─────────────────────────────────────── */}
        <div className="flex flex-col overflow-hidden">
          {/* Toolbar */}
          <div className="flex items-center gap-2 px-4 py-2 flex-shrink-0"
               style={{ borderBottom: '1px solid var(--border)', background: 'var(--surface)' }}>
            <span className="text-xs" style={{ color: 'var(--text-muted)' }}>Qubits:</span>
            {[1,2,3,4,5,6].map(n => (
              <button
                key={n}
                onClick={() => { setNumQubits(n); clearCircuit(); }}
                className="w-6 h-6 rounded text-xs font-bold transition-colors"
                style={{
                  background: numQubits === n ? 'var(--accent)' : 'var(--elevated)',
                  color: numQubits === n ? '#fff' : 'var(--text-muted)',
                }}
              >
                {n}
              </button>
            ))}
            <div className="flex-1" />
            {pendingGate && (
              <span className="text-xs px-2 py-0.5 rounded-full animate-pulse"
                    style={{ background: 'rgba(245,158,11,0.2)', color: '#fbbf24' }}>
                Click a qubit in col {pendingGate.step + 1} to set control
              </span>
            )}
            <button onClick={clearCircuit}
                    className="text-xs px-3 py-1 rounded"
                    style={{ background: 'var(--elevated)', color: 'var(--text-muted)', border: '1px solid var(--border)' }}>
              Clear
            </button>
            <button onClick={exportQsim}
                    className="text-xs px-3 py-1 rounded"
                    style={{ background: 'var(--elevated)', color: 'var(--accent-lit)', border: '1px solid var(--border)' }}>
              Copy .qsim
            </button>
            <button
              onClick={() => runCircuit()}
              disabled={loading}
              className="text-xs px-4 py-1.5 rounded font-semibold transition-opacity"
              style={{ background: loading ? 'var(--elevated)' : 'var(--accent)', color: '#fff', opacity: loading ? 0.6 : 1 }}
            >
              {loading ? 'Running…' : '▶ Run'}
            </button>
          </div>

          {/* Circuit canvas */}
          <div className="flex-1 overflow-auto p-4 thin-scroll">
            <CircuitCanvas
              numQubits={numQubits}
              numSteps={NUM_STEPS}
              gates={gates}
              pendingGate={pendingGate}
              onDrop={handleDrop}
              onRemove={handleRemove}
              onSetControl={handleSetControl}
            />
          </div>

          {/* State viewer */}
          <div className="flex-shrink-0 p-4"
               style={{ borderTop: '1px solid var(--border)', maxHeight: '40vh', overflowY: 'auto' }}>
            <StateViewer result={result} loading={loading} error={simError} />
          </div>
        </div>

        {/* ── Right info panel ─────────────────────────────────────── */}
        <div className="p-3 overflow-y-auto thin-scroll"
             style={{ borderLeft: '1px solid var(--border)' }}>
          <InfoPanel context={infoCtx} />
        </div>
      </div>

      {/* Toast */}
      {toast && (
        <div
          className="fixed bottom-6 left-1/2 -translate-x-1/2 text-sm px-4 py-2 rounded-full shadow-lg"
          style={{ background: 'var(--accent)', color: '#fff', zIndex: 999 }}
        >
          {toast}
        </div>
      )}
    </DndProvider>
  );
}
