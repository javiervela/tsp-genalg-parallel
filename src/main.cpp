/**
 * @file main.cpp
 * @author Javier Vela
 * @brief Main Source File of implementation of TSP solver with Genetic Algorithms
 * @version 0.1
 * @date 2021-11-04
 *
 */

#include <cstring>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include "tsplib.h"
#include "genetic.h"
#include "mpi.h"

using namespace std;

/// Initial population size for the algorithm
#define POPULATION_SIZE 10000
/// Number of child per gnome in breeding (must be a divisors of POPULATION_SIZE)
#define CHILD_PER_GNOME 100
/// Maximum number of mutations per child
#define MAX_NUMBER_MUTATIONS 10
/// Number of generations for the algorithm
#define NUMBER_GENERATIONS 1000
/// Number of generations computed per batch (before synchronizing all processors) (must be a divisors of NUMBER_GENERATIONS)
#define GEN_BATCH 5

/**
 * @brief Main procedure
 */
int main(int argc, char **argv)
{

	MPI_Init(&argc, &argv);

	int mpi_rank, mpi_size;
	int mpi_root = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

	srand(time(NULL) + mpi_rank);

	ifstream probfs, solfs;
	parseArgs(argc, argv, probfs, solfs);
	Map tsp = readProblem(probfs);
	GenAlg(tsp, POPULATION_SIZE, NUMBER_GENERATIONS, CHILD_PER_GNOME, MAX_NUMBER_MUTATIONS, GEN_BATCH, mpi_rank, mpi_size, mpi_root);
	readSolution(solfs, tsp);
	cout << "OPT          " << tsp.optimalCost << endl;

	MPI_Finalize();

	return 0;
}
