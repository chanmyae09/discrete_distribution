import os
import glob
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import matplotlib.gridspec as gridspec
from matplotlib.widgets import CheckButtons
import numpy as np

# Load data as before
files = sorted(glob.glob("results_*.csv"), key=lambda x: int(x.split("_")[1].split(".")[0]))
levels = []
data_by_test = {}

for file in files:
    level = int(file.split("_")[1].split(".")[0])
    levels.append(level)
    df = pd.read_csv(file, header=0)
    average_row = df.iloc[-1]
    for col in df.columns:
        data_by_test.setdefault(col.strip(), []).append(float(average_row[col]))

# Define families and marker styles
family_colors_rgb = {
    "fenwick": mcolors.to_rgb("red"),
    "discrete": mcolors.to_rgb("blue"),
    "leafsum": mcolors.to_rgb("green"),
    "wrs": mcolors.to_rgb("orange"),
    "heap": mcolors.to_rgb("purple"),
}
families = list(family_colors_rgb.keys())
family_to_tests = {fam: [] for fam in families}
others = []

for test_name in data_by_test.keys():
    found = False
    for fam in families:
        if fam in test_name.lower():
            family_to_tests[fam].append(test_name)
            found = True
            break
    if not found:
        others.append(test_name)

distribution_markers = {
    "uniform": "o",
    "normal": "s",
    "weibull": "D",
}
default_marker = "x"

def get_variant_color(base_rgb, index, total):
    blend_ratio = 0.0 + 0.5 * (index / max(total - 1, 1))  # up to 50% blend toward white
    return tuple(blend_ratio * 1.0 + (1 - blend_ratio) * c for c in base_rgb)

# --- Begin new figure layout ---
fig = plt.figure(figsize=(14, 10))
gs = gridspec.GridSpec(1, 2, width_ratios=[4, 1])
ax_main = fig.add_subplot(gs[0])
ax_legend = fig.add_subplot(gs[1])
ax_legend.set_title("Legend (click to toggle)")
ax_legend.axis("off")

line_objects = []

# Plot known families
for fam in families:
    tests = family_to_tests[fam]
    base_rgb = family_colors_rgb[fam]
    for idx, test_name in enumerate(tests):
        times = data_by_test[test_name]
        linestyle = '--' if "static" in test_name.lower() else '-'
        marker = default_marker
        for dist, sym in distribution_markers.items():
            if dist in test_name.lower():
                marker = sym
                break
        color = get_variant_color(base_rgb, idx, len(tests))
        line, = ax_main.plot(
            levels,
            times,
            label=test_name,
            color=color,
            linestyle=linestyle,
            marker=marker,
            markersize=5
        )
        line_objects.append((test_name, line, color))

# Plot unknowns
for idx, test_name in enumerate(others):
    times = data_by_test[test_name]
    linestyle = '--' if "static" in test_name.lower() else '-'
    marker = default_marker
    for dist, sym in distribution_markers.items():
        if dist in test_name.lower():
            marker = sym
            break
    color = (0.2, 0.2, 0.2)
    line, = ax_main.plot(
        levels,
        times,
        label=test_name,
        color=color,
        linestyle=linestyle,
        marker=marker,
        markersize=5
    )
    line_objects.append((test_name, line, color))

# Sort lines
line_objects.sort(key=lambda x: x[0])
labels, lines, colors = zip(*line_objects)

# Create CheckButtons
check = CheckButtons(ax_legend, labels, [True] * len(labels))

# Style labels with colors
for label_text, color in zip(check.labels, colors):
    label_text.set_color(color)
    label_text.set_fontsize(9)

# Toggle visibility
def func(label):
    index = labels.index(label)
    line = lines[index]
    visible = not line.get_visible()
    line.set_visible(visible)
    plt.draw()

check.on_clicked(func)

# Final formatting
ax_main.set_xlabel("Number of Weights (Level)")
ax_main.set_ylabel("Average Time (seconds)")
ax_main.set_title("Performance of Weighted Random Selection Methods")
ax_main.grid(True)
ax_main.set_xscale("log")

plt.tight_layout()
plt.show()
