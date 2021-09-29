#include <vector>
#include <random>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../src/simulations.hpp"

int main(int argc, char *argv[])
{
    // process cliargs
    std::vector<std::string> cliArgs(argv, argv+argc);

    int nFood = 5;
    float landsize = 5.0f;
    int foodClusters = 2;
    float clusterDispersal = 1.0f;
    int popsize = 5;
    int genmax = 1;
    int tmax = 2;
    int regen_time = 1;

    // prepare landscape
    Resources food (nFood, landsize, foodClusters, clusterDispersal, regen_time);
    food.initResources();
    food.countAvailable();
    std::cout << "landscape with " << foodClusters << " clusters\n";
    /// export landscape

    // prepare population
    Population pop (popsize, 0.1, 0.2, 1, 0.3);
    // pop.initPop(popsize);
    pop.setTrait();
    std::cout << pop.nAgents << " agents over " << genmax << " gens of " << tmax << " timesteps\n";

    return 0;
}
