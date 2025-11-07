'use client';

import { CircuitState } from '@/types/quantum';

interface QubitVisualizerProps {
  qubitStates: CircuitState[];
}

export default function QubitVisualizer({ qubitStates }: QubitVisualizerProps) {
  return (
    <div className="bg-white border-2 border-black rounded-none p-6">
      <h2 className="text-2xl font-extrabold text-black mb-4">Qubit States</h2>
      <div className="space-y-4">
        {qubitStates.map((state, index) => (
          <div key={index} className={`bg-white border-2 border-black p-4 ${state.entangled ? 'outline outline-2 outline-blue-600' : ''}`}>
            <div className="flex justify-between items-center mb-3">
              <div className="text-black font-extrabold">Qubit {index}</div>
              {state.entangled && (
                <div className="text-xs bg-black text-white px-2 py-1">
                  🔗 Entangled {state.entangledWith && state.entangledWith.length > 0 ? `with q[${state.entangledWith.join(', ')}]` : ''}
                </div>
              )}
            </div>
            
            {/* Bloch Sphere Visualization */}
            <div className="relative w-full h-32 mb-4">
              <svg viewBox="0 0 200 200" className="w-full h-full">
                {/* Sphere */}
                <circle cx="100" cy="100" r="80" fill="none" stroke="#000" strokeWidth="2" />
                
                {/* Axes */}
                <line x1="100" y1="20" x2="100" y2="180" stroke="#999" strokeWidth="1" />
                <line x1="20" y1="100" x2="180" y2="100" stroke="#999" strokeWidth="1" />
                
                {/* State vector */}
                <line 
                  x1="100" 
                  y1="100" 
                  x2={100 + Math.sin(state.beta * Math.PI) * 70} 
                  y2={100 - Math.cos(state.beta * Math.PI) * 70}
                  stroke={state.measured ? "#ef4444" : "#111111"}
                  strokeWidth="3"
                  markerEnd="url(#arrowhead)"
                />
                
                {/* Arrow head */}
                <defs>
                  <marker id="arrowhead" markerWidth="10" markerHeight="10" refX="9" refY="3" orient="auto">
                    <polygon points="0 0, 10 3, 0 6" fill={state.measured ? "#ef4444" : "#111111"} />
                  </marker>
                </defs>
                
                {/* Labels */}
                <text x="100" y="15" fill="#000" fontSize="12" textAnchor="middle">|0⟩</text>
                <text x="100" y="195" fill="#000" fontSize="12" textAnchor="middle">|1⟩</text>
              </svg>
            </div>

            {/* State probabilities */}
            <div className="space-y-2">
              <div>
                <div className="flex justify-between text-[10px] text-zinc-700 mb-1">
                  <span>|0⟩ Probability</span>
                  <span>{(state.alpha * state.alpha * 100).toFixed(1)}%</span>
                </div>
                <div className="w-full bg-zinc-200 h-2 border border-black">
                  <div 
                    className="bg-black h-2 transition-all duration-300"
                    style={{ width: `${state.alpha * state.alpha * 100}%` }}
                  />
                </div>
              </div>
              
              <div>
                <div className="flex justify-between text-[10px] text-zinc-700 mb-1">
                  <span>|1⟩ Probability</span>
                  <span>{(state.beta * state.beta * 100).toFixed(1)}%</span>
                </div>
                <div className="w-full bg-zinc-200 h-2 border border-black">
                  <div 
                    className="bg-zinc-700 h-2 transition-all duration-300"
                    style={{ width: `${state.beta * state.beta * 100}%` }}
                  />
                </div>
              </div>
            </div>

            {/* State notation */}
            <div className="mt-3 text-center">
              <div className="text-black text-sm font-mono bg-zinc-100 border border-black px-2 py-1 inline-block">
                {state.measured ? (
                  state.beta > 0.5 ? '|1⟩' : '|0⟩'
                ) : (
                  `${state.alpha.toFixed(2)}|0⟩ + ${state.beta.toFixed(2)}|1⟩`
                )}
              </div>
              {state.measured && (
                <div className="text-[10px] text-red-700 mt-1">Measured</div>
              )}
            </div>
          </div>
        ))}
      </div>

      {/* Legend */}
      <div className="mt-6 p-4 bg-white border-2 border-black">
        <h3 className="text-black font-bold mb-2 text-sm">Legend:</h3>
        <ul className="text-zinc-800 text-xs space-y-1">
          <li>• Dark bar: Probability of |0⟩</li>
          <li>• Gray bar: Probability of |1⟩</li>
          <li>• Black arrow: Superposition</li>
          <li>• Red arrow: Measured state</li>
        </ul>
      </div>
    </div>
  );
}
