#!/usr/bin/env bash
# Build and run every benchmark in the benchmarks/ directory, printing the
# location of each CSV when it's finished.
#
# Run from the repository root:
#   bash benchmarks/run_all.sh
#
# Requires `make benchmarks` to have produced binaries under build/.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

mkdir -p benchmarks/results

if [ ! -x build/bench_gate_throughput ] \
   || [ ! -x build/bench_norm_drift ] \
   || [ ! -x build/bench_max_qubits ] \
   || [ ! -x build/bench_chsh_violation ]; then
    echo "Building benchmark binaries..."
    make benchmarks
fi

echo "=== bench_gate_throughput ==="
./build/bench_gate_throughput

echo
echo "=== bench_norm_drift ==="
./build/bench_norm_drift

echo
echo "=== bench_max_qubits ==="
./build/bench_max_qubits

echo
echo "=== bench_chsh_violation ==="
./build/bench_chsh_violation

echo
echo "All CSVs:"
ls -1 benchmarks/results/
