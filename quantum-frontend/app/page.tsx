import QuantumPlayground from '@/components/QuantumPlayground';

export const metadata = {
  title: 'Bhramanantdarshana — Quantum Simulator',
  description: 'Interactive quantum circuit playground powered by a C++ state-vector backend.',
};

export default function Home() {
  return (
    <div className="min-h-screen" style={{ background: 'var(--bg)' }}>
      {/* Top navigation bar */}
      <header
        className="flex items-center gap-4 px-6 py-3 border-b"
        style={{ background: 'var(--surface)', borderColor: 'var(--border)' }}
      >
        <span className="text-2xl">⚛</span>
        <div>
          <h1 className="text-base font-bold tracking-wide" style={{ color: 'var(--text)' }}>
            Bhramanantdarshana
          </h1>
          <p className="text-xs" style={{ color: 'var(--text-muted)' }}>
            Quantum circuit simulator · C++ state-vector backend
          </p>
        </div>
        <div className="ml-auto flex items-center gap-2">
          <span
            className="text-xs px-2 py-1 rounded-full"
            style={{ background: 'var(--elevated)', color: 'var(--accent-lit)' }}
          >
            C++ backend
          </span>
          <span
            className="text-xs px-2 py-1 rounded-full"
            style={{ background: 'var(--elevated)', color: 'var(--text-muted)' }}
          >
            kMaxQubits = 14
          </span>
        </div>
      </header>

      {/* Main playground */}
      <QuantumPlayground />
    </div>
  );
}
