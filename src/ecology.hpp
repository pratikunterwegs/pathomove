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
    pbsn.initAdjMat (popsize);

    genData thisEcoData;

    pop.initPos(landscape);

    gsl_rng_set(r, seed);

    std::vector<int> shuffleVec (pop.nAgents, 0);
    for (int i = 0; i < pop.nAgents; ++i) {
        shuffleVec[i] = i;
    }

    for(int s = 0; s < scenes; s++) {
        landscape.initResources();

        // reset population associations and degree
        pop.associations = std::vector<int> (pop.nAgents, 0);
        pop.degree = std::vector<int> (pop.nAgents, 0);
        pop.energy = std::vector<double> (pop.nAgents, 0.0);

        // reset pbsn
        pbsn.initAssociations(popsize);
        pbsn.initAdjMat(popsize);

        // set up gillespie loop
        double total_act = std::accumulate(pop.trait.begin(), pop.trait.end(), 0.0);
        double trait_array[pop.nAgents];
        std::copy(pop.trait.begin(), pop.trait.end(), trait_array);
        double time = 0.0;
        double eat_time = 0.0;
        double it_t = 0.0;
        size_t id;
        double increment = 0.1;

        // lookup table for discrete distr
        gsl_ran_discrete_t*g = gsl_ran_discrete_preproc(static_cast<size_t>(pop.nAgents), trait_array);
        for(; time < tmax; ) {
            time += gsl_ran_exponential(r, total_act);

            // do move
            if (time > it_t) {
                id = gsl_ran_discrete(r, g);
                pop.move(id, landscape, moveCost, collective, sensoryRange);
                it_t = (std::floor(time / increment) * increment) + increment;
            }

            // forage, count neighbours, and update pbsn at save points
            if (time > eat_time) {
                for (int i = 0; i < pop.nAgents; i++) {
                    forage(static_cast<size_t> (i), landscape, pop, sensoryRange);
                    pop.countNeighbours(i, sensoryRange);
                    pop.updatePbsn(pbsn, sensoryRange, landsize);
                }
                eat_time += increment;
            }
        }
        pop.degree = getDegree(pbsn);
        thisEcoData.updateGenData(pop, s);
    }

    return Rcpp::List::create(
                Named("trait_data") = thisEcoData.getGenData(),
                Named("pbsn") = pbsn.adjacencyMatrix
    );
}
