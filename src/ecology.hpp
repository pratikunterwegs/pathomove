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
                       const double stopTime,
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
        double total_act = std::accumulate(pop.trait.begin(), pop.trait.end(), 0.0); // prelim only
        double trait_array[popsize];
        std::copy(pop.trait.begin(), pop.trait.end(), trait_array);
        gsl_ran_discrete_t*g = gsl_ran_discrete_preproc(static_cast<size_t>(popsize), );
        double time = 0.0;
        double eat_time = 0.0;
        double it_t = 0.0;
        size_t id;
        double increment = 0.1;

        // lookup table for discrete distr
        for(; time < tmax; ) {
            // populate rate vector
            std::vector<double> tmpAct;
            std::vector<int> tmpQueue;
            for(int i = 0; i < pop.nAgents; i++) {
                if(!(pop.counter[i] > 0.0)) {
                    tmpAct.push_back(pop.trait[i]);
                    tmpQueue.push_back(i);
                }                   
            }

            // check if anyone can move
            if (tmpAct.size() > 0) {
                // prepare rates
                total_act = std::accumulate(tmpAct.begin(),tmpAct.end(), 0.0);

                double trait_array[static_cast<int>(tmpAct.size())];
                std::copy(tmpAct.begin(), tmpAct.end(), trait_array);

                g = gsl_ran_discrete_preproc(tmpAct.size(), trait_array);

                // main dynamics
                double dt = gsl_ran_exponential(r, total_act);
                time += dt;

                // decrease counters for all
                for(int i = 0; i < pop.nAgents; i++) {
                    if(pop.counter[i] > 0.0) {
                        pop.counter[i] -= dt;
                    }
                    if(pop.counter[i] < 0.0) {
                        pop.counter[i] = 0.0;
                    }                 
                }

                // do move
                if (time > it_t) {
                    id = gsl_ran_discrete(r, g);
                    // which individual to move, not the same as index of rate vec
                    size_t id_to_move = static_cast<size_t>(tmpQueue[id]);
                    pop.move(id_to_move, landscape, moveCost, collective, sensoryRange);
                    it_t = (std::floor(time / increment) * increment) + increment;
                }

                // forage, count neighbours, and update pbsn at save points
                if (time > eat_time) {
                    // add vec shuffle
                    for (int i = 0; i < pop.nAgents; i++) {
                        pop.forage(static_cast<size_t> (i), landscape, sensoryRange, stopTime);
                        pop.countNeighbours(i, sensoryRange);
                    }
                    pop.updatePbsn(pbsn, sensoryRange, landsize);
                    eat_time += increment;
                }
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
