#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "simulations.hpp"

#include <Rcpp.h>

using namespace Rcpp;

// function to evolve population
void evolve_pop(int genmax, double tmax,
                Population &pop,
                Resources &food,
                genData &thisGenData,
                double competitionCost,
                float sensoryRange,
                const int scenes,
                const int stopTime)
{

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
                pop.updateRtree();
                // movement section
                for (size_t i = 0; i < pop.nAgents; i++)
                {
                    size_t id_to_move = shuffleVec[i];
                    // check if agent can move
                    if (pop.counter[id_to_move] <= 0) {
                        pop.move(id_to_move, food, competitionCost, sensoryRange); // movecost hardcoded to 0
                    } else if (pop.counter[id_to_move] > 0) {
                        pop.counter[id_to_move] --;
                    }
                }

                // foraging section
                for (int i = 0; i < pop.nAgents; i++) {
                    size_t id_to_move = shuffleVec[i];
                    pop.forage(id_to_move, food, sensoryRange, stopTime);
                    // count associations
                    pop.associations[id_to_move] += (pop.countNearby(pop.agentRtree, id_to_move, sensoryRange, pop.coordX[id], pop.coordY[id])).first;
                }

                // PBSN etc
                // pop.updatePbsn(pbsn, sensoryRange);
            }
            // timestep ends here
            // pop.degree = getDegree(pbsn);

            // food.countAvailable();
        }
        // generation ends here
        // update gendata
        if ((gen == 0) | (gen % 10 == 0) | (gen == (genmax - 1))) {
            thisGenData.updateGenData(pop, gen);
        }
        // thisNetworkData.updateNetworkData(pop, gen, pbsn);
        // subtract competition costs
        // pop.competitionCosts(competitionCost);
        // reproduce
        pop.Reproduce();
    }
    // all gens end here

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
//' @param nScenes How many scenes.
//' @param stopTime The handling time.
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
Rcpp::List do_simulation(int popsize, int genmax, int tmax, 
                         int nFood, int foodClusters, double clusterDispersal, double landsize,
                         double competitionCost, float sensoryRange,
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

    // prepare data structure
    genData thisGenData;
    // prepare social network struct
    // Network pbsn;
    // pbsn.initAssociations(pop.nAgents);

    // evolve population and store data
    evolve_pop(genmax, tmax, pop, food, thisGenData, competitionCost, sensoryRange, nScenes, stopTime);

    Rcpp::Rcout << "data prepared\n";

    return thisGenData.getGenData();
}
