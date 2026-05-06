/**
 * POST /api/simulate
 *
 * Body: { program: string }  — a .qsim circuit description (see cli/PROTOCOL.md)
 *
 * Forks qsim_cli, pipes the program to its stdin (with an appended
 * `output_state <tmpfile>` command), reads the JSON state vector back, and
 * returns it to the caller.
 *
 * Response: { ok: true, bits: number, amplitudes: [number,number][] }
 *        or { ok: false, error: string }  (HTTP 500)
 *
 * The binary is resolved as:  $QSIM_CLI_PATH  or  <cwd>/../build/qsim_cli
 */

import { NextRequest, NextResponse } from 'next/server';
import { spawn } from 'child_process';
import { readFile, unlink } from 'fs/promises';
import path from 'path';
import os from 'os';

// Force the Node.js runtime — this route uses child_process and fs, which are
// unavailable in the Edge runtime.
export const runtime = 'nodejs';

const QSIM_CLI =
  process.env.QSIM_CLI_PATH ??
  path.join(process.cwd(), '..', 'build', 'qsim_cli');

function runQsim(program: string, outPath: string): Promise<void> {
  return new Promise((resolve, reject) => {
    const proc = spawn(QSIM_CLI, [], { stdio: ['pipe', 'pipe', 'pipe'] });
    let stderr = '';
    proc.stderr?.on('data', (d: Buffer) => { stderr += d.toString(); });
    proc.on('error', reject);
    proc.on('close', (code: number | null) => {
      if (code === 0) resolve();
      else reject(new Error(stderr.trim() || `qsim_cli exited with code ${code}`));
    });
    proc.stdin?.write(`${program}\noutput_state ${outPath}\n`);
    proc.stdin?.end();
  });
}

export async function POST(req: NextRequest) {
  let body: { program?: unknown };
  try {
    body = await req.json();
  } catch {
    return NextResponse.json({ ok: false, error: 'Invalid JSON body' }, { status: 400 });
  }

  if (typeof body.program !== 'string' || !body.program.trim()) {
    return NextResponse.json({ ok: false, error: 'Missing "program" field' }, { status: 400 });
  }

  const outPath = path.join(
    os.tmpdir(),
    `qsim_${Date.now()}_${Math.random().toString(36).slice(2)}.json`
  );

  try {
    await runQsim(body.program, outPath);
    const json = await readFile(outPath, 'utf-8');
    const data = JSON.parse(json) as { bits: number; amplitudes: [number, number][] };
    return NextResponse.json({ ok: true, bits: data.bits, amplitudes: data.amplitudes });
  } catch (err) {
    return NextResponse.json({ ok: false, error: String(err) }, { status: 500 });
  } finally {
    try { await unlink(outPath); } catch { /* ignore */ }
  }
}
