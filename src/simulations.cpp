#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "parameters.h"
#include "landscape.h"
#include "agents.h"
#include "network.h"
#include "network_operations.hpp"
#include "data_types.h"

#include <Rcpp.h>

using namespace Rcpp;

// function to evolve population
Rcpp::List evolve_pop(int genmax, double tmax,
                      Population &pop, 
                      Resources &food,
                      double competitionCost,
                      const bool collective,
                      const double sensoryRange,
                      const int scenes,
                      const int stopTime)
{
    // make generation data
    genData thisGenData;
    // networkData thisNetworkData;

    // set seed
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);

    std::vector<int> shuffleVec (pop.nAgents, 0);
    for (int i = 0; i < pop.nAgents; ++i) {
        shuffleVec[i] = i;
    }

    // go over gens
    for(int gen = 0; gen < genmax; gen++) {
        // go over scenes
        for(int s = 0; s < scenes; s++) {
        food.initResources();
        food.countAvailable();
        // reset population associations and degree
        // pop.associations = std::vector<int> (pop.nAgents, 0);
        // pop.degree = std::vector<int> (pop.nAgents, 0);

        // reset counter and positions
        pop.counter = std::vector<int> (pop.nAgents, 0);
        pop.initPos(food);

        // reset pbsn
        // pbsn.initAssociations(pop.nAgents);
        // pbsn.initAdjMat(pop.nAgents);

        // no to gillespie loop
        for (size_t t = 0; t < tmax; t++)
        {
            // movement section
            for (size_t i = 0; i < pop.nAgents; i++)
            {   
                size_t id_to_move = shuffleVec[i];
                // check if agent can move
                if (pop.counter[id_to_move] == 0) {
                    // pop.move(id_to_move, food, 0.0, collective, sensoryRange); // movecost hardcoded to 0
                } else if (pop.counter[id_to_move] > 0) {
                    pop.counter[id_to_move] --;
                }
            }

            // foraging section
            for (int i = 0; i < pop.nAgents; i++) {
                size_t id_to_move = shuffleVec[i];
                pop.forage(id_to_move, food, sensoryRange, stopTime);
                pop.countNeighbours(id_to_move, sensoryRange);
            }

            // PBSN etc
            // pop.updatePbsn(pbsn, sensoryRange);            
        }
        // timestep ends here        
        // pop.degree = getDegree(pbsn);

        food.countAvailable();
        }
        // generation ends here
        // update gendata
        if ((gen == 0) | (gen % 10 == 0) | (gen == (genmax - 1))) {
            thisGenData.updateGenData(pop, gen);
        }
        // thisNetworkData.updateNetworkData(pop, gen, pbsn);
        // subtract competition costs
        pop.competitionCosts(competitionCost);
        // reproduce
        pop.Reproduce();
    }
    // all gens end here
    return Rcpp::List::create(
            Named("trait_data") = thisGenData.getGenData()
    );
}

//' Runs the sociality model simulation.
//'
//' @description Run the simulation using parameters passed as
//' arguments to the corresponding R function.
//' 
//' @param popsize The population size.
//' @param genmax The maximum number of generations per simulation.
//' @param tmax The number of timesteps per generation.
//' @param nFood The number of food items.
//' @param foodClusters Number of clusters around which food is generated.
//' @param clusterDispersal How dispersed food is around the cluster centre.
//' @param landsize The size of the landscape as a numeric (double).
//' @param competitionCost Cost of associations.
//' @param sensoryRange The sensory range.
//' @param collective Whether to move collectively.
//' @param nScenes How many scenes.
//' @param stopTime The handling time.
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
Rcpp::List do_simulation(int popsize, int genmax, int tmax, 
                         int nFood, int foodClusters, double clusterDispersal, double landsize,
                         double competitionCost, const double sensoryRange, const bool collective,
                         const int nScenes, const int stopTime) {

    // prepare landscape
    Resources food (nFood, landsize, foodClusters, clusterDispersal);
    food.initResources();
    food.countAvailable();
    Rcpp::Rcout << "landscape with " << foodClusters << " clusters\n";
    /// export landscape

    // prepare population
    Population pop (popsize, 0);
    // pop.initPop(popsize);
    pop.setTrait();
    Rcpp::Rcout << pop.nAgents << " agents over " << genmax << " gens of " << tmax << " timesteps\n";

    // prepare social network struct
    // Network pbsn;
    // pbsn.initAssociations(pop.nAgents);

    // evolve population and store data
    Rcpp::List evoSimData = evolve_pop(genmax, tmax, pop, food,
                                        competitionCost, sensoryRange, 
                                        collective, nScenes, stopTime);

    Rcpp::Rcout << "data prepared\n";

    return evoSimData;
}
