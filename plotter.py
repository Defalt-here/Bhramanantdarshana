"""
Quantum Measurement Visualization Script

This script creates statistical visualizations of quantum measurement results
from quantum register collapse measurements. It reads measurement data from
CSV files and generates both bar charts and line plots to analyze the
distribution of quantum measurement outcomes.

Scientific Purpose:
- Visualize Born rule probability distributions
- Analyze quantum state measurement statistics
- Validate quantum algorithm implementations
- Compare theoretical vs. experimental probabilities

Data Format Expected:
CSV file 'collapse_measurements.csv' with columns:
- 'Measurement': Binary string representation of quantum states (e.g., "000", "101")
- 'Count': Number of times each state was measured

Features:
- Dual visualization (bars + line trend)
- Proper quantum state labeling on x-axis
- Statistical value annotations
- Professional scientific plotting style
- High-resolution output for publications

Author: Your Name
Date: 2025
Dependencies: pandas, matplotlib, seaborn
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Set style for better visualization
plt.style.use('classic')  # Classic matplotlib style for scientific plots

# Read the quantum measurement data from CSV file
# Expected format: Measurement,Count where Measurement is binary string (e.g., "000", "101")
df = pd.read_csv('collapse_measurements.csv')

# Validate data format
if 'Measurement' not in df.columns or 'Count' not in df.columns:
    raise ValueError("CSV must contain 'Measurement' and 'Count' columns")

print(f"Loaded {len(df)} unique quantum states from measurement data")
print(f"Total measurements: {df['Count'].sum()}")

# Create figure and axis objects with a single subplot
# Figure size chosen for optimal visibility of quantum state labels
fig, ax1 = plt.subplots(figsize=(12, 6))

# Create the bar plot representing measurement frequency distribution
# Each bar height corresponds to the number of times that quantum state was observed
bars = ax1.bar(range(len(df['Measurement'])), df['Count'], 
        color=sns.color_palette("husl", 8),     # Use color palette for visual distinction
        edgecolor='black',                      # Black borders for clarity
        width=0.6,                             # Bar width for optimal spacing
        alpha=0.7)                             # Transparency for overlaying line plot

# Create the line plot on the same axis to show measurement trends
# Useful for identifying patterns in quantum state probabilities
line = ax1.plot(range(len(df['Measurement'])), df['Count'], 
        color='red',                           # Prominent color for trend visibility
        linewidth=2,                           # Thick line for clarity
        marker='o',                            # Circle markers at data points
        linestyle='-',                         # Solid line connection
        label='Measurement Trend')             # Label for legend

# Customize the plot with scientific formatting and clear labeling
ax1.set_title('Quantum Register Measurement Distribution', fontsize=14, pad=15)
ax1.set_xlabel('Quantum State (Binary Representation)', fontsize=12)
ax1.set_ylabel('Number of Measurements', fontsize=12)

# Set the x-axis ticks to show the actual quantum states from the data
# This maps array indices to meaningful quantum state labels (e.g., "000", "101")
ax1.set_xticks(range(len(df['Measurement'])))
ax1.set_xticklabels(df['Measurement'], rotation=45)  # Rotate labels for readability

# Add value labels on top of each bar for precise reading
# Shows exact count values above bars to aid in data interpretation
for i, v in enumerate(df['Count']):
    ax1.text(i, v + 1, str(v), ha='center', fontsize=10)

# Add grid for better readability and data estimation
# Horizontal grid lines help estimate intermediate values
ax1.grid(True, axis='y', linestyle='--', alpha=0.7)

# Add legend to distinguish between bar and line representations
ax1.legend(loc='best')  # Automatically position legend for minimal overlap

# Adjust layout to prevent label cutoff
# Ensures all text elements are visible and properly positioned
plt.tight_layout()

# Save the plot as high-resolution image for scientific documentation
# PNG format chosen for lossless compression and wide compatibility
plt.savefig('quantum_measurements.png', 
           dpi=300,                    # High resolution for publication quality
           bbox_inches='tight',        # Minimize whitespace around plot
           facecolor='white')          # White background for clean appearance

# Display the interactive plot
plt.show()

# Optional: Print statistical summary for data analysis
print("\nStatistical Summary:")
print(f"Most frequent state: {df.loc[df['Count'].idxmax(), 'Measurement']} ({df['Count'].max()} occurrences)")
print(f"Least frequent state: {df.loc[df['Count'].idxmin(), 'Measurement']} ({df['Count'].min()} occurrences)")
print(f"Average measurements per state: {df['Count'].mean():.2f}")
print(f"Standard deviation: {df['Count'].std():.2f}")

# Calculate and display theoretical vs. experimental probabilities
total_measurements = df['Count'].sum()
print(f"\nExperimental Probabilities:")
for _, row in df.iterrows():
    probability = row['Count'] / total_measurements
    print(f"P(|{row['Measurement']}⟩) = {probability:.4f} ({row['Count']}/{total_measurements})")
