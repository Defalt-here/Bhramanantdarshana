'use client';

import { useState } from 'react';
import { quantumTemplates, CircuitTemplate } from '@/types/templates';

interface TemplateGalleryProps {
  onLoadTemplate: (template: CircuitTemplate) => void;
}

export default function TemplateGallery({ onLoadTemplate }: TemplateGalleryProps) {
  const [open, setOpen] = useState(false);
  const [filter, setFilter] = useState<string>('all');

  const categories = ['all', 'algorithm', 'entanglement', 'gate-demo', 'measurement'] as const;
  const visibleTemplates = filter === 'all'
    ? quantumTemplates
    : quantumTemplates.filter((t) => t.category === filter);

  return (
    <div className="fixed bottom-6 right-6 z-40">
      <button
        onClick={() => setOpen(true)}
        className="px-5 py-2 bg-white border-2 border-black text-black font-bold text-sm hover:bg-black hover:text-white transition flex items-center gap-2"
      >
        <span>⚡</span>
        Templates
      </button>
      {open && (
        <div
          className="fixed inset-0 bg-black/30 flex items-end sm:items-center justify-center z-50"
          onClick={() => setOpen(false)}
        >
          <div
            className="bg-white border-2 border-black w-full sm:max-w-4xl max-h-[85vh] flex flex-col overflow-hidden"
            onClick={(e) => e.stopPropagation()}
          >
            <div className="px-6 py-3 border-b-2 border-black flex items-center justify-between bg-white">
              <h2 className="text-lg font-extrabold tracking-wide flex items-center gap-2">
                <span>⚡</span> Circuit Templates
              </h2>
              <button
                onClick={() => setOpen(false)}
                className="text-black font-bold hover:bg-black hover:text-white px-2"
              >
                ✕
              </button>
            </div>
            <div className="flex flex-col sm:flex-row flex-1 overflow-hidden">
              <aside className="sm:w-64 border-r-2 border-black p-4 space-y-4 bg-white">
                <div className="space-y-2">
                  <p className="text-xs font-extrabold uppercase text-black">Categories</p>
                  <div className="flex flex-wrap gap-2">
                    {categories.map((cat) => (
                      <button
                        key={cat}
                        onClick={() => setFilter(cat)}
                        className={`px-3 py-1 text-xs font-bold border-2 ${
                          filter === cat
                            ? 'bg-black text-white'
                            : 'bg-white text-black hover:bg-black hover:text-white'
                        }`}
                      >
                        {cat}
                      </button>
                    ))}
                  </div>
                </div>
                <div className="text-xs text-black leading-relaxed bg-white border-2 border-black p-3">
                  <p className="font-extrabold mb-1">Usage Tips:</p>
                  <ul className="list-disc ml-4 space-y-1">
                    <li>Click Load to insert a template.</li>
                    <li>Connections auto-draw after load.</li>
                    <li>Modify gates to explore variations.</li>
                    <li>Bell / GHZ show entanglement basics.</li>
                  </ul>
                </div>
              </aside>
              <main className="flex-1 overflow-y-auto p-6 bg-white">
                <div className="grid sm:grid-cols-2 lg:grid-cols-3 gap-4">
                  {visibleTemplates.map((tpl) => (
                    <div key={tpl.id} className="group bg-white border-2 border-black p-4 flex flex-col">
                      <div className="flex items-start justify-between mb-2">
                        <h3 className="font-extrabold text-sm tracking-wide flex items-center gap-2">
                          <span>{tpl.icon}</span> {tpl.name}
                        </h3>
                        <span className="text-[10px] font-black uppercase tracking-wider bg-white border-2 border-black px-2 py-0.5">
                          {tpl.category}
                        </span>
                      </div>
                      <p className="text-xs text-black flex-1 leading-relaxed mb-3">{tpl.description}</p>
                      <button
                        onClick={() => {
                          onLoadTemplate(tpl);
                          setOpen(false);
                        }}
                        className="mt-auto text-xs font-bold px-3 py-2 bg-black text-white border-2 border-black hover:bg-white hover:text-black transition"
                      >
                        Load →
                      </button>
                    </div>
                  ))}
                </div>
              </main>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
