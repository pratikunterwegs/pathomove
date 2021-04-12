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
    test_wrapped_distance();

    return 0;
}
