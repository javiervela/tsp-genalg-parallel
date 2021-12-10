/**
 * @file tsplib.h
 * @author Annihil (https://github.com/Annihil/Little-TSP-solver/) & Javier Vela
 * @brief Header file for reading TSPLIB problems and other aditional functionality
 * @version 0.1
 * @date 2021-12-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef TSPLIB_H
#define TSPLIB_H

#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <sstream>

struct Map
{
    std::string name;
    int dimension;
    std::vector<std::vector<float>> matrix;
    float optimalCost;
};

struct City
{
    int index;
    float x, y; // Coordinates
};

Map readProblem(std::ifstream &inputFile);
void readSolution(std::ifstream &inputFile, Map &tsp);
std::string trim(std::string s);
bool checkKeyword(std::string keyword, std::string value, std::string &name, int &dimension);
void printSolution(Map &tsp, float optimalTourSize, float optimalTourCost);
std::string getParam(std::string cmd, int argc, char **argv);
void parseArgs(int argc, char **argv, std::ifstream &problemFileStream, std::ifstream &solutionFileStream);

#endif /* TSPLIB_H */