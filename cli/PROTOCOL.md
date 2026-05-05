# qsim_cli protocol

`qsim_cli` reads a quantum circuit description from `stdin` and executes
it against the in-process state-vector simulator. It is intended as a
language-agnostic bridge — Python wrappers, GUI frontends, and the Qiskit
cross-validation script all talk to the simulator through this protocol.

The grammar described here is referred to as the **`.qsim` format**.

## Indexing

All qubit indices are **big-endian**: qubit 0 is the most-significant bit
of the state-vector index, matching the convention defined in
[`include/Conventions.h`](../include/Conventions.h).

## Grammar

One command per line. Empty lines and lines whose first non-whitespace
character is `#` are ignored. Tokens are separated by whitespace.

```
qubits N                      # declare register size; must come first
H q                           # Hadamard on qubit q
X q | Y q | Z q | S q | T q   # Pauli + phase gates on qubit q
RX q theta                    # rotation about X-axis by theta (radians)
RY q theta
RZ q theta
CNOT c t                      # CNOT, control c, target t
CZ   c t
SWAP c t
TOFFOLI c1 c2 t               # CCNOT (Toffoli)
print                         # print state to stderr (in Dirac notation)
measure_all NSAMPLES outfile.csv
                              # NSAMPLES Born-rule samples → CSV
output_state outfile.json     # full state vector → JSON
```

Notes.

- `qubits N` is mandatory and must precede any gate command.
- `1 ≤ N ≤ kMaxQubits` (currently 14); larger values are rejected.
- `theta` is in radians.
- `print` writes to stdout; everything else either writes to a file or
  produces no output on success. Errors go to stderr.

## Exit status

```
0  — every command parsed and executed
≠0 — a parse or semantic error (a diagnostic is printed to stderr)
```

## Output formats

### measure_all → CSV

```
outcome
0110
1101
…
```

Single column with header `outcome`. Each row is the bit-string outcome of
one Born-rule sample (taken via `MeasureWithoutCollapse`, so the register
state is unchanged across the call). Bit-strings are big-endian: the
leftmost character is qubit 0.

### output_state → JSON

```json
{
  "bits": 2,
  "amplitudes": [
    [0.7071067811865476, 0.0],
    [0.0,                0.0],
    [0.0,                0.0],
    [0.7071067811865476, 0.0]
  ]
}
```

Schema:

| Field        | Type                       | Meaning                                                           |
|--------------|----------------------------|-------------------------------------------------------------------|
| `bits`       | integer                    | Number of qubits (`n`).                                           |
| `amplitudes` | array of `[real, imag]`    | `2^n` complex amplitudes, indexed in big-endian order.            |

The amplitude at JSON index `i` is the amplitude of basis state `|i⟩`
where qubit 0 is the most-significant bit of `i`. The example above is
the Bell state `|Φ⁺⟩ = (|00⟩ + |11⟩)/√2`.

Real and imaginary parts are written with `std::setprecision(17)`, so a
double round-trips losslessly through the JSON.

## Example session

```text
$ cat <<'EOF' | ./build/qsim_cli
# Bell |Φ⁺⟩ preparation
qubits 2
H 0
CNOT 0 1
print
output_state /tmp/bell_state.json
measure_all 1000 /tmp/bell_samples.csv
EOF
|ψ⟩ = (0.707107,0)|00⟩ + (0.707107,0)|11⟩
```
