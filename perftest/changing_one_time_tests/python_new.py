# import glob
# import re
# import pandas as pd
# import matplotlib.pyplot as plt

# # --- Whitelists you specified ---
# ITEMS_WHITELIST = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
#                    8192, 16384, 32768, 65536, 131072}
# THREADS_WHITELIST = {1, 2, 4, 6, 12}

# # Data structure:
# # data[test_name][threads][items] = avg_time
# data = {}

# # Discover files like results_{items}_{threads}.csv
# file_pattern = "results_*_*.csv"
# rx = re.compile(r"results_(\d+)_(\d+)\.csv$", re.IGNORECASE)

# for path in glob.glob(file_pattern):
#     m = rx.search(path)
#     if not m:
#         continue
#     items = int(m.group(1))
#     threads = int(m.group(2))
#     if items not in ITEMS_WHITELIST or threads not in THREADS_WHITELIST:
#         continue

#     # Read CSV: infer delimiter (comma or tab), keep header row as column names
#     # Drop fully empty rows to ignore blank lines
#     df = pd.read_csv(path, header=0, sep=None, engine="python")
#     df.columns = [str(c).strip() for c in df.columns]
#     df = df.dropna(how="all")

#     # For each column (test), coerce to numeric, take the last numeric as the average
#     for col in df.columns:
#         s = pd.to_numeric(df[col], errors="coerce").dropna()
#         if s.empty:
#             continue
#         avg_time = float(s.iloc[-1])
#         test_name = col.strip()
#         data.setdefault(test_name, {}).setdefault(threads, {})[items] = avg_time

# # --- Plotting: one figure per test; lines = different thread counts ---
# if not data:
#     print("No matching data found. Check file names and locations.")
# else:
#     # Optional consistent markers per thread count
#     markers = {1: "o", 2: "s", 4: "D", 6: "^", 12: "v"}

#     for test_name, threads_map in sorted(data.items()):
#         plt.figure(figsize=(10, 6))
#         for threads in sorted(threads_map.keys()):
#             xs = sorted(threads_map[threads].keys())
#             ys = [threads_map[threads][i] for i in xs]
#             plt.plot(xs, ys, marker=markers.get(threads, "o"), label=f"{threads} threads")

#         plt.xscale("log")
#         plt.xlabel("Number of Items")
#         plt.ylabel("Average Time (seconds)")
#         plt.title(f"Performance vs Items — {test_name}")
#         plt.grid(True, which="both", linestyle=":")
#         plt.legend(title="Threads")
#         plt.tight_layout()
#         plt.show()



import glob
import re
import pandas as pd
import matplotlib.pyplot as plt

# Whitelists
ITEMS_WHITELIST = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
                   8192, 16384, 32768, 65536, 131072}
THREADS_WHITELIST = {1, 2, 4, 6, 12}

# data[(test_name, threads)][items] = avg_time
data = {}

file_pattern = "results_*_*.csv"
rx = re.compile(r"results_(\d+)_(\d+)\.csv$", re.IGNORECASE)

for path in glob.glob(file_pattern):
    m = rx.search(path)
    if not m:
        continue
    items = int(m.group(1))
    threads = int(m.group(2))
    if items not in ITEMS_WHITELIST or threads not in THREADS_WHITELIST:
        continue

    # Read CSV; infer delimiter; keep header as test names; drop blank lines
    df = pd.read_csv(path, header=0, sep=None, engine="python")
    df.columns = [str(c).strip() for c in df.columns]
    df = df.dropna(how="all")

    # For each test column, take the last numeric row as the average
    for col in df.columns:
        s = pd.to_numeric(df[col], errors="coerce").dropna()
        if s.empty:
            continue
        avg_time = float(s.iloc[-1])
        key = (col.strip(), threads)
        data.setdefault(key, {})[items] = avg_time

if not data:
    print("No matching data found. Check file names / location.")
else:
    # Consistent markers per thread; different linestyles per test name
    markers = {1: "o", 2: "s", 4: "D", 6: "^", 12: "v"}
    linestyles = ["-", "--", "-.", ":"]
    # Map each test name to a linestyle
    test_names = sorted({k[0] for k in data.keys()})
    style_map = {name: linestyles[i % len(linestyles)] for i, name in enumerate(test_names)}

    plt.figure(figsize=(11, 7))

    # Plot every (test_name, threads) series on the SAME graph
    for (test_name, threads) in sorted(data.keys(), key=lambda x: (x[0], x[1])):
        series = data[(test_name, threads)]
        xs = sorted(series.keys())
        ys = [series[i] for i in xs]
        label = f"{test_name} — {threads} thread{'s' if threads != 1 else ''}"
        plt.plot(xs, ys,
                 marker=markers.get(threads, "o"),
                 linestyle=style_map[test_name],
                 label=label)

    plt.xscale("log")
    plt.xlabel("Number of Items")
    plt.ylabel("Average Time (seconds)")
    plt.title("Performance vs Number of Items (combined tests)")
    plt.grid(True, which="both", linestyle=":")
    plt.legend(title="Series")
    plt.tight_layout()
    plt.show()
