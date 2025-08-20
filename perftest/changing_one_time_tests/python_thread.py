# import glob
# import re
# import pandas as pd
# import matplotlib.pyplot as plt

# # Whitelists
# ITEMS_WHITELIST = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
#                    8192, 16384, 32768, 65536, 131072}
# THREADS_WHITELIST = {1, 2, 4, 6, 12}

# # data[(test_name, threads)][items] = avg_time
# data = {}

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

#     # Read CSV; infer delimiter; keep header as test names; drop blank lines
#     df = pd.read_csv(path, header=0, sep=None, engine="python")
#     df.columns = [str(c).strip() for c in df.columns]
#     df = df.dropna(how="all")

#     # For each test column, take the last numeric row as the average
#     for col in df.columns:
#         s = pd.to_numeric(df[col], errors="coerce").dropna()
#         if s.empty:
#             continue
#         avg_time = float(s.iloc[-1])
#         key = (col.strip(), threads)
#         data.setdefault(key, {})[items] = avg_time

# if not data:
#     print("No matching data found. Check file names / location.")
# else:
#     # Consistent markers per thread; different linestyles per test name
#     markers = {1: "o", 2: "s", 4: "D", 6: "^", 12: "v"}
#     linestyles = ["-", "--", "-.", ":"]
#     # Map each test name to a linestyle
#     test_names = sorted({k[0] for k in data.keys()})
#     style_map = {name: linestyles[i % len(linestyles)] for i, name in enumerate(test_names)}

#     plt.figure(figsize=(11, 7))

#     # Plot every (test_name, threads) series on the SAME graph
#     for (test_name, threads) in sorted(data.keys(), key=lambda x: (x[0], x[1])):
#         series = data[(test_name, threads)]
#         xs = sorted(series.keys())
#         ys = [series[i] for i in xs]
#         label = f"{test_name} — {threads} thread{'s' if threads != 1 else ''}"
#         plt.plot(xs, ys,
#                  marker=markers.get(threads, "o"),
#                  linestyle=style_map[test_name],
#                  label=label)

#     plt.xscale("log")
#     plt.xlabel("Number of Items")
#     plt.ylabel("Average Time (seconds)")
#     plt.title("Performance vs Number of Items (combined tests)")
#     plt.grid(True, which="both", linestyle=":")
#     plt.legend(title="Series")
#     plt.tight_layout()
#     plt.show()


import glob
import re
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.widgets import CheckButtons

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

    df = pd.read_csv(path, header=0, sep=None, engine="python")
    df.columns = [str(c).strip() for c in df.columns]
    df = df.dropna(how="all")

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
    # Styling
    markers = {1: "o", 2: "s", 4: "D", 6: "^", 12: "v"}
    linestyles = ["-", "--", "-.", ":"]
    test_names = sorted({k[0] for k in data.keys()})
    style_map = {name: linestyles[i % len(linestyles)] for i, name in enumerate(test_names)}

    # Layout with sidebar for checkboxes
    fig = plt.figure(figsize=(14, 10))
    gs = gridspec.GridSpec(1, 2, width_ratios=[5, 1])
    ax_main = fig.add_subplot(gs[0])
    ax_check = fig.add_subplot(gs[1])
    ax_check.set_title("Toggle Series")
    ax_check.axis("off")

    line_objects = []

    for (test_name, threads) in sorted(data.keys(), key=lambda x: (x[0], x[1])):
        series = data[(test_name, threads)]
        xs = sorted(series.keys())
        ys = [series[i] for i in xs]
        label = f"{test_name} — {threads} thread{'s' if threads != 1 else ''}"
        line, = ax_main.plot(xs, ys,
                             marker=markers.get(threads, "o"),
                             linestyle=style_map[test_name],
                             label=label)
        line_objects.append((label, line, line.get_color()))

    # Sort and extract
    line_objects.sort(key=lambda x: x[0])
    labels, lines, colors = zip(*line_objects)

    # Checkboxes
    check = CheckButtons(ax_check, labels, [True] * len(labels))
    for label_text, color in zip(check.labels, colors):
        label_text.set_color(color)
        label_text.set_fontsize(9)

    def toggle_visibility(label):
        index = labels.index(label)
        line = lines[index]
        line.set_visible(not line.get_visible())
        plt.draw()

    check.on_clicked(toggle_visibility)

    # Final plot formatting
    ax_main.set_xscale("log")
    ax_main.set_xlabel("Number of Items")
    ax_main.set_ylabel("Average Time (seconds)")
    ax_main.set_title("Performance vs Number of Items (combined tests)")
    ax_main.grid(True, which="both", linestyle=":")

    plt.tight_layout()
    plt.show()
