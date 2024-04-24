import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Load the data
df = pd.read_csv('performance_data.txt')

# Pivot the data for easy plotting
pivot_df = df.pivot(index='MatrixSize', columns='LoopOrder', values='TimeInMicroseconds')

# Plotting
plt.figure(figsize=(10, 6))
for column in pivot_df.columns:
    plt.plot(pivot_df.index, pivot_df[column], marker='o', label=column)

plt.xlabel('Matrix Size')
plt.ylabel('Time in Microseconds')
plt.title('Matrix Multiplication Performance by Loop Order')
plt.yscale('log')
plt.xscale('log')

x_ticks = np.logspace(np.log10(pivot_df.index.min()), np.log10(pivot_df.index.max()), num=10)
plt.xticks(x_ticks, labels=[str(int(x)) for x in x_ticks])
plt.legend()
plt.grid(True)
plt.show()