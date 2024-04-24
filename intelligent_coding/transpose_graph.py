import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Load the data
df = pd.read_csv('transpose_performance_data.txt')

# Pivot the data for easy plotting
pivot_df = df.pivot(index='MatrixSize', columns='Method', values='TimeInMicroseconds')

# Plotting
plt.figure(figsize=(10, 6))
for method in pivot_df.columns:
    plt.plot(pivot_df.index, pivot_df[method], marker='o', label=method)

plt.xlabel('Matrix Size')
plt.ylabel('Time in Microseconds')
plt.title('Performance of Matrix Transposition Methods')
plt.yscale('log')
plt.xscale('log')

# Generate x-ticks
x_ticks = np.logspace(np.log10(pivot_df.index.min()), np.log10(pivot_df.index.max()), num=10)
plt.xticks(x_ticks, labels=[str(int(x)) for x in x_ticks])

plt.legend()
plt.grid(True)
plt.show()