// Evolution experiment
// Jacob Conrad Martin
// http://jacobconradmartin.com

import std.stdio;
import std.string;
import std.random;
import std.c.time;
import std.array;

// Global constants
const float SUCCESS_LEVEL = 0.75;
const int DNA_LENGTH = 16;
const int DNA_CHOICES = 4;
const int POPULATION_SIZE = 1000;
const int MAX_GENERATIONS = 1000;
const int EXPERIMENTS = 100;
// Vary these between sets of experiments
const int BREEDING_POOL_SIZE = 100;
const float MUTATION_RATE = 0.01;
// This will vary (although not during )
string PERFECT_INDIVIDUAL;

int mutations;
int breedings;

void main() {
	
	// Init randomness
	auto rnd = Random(unpredictableSeed);
	
	// Init reference string
	PERFECT_INDIVIDUAL = random_individual();
	//writeln("MR PERFECT: ", PERFECT_INDIVIDUAL);
	
	// Keep track of totals so that we can later compute averages
	int final_bin_count_totals[];
	
	// Keep track of when the success level was reached
	int success_time[EXPERIMENTS + 1];
	
	// Run the experiment several times
	for (int e = 0; e < EXPERIMENTS + 1; e++) {
		
		// Show progress
		//writeln("EXPERIMENT ", e);
	
		// Init population
		string population[] = random_population();
		//writeln("\nPopulation: ", population);
		
		for (int g = 0; g < MAX_GENERATIONS + 1; g++) {
		
			// Init breeding_pool and fitness bins
			string breeding_pool[];
			string fitness_bins[DNA_LENGTH + 1][];
			
			// Measure fitness of individuals in population and sort these into bins, maintaining a bin count
			sort_by_fitness(population, fitness_bins);
			//writeln("\nFitness Bins: ", fitness_bins);
			
			// Check to see whether the population has evolved to a sufficiently successful level
			if (fitness_bins[DNA_LENGTH].length >= (SUCCESS_LEVEL * POPULATION_SIZE)) {
				success_time[e] = g;
				//writeln("SUCCESS at Generation: ", g);
				break;
			}
			
			// Select individuals for admission to breeding pool, based on fitness
			select_breeding_pool(fitness_bins, breeding_pool);
			//writeln("\nBreeding Pool: ", breeding_pool);
			
			// Create a new population by mating individuals in the breeding pool
			random_population_from_pool(breeding_pool, population);
			//writeln("\nPopulation: ", population);
		
		}
		
		// End of experiment
		
	}	// End of all experiments
	
	writeln("Done.");
	
	// Show average success time over all experiments
	print_average_success_time(success_time);

}


// -----------------
// UTILITY FUNCTIONS
// -----------------

void print_average_success_time(int success_time[]) {
    // Prints average of success times
    int total_time_count = 0;
    int occurrence_count = 0;
    float average;
    for (int i = 1; i < EXPERIMENTS + 1; i++) {
        if (success_time[i] != 0) {
            total_time_count += success_time[i];
            occurrence_count++;
        }
    }
    writefln("Successful evolution achieved %d times\n", occurrence_count);
    writefln("Total time count: %d\n", total_time_count);
    if (occurrence_count > 0) {
        average = cast(float)total_time_count / cast(float)occurrence_count;
        printf("Average generations to success: %0.2f\n", average);
    }
	

}

char random_char() {
	auto r = uniform(0, DNA_CHOICES);
	char c = cast(char)('A' + r);
	return c;
}

string random_individual() {
	// Returns a random individual
	string temp;
	while (temp.length < DNA_LENGTH) {
		temp ~= random_char();
	}
	return temp;
}


string[] random_population() {
	// Returns a population of random individuals.
	string population[];
	foreach (int i; 0 .. POPULATION_SIZE) {
		population ~= random_individual;
	}
	return population;
}


int count_similarities(string string_one, string string_two) {
	// Count the similarities between two strings (assumed to be the same length)
	int count = 0;
	foreach(index, letter; string_one) {
		if (string_two[index] == letter) {
			count++;
		}
	}
	return count;
}


void sort_by_fitness(string population[], string fitness_bins[][]) {
    // Iterate over the population and assign items to bins according to how many similarities there are
	int similarities;
	for (int i = 0; i < POPULATION_SIZE; i++) {
		string individual = population[i];
		similarities = count_similarities(PERFECT_INDIVIDUAL, individual);
		fitness_bins[similarities] ~= individual;
	}
}


void select_breeding_pool(string fitness_bins[][], ref string breeding_pool[]) {
    // Select individuals from population for admission to breeding pool, based on fitness
    // We start by selecting from the bin with the most matches and work down,
    // picking randomly from a bin if there are more individuals in the bin than we need.
	for (int i = DNA_LENGTH; i >= 0; i--) {
		// If a bin doesn't have any individuals then we can skip it
		if (fitness_bins[i].length == 0) {
			continue;
		}
		int pool_space = BREEDING_POOL_SIZE - cast(int)breeding_pool.length;
		if (fitness_bins[i].length < pool_space) {
			// If there fewer individuals in the bin than we require then add all of them to the breeding pool
			breeding_pool ~= fitness_bins[i];
		} else {
			// Pick a subset of the individuals to fill up the breeding pool (using randomSample library function)
			foreach (winner; randomSample(fitness_bins[i], pool_space)) {
				breeding_pool ~= winner;
			}
		}
	}
}


void random_population_from_pool(string breeding_pool[], ref string population[]) {
    // Repeatedly select different members from the breeding pool at random and mate them to
    // produce a new individual in the population[] array, stopping when that array is full.
	for (int i = 0; i < POPULATION_SIZE; i++) {
		// Pick two individuals from breeding pool.
		// NOTE: This differs from the C version because we're definitely picking two different individuals.
		string partners[];
		foreach (partner; randomSample(breeding_pool, 2)) {
			partners ~= partner;
		}
		population[i] = child(partners[0], partners[1]);
	}
}


string child(string first_parent, string second_parent) {
	// Returns the offspring of first_parent and second_parent, which may be a mutant!
	string offspring;
	for (int i = 0; i < DNA_LENGTH; i++) {
		if (MUTATION_RATE == 0) {
			// No mutation: choose one of the parent's characters at random
			if (uniform(0.0, 1.0) < 0.5 ) {
				offspring ~= first_parent[i];
			} else {
				offspring ~= second_parent[i];
			}
		} else {
			// See if we need to mutate
			if (uniform(0.0, 1.0) <= MUTATION_RATE) {
				// Mutation: choose a random character
				offspring ~= random_char();
			} else {
				// No mutation: choose one of the parent's characters at random
				if (uniform(0.0, 1.0) < 0.5 ) {
					offspring ~= first_parent[i];
				} else {
					offspring ~= second_parent[i];
				}
			}
		}
	}
	return offspring;
}






























