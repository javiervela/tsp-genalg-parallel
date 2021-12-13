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
#include <chrono>
#include "genetic.h"
#include "omp.h"
#include "mpi.h"

using namespace std;
using namespace std::chrono;

/**
 * @brief Serialize <first_n> individuals of <population> into 2 vectors
 * 
 * @param population population to serialize
 * @param first_n number of individuals to serialize
 * @param gnome_v vector of integers where to serialize gnome of individuals
 * @param fitness_v vector of float where to serialize fitness of individuals
 */
void serialize_population(std::vector<individual> &population, int first_n, int *gnome_v, float *fitness_v, int size_gnome)
{
	int n = 1;
	int g_v_i = 0;
	int f_v_i = 0;

	for (auto indi : population)
	{
		//cout << "FIRST: " << first_n << " " << g_v_i << " " << f_v_i << endl;

		fitness_v[f_v_i++] = indi.fitness;
		for (auto city : indi.gnome)
		{
			//cout << " city " << city;
			gnome_v[g_v_i++] = city;
		}

		// Only do <first_n> individuals in population
		if (n == first_n)
			break;

		//cout << "n: " << n << endl;
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
void deserialize_population(std::vector<individual> &population, int n, int *gnome_v, float *fitness_v, int size_gnome)
{
	//population.clear();

	int g_v_i = 0;
	int f_v_i = 0;

	// Deserialize vectors
	for (int indi_i = 0; indi_i < n; indi_i++)
	{
		individual aux_ind;
		aux_ind.fitness = fitness_v[f_v_i++];
		for (int city_i = 0; city_i < size_gnome; city_i++)
		{
			aux_ind.gnome.push_back(gnome_v[g_v_i++]);
		}
		population.push_back(aux_ind);
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
 * @brief Function to return a mutated GNOME
 *
 * @param gnome initial gnome
 * @param V size of map
 * @return Mutated GNOME is a string with a random interchange of two genes to create variation in species
 */
std::vector<int> mutate_gnome(std::vector<int> gnome, int V, int thread_id, int thread_total)
{
	int begin = (V * thread_id) / thread_total + 1;
	int end = ((V * (thread_id + 1)) / thread_total);
	bool mutated = false;

	while (!mutated)
	{
		int r = rand_num(begin, end);
		int r1 = rand_num(begin, end);
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

	for (int i = 1; i <= V; i++)
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
 * @brief Print gnome with best fitness in the population for a certain generation, Login levels control verbosity of output
 *
 * @param gen Generation number
 * @param population Vector of individuals, EXPECTED to be order by fitness
 * @param oss output stream
 */
void print_best_gnome(int gen, int mpi_rank, std::vector<individual> &population, std::ostream &oss)
{
	bool FINAL = gen < 0;
	if (FINAL && LOG_LEVEL > 1)
	{
		oss << "Generation FINAL \n";
		oss << "BEST GNOME	 FITNESS VALUE\n";
		for (int c : population[0].gnome)
			oss << c << ",";
		oss << " " << population[0].fitness << endl;
	}
	else if (!FINAL && LOG_LEVEL > 0)
	{
		oss << mpi_rank << "-" << gen << "          " << population[0].fitness << endl;
	}
	else if (!FINAL && LOG_LEVEL > 1)
	{
		oss << "Generation " << gen << " \n";

		oss << "BEST GNOME	 FITNESS VALUE\n";

		for (int c : population[0].gnome)
			oss << c << ",";

		oss << " " << population[0].fitness << endl;
	}
}

/**
 * @brief Execute genetic algorithm
 *
 * @param tsp TSP Problem
 * @param POPULATION_SIZE Desired size of the populations
 * @param NUMBER_GENERATIONS Desired number of generations
 * @param CHILD_PER_GNOME Number of children each individual has through mutations each iteration
 * @param MAX_NUMBER_MUTATIONS Maximum number of mutations per gnome 
 * @param GEN_BATCH Number of generations for each processor before logging (and synchronizing)
 * @param mpi_rank MPI rank of current node
 * @param mpi_size MPI size
 * @param mpi_root MPI rank of the root node
 * @param oss output stream
 * @param best_fitness_sol reference to return the best solution found by node
 * @param execution_time reference to return execution time in milliseconds
 */
void GenAlg(Map &tsp, int POPULATION_SIZE, int NUMBER_GENERATIONS, int CHILD_PER_GNOME, int MAX_NUMBER_MUTATIONS, int GEN_BATCH, int mpi_rank, int mpi_size, int mpi_root, bool SYNC_BATCH, std::ostream &oss, float &best_fitness_sol, microseconds &execution_time)
{

	// Generation Number
	int gen = 1;

	vector<struct individual> population, intermediate_population, new_population;
	struct individual temp;

	// Each node initialize its particles
	int NODE_POPULATION_SIZE;
	if (mpi_rank == mpi_root)
	{
		NODE_POPULATION_SIZE = POPULATION_SIZE / mpi_size + (POPULATION_SIZE - (POPULATION_SIZE / mpi_size) * mpi_size);
	}
	else
	{
		NODE_POPULATION_SIZE = POPULATION_SIZE / mpi_size;
	}

	// Populating the GNOME pool.
	int initial_city = 0;
	for (int i = 0; i < NODE_POPULATION_SIZE; i++)
	{
		temp.gnome = create_gnome(tsp.dimension, initial_city);
		temp.fitness = calculate_fitness(temp.gnome, tsp);
		population.push_back(temp);
	}

	// Order population based on fitness
	sort(population.begin(), population.end(), less_than);

	/* LOG */ print_best_gnome(1, mpi_rank, population, oss);

	auto start = high_resolution_clock::now();

	// Iteration to perform population crossing and gene mutation (each generation)
	for (gen; gen <= NUMBER_GENERATIONS; gen += GEN_BATCH)
	{
		for (int gen_batch = gen; gen_batch < gen + GEN_BATCH; gen_batch++)
		{

			/* SELECTION */
			// POPULATION_SIZE / CHILD_PER_GNOME gnomes are selected to breed next generation

			// The fittest does not mutate
			for (int child = 0; child < CHILD_PER_GNOME; child++)
			{
				new_population.push_back(population[0]);
			}

			/* DEBUG */ // static bool first = true;

#pragma omp parallel
			{
				/* DEBIG */ // if (first)
				/* DEBIG */ // {
				/* DEBIG */ // 	first = false;
				/* DEBIG */ // 	cout << " threads; " << omp_get_num_threads() << endl;
				/* DEBIG */ // }
				// TODO make a population for each thread private and join at the end
				vector<struct individual> thread_population;
				thread_population.clear();
				// For every other selected member of the population

#pragma omp for schedule(dynamic,1)
				for (int member = 1; member < (NODE_POPULATION_SIZE / CHILD_PER_GNOME); member++)
				{
					/* DEBUG */ // cout << "ID: " << omp_get_thread_num() << " TOT: " << omp_get_num_threads() << " member: " << member << endl;
					struct individual p1 = population[member];

					/* BREEDING / MUTATING */
					// For simplicity of algorithm selected gnomes will have CHILD_PER_GNOME children
					// These children are computed mutating a random amount of times
					for (int child = 0; child < CHILD_PER_GNOME; child++)
					{
						// Random number of mutations for child
						int number_mutations = ((double)rand() / (double)RAND_MAX) * (MAX_NUMBER_MUTATIONS + 1);
						struct individual paux = p1;

						// TODO index is passed to the mutate to ensure 2 threads dont get the same city in the gnome and parallelize mutation
						for (int mut_i = 0; mut_i < number_mutations; mut_i++)
						{
							//cout << "member: " << i << " child: " << child << " mutation: " << mut_i << " total n mut: " << number_mutations << " total threads: " << omp_get_num_threads() << endl;
							//paux.gnome = mutate_gnome(paux.gnome, tsp.dimension, mut_i, omp_get_num_threads());
							paux.gnome = mutate_gnome(paux.gnome, tsp.dimension, 0, 1);
						}
						paux.fitness = calculate_fitness(paux.gnome, tsp);
						thread_population.push_back(paux);
					}
				}

#pragma omp critical
				{
					new_population.insert(new_population.end(), thread_population.begin(), thread_population.end());
				}
			}

			population = new_population;
			new_population.clear();

			// Order population based on fitness
			sort(population.begin(), population.end(), less_than);
		}
		/* LOG */ print_best_gnome(gen + GEN_BATCH - 1, mpi_rank, population, oss);

		if (SYNC_BATCH)
		{

			// Share between all of them the best individuals and start from the same population
			int *gnome_v = new int[NODE_POPULATION_SIZE * tsp.dimension];
			float *fitness_v = new float[NODE_POPULATION_SIZE];

			serialize_population(population, NODE_POPULATION_SIZE, gnome_v, fitness_v, tsp.dimension);

			float *received_fitness_v = NULL;
			int *received_gnome_v = NULL;

			if (mpi_rank == mpi_root)
			{
				received_gnome_v = new int[POPULATION_SIZE * tsp.dimension];
				received_fitness_v = new float[POPULATION_SIZE];
			}

			MPI_Gather(fitness_v, NODE_POPULATION_SIZE, MPI_FLOAT, received_fitness_v, NODE_POPULATION_SIZE, MPI_FLOAT, mpi_root, MPI_COMM_WORLD);

			MPI_Gather(gnome_v, NODE_POPULATION_SIZE * tsp.dimension, MPI_INT, received_gnome_v, NODE_POPULATION_SIZE * tsp.dimension, MPI_INT, mpi_root, MPI_COMM_WORLD);

			delete[] gnome_v;
			delete[] fitness_v;

			gnome_v = new int[NODE_POPULATION_SIZE * tsp.dimension];
			fitness_v = new float[NODE_POPULATION_SIZE];
			if (mpi_rank == mpi_root)
			{
				intermediate_population.clear();
				deserialize_population(intermediate_population, POPULATION_SIZE, received_gnome_v, received_fitness_v, tsp.dimension);
				delete[] received_gnome_v, received_fitness_v;

				sort(intermediate_population.begin(), intermediate_population.end(), less_than);

				serialize_population(intermediate_population, NODE_POPULATION_SIZE, gnome_v, fitness_v, tsp.dimension);
			}

			MPI_Bcast(gnome_v, NODE_POPULATION_SIZE * tsp.dimension, MPI_INT, mpi_root, MPI_COMM_WORLD);
			MPI_Bcast(fitness_v, NODE_POPULATION_SIZE, MPI_FLOAT, mpi_root, MPI_COMM_WORLD);

			population.clear();
			deserialize_population(population, NODE_POPULATION_SIZE, gnome_v, fitness_v, tsp.dimension);

			/* LOG */ print_best_gnome(gen + GEN_BATCH - 1, mpi_rank, population, oss);
		}
	}

	auto stop = high_resolution_clock::now();
	execution_time = duration_cast<microseconds>(stop - start);

	// Seriliaze first solution to send to reduce to best
	int first_n = 1;

	int *gnome_v = new int[1 * tsp.dimension];
	float *fitness_v = new float[1];
	float best_fitness_sol_v[1];

	serialize_population(population, first_n, gnome_v, fitness_v, tsp.dimension);
	MPI_Reduce(fitness_v, best_fitness_sol_v, 1, MPI_FLOAT, MPI_MIN, mpi_root, MPI_COMM_WORLD);

	if (mpi_rank == mpi_root)
	{
		best_fitness_sol = best_fitness_sol_v[0];
	}
}