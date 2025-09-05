#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

# --- Simulation Parameter Definition ---
# Change these values to run the simulation with different parameters.

# MODEL_TYPE: "A" or "B"
MODEL_TYPE="B"

# Numerical parameters
TMAX=100.0
NTOT=50
S=0.15
D=0.35
L=10.0
P=0.2

# --- End of Parameters Section ---


# C++ source program and the resulting executable file name
CPP_SOURCE="moran_model.cpp"
EXECUTABLE="moran_model"

# Name of the subdirectory for results
OUTPUT_DIR="outputs"


# 1. Compile the C++ program
echo "Compiling $CPP_SOURCE..."
g++ -std=c++17 -O3 -Wall -Wextra -o "$EXECUTABLE" "$CPP_SOURCE"

# Check if the compilation was successful
echo "Compilation successful."


# 2. Create the output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"
echo "Output directory: '$OUTPUT_DIR'."


# 3. Create a unique output filename
FILENAME="${OUTPUT_DIR}/model_${MODEL_TYPE}_tmax_${TMAX}_N_${NTOT}_s_${S}_d_${D}.csv"
echo "Output file: $FILENAME"


# 4. Run the simulation with the defined parameters
echo "Running simulation for model: $MODEL_TYPE..."
./"$EXECUTABLE" "$MODEL_TYPE" "$TMAX" "$NTOT" "$S" "$D" "$L" "$P" "$FILENAME"

# Check if the program finished correctly
:

echo "Script finished."