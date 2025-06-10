import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Set style for better visualization
plt.style.use('classic')

# Read the CSV file
df = pd.read_csv('collapse_measurements.csv')

# Create figure and axis objects with a single subplot
fig, ax1 = plt.subplots(figsize=(12, 6))

# Create the bar plot
bars = ax1.bar(range(len(df['Measurement'])), df['Count'], 
        color=sns.color_palette("husl", 8),
        edgecolor='black',
        width=0.6,
        alpha=0.7)  # Make bars slightly transparent

# Create the line plot on the same axis
line = ax1.plot(range(len(df['Measurement'])), df['Count'], 
        color='red', linewidth=2, marker='o', 
        linestyle='-', label='Measurement Trend')

# Customize the plot
ax1.set_title('Quantum Register Measurement Distribution', fontsize=14, pad=15)
ax1.set_xlabel('Quantum State', fontsize=12)
ax1.set_ylabel('Number of Measurements', fontsize=12)

# Set the x-axis ticks to show the actual quantum states
ax1.set_xticks(range(len(df['Measurement'])))
ax1.set_xticklabels(df['Measurement'], rotation=45)

# Add value labels on top of each bar
for i, v in enumerate(df['Count']):
    ax1.text(i, v + 1, str(v), ha='center', fontsize=10)

# Add grid for better readability
ax1.grid(True, axis='y', linestyle='--', alpha=0.7)

# Add legend on the left
ax1.legend(loc='best', bbox_to_anchor=(-.15, 0.5))

# Adjust layout to prevent label cutoff
plt.tight_layout()

# Save the plot
plt.savefig('quantum_measurements.png', dpi=300, bbox_inches='tight')

# Display the plot
plt.show()
