#pragma once
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

//' Get movement in ecological time.
//'
//' @description Run the movement component on a starter population.
//'
//' @param popsize The population size.
//' @param tmax The integer number of timesteps per scene.
//' @param nFood The number of food items.
//' @param nClusters Number of clusters around which food is generated.
//' @param clusterDispersal How dispersed food is around the cluster centre.
//' @param landsize The size of the landscape as a numeric (double).
//' @param collective Whether to move collectively.
//' @param sensoryRange The sensory range of agents.
//' @param stopTime The time (double) that individuals stop after finding food.
//' @param maxAct The maximum activity.
//' @param activityRatio The ratio of activity levels of inactive individuals relative to active individuals.
//' @param pInactive The proportion of inactive individuals in the population.
//' @param scenes Number of scenes.
//' @return A list with data frames of the population movement.
// [[Rcpp::export]]
Rcpp::List do_eco_sim (const int popsize, const double landsize,
                       const int nFood, const int nClusters,
                       const double clusterDispersal,
                       const double maxAct,
                       const double activityRatio,
                       const double pInactive,
                       const bool collective,
                       const double sensoryRange,
                       const int stopTime,
                       const int tmax,
                       const int scenes) {

    Population pop (popsize, 0.0);
    pop.setTraitBimodal(maxAct, activityRatio, pInactive);

    Resources landscape (nFood, landsize, nClusters, clusterDispersal);
    landscape.initResources();

    Network pbsn;
    pbsn.initAssociations(popsize);
    pbsn.initAdjMat (popsize);

    genData thisEcoData;
    moveData thisMoveData;

    pop.initPos(landscape);

    // set seed
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);

    std::vector<int> shuffleVec (pop.nAgents, 0);
    for (int i = 0; i < pop.nAgents; ++i) {
        shuffleVec[i] = i;
    }

    for(int s = 0; s < scenes; s++) {
        landscape.initResources();
        landscape.countAvailable();
        // reset population associations and degree
        pop.associations = std::vector<int> (pop.nAgents, 0);
        pop.degree = std::vector<int> (pop.nAgents, 0);
        pop.energy = std::vector<double> (pop.nAgents, 0.0);

        // reset counter and positions
        pop.counter = std::vector<int> (pop.nAgents, 0);
        pop.initPos(landscape);

        // reset pbsn
        pbsn.initAssociations(popsize);
        pbsn.initAdjMat(popsize);

        // no to gillespie loop
        for (size_t t = 0; t < tmax; t++)
        {
            // movement section
            for (size_t i = 0; i < pop.nAgents; i++)
            {   
                size_t id_to_move = shuffleVec[i];
                // check if agent can move
                if (pop.counter[id_to_move] == 0) {
                    pop.move(id_to_move, landscape, 0.0, collective, sensoryRange); // movecost hardcoded to 0
                } else if (pop.counter[id_to_move] > 0) {
                    pop.counter[id_to_move] --;
                }
            }

            // foraging section
            for (int i = 0; i < pop.nAgents; i++) {
                size_t id_to_move = shuffleVec[i];
                pop.forage(id_to_move, landscape, sensoryRange, stopTime);
                pop.countNeighbours(id_to_move, sensoryRange);
            }

            // PBSN etc
            pop.updatePbsn(pbsn, sensoryRange);
            if(s == scenes - 1) {
                // update move data
                thisMoveData.updateMoveData(pop, static_cast<int>(std::floor(t)));
            }
            
        }
        // timestep ends here        
        pop.degree = getDegree(pbsn);
        thisEcoData.updateGenData(pop, s);

        landscape.countAvailable();
    }

    return Rcpp::List::create(
                Named("trait_data") = thisEcoData.getGenData(),
                Named("pbsn") = pbsn.adjacencyMatrix,
                Named("movedata") = thisMoveData.getMoveData()
    );
}
