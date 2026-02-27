#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <cmath>
#include <numeric>
#include <string>

// ============================================================
// Union-Find with four variants
// ============================================================

class UnionFind {
public:
    std::vector<int> parent;
    std::vector<int> rank;
    long long traversals;
    int components;

    UnionFind(int n) : parent(n), rank(n, 0), traversals(0), components(n) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    // ---- VARIANT 1: Naive ----
    int findNaive(int x) {
        while (parent[x] != x) {
            traversals++;
            x = parent[x];
        }
        return x;
    }

    bool unionNaive(int x, int y) {
        int rootX = findNaive(x);
        int rootY = findNaive(y);
        if (rootX == rootY) return false;
        parent[rootY] = rootX;
        components--;
        return true;
    }

    // ---- VARIANT 2: Path Compression Only ----
    int findPC(int x) {
        int root = x;
        while (parent[root] != root) {
            traversals++;
            root = parent[root];
        }
        // flatten: point every node on path directly to root
        while (parent[x] != root) {
            int next = parent[x];
            parent[x] = root;
            x = next;
        }
        return root;
    }

    bool unionPC(int x, int y) {
        int rootX = findPC(x);
        int rootY = findPC(y);
        if (rootX == rootY) return false;
        parent[rootY] = rootX;
        components--;
        return true;
    }

    // ---- VARIANT 3: Union by Rank Only ----
    int findRank(int x) {
        while (parent[x] != x) {
            traversals++;
            x = parent[x];
        }
        return x;
    }

    bool unionByRank(int x, int y) {
        int rootX = findRank(x);
        int rootY = findRank(y);
        if (rootX == rootY) return false;
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
        components--;
        return true;
    }

    // ---- VARIANT 4: Both Combined ----
    int findBoth(int x) {
        int root = x;
        while (parent[root] != root) {
            traversals++;
            root = parent[root];
        }
        // flatten
        while (parent[x] != root) {
            int next = parent[x];
            parent[x] = root;
            x = next;
        }
        return root;
    }

    bool unionBoth(int x, int y) {
        int rootX = findBoth(x);
        int rootY = findBoth(y);
        if (rootX == rootY) return false;
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
        components--;
        return true;
    }
};

// ============================================================
// Dungeon grid and edge generation
// ============================================================

struct Edge {
    int u, v;
};

// Generate all possible corridors between adjacent rooms on a 2D grid
// Grid dimensions: rows x cols where rows * cols = n
std::vector<Edge> generateGridEdges(int rows, int cols) {
    std::vector<Edge> edges;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int current = r * cols + c;

            // right neighbor
            if (c + 1 < cols) {
                edges.push_back({current, current + 1});
            }
            // down neighbor
            if (r + 1 < rows) {
                edges.push_back({current, (r + 1) * cols + c});
            }
        }
    }

    return edges;
}

// Find grid dimensions close to square for n rooms
std::pair<int, int> getGridDimensions(int n) {
    int cols = (int)std::sqrt((double)n);
    while (n % cols != 0) {
        cols--;
    }
    int rows = n / cols;
    return {rows, cols};
}

// ============================================================
// Run one trial: generate dungeon and count traversals
// ============================================================

// variant: 0=naive, 1=path compression, 2=union by rank, 3=both
long long runTrial(int n, const std::vector<Edge>& shuffledEdges, int variant) {
    UnionFind uf(n);
    long long totalOps = 0; // count find/union calls

    for (const auto& edge : shuffledEdges) {
        if (uf.components == 1) break;

        switch (variant) {
            case 0: // naive
                uf.unionNaive(edge.u, edge.v);
                break;
            case 1: // path compression only
                uf.unionPC(edge.u, edge.v);
                break;
            case 2: // union by rank only
                uf.unionByRank(edge.u, edge.v);
                break;
            case 3: // both
                uf.unionBoth(edge.u, edge.v);
                break;
        }
        totalOps++;
    }

    return uf.traversals;
}

// Count total operations (edges processed) for a trial
long long countOps(int n, const std::vector<Edge>& shuffledEdges) {
    // We need to know how many edges are processed before full connectivity
    UnionFind uf(n);
    long long ops = 0;
    for (const auto& edge : shuffledEdges) {
        if (uf.components == 1) break;
        // Use naive find just to count - doesn't matter which variant
        int rootX = edge.u, rootY = edge.v;
        while (uf.parent[rootX] != rootX) rootX = uf.parent[rootX];
        while (uf.parent[rootY] != rootY) rootY = uf.parent[rootY];
        if (rootX != rootY) {
            uf.parent[rootY] = rootX;
            uf.components--;
        }
        ops++;
    }
    return ops;
}

// ============================================================
// Main experiment
// ============================================================

int main() {
    // Input sizes to test
    std::vector<int> sizes = {100, 500, 1000, 5000, 10000, 25000, 50000};
    int numTrials = 50;

    std::string variantNames[] = {"naive", "path_compression", "union_by_rank", "both_combined"};

    // Open output CSV files
    std::ofstream avgCostFile("data/avg_cost_per_op.csv");
    std::ofstream totalTraversalsFile("data/total_traversals.csv");
    std::ofstream stddevFile("data/stddev.csv");

    // Write headers
    avgCostFile << "n,naive,path_compression,union_by_rank,both_combined" << std::endl;
    totalTraversalsFile << "n,naive,path_compression,union_by_rank,both_combined" << std::endl;
    stddevFile << "n,naive,path_compression,union_by_rank,both_combined" << std::endl;

    for (int n : sizes) {
        std::cout << "========================================" << std::endl;
        std::cout << "Testing n = " << n << " rooms" << std::endl;
        std::cout << "========================================" << std::endl;

        auto [rows, cols] = getGridDimensions(n);
        std::cout << "Grid: " << rows << " x " << cols << std::endl;

        // Generate the base edge list once
        std::vector<Edge> edges = generateGridEdges(rows, cols);
        std::cout << "Total possible corridors: " << edges.size() << std::endl;

        // Storage for results: [variant][trial]
        std::vector<std::vector<double>> traversalResults(4, std::vector<double>(numTrials));
        std::vector<std::vector<double>> costPerOpResults(4, std::vector<double>(numTrials));

        for (int trial = 0; trial < numTrials; trial++) {
            if (trial % 10 == 0) {
                std::cout << "  Running trial " << (trial + 1) << "/" << numTrials << "..." << std::endl;
            }

            // Fixed seed per trial for reproducibility
            // Different seed per trial so we get different dungeons
            unsigned int seed = 42 + trial * 1000 + n;
            std::mt19937 rng(seed);

            // Shuffle edges
            std::vector<Edge> shuffled = edges;
            std::shuffle(shuffled.begin(), shuffled.end(), rng);

            // Count operations (edges processed before connectivity)
            long long ops = countOps(n, shuffled);

            // Run all four variants on the SAME shuffled edges
            for (int variant = 0; variant < 4; variant++) {
                long long traversals = runTrial(n, shuffled, variant);
                traversalResults[variant][trial] = (double)traversals;
                costPerOpResults[variant][trial] = (double)traversals / (double)ops;
            }
        }

        // Compute averages and standard deviations
        std::vector<double> avgTraversals(4), avgCostPerOp(4), stddevCostPerOp(4);

        for (int v = 0; v < 4; v++) {
            // Average traversals
            double sumT = 0;
            for (double val : traversalResults[v]) sumT += val;
            avgTraversals[v] = sumT / numTrials;

            // Average cost per operation
            double sumC = 0;
            for (double val : costPerOpResults[v]) sumC += val;
            avgCostPerOp[v] = sumC / numTrials;

            // Standard deviation of cost per operation
            double sumSq = 0;
            for (double val : costPerOpResults[v]) {
                double diff = val - avgCostPerOp[v];
                sumSq += diff * diff;
            }
            stddevCostPerOp[v] = std::sqrt(sumSq / numTrials);
        }

        // Print results
        std::cout << "  Results (avg cost per operation):" << std::endl;
        for (int v = 0; v < 4; v++) {
            std::cout << "    " << variantNames[v] << ": "
                      << avgCostPerOp[v] << " (stddev: " << stddevCostPerOp[v] << ")" << std::endl;
        }

        // Write to CSV
        avgCostFile << n;
        totalTraversalsFile << n;
        stddevFile << n;
        for (int v = 0; v < 4; v++) {
            avgCostFile << "," << avgCostPerOp[v];
            totalTraversalsFile << "," << avgTraversals[v];
            stddevFile << "," << stddevCostPerOp[v];
        }
        avgCostFile << std::endl;
        totalTraversalsFile << std::endl;
        stddevFile << std::endl;
    }

    avgCostFile.close();
    totalTraversalsFile.close();
    stddevFile.close();

    std::cout << std::endl;
    std::cout << "Done! Results saved to data/ directory." << std::endl;
    std::cout << "Run 'python3 code/plot.py' to generate plots." << std::endl;

    return 0;
}
