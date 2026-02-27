#!/bin/bash
# reproduce.sh - Reproduces all experimental results
# Run this script from the project root directory

echo "============================================"
echo "Dungeon Generation Union-Find Experiment"
echo "============================================"
echo ""

# Create directories
mkdir -p data
mkdir -p plots

# Compile C++ code
echo "Compiling experiment..."
g++ -O2 -o experiment code/experiment.cpp
if [ $? -ne 0 ]; then
    echo "ERROR: Compilation failed!"
    exit 1
fi
echo "Compilation successful."
echo ""

# Run experiment
echo "Running experiment..."
./experiment
echo ""

# Generate plots
echo "Generating plots..."
python3 code/plot.py
echo ""

echo "============================================"
echo "Done! Check the plots/ directory for results."
echo "============================================"
