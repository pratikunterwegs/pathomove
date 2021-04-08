#include <vector>
#include <random>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../src/parameters.h"
#include "../src/landscape.h"
#include "../src/network.h"
#include "../src/agents.h"
#include "../src/network_operations.hpp"
#include "../src/simulations.cpp"

void test_wrapped_distance () {
    // check for correct case
    bgi::rtree< value, bgi::quadratic<16> > tmpRtree;
    point p = point(0.1, 0.1);
    tmpRtree.insert(std::make_pair(p, 1));
    double unwrapped_dist = wrappedDistance(p, 0.2, 0.1, 10.0);

    assert( fabs((unwrapped_dist - 0.1) < 0.0001) && "regular distance not working" );

    double wrapped_dist = wrappedDistance(p, -0.1, 0.1, 10.0);

    assert( fabs((wrapped_dist - 0.2) < 0.0001) && "wrapped distance not working");
    std::cout << wrapped_dist - 0.2 << "\n"; // not really 0 because floats
    std::cout << "wrapped distance test passes\n";
}

int main(int argc, char *argv[])
{
    // process cliargs
    std::vector<std::string> cliArgs(argv, argv+argc);
    // gather cli args
//    const int genmax = std::stoi(cliArgs[1]);
//    const int tmax = std::stoi(cliArgs[2]);
//    const int clusters = std::stoi(cliArgs[3]);
//    const double dispersal = std::stod(cliArgs[4]);
//    const double landsize = std::stod(cliArgs[5]);
    //        std::string rep = cliArgs[5];a
//    do_simulation(genmax, tmax, clusters, dispersal, landsize);
//    export_test_landscapes(clusters, dispersal, landsize, 3);
    test_wrapped_distance();
}
