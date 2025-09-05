// Moran_model_save.cpp
//
// Execution (via the run.sh script):
// ./run.sh

#include <iostream>     // For console input/output (e.g., std::cout, std::cerr)
#include <vector>       // For using the std::vector container (dynamic array)
#include <random>       // For advanced random number generation
#include <cmath>        // For mathematical functions like std::pow
#include <numeric>      // For numerical operations (not heavily used here, but good practice)
#include <chrono>       // For seeding the random number generator and timing the execution
#include <algorithm>    // For algorithms like std::count_if
#include <fstream>      // For file stream operations (reading/writing files)
#include <string>       // For using the std::string class

// Structure to hold all data for a single cell clone.
struct Clone {
    int k; // number of driver mutations
    int l; // number of passenger mutations
    int n; // number of cells in the clone (population size)
};

// Helper function to generate a random number from an exponential distribution.
// This is used in the Gillespie algorithm to determine the time to the next event.
template<typename Gen>
double exponential_dist(Gen& g, double lambda) {
    std::exponential_distribution<> dist(lambda);
    return dist(g);
}

int main(int argc, char* argv[]) {
    // --- Command-line Argument Parsing ---
    // The program expects exactly 8 arguments plus the program name.
    if (argc != 9) {
        std::cerr << "Usage: " << argv[0] << " <MODEL_TYPE: A|B> <tmax> <Ntot> <s> <d> <L> <p> <output_file>\n";
        return 1; // Exit with an error code
    }

    // Convert command-line arguments from text to their appropriate types.
    const std::string model_type = argv[1];
    const double tmax = std::stod(argv[2]);      // Maximum simulation time
    const int Ntot = std::stoi(argv[3]);         // Total number of cells
    const double s = std::stod(argv[4]);         // Fitness advantage of a driver mutation
    const double d = std::stod(argv[5]);         // Fitness disadvantage of a passenger mutation
    const double L = std::stod(argv[6]);         // Total mutation rate
    const double p = std::stod(argv[7]);         // Probability of a mutation being a driver
    const std::string output_filename = argv[8]; // Filename for the results

    // Validate the model type argument.
    if (model_type != "A" && model_type != "B") {
        std::cerr << "Error: Model type must be 'A' or 'B'.\n";
        return 1;
    }

    // --- Random Number Generator Initialization ---
    // Use a high-quality random number generator (Mersenne Twister)
    // and seed it with the current time to ensure different random sequences on each run.
    std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> unif_dist(0.0, 1.0); // For generating random numbers between 0 and 1.
    
    // --- Output File Initialization ---
    // Open the specified file for writing.
    std::ofstream data_file(output_filename);
    if (!data_file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << output_filename << std::endl;
        return 1;
    }
    // Write the header row to the CSV file.
    data_file << "Time,ActiveClones,CumulativeDrivers,CumulativePassengers\n";

    // --- Simulation State Variables ---
    double t = 0.0; // Current simulation time
    std::vector<Clone> clones; // A vector to store all clones
    // Start with one clone (wild type) with no mutations, containing all Ntot cells.
    clones.push_back({0, 0, Ntot});

    long long cumulative_drivers = 0;    // Total driver mutations that have occurred
    long long cumulative_passengers = 0; // Total passenger mutations that have occurred
    
    // Save the initial state (t=0) to the file.
    data_file << t << "," << 1 << "," << 0 << "," << 0 << "\n";

    // Start the timer to measure execution time.
    auto start_time = std::chrono::high_resolution_clock::now();

    // --- Main Simulation Loop (Gillespie Algorithm) ---
    // The loop continues until the simulation time 't' reaches the maximum time 'tmax'.
    while (t < tmax) {
        // --- Calculate Propensities (Rates) for All Possible Events ---
        std::vector<double> propensities;
        std::vector<int> active_clone_indices; // Keep track of clones that are not extinct
        propensities.reserve(clones.size());
        active_clone_indices.reserve(clones.size());
        
        double total_division_propensity = 0.0;

        // Iterate over all clones to calculate their division propensity.
        for (int i = 0; i < clones.size(); ++i) {
            if (clones[i].n > 0) { // Only consider clones with at least one cell.
                // Fitness is calculated based on driver (s) and passenger (d) mutations.
                double fitness = std::pow(1.0 + s, clones[i].k) * std::pow(1.0 - d, clones[i].l);
                // Propensity is the fitness of the clone multiplied by its population size.
                double propensity = clones[i].n * fitness;
                propensities.push_back(propensity);
                active_clone_indices.push_back(i);
                total_division_propensity += propensity;
            }
        }
        
        // The total propensity of all events is the sum of division propensities plus the mutation rate.
        double total_propensity = total_division_propensity + L;

        // --- Determine Time to Next Event ---
        // Draw a time step 'dt' from an exponential distribution with rate equal to total_propensity.
        double dt = exponential_dist(rng, total_propensity);
        t += dt; // Advance simulation time.

        if (t >= tmax) break; // Stop if we've exceeded the maximum time.
        
        // --- Select Which Event Occurs ---
        // Generate a random number to decide between division/death and mutation.
        double r1 = unif_dist(rng) * total_propensity;

        if (r1 < total_division_propensity) {
            // EVENT: DIVISION/DEATH
            
            // --- Select a clone for reproduction (always fitness-dependent) ---
            double r_repro = unif_dist(rng) * total_division_propensity;
            double cumulative_prop_repro = 0.0;
            int reproducing_clone_idx = -1;
            // Roulette wheel selection based on propensities.
            for (int i = 0; i < propensities.size(); ++i) {
                cumulative_prop_repro += propensities[i];
                if (r_repro < cumulative_prop_repro) {
                    reproducing_clone_idx = active_clone_indices[i];
                    break;
                }
            }

            // --- Select a clone for death (model-dependent) ---
            int dying_clone_idx = -1;
            if (model_type == "A") {
                // Model A: death is also fitness-dependent (proportional to propensity).
                double r_death = unif_dist(rng) * total_division_propensity;
                double cumulative_prop_death = 0.0;
                for (int i = 0; i < propensities.size(); ++i) {
                    cumulative_prop_death += propensities[i];
                    if (r_death < cumulative_prop_death) {
                        dying_clone_idx = active_clone_indices[i];
                        break;
                    }
                }
            } else { // model_type == "B"
                // Model B: death is random (uniform choice across all cells).
                std::uniform_int_distribution<> cell_dist(1, Ntot);
                int random_cell_pos = cell_dist(rng);
                int cumulative_n = 0;
                // Iterate through all clones to find which one contains the chosen cell.
                for (int i = 0; i < clones.size(); ++i) { 
                    if (clones[i].n > 0) {
                         cumulative_n += clones[i].n;
                         if (random_cell_pos <= cumulative_n) {
                             dying_clone_idx = i;
                             break;
                         }
                    }
                }
            }
            
            // Update cell counts: one clone loses a cell, another gains one.
            if (dying_clone_idx != -1 && reproducing_clone_idx != -1) {
                clones[dying_clone_idx].n--;
                clones[reproducing_clone_idx].n++;
            }

        } else {
            // EVENT: MUTATION
            // Select a random cell from the total population to mutate.
            std::uniform_int_distribution<> cell_dist(1, Ntot);
            int random_cell_pos = cell_dist(rng);
            int cumulative_n = 0;
            int parent_clone_idx = -1;
            
            // Find the clone to which the selected cell belongs.
            for(int i = 0; i < clones.size(); ++i) {
                cumulative_n += clones[i].n;
                if (random_cell_pos <= cumulative_n) {
                    parent_clone_idx = i;
                    break;
                }
            }

            if (parent_clone_idx != -1) {
                clones[parent_clone_idx].n--; // Parent clone loses one cell.
                Clone new_clone = clones[parent_clone_idx]; // Create a new clone...
                new_clone.n = 1; // ...with a population of 1.

                // Decide if the mutation is a driver or passenger based on probability 'p'.
                if (unif_dist(rng) < p) {
                    new_clone.k++; // Add a driver mutation.
                    cumulative_drivers++;
                } else {
                    new_clone.l++; // Add a passenger mutation.
                    cumulative_passengers++;
                }
                clones.push_back(new_clone); // Add the new clone to the list.
            }
        }

        // --- Data Recording ---
        // Count the number of active clones (population > 0).
        long active_clones_count = std::count_if(clones.begin(), clones.end(), 
            [](const Clone& c){ return c.n > 0; });
            
        // Write the current state of the simulation to the output file.
        data_file << t << "," << active_clones_count << "," << cumulative_drivers << "," << cumulative_passengers << "\n";
    }

    data_file.close(); // Close the output file.

    // Stop the timer.
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // --- Summary Statistics (displayed in the console) ---
    std::cout << "Simulation (Model " << model_type << ") finished. Data saved to file " << output_filename << "." << std::endl;
    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;
    
    return 0; // Exit successfully
}