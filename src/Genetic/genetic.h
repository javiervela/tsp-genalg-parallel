/**
 * @file genetic.hpp
 * @author (https://www.geeksforgeeks.org/traveling-salesman-problem-using-genetic-algorithm/) & Javier Vela
 * @brief Header file of Genetic Algorithm for solving TSP
 * @version 0.1
 * @date 2021-11-05
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef LOG_LEVEL
#define LOG_LEVEL 0
#endif

#include <cstring>
#include "tsplib.h"

/// Structure of a GNOME string defines the path traversed by the salesman while the fitness value of the path is stored in an integer
struct individual
{
	std::vector<int> gnome;
	float fitness;
};

void GenAlg(Map &tsp, int POPULATION_SIZE, int NUMBER_GENERATIONS, int CHILD_PER_GNOME, int MAX_NUMBER_MUTATIONS, int GEN_BATCH, int mpi_rank, int mpi_size, int mpi_root);