import csv
import matplotlib.pyplot as plt
import os

# ============================================================
# Read CSV data
# ============================================================

def read_csv(filename):
    """Read a CSV file and return n values and data for each variant."""
    n_values = []
    data = {"naive": [], "path_compression": [], "union_by_rank": [], "both_combined": []}
    
    with open(filename, "r") as f:
        reader = csv.DictReader(f)
        for row in reader:
            n_values.append(int(row["n"]))
            for variant in data:
                data[variant].append(float(row[variant]))
    
    return n_values, data

# ============================================================
# Plot styling
# ============================================================

# Colors for each variant
COLORS = {
    "naive": "#e74c3c",           # red
    "path_compression": "#f39c12", # orange
    "union_by_rank": "#3498db",    # blue
    "both_combined": "#2ecc71"     # green
}

# Nice labels for the legend
LABELS = {
    "naive": "Naive",
    "path_compression": "Path Compression Only",
    "union_by_rank": "Union by Rank Only",
    "both_combined": "Both Combined"
}

MARKERS = {
    "naive": "o",
    "path_compression": "s",
    "union_by_rank": "^",
    "both_combined": "D"
}

# ============================================================
# Plot 1: Average cost per operation vs n
# ============================================================

def plot_avg_cost(n_values, avg_data, stddev_data, output_path):
    """Line plot of average cost per operation for all four variants."""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    for variant in COLORS:
        ax.errorbar(
            n_values, 
            avg_data[variant],
            yerr=stddev_data[variant],
            label=LABELS[variant],
            color=COLORS[variant],
            marker=MARKERS[variant],
            markersize=8,
            linewidth=2,
            capsize=4,
            capthick=1.5
        )
    
    ax.set_xlabel("Number of Rooms (n)", fontsize=13)
    ax.set_ylabel("Average Pointer Traversals per Operation", fontsize=13)
    ax.set_title("Union-Find Variant Performance: Average Cost per Operation", fontsize=15, fontweight="bold")
    ax.legend(fontsize=11, loc="upper left")
    ax.grid(True, alpha=0.3)
    ax.set_xscale("log")
    
    plt.tight_layout()
    plt.savefig(output_path, dpi=150, bbox_inches="tight")
    plt.close()
    print(f"Saved: {output_path}")

# ============================================================
# Plot 2: Total traversals vs n
# ============================================================

def plot_total_traversals(n_values, total_data, output_path):
    """Line plot of total traversals for all four variants."""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    for variant in COLORS:
        ax.plot(
            n_values,
            total_data[variant],
            label=LABELS[variant],
            color=COLORS[variant],
            marker=MARKERS[variant],
            markersize=8,
            linewidth=2
        )
    
    ax.set_xlabel("Number of Rooms (n)", fontsize=13)
    ax.set_ylabel("Total Pointer Traversals", fontsize=13)
    ax.set_title("Union-Find Variant Performance: Total Traversals", fontsize=15, fontweight="bold")
    ax.legend(fontsize=11, loc="upper left")
    ax.grid(True, alpha=0.3)
    ax.set_xscale("log")
    ax.set_yscale("log")
    
    plt.tight_layout()
    plt.savefig(output_path, dpi=150, bbox_inches="tight")
    plt.close()
    print(f"Saved: {output_path}")

# ============================================================
# Main
# ============================================================

def plot_zoomed(n_values, avg_data, stddev_data, output_path):
    """Zoomed-in plot showing only the three optimized variants."""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    for variant in ["path_compression", "union_by_rank", "both_combined"]:
        ax.errorbar(
            n_values,
            avg_data[variant],
            yerr=stddev_data[variant],
            label=LABELS[variant],
            color=COLORS[variant],
            marker=MARKERS[variant],
            markersize=8,
            linewidth=2,
            capsize=4,
            capthick=1.5
        )
    
    ax.set_xlabel("Number of Rooms (n)", fontsize=13)
    ax.set_ylabel("Average Pointer Traversals per Operation", fontsize=13)
    ax.set_title("Optimized Variants: Average Cost per Operation (Zoomed In)", fontsize=15, fontweight="bold")
    ax.legend(fontsize=11, loc="upper left")
    ax.grid(True, alpha=0.3)
    ax.set_xscale("log")
    
    plt.tight_layout()
    plt.savefig(output_path, dpi=150, bbox_inches="tight")
    plt.close()
    print(f"Saved: {output_path}")


def main():
    # Make sure plots directory exists
    os.makedirs("plots", exist_ok=True)
    
    # Read data
    print("Reading experiment data...")
    n_values, avg_data = read_csv("data/avg_cost_per_op.csv")
    _, total_data = read_csv("data/total_traversals.csv")
    _, stddev_data = read_csv("data/stddev.csv")
    
    # Generate plots
    print("Generating plots...")
    plot_avg_cost(n_values, avg_data, stddev_data, "plots/avg_cost_per_operation.png")
    plot_total_traversals(n_values, total_data, "plots/total_traversals.png")
    plot_zoomed(n_values, avg_data, stddev_data, "plots/optimized_variants_zoomed.png")
    
    print("All plots generated successfully!")

if __name__ == "__main__":
    main()
