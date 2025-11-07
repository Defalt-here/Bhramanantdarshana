import QuantumCircuitBuilder from '@/components/QuantumCircuitBuilderV2';

export default function Home() {
  return (
    <main className="min-h-screen bg-zinc-50 text-black">
      <div className="max-w-7xl mx-auto px-4 py-8">
        <header className="mb-8">
          <h1 className="text-4xl font-extrabold tracking-tight">
            Quantum Circuit Simulator
          </h1>
          <p className="text-base text-zinc-700 mt-2">
            Interactive educational tool for quantum computing
          </p>
        </header>
        <QuantumCircuitBuilder />
      </div>
    </main>
  );
}
