import glob
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# --- Configurable axes domains (as you specified) ---
ITEMS_WHITELIST = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
                   8192, 16384, 32768, 65536, 131072}
THREADS_WHITELIST = {1, 2, 4, 6, 12}

# --- Discover files like results_{items}_{threads}.csv ---
file_pattern = "results_*_*.csv"
files = glob.glob(file_pattern)

# Data structure:
# data[test_name][threads][items] = avg_time
data = {}

rx = re.compile(r"results_(\d+)_(\d+)\.csv$", re.IGNORECASE)

for path in files:
    m = rx.search(path)
    if not m:
        continue
    items = int(m.group(1))
    threads = int(m.group(2))

    if items not in ITEMS_WHITELIST or threads not in THREADS_WHITELIST:
        continue

    # Read file: first row = test name (header), last numeric row = average
    # Some files may have blank lines; coerce to numeric to isolate values
    df = pd.read_csv(path, header=0)
    if df.shape[1] == 0:
        continue

    test_name = df.columns[0].strip()
    s = pd.to_numeric(df.iloc[:, 0], errors="coerce").dropna()
    if len(s) == 0:
        continue

    avg_time = float(s.iloc[-1])

    data.setdefault(test_name, {}).setdefault(threads, {})[items] = avg_time

# --- Plotting: one figure per test, lines = different thread counts ---
if not data:
    print("No matching data found. Check file names and location.")
else:
    for test_name, threads_map in sorted(data.items()):
        plt.figure(figsize=(10, 6))
        for threads in sorted(threads_map.keys()):
            # Build x (items) and y (avg times) in sorted item order
            items_sorted = sorted(threads_map[threads].keys())
            y = [threads_map[threads][i] for i in items_sorted]

            # A simple line with markers (no specific colors/styles)
            plt.plot(items_sorted, y, marker='o', label=f"{threads} threads")

        plt.xscale("log")
        plt.xlabel("Number of Items")
        plt.ylabel("Average Time (seconds)")
        plt.title(f"Performance vs Items — {test_name}")
        plt.grid(True, which="both", linestyle=":")
        plt.legend(title="Threads")
        plt.tight_layout()
        plt.show()
