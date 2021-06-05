#include <vector>
#include <random>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../src/simulations.h"

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
    float competitionCost = 0.f;
    float sensoryRange = 3.f;
    int nScenes = 1;
    int stopTime = 1;

    // prepare landscape
    Resources food (nFood, landsize, foodClusters, clusterDispersal);
    food.initResources();
    food.countAvailable();
    std::cout << "landscape with " << foodClusters << " clusters\n";
    /// export landscape

    // prepare population
    Population pop (popsize, 0);
    // pop.initPop(popsize);
    pop.setTrait();
    std::cout << pop.nAgents << " agents over " << genmax << " gens of " << tmax << " timesteps\n";

    genData thisGenData;
    // prepare social network struct
    // Network pbsn;
    // pbsn.initAssociations(pop.nAgents);

    // evolve population and store data
    evolve_pop(genmax, tmax, pop, food, thisGenData, competitionCost, sensoryRange, nScenes, stopTime);

    return 0;
}
