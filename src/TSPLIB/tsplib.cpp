#include "tsplib.h"

using namespace std;

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

string trim(string s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// Check and store the value of each keyword
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

// Dislay the solution on the standard output
void printSolution(Map &tsp, float optimalTourSize, float optimalTourCost)
{
    time_t now = time(0);
    tm *localtm = localtime(&now);

    cout << "NAME : " << tsp.name << "." << optimalTourSize << ".tour" << endl;
    cout << "COMMENT : Lenght = " << optimalTourCost << ". Found by John D.C. Little " << asctime(localtm);
    cout << "TYPE : TOUR" << endl;
    cout << "DIMENSION : " << optimalTourSize << endl;
}

string getParam(string cmd, int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        if (argv[i] == cmd && i < argc - 1)
        {
            return argv[i + 1];
        }
    }
    return "";
}

void parseArgs(int argc, char **argv, std::ifstream &problemFileStream, std::ifstream &solutionFileStream)
{
    string inputParam = getParam("-i", argc, argv);
    if (inputParam == "")
    {
        cout << "Error : -i parameter not detected" << endl;
        exit(-1);
    }

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