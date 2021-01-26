#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../src/parameters.h"
#include "../src/landscape.h"
#include "../src/network.h"
#include "../src/agents.h"
#include "../src/simulations.cpp"

int main(int argc, char *argv[])
{
    // process cliargs
    std::vector<std::string> cliArgs(argv, argv+argc);
    // gather cli args
    const int genmax = std::stoi(cliArgs[1]);
    const int tmax = std::stoi(cliArgs[2]);
    const int clusters = std::stoi(cliArgs[3]);
    const double dispersal = std::stod(cliArgs[4]);
    const double landsize = std::stod(cliArgs[5]);
    //        std::string rep = cliArgs[5];a
//    do_simulation(genmax, tmax, clusters, dispersal);
    export_test_landscapes(clusters, dispersal, 3, landsize);

    return 0;

}
