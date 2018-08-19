// Evolution experiment
// Jacob Conrad Martin
// http://jacobconradmartin.com

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

// ----------------
// GLOBAL VARIABLES
// ----------------

// Keep these fixed over one set of experiments.
// Note that we have to #define some of these so that we can use them for initialising arrays.
// This is really sucky and is discussed here: http://stackoverflow.com/questions/1674032/static-const-vs-define-in-c

const float SUCCESS_LEVEL = 0.75;
const int DNA_CHOICES = 4;
const int MAX_GENERATIONS = 1000;
#define POPULATION_SIZE        1000
#define DNA_LENGTH             16
#define EXPERIMENTS            100
// See how sucky C can be!
#define DNA_LENGTH_PLUS_ONE    DNA_LENGTH + 1
#define EXPERIMENTS_PLUS_ONE   EXPERIMENTS + 1

// Vary these during one set of experiments

const int BREEDING_POOL_SIZE = 100; // How many individuals selected for breeding each turn.
const int MUTATION_RATE = 100;     // Copying error occurs 1/MUTATION_RATE times (disabled by setting to zero).

// This will vary (although not during an experiment!) and needs to be globally accessible.

char* PERFECT_INDIVIDUAL;   // The theoretically "perfectly fit" individual against which others are compared.


// -------------------
// FUNCTION PROTOTYPES
// -------------------

char random_char(void);
char* random_individual(void);

unsigned int count_similarities(char* string_one, char* string_two);

void print_average_success_time(int success_time[]);

void child(char* first_parent, char* second_parent, char* offspring);

void random_population(char* population[]);
void random_population_from_pool(char* population[], char* breeding_pool[]);

void sort_by_fitness(char* population[], char* bins[][POPULATION_SIZE], int bin_count[]);
void select_breeding_pool(char* bins[][POPULATION_SIZE], int bin_count[], char* breeding_pool[]);


// ---------
// MAIN LOOP
// ---------

int main(void) {
    
    // Init randomness
    srand((int)time(NULL));
    
    // Keep track of totals so that we can later compute averages
    int final_bin_count_totals[DNA_LENGTH_PLUS_ONE] = {0};
    
    // Init reference string
    PERFECT_INDIVIDUAL = random_individual();
    
    // Keep track of when the success level was reached
    int success_time[EXPERIMENTS_PLUS_ONE] = {0};
    
    // Run the experiment several times
    for (int e = 1; e < EXPERIMENTS_PLUS_ONE; e++) {
        
        // Show progress
        printf("\n%d",e);
        
        // Init population, new_population and breeding_pool
        char* population[POPULATION_SIZE];
        char* breeding_pool[BREEDING_POOL_SIZE];
        
        // Init bins and bin_count (to track fitness)
        char* bins[DNA_LENGTH_PLUS_ONE][POPULATION_SIZE] = {0};
        int bin_count[DNA_LENGTH_PLUS_ONE] = {0};
        
        // Create random starting population
        random_population(population);
        
        for (int g = 0; g < MAX_GENERATIONS + 1; g++) {
            
            // Measure fitness of individuals in population and sort these into bins, maintaining a bin count
            sort_by_fitness(population, bins, bin_count);
            
            // Check to see whether the population has evolved to a sufficiently successful level
            if (bin_count[DNA_LENGTH] >= (SUCCESS_LEVEL * POPULATION_SIZE)) {
                success_time[e] = g;
                break;
            }
            
            // Select individuals from population for admission to breeding pool, based on fitness
            select_breeding_pool(bins, bin_count, breeding_pool);
            
            // Create a new population by mating individuals in the breeding pool
            random_population_from_pool(population, breeding_pool);
            
            // Initialise bins and bin_count
            for (int k = 0; k < DNA_LENGTH + 1; k++) {
                bin_count[k] = 0;
                for (int l = 0; l < POPULATION_SIZE; l++) {
                    bins[k][l] = 0;
                }
            }
            
        }
        
        // Do final fitness sort
        sort_by_fitness(population, bins, bin_count);
        
        // Update the total bin counts
        for (int i = 0; i < DNA_LENGTH + 1; i++) {
            final_bin_count_totals[i] += bin_count[i];
            for (int j = 0; j < bin_count[i]; j++) {
                bins[i][j] = 0;
            }
            bin_count[i] = 0;
        }
        
        // End of experiment
        
        // Free population memory
        for (int i = 0; i < POPULATION_SIZE; i++) {
            free(population[i]);
        }
        
    } // End of all experiments
    
    // Show average success time over all experiments
    // FIXME: Assumes they were all successful?
    printf("\n-- AVERAGE SUCCESS TIME: ---\n");
    print_average_success_time(success_time);
    
    return 0;
    
}


// ----------------
// OUTPUT FUNCTIONS
// ----------------


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
    printf("Successful evolution achieved %d times\n", occurrence_count);
    printf("Total time count: %d\n", total_time_count);
    if (occurrence_count > 0) {
        average = (float)total_time_count / (float)occurrence_count;
        printf("Average generations to success: %0.2f\n", average);
    }
}


// -----------------
// UTILITY FUNCTIONS
// -----------------


char random_char() {
    // Returns a random character
    int r = rand() % DNA_CHOICES;
    return 'A' + r;
}


void child(char* first_parent, char* second_parent, char* offspring) {
    // Returns the offspring of first_parent and second_parent, which may be a mutant!
    int r, r1;
    for (int i = 0; i < DNA_LENGTH; i++) {
        if (MUTATION_RATE == 0) {
            // No mutation: choose one of the parents' characters at random
            r = rand() % 1;
            if (r == 0) {
                offspring[i] = first_parent[i];
            } else {
                offspring[i] = second_parent[i];
            }
        } else {
            // See if we need to mutate
            r1 = rand() % MUTATION_RATE;
            if (r1 == 0) {
                // Mutation: choose a random character
                offspring[i] = random_char();
            } else {
                // No mutation: choose one of the parents' characters at random
                r = rand() % 1;
                if (r == 0) {
                    offspring[i] = first_parent[i];
                } else {
                    offspring[i] = second_parent[i];
                }
            }
        }
    }
}


char* random_individual() {
    // Returns a random individual.
    char* buffer = malloc(DNA_LENGTH * sizeof(char) + 1);
    for (int i = 0; i < DNA_LENGTH; i++) {
        buffer[i] = random_char();
    }
    buffer[DNA_LENGTH] = '\0';
    return buffer;
}


void random_population(char* population[]) {
    // Creates a random population of individuals and puts them in the population[] array.
    for (int i = 0; i < POPULATION_SIZE; i++) {
        // Generate a new individual
        population[i] = random_individual();
    }
}


void random_population_from_pool(char* population[], char* breeding_pool[]) {
    // Repeatedly select different members from the breeding pool at random and mate them to
    // produce a new individual in the population[] array, stopping when that array is full.
    int new_population_size = 0;
    int r1, r2;
    char* first_parent;
    char* second_parent;
    while (new_population_size < POPULATION_SIZE) {
        // Pick one individual from breeding pool
        r1 = rand() % BREEDING_POOL_SIZE;
        first_parent = breeding_pool[r1];
        // Select another item, ensuring it's not the same one
        r2 = r1;
        while (r2 == r1) {
            r2 = rand() % BREEDING_POOL_SIZE;
        }
        second_parent = breeding_pool[r2];
        // Mate them and add the offspring to the new population pool
        child(first_parent, second_parent, population[new_population_size]);
        new_population_size++;
    }
}


unsigned int count_similarities(char* string_one, char* string_two) {
    // Count the similarities between two strings (assumed to be the same length)
    int count = 0;
    int length = (int)strlen(string_one);
    for (int i = 0; i < length; i++) {
        if (string_one[i] == string_two[i]) {
            count++;
        }
    }
    return count;
}


void sort_by_fitness(char* population[], char* bins[][POPULATION_SIZE], int bin_count[]) {
    // Iterate over the population and assign items to bins according to how many
    // similarities there are, keeping track of the number of items in each bin
    int similarities;
    for (int i = 0; i < POPULATION_SIZE; i++) {
        similarities = count_similarities(PERFECT_INDIVIDUAL, population[i]);
        bins[similarities][bin_count[similarities]] = population[i];
        bin_count[similarities] += 1;
    }
}


void select_breeding_pool(char* bins[][POPULATION_SIZE], int bin_count[], char* breeding_pool[]) {
    // Select individuals from population for admission to breeding pool, based on fitness
    // We start by selecting from the bin with the most matches and work down,
    // picking randomly from a bin if there are more individuals in the bin than we need.
    int r;
    int pool_size = 0;
    int pool_space;
    for (int i = DNA_LENGTH; i >= 0; i--) {
        //If a bin doesn't have any individuals in it then we can skip it
        if (bin_count[i] > 0) {
            pool_space = BREEDING_POOL_SIZE - pool_size;
            if ( bin_count[i] < pool_space ) {
                // If there fewer individuals in the bin than we require then add all of them to the breeding pool
                for (int j = 0; j < bin_count[i]; j++) {
                    breeding_pool[pool_size] = bins[i][j];
                    pool_size++;
                }
            } else {
                // Pick a subset of the individuals to fill up the breeding pool
                // FIXME: If there are a lot of individuals in the bin and we want
                // a large number of them then this approach is not very efficient.
                while (pool_space > 0) {
                    r = rand() % bin_count[i];
                    if (bins[i][r] != NULL) {
                        breeding_pool[pool_size] = bins[i][r];
                        pool_size++;
                        pool_space--;
                        bins[i][r] = NULL;    // Prevent this one being chosen again
                    }
                }
            }
        }
    }
}
