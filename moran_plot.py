#!/usr/bin/env python3

import os
import pandas as pd
import matplotlib.pyplot as plt

# --- Configuration ---
# Directory with input data (simulation results)
INPUT_DIR = "outputs"
# Directory where the plots will be saved
PLOT_DIR = "plots"

# Definition of statistics to plot and their corresponding labels for the titles.
STATS_TO_PLOT = {
    'ActiveClones': 'Number of Active Clones',
    'CumulativeDrivers': 'Cumulative Number of Driver Mutations',
    'CumulativePassengers': 'Cumulative Number of Passenger Mutations'
}

def parse_filename(filename):
    try:
        # Remove the extension and split the name into key-value pairs
        parts = filename.replace('.csv', '').split('_')
        params = {parts[i]: parts[i+1] for i in range(0, len(parts), 2)}
        
        # Safely get the parameters, providing a default if not found
        model_type = params.get('model', 'N/A')
        s_val = params.get('s', 'N/A')
        d_val = params.get('d', 'N/A')
        
        # Format the string for the legend
        return f"Model {model_type} (s={s_val}, d={d_val})"
    except IndexError:
        # Return the filename itself if parsing fails
        return filename.replace('.csv', '')

def main():
    """
    The main function of the script.
    """
    # 1. Check for the input directory and create the output directory
    if not os.path.exists(INPUT_DIR):
        print(f"Error: Input directory '{INPUT_DIR}' does not exist. Please run the simulations first.")
        return

    # Create the plot directory if it doesn't exist; no error if it already does.
    os.makedirs(PLOT_DIR, exist_ok=True)
    print(f"Plots will be saved in the directory: '{PLOT_DIR}'")

    # 2. Find all CSV files in the input directory
    try:
        # Get a sorted list of all files ending with .csv
        csv_files = sorted([f for f in os.listdir(INPUT_DIR) if f.endswith('.csv')])
        if not csv_files:
            print(f"Error: No .csv files found in directory '{INPUT_DIR}'.")
            return
    except FileNotFoundError:
        print(f"Error: Cannot find directory '{INPUT_DIR}'.")
        return

    # 3. Generate a separate plot for each statistic
    # This outer loop iterates through the statistics (e.g., 'ActiveClones', 'CumulativeDrivers').
    for stat_column, plot_title in STATS_TO_PLOT.items():
        
        print(f"Generating plot for: {plot_title}...")
        
        # Create a new, clean figure for each statistic to avoid plotting on top of the previous one.
        plt.figure(figsize=(14, 9))
        
        # This inner loop iterates over each simulation result file.
        # All results will be plotted on the *same* figure for comparison.
        for filename in csv_files:
            file_path = os.path.join(INPUT_DIR, filename)
            
            try:
                # Load the data from the CSV file into a pandas DataFrame
                df = pd.read_csv(file_path)
                
                # Check if the required columns ('Time' and the current statistic) exist in the data
                if 'Time' in df.columns and stat_column in df.columns:
                    # Generate a descriptive label for this data series from its filename
                    label = parse_filename(filename)
                    
                    # Add the data series (Time vs. statistic) to the current plot
                    plt.plot(df['Time'], df[stat_column], label=label, alpha=0.8)
                else:
                    print(f"  Warning: File {filename} is missing 'Time' or '{stat_column}' column.")

            except pd.errors.EmptyDataError:
                print(f"  Warning: File {filename} is empty and will be skipped.")
            except Exception as e:
                print(f"  Error processing file {filename}: {e}")

        # 4. Set the description and formatting for the completed plot
        plt.title(plot_title, fontsize=16)
        plt.xlabel("Time (t)", fontsize=12)
        plt.ylabel("Count", fontsize=12)
        plt.grid(True, which='both', linestyle='--', linewidth=0.5)
        plt.legend(fontsize=10)
        plt.tight_layout()  # Automatically adjust plot elements to fit

        # 5. Save the plot to a file
        plot_filename = os.path.join(PLOT_DIR, f"plot_{stat_column}.png")
        plt.savefig(plot_filename, dpi=150)
        print(f"Saved plot: {plot_filename}")
        
        # Close the figure to free up memory before the next iteration of the outer loop
        plt.close()

    print("\nAll plots have been generated.")


# This standard Python construct ensures that the main() function is called
# only when the script is executed directly.
if __name__ == "__main__":
    main()