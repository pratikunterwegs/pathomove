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
//' @param maxAct The maximum acitivity.
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
                       const double tmax,
                       const int scenes) {

    Population pop (popsize, 0.0);
    pop.setTraitBimodal(maxAct, activityRatio, pInactive);

    Resources landscape (nFood, landsize, nClusters, clusterDispersal);
    landscape.initResources();

    Network pbsn;
    pbsn.initAssociations(popsize);

    genData thisGenData;

    pop.initPos(landscape);

    gsl_rng_set(r, seed);

    std::vector<int> shuffleVec (pop.nAgents, 0);
    for (int i = 0; i < pop.nAgents; ++i) {
        shuffleVec[i] = i;
    }

    for(int s = 0; s < scenes; s++) {
        landscape.initResources();
        for(int t = 0; t < tmax; t++) {

            landscape.countAvailable();

            std::random_shuffle(shuffleVec.begin(), shuffleVec.end());
            // reduce counter for all
            for (size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++) {
                size_t id = shuffleVec[i];
                if (gsl_ran_bernoulli(r, pop.trait[id]) == 1) {
                    // move if counter is okay
                    if (!(pop.counter[id] > 0.0)) {
                        pop.move(id, landscape, moveCost, collective, sensoryRange);
                    }
                    pop.counter[id] -= 1.0;
                    if (pop.counter[id] < 0.0) {
                        pop.counter[id] = 0.0;
                    }
                    
                }
                forage(id, landscape, pop, sensoryRange);
                pop.countNeighbours(id, sensoryRange, landsize);
            }

            pop.updatePbsn(pbsn, sensoryRange, landsize);
        }
        pop.degree = getDegree(pbsn, pop);
        thisGenData.updateGenData(pop, s);

        // reset population associations
        pop.associations = std::vector<int> (pop.nAgents, 0);
    }

    return Rcpp::List::create(
                Named("trait_data") = thisGenData.getGenData()
    );
}
