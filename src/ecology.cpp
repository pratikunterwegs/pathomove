#ifndef ECOLOGY_HPP
#define ECOLOGY_HPP

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
//' @param tmax The number of timesteps per generation.
//' @param nFood The number of food items.
//' @param nClusters Number of clusters around which food is generated.
//' @param clusterDispersal How dispersed food is around the cluster centre.
//' @param landsize The size of the landscape as a numeric (double).
//' @param regenTime Regeneration time of items.
//' @param collective Whether to move collectively.
//' @return A list with data frames of the population movement.
// [[Rcpp::export]]
Rcpp::List getMovement (const int popsize, const double landsize,
                        const int nFood, const int nClusters,
                        const double clusterDispersal,
                        const double regenTime,
                        const bool collective,
                        const double tmax) {

    Population pop (popsize, 0.0);
    pop.setTrait();

    Resources landscape (nFood, landsize, regenTime);
    landscape.initResources(nClusters, clusterDispersal);

    pop.initPos(landscape);

    gsl_rng_set(r, seed);

    moveData thisMoveData;

    double total_act = std::accumulate(pop.trait.begin(), pop.trait.end(), 0.0);
    double trait_array[pop.nAgents];
    std::copy(pop.trait.begin(), pop.trait.end(), trait_array);
    double time = 0.0;
    double feed_time = 0.0;
    double it_t = 0.0;
    size_t id;
    double increment = 0.1;

    // lookup table for discrete distr
    gsl_ran_discrete_t*g = gsl_ran_discrete_preproc(static_cast<size_t>(pop.nAgents), trait_array);

    for(; time < tmax; ) {
        //increment time
        time += gsl_ran_exponential(r, total_act);
        /// movement dynamic
        if (time > it_t) {

            // reduce counter for all
            for (size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++) {
                pop.counter[i] -= time;
                if (pop.counter[i] < 0.0) {
                    pop.counter[i] = 0.0;
                }
            }
            // pick an agent to move
            id = gsl_ran_discrete(r, g);
            // if that agent can move, move it
            if (!(pop.counter[id] > 0.0)) {
                pop.move(id, landscape, moveCost, collective, 1.0);
            }
            it_t = (std::floor(time / increment) * increment) + increment;
        }

        // check which food is available and reduce regeneration time
        landscape.countAvailable();
        for (size_t j = 0; j < static_cast<size_t>(landscape.nItems); j++)
        {
            if(landscape.counter[j] > 0.0) {
                landscape.counter[j] -= time;
            }
            if(landscape.counter[j] < 0.0) {
                landscape.counter[j] = 0.0;
            }
        }

        // when time has advanced by more than an increment,
        // all agents forage and the data is updated
        if (time > feed_time + increment) {
            feed_time = feed_time + time; // increase time here
            // pop forages
            for (size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++) {
                forage(i, landscape, pop, 2.0);
            }

            thisMoveData.updateMoveData(pop, static_cast<int>(std::floor(time)));

        }
    }

    return thisMoveData.getMoveData();

}
