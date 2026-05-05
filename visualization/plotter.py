"""
plotter.py — bar + line plot of measurement outcomes from a single-column CSV.

The CSV format produced by Register::WriteMeasurementsCSV is one outcome
per row, with the header line "outcome":

    outcome
    11
    00
    11
    ...

This script groups outcomes, counts them, and renders a histogram (bars)
with a frequency-curve overlay (line). Save path defaults to
data/<input_basename>_plot.png.

Usage:
    python visualization/plotter.py [csv_path]

If no path is given, defaults to data/bell_500_samples.csv.
"""

import os
import sys
from collections import Counter

import matplotlib

matplotlib.use("Agg")  # headless-safe backend
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


def main(argv):
    csv_path = argv[1] if len(argv) >= 2 else "data/bell_500_samples.csv"
    if not os.path.isfile(csv_path):
        print(f"error: {csv_path} not found", file=sys.stderr)
        return 1

    df = pd.read_csv(csv_path, dtype={"outcome": str})
    if "outcome" not in df.columns:
        print(
            f"error: expected a column named 'outcome' in {csv_path}, "
            f"got {list(df.columns)}",
            file=sys.stderr,
        )
        return 2

    counts = Counter(df["outcome"].tolist())
    # Sort outcomes lexicographically — equivalent to sorting by the
    # big-endian integer they represent, since all strings have the same
    # length.
    sorted_items = sorted(counts.items(), key=lambda kv: kv[0])
    labels = [k for k, _ in sorted_items]
    values = [v for _, v in sorted_items]

    sns.set_theme(style="whitegrid")
    fig, ax = plt.subplots(figsize=(max(6, len(labels) * 0.5), 4.5))
    bar_color = "#4C72B0"
    line_color = "#C44E52"

    ax.bar(labels, values, color=bar_color, alpha=0.8, label="counts")
    ax.plot(labels, values, color=line_color, marker="o", linewidth=2,
            label="frequency")
    ax.set_xlabel("Measurement outcome (big-endian)")
    ax.set_ylabel("Count")

    base = os.path.splitext(os.path.basename(csv_path))[0]
    title = f"{base} — {sum(values)} samples, {len(labels)} distinct outcomes"
    ax.set_title(title)
    ax.legend()
    plt.xticks(rotation=45)
    plt.tight_layout()

    out_path = os.path.join("data", f"{base}_plot.png")
    fig.savefig(out_path, dpi=150)
    print(f"Wrote {out_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
