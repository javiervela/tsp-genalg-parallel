/**
 * @file tsplib.cpp
 * @author Annihil (https://github.com/Annihil/Little-TSP-solver/) & Javier Vela
 * @brief Source file for reading TSPLIB problems and other aditional functionality
 * @version 0.1
 * @date 2021-12-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "tsplib.h"

using namespace std;

/**
 * @brief Read TSPLIB problem into Map struct
 * 
 * @param inputFile input file stream 
 * @return Map Problem information
 */
Map readProblem(ifstream &inputFile)
{
    Map tsp;
    const char delimiter = ':';
    string line;
    bool isMatrix = 0;
    std::vector<City> cities;
    while (inputFile)
    {
        getline(inputFile, line);

        if (line == "EOF" || line == "DISPLAY_DATA_SECTION")
        {
            break;
        }
        if (line.find(delimiter) != string::npos)
        {
            string keyword = line.substr(0, line.find(delimiter));
            string value = line.substr(line.find(delimiter) + 1, line.npos);

            checkKeyword(trim(keyword), trim(value), tsp.name, tsp.dimension);
        }
        if (isMatrix)
        {
            stringstream stream(line);
            City aux;
            stream >> aux.index;
            stream >> aux.x;
            stream >> aux.y;

            cities.push_back(aux);
        }
        if (line == "NODE_COORD_SECTION")
        {
            isMatrix = true;
        }
    }

    // init map
    tsp.matrix = std::vector<std::vector<float>>(tsp.dimension + 1, std::vector<float>(tsp.dimension + 1, 0.0));

    for (City c1 : cities)
    {
        for (City c2 : cities)
        {
            tsp.matrix.at(c1.index).at(c2.index) = sqrt(pow(c2.x - c1.x, 2) + pow(c2.y - c1.y, 2));
        }
    }

    /*  for (auto line : tsp.matrix)
    {
        for (auto city : line)
        {
            cout << city << " ";
        }
        cout << endl;
    } */

    return tsp;
}

/**
 * @brief Read TSPLIB solution into Map struct
 * 
 * @param inputFile input file stream
 * @param tsp Map struct where to read solution
 */
void readSolution(ifstream &inputFile, Map &tsp)
{
    const char delimiter = ':';
    string line;
    bool isSolution = 0;
    std::vector<City> cities;
    while (inputFile)
    {
        getline(inputFile, line);

        if (line == "EOF" || line == "DISPLAY_DATA_SECTION")
        {
            break;
        }
        if (line.find(delimiter) != string::npos)
        {
            string keyword = line.substr(0, line.find(delimiter));
            string value = line.substr(line.find(delimiter) + 1, line.npos);

            checkKeyword(trim(keyword), trim(value), tsp.name, tsp.dimension);
        }
        if (isSolution)
        {
            stringstream stream(line);
            City aux;
            /* DEBUG */ // cout << "reading: " << line << endl;
            stream >> aux.index;
            cities.push_back(aux);
        }
        if (line == "TOUR_SECTION")
        {
            isSolution = true;
        }
    }

    bool first = true;
    City c1, c2;
    tsp.optimalCost = 0.0;
    for (City c1 : cities)
    {
        /* DEBUG */ // cout << "-" << c1.index << "-" << endl;
        if (first)
        {
            first = false;
            c2 = c1;
            /* DEBUG */ // cout << "primero" << endl;
            continue;
        }
        if (c1.index == -1)
        {
            /* DEBUG */ // cout << "ultimo" << endl;
            break;
        }

        /* DEBUG */ // cout << "sumar: " <<tsp.matrix.at(c1.index).at(c2.index) << endl;
        tsp.optimalCost += tsp.matrix.at(c1.index).at(c2.index);
        c2 = c1;
    }

    return;
}

/**
 * @brief Trimp string
 * 
 * @param s string to be trimmed
 * @return string trimmed
 */
string trim(string s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

/**
 * @brief  Check and store the value of each keyword
 * 
 * @param keyword 
 * @param value 
 * @param name name of TSPLIB problem
 * @param dimension dimension of TSPLIB problem
 * @return true if a known keyword has been detected
 * @return false if a unknow keyword has been detected
 */
bool checkKeyword(string keyword, string value, string &name, int &dimension)
{
    if (keyword == "NAME")
    {
        name = value;
    }
    else if (keyword == "DIMENSION")
    {
        dimension = stoi(value);
    }
    else if (keyword == "COMMENT")
    {
    }
    else if (keyword == "TYPE")
    {
    }
    else if (keyword == "EDGE_WEIGHT_TYPE")
    {
    }
    else
    {
        //cout << "Error : Unknown keyword " << keyword << endl;
        return false;
    }
    return true;
}

/**
 * @brief Get a parameter out of command line arguments
 * 
 * @param cmd option to be parsed
 * @param argc 
 * @param argv 
 * @return string value of the option
 */
string getParam(string cmd, int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        if (argv[i] == cmd && i < argc - 1)
        {
            return argv[i + 1];
        }
        if (argv[i] == "-h")
        {
            return "help";
        }
        if (argv[i] == "-S")
        {
            return "sync";
        }
    }
    return "";
}

/**
 * @brief Parse command line options
 * 
 * @param argc 
 * @param argv 
 * @param problemFileStream references to input file stream for problem
 * @param solutionFileStream references to input file stream for solution
 */
void parseArgs(int argc, char **argv, std::ifstream &problemFileStream, std::ifstream &solutionFileStream, int &POPULATION_SIZE, int &CHILD_PER_GNOME, int &MAX_NUMBER_MUTATIONS, int &NUMBER_GENERATIONS, int &GEN_BATCH, bool &SYNC_BATCH)
{

    string helpParam = getParam("-h", argc, argv);
    if (helpParam == "help")
    {
        cout << "E-i <INPUT_FILE>"
             << endl
             << "-P <POPULATION_SIZE>"
             << endl
             << "-C <CHILD_PER_GNOME>"
             << endl
             << "-M <MAX_NUMBER_MUTATIONS>"
             << endl
             << "-G <NUMBER_GENERATIONS>"
             << endl
             << "-B <GEN_BATCH>"
             << endl
             << "-S <SYNC_BATCH>" << endl;
        exit(0);
    }

    string inputParam = getParam("-i", argc, argv);
    if (inputParam == "")
    {
        cout << "Error : -i <INPUT_FILE> parameter not detected" << endl;
        exit(-1);
    }

    string POPULATION_SIZE_string = getParam("-P", argc, argv);
    if (POPULATION_SIZE_string == "")
    {
        cout << "Error : -P <POPULATION_SIZE> parameter not detected" << endl;
        exit(-1);
    }
    else
    {
        POPULATION_SIZE = stoi(POPULATION_SIZE_string);
    }

    string CHILD_PER_GNOME_string = getParam("-C", argc, argv);
    if (CHILD_PER_GNOME_string == "")
    {
        cout << "Error : -C <CHILD_PER_GNOME> parameter not detected" << endl;
        exit(-1);
    }
    else
    {
        CHILD_PER_GNOME = stoi(CHILD_PER_GNOME_string);
    }

    string MAX_NUMBER_MUTATIONS_string = getParam("-M", argc, argv);
    if (MAX_NUMBER_MUTATIONS_string == "")
    {
        cout << "Error : -M <MAX_NUMBER_MUTATIONS> parameter not detected" << endl;
        exit(-1);
    }
    else
    {
        MAX_NUMBER_MUTATIONS = stoi(MAX_NUMBER_MUTATIONS_string);
    }

    string NUMBER_GENERATIONS_string = getParam("-G", argc, argv);
    if (NUMBER_GENERATIONS_string == "")
    {
        cout << "Error : -G <NUMBER_GENERATIONS> parameter not detected" << endl;
        exit(-1);
    }
    else
    {
        NUMBER_GENERATIONS = stoi(NUMBER_GENERATIONS_string);
    }

    string GEN_BATCH_string = getParam("-B", argc, argv);
    if (GEN_BATCH_string == "")
    {
        cout << "Error : -B <GEN_BATCH> parameter not detected" << endl;
        exit(-1);
    }
    else
    {
        GEN_BATCH = stoi(GEN_BATCH_string);
    }

    string SYNC_BATCH_string = getParam("-S", argc, argv);
    SYNC_BATCH = SYNC_BATCH_string == "sync";

    string problemFile = inputParam + ".tsp";
    problemFileStream = ifstream(problemFile);

    string solutionFile = inputParam + ".opt.tour";
    solutionFileStream = ifstream(solutionFile);

    if (!problemFileStream)
    {
        cout << "Error : Input problem file (" << inputParam << ".tsp) not found" << endl;
        exit(-1);
    }

    if (!solutionFileStream)
    {
        cout << "Error : Input solution file (" << inputParam << ".opt.tour) not found" << endl;
    }

    return;
}