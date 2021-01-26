#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "parameters.h"
#include "landscape.h"
#include "network.h"
#include "agents.h"
#include "simulations.cpp"

int main(int argc, char *argv[])
{
    // process cliargs
    std::vector<std::string> cliArgs(argv, argv+argc);
    // gather cli args
    const int genmax = std::stoi(cliArgs[1]);
    const int tmax = std::stoi(cliArgs[2]);
    const int clusters = std::stoi(cliArgs[3]);
    const double dispersal = std::stod(cliArgs[4]);
    //        std::string rep = cliArgs[5];a
    do_simulation(genmax, tmax, clusters, dispersal);

    return 0;

}
