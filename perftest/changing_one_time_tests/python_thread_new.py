import glob
import re
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.widgets import CheckButtons

# Whitelist of item sizes (weightnums)
ITEMS_WHITELIST = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
                   8192, 16384, 32768, 65536, 131072}

data = {}  # (test_name) → {items → avg_time}

# Match files like results_4096.csv
file_pattern = "results_*.csv"
rx = re.compile(r"results_(\d+)\.csv$", re.IGNORECASE)

for path in glob.glob(file_pattern):
    m = rx.search(path)
    if not m:
        continue
    items = int(m.group(1))
    if items not in ITEMS_WHITELIST:
        continue

    df = pd.read_csv(path, header=0)
    df.columns = [str(c).strip() for c in df.columns]
    df = df.dropna(how="all")

    for col in df.columns:
        s = pd.to_numeric(df[col], errors="coerce").dropna()
        if s.empty:
            continue
        avg_time = float(s.iloc[-1])  # assume last row is average
        data.setdefault(col, {})[items] = avg_time

# Plotting
if not data:
    print("No matching data found.")
else:
    fig, (ax_main, ax_check) = plt.subplots(
        1, 2, figsize=(14, 10), gridspec_kw={'width_ratios': [5, 1]}
    )
    ax_check.set_title("Toggle Series")
    ax_check.axis("off")

    line_objects = []

    for test_name in sorted(data.keys()):
        series = data[test_name]
        xs = sorted(series.keys())
        ys = [series[x] for x in xs]
        line, = ax_main.plot(xs, ys, marker='o', label=test_name)
        line_objects.append((test_name, line, line.get_color()))

    # Checkbox interactivity
    line_objects.sort(key=lambda x: x[0])
    labels, lines, colors = zip(*line_objects)
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

    # Main plot formatting
    ax_main.set_xscale("log")
    ax_main.set_xlabel("Number of Items")
    ax_main.set_ylabel("Average Time (seconds)")
    ax_main.set_title("Performance Comparison — 12 Threads")
    ax_main.grid(True, which="both", linestyle=":")

    plt.tight_layout()
    plt.show()
