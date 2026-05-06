'use client';

import { DEMOS, DemoSpec } from '@/lib/demos';
import { useState } from 'react';

interface DemoGalleryProps {
  onLoad: (demo: DemoSpec) => void;
  onRun: (demo: DemoSpec) => void;
  onHover: (demoId: string | null) => void;
  activeId: string | null;
}

const CAT_LABELS: Record<string, string> = {
  algorithm:   '⚡ Algorithm',
  entanglement: '🔗 Entanglement',
  'gate-demo': '🎯 Gate demo',
  misc:        '🔬 Other',
};

export default function DemoGallery({ onLoad, onRun, onHover, activeId }: DemoGalleryProps) {
  const [filter, setFilter] = useState<string>('all');

  const categories = ['all', ...Array.from(new Set(DEMOS.map(d => d.category)))];
  const visible = filter === 'all' ? DEMOS : DEMOS.filter(d => d.category === filter);

  return (
    <div className="flex flex-col gap-3 thin-scroll overflow-y-auto"
         style={{ background: 'var(--surface)', border: '1px solid var(--border)', borderRadius: 12, padding: 16 }}>
      <p className="text-xs font-semibold uppercase tracking-widest" style={{ color: 'var(--text-muted)' }}>
        Demos
      </p>

      {/* Category filter pills */}
      <div className="flex flex-wrap gap-1.5">
        {categories.map(c => (
          <button
            key={c}
            onClick={() => setFilter(c)}
            className="text-[10px] px-2 py-0.5 rounded-full transition-colors"
            style={{
              background: filter === c ? 'var(--accent)' : 'var(--elevated)',
              color: filter === c ? '#fff' : 'var(--text-muted)',
              border: '1px solid var(--border)',
            }}
          >
            {c === 'all' ? 'All' : CAT_LABELS[c] ?? c}
          </button>
        ))}
      </div>

      {/* Demo cards */}
      <div className="space-y-2">
        {visible.map(demo => {
          const isActive = demo.id === activeId;
          return (
            <div
              key={demo.id}
              onMouseEnter={() => onHover(demo.id)}
              onMouseLeave={() => onHover(null)}
              className="p-3 rounded-lg transition-all"
              style={{
                background: isActive ? 'rgba(124,58,237,0.18)' : 'var(--elevated)',
                border: `1px solid ${isActive ? 'var(--accent)' : 'var(--border)'}`,
                cursor: 'default',
              }}
            >
              <div className="flex items-start gap-2 mb-2">
                <span className="text-base">{demo.icon}</span>
                <div className="flex-1 min-w-0">
                  <p className="text-xs font-semibold truncate" style={{ color: 'var(--text)' }}>
                    {demo.name}
                  </p>
                  <p className="text-[10px] mt-0.5" style={{ color: 'var(--text-muted)' }}>
                    {demo.numQubits} qubits · {demo.tagline}
                  </p>
                </div>
              </div>
              <div className="flex gap-1.5">
                <button
                  onClick={() => onLoad(demo)}
                  className="flex-1 text-[10px] py-1 rounded font-medium transition-colors"
                  style={{ background: 'var(--surface)', color: 'var(--accent-lit)', border: '1px solid var(--border)' }}
                >
                  Load in playground
                </button>
                <button
                  onClick={() => onRun(demo)}
                  className="flex-1 text-[10px] py-1 rounded font-medium transition-colors"
                  style={{ background: 'var(--accent)', color: '#fff' }}
                >
                  ▶ Run
                </button>
              </div>
            </div>
          );
        })}
      </div>
    </div>
  );
}
