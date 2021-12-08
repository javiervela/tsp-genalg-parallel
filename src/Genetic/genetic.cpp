/**
 * @file genetic.cpp
 * @author (https://www.geeksforgeeks.org/traveling-salesman-problem-using-genetic-algorithm/) & Javier Vela
 * @brief Source file of Genetic Algorithm for solving TSP
 * @version 0.1
 * @date 2021-11-05
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <limits.h>
#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "genetic.h"
#include "mpi.h"

using namespace std;

/**
 * @brief Serialize <first_n> individuals of <population> into 2 vectors
 * 
 * @param population population to serialize
 * @param first_n number of individuals to serialize
 * @param gnome_v vector of integers where to serialize gnome of individuals
 * @param fitness_v vector of float where to serialize fitness of individuals
 */
void serialize_population(std::vector<individual> &population, int first_n, int *gnome_v, float *fitness_v)
{
	gnome_v = new int[first_n * population[1].size() + 1];
	fitness_v = new float[first_n];

	int n = 1;
	int g_v_i = 0;
	int f_v_i = 0;

	// Set gnome size at beggining of vector
	gnome_v[g_v_i++] = population[1].size();

	for (auto indi : population)
	{
		fitness_v[f_v_i++] = indi.fitness;
		for (auto city : indi.gnome)
		{
			gnome_v[g_v_i++] = city;
		}

		// Only do <first_n> individuals in population
		if (n == first_n)
			break;
		n++;
	}
}

/**
 * @brief Deserialize <n> individuals from 2 vectors into <population>
 * 
 * @param population population
 * @param n number of indiiduals to deserialize
 * @param gnome_v vector of integers to deserialize from gnome of individuals
 * @param fitness_v vector of float to deserialize from fitness of individuals
 */
void deserialize_population(std::vector<individual> &population, int n, int *gnome_v, float *fitness_v)
{
	population.clear();

	int g_v_i = 0;
	int f_v_i = 0;

	// Read size of gnomes
	int size_gnome = gnome_v[g_v_i++];

	// Deserialize vectors
	for (int indi_i = 0; indi_i < n; indi_i++)
	{
		individual aux_ind;
		aux_ind.fitness = fitness_v[f_v_i++];
		for (int city_i = 0; city_i < size_gnome; city_i++)
		{
			aux_ind.gnome.push_back(gnome_v[g_v_i++]);
		}
	}
}

/**
 * @brief Function to return a random number from start and end
 *
 * @param start lower limit
 * @param end upper limit
 * @return random integer
 */
int rand_num(int start, int end)
{
	int r = end - start;
	int rnum = start + rand() % r;
	return rnum;
}

/**
 * @brief Function to check if the character has already occurred in the string
 *
 * @param s string to be checked
 * @param ch character to be checked for
 * @return ch appears in s
 */
bool repeat(string s, char ch)
{
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == ch)
			return true;
	}
	return false;
}

/**
 * @brief Function to return a mutated GNOME
 *
 * @param gnome initial gnome
 * @param V size of map
 * @return Mutated GNOME is a string with a random interchange of two genes to create variation in species
 */
std::vector<int> mutate_gnome(std::vector<int> gnome, int V)
{
	bool mutated = false;

	while (!mutated)
	{
		int r = rand_num(1, V);
		int r1 = rand_num(1, V);
		if (r1 != r)
		{
			int temp = gnome[r];
			gnome[r] = gnome[r1];
			gnome[r1] = temp;

			mutated = true;
		}
	}
	return gnome;
}

/**
 * @brief Function to return a valid GNOME string required to create the population
 *
 * @param V size of map
 * @return valid GNOME string
 */
std::vector<int> create_gnome(int V, int initial)
{
	std::vector<int> gnome;
	gnome.push_back(initial);

	for (int i = 1; i < V; i++)
	{
		gnome.push_back(i);
	}

	random_shuffle(gnome.begin(), gnome.end());

	// gnome.push_back(gnome[0]);

	return gnome;
}

/**
 * @brief Function to return the fitness value of a gnome.
 *
 * @param gnome Gnome to be evaluated
 * @param tsp TSP problem object
 * @return The fitness value is the path length of the path represented by the GNOME.
 */
float calculate_fitness(std::vector<int> gnome, Map &tsp)
{
	float f = 0;
	for (int i = 0; i < gnome.size() - 1; i++)
	{
		if (tsp.matrix.at(gnome[i]).at(gnome[i + 1]) == INT_MAX)
			return INT_MAX;
		f += tsp.matrix.at(gnome[i]).at(gnome[i + 1]);
	}
	return f;
}

/**
 * @brief Compare gnome struct
 *
 * @param t1 gnome 1
 * @param t2 gnome 2
 * @return t1 < t2
 */
bool less_than(struct individual const &t1,
			   struct individual const &t2)
{
	return t1.fitness < t2.fitness;
}

/**
 * @brief Print Population (fitness and gnome) of a certain generation
 *
 * @param gen Generation number
 * @param population Vector of individuals
 */
void print_generation(int gen, std::vector<individual> &population)
{
	cout << "Generation " << gen << " \n";
	cout << "GNOME	 FITNESS VALUE\n";

	for (int i = 0; i < population.size(); i++)
	{
		for (int c : population[i].gnome)
			cout << c << ",";
		cout << " " << population[i].fitness << endl;
	}
}

/**
 * @brief Print gnome with best fitness in the population for a certain generation
 *
 * @param gen Generation number
 * @param population Vector of individuals, EXPECTED to be order by fitness
 */
void print_best_gnome(int gen, std::vector<individual> &population, std::ostream &ofs)
{
	bool FINAL = gen < 0;
	if (FINAL && LOG_LEVEL > 1)
	{
		ofs << "Generation FINAL \n";
		ofs << "BEST GNOME	 FITNESS VALUE\n";
		for (int c : population[1].gnome)
			ofs << c << ",";
		ofs << " " << population[1].fitness << endl;
	}
	else if (!FINAL && LOG_LEVEL > 0)
	{
		ofs << gen << "          " << population[1].fitness << endl;
	}
	else if (!FINAL && LOG_LEVEL > 1)
	{
		ofs << "Generation " << gen << " \n";

		ofs << "BEST GNOME	 FITNESS VALUE\n";

		for (int c : population[1].gnome)
			ofs << c << ",";

		ofs << " " << population[1].fitness << endl;
	}
}

/**
 * @brief Execute genetic algorithm
 *
 * @param tsp TSP Problem
 * @param POPULATION_SIZE Desired size of the populations
 * @param NUMBER_GENERATIONS Desired number of generations
 */
void GenAlg(Map &tsp, int POPULATION_SIZE, int NUMBER_GENERATIONS, int CHILD_PER_GNOME, int MAX_NUMBER_MUTATIONS, int GEN_BATCH)
{

	int rank, size;
	int root = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	srand(time(NULL));

	// Generation Number
	int gen = 1;

	vector<struct individual> population;
	struct individual temp;

	// Populating the GNOME pool.
	int initial_city = 0;

	if (rank == 0)
	{
		int value = 17;
		int result = MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		if (result == MPI_SUCCESS)
			std::cout << "Rank 0 OK!" << std::endl;
	}
	else if (rank == 1)
	{
		int value;
		int result = MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
							  MPI_STATUS_IGNORE);
		if (result == MPI_SUCCESS && value == 17)
			std::cout << "Rank 1 OK!" << std::endl;
	}

	return;

	// Root initializes the particles and broadcasts them
	// if (rank == root)
	//{
	for (int i = 0; i < POPULATION_SIZE; i++)
	{
		temp.gnome = create_gnome(tsp.dimension, initial_city);
		temp.fitness = calculate_fitness(temp.gnome, tsp);
		population.push_back(temp);
	}
	//}

	/* DEBUG */ // print_best_gnome(gen, population);

	// Order population based on fitness
	sort(population.begin(), population.end(), less_than);

	// Iteration to perform population crossing and gene mutation (each generation)
	for (gen; gen <= NUMBER_GENERATIONS; gen += GEN_BATCH)
	{
		for (int gen_batch = gen; gen_batch < gen + GEN_BATCH; gen_batch++)
		{
			vector<struct individual> new_population;

			/* SELECTION */
			// POPULATION_SIZE / CHILD_PER_GNOME gnomes are selected to breed next generation

			// The fittest does not mutate
			for (int i = 0; i < CHILD_PER_GNOME; i++)
			{
				new_population.push_back(population[1]);
			}

			// For every other selected member of the population
			for (int i = 1; i < (population.size() / CHILD_PER_GNOME); i++)
			{
				struct individual p1 = population[i];

				/* BREEDING / MUTATING */
				// For simplicity of algorithm selected gnomes will have CHILD_PER_GNOME children
				// These children are computed mutating a random amount of times

				for (int child = 0; child < CHILD_PER_GNOME; child++)
				{
					// Random number of mutations for child
					int number_mutations = ((double)rand() / (double)RAND_MAX) * (MAX_NUMBER_MUTATIONS + 1);
					struct individual paux = p1;
					for (int i = 0; i < number_mutations; i++)
					{
						paux.gnome = mutate_gnome(paux.gnome, tsp.dimension);
					}
					paux.fitness = calculate_fitness(paux.gnome, tsp);
					new_population.push_back(paux);
				}
			}

			population = new_population;

			// Order population based on fitness
			sort(population.begin(), population.end(), less_than);
		}
		/* LOG */ print_best_gnome(gen + GEN_BATCH - 1, population, cout);
	}
	/* LOG */ print_best_gnome(-1, population, cout);
}