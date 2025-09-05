# Moran Model – Simulation and Visualization

This project contains an implementation of a simple Moran process with driver and passenger mutations, along with a script to visualize the results.

## Requirements

- A C++ compiler with C++17 support (e.g., `g++`)
- Bash (to run the `run_model.sh` script !!! chmod +x !!!)
- Python 3 and the following packages: `pandas`, `matplotlib`

Installing Python packages:
```bash
python3 -m pip install --upgrade pip
python3 -m pip install pandas matplotlib
```

## Project Structure

- `moran_model.cpp` – C++ implementation of the model (generates a CSV file with results)
- `run_model.sh` – a script that compiles and runs the simulation
- `moran_plot.py` – a script for generating plots from CSV files
- `outputs/` – directory for simulation results (CSV)
- `plots/` – directory for plots (PNG)

## Compilation and Execution

The easiest way is to use the script:
```bash
bash run_model.sh
```
The script:
- compiles `moran_model.cpp` with the flags `-O3 -Wall -Wextra`
- runs the simulation
- saves the output to `outputs/model_<…>.csv`

If you want to compile manually:
```bash
g++ -std=c++17 -O3 -Wall -Wextra -o moran_model moran_model.cpp
./moran_model A 100.0 50 0.15 0.35 10.0 0.2 outputs/custom.csv
```

## Simulation Parameters

The C++ program accepts 8 arguments:
```
<MODEL_TYPE: A|B> <tmax> <Ntot> <s> <d> <L> <p> <output_file>
```
- `MODEL_TYPE` – A: fitness-dependent death; B: random death
- `tmax` – maximum simulation time (double)
- `Ntot` – total number of cells (int, constant population)
- `s` – fitness advantage of a driver mutation
- `d` – fitness cost of a passenger mutation
- `L` – total mutation rate
- `p` – probability that a mutation is a driver (otherwise, it's a passenger)
- `output_file` – path to the output CSV file

You can change the default values in `run_model.sh` by editing the parameters section:
```bash
MODEL_TYPE="B"
TMAX=100.0
NTOT=50
S=0.15
D=0.35
L=10.0
P=0.2
```

## Output Data Format (CSV)

Header:
```
Time,ActiveClones,CumulativeDrivers,CumulativePassengers
```
- `Time` – simulation time
- `ActiveClones` – number of active clones (n > 0)
- `CumulativeDrivers` – cumulative number of driver mutations
- `CumulativePassengers` – cumulative number of passenger mutations

## Generating Plots

After running the simulation, execute:
```bash
python3 moran_plot.py```
The script:
- reads all `*.csv` files from the `outputs/` directory
- generates PNG plots in the `plots/` directory:
  - `plot_ActiveClones.png`
  - `plot_CumulativeDrivers.png`
  - `plot_CumulativePassengers.png`
