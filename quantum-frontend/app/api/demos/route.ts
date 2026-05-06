/**
 * GET /api/demos
 * Returns the full list of embedded demo specs (without the long qsimProgram fields
 * to keep the listing light; individual programs are fetched via the simulate endpoint).
 */

import { NextResponse } from 'next/server';
import { DEMOS } from '@/lib/demos';

export function GET() {
  return NextResponse.json(
    DEMOS.map(({ qsimProgram: _p, ...rest }) => rest)
  );
}
