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
#include "ecology.hpp"

#include <Rcpp.h>

using namespace Rcpp;

// function to evolve population
Rcpp::List evolve_pop(int genmax, double tmax,
                      Population &pop, Resources &food, Network &pbsn,
                      double competitionCost,
                      const bool collective,
                      const int scenes)
{
    // make generation data
    genData thisGenData;
    networkData thisNetworkData;
    // set seed
    gsl_rng_set(r, seed);
    // random starting position only in first generation
    pop.initPos(food);
    for(int gen = 0; gen < genmax; gen++) {
        // set up gillespie loop
        double total_act = std::accumulate(pop.trait.begin(), pop.trait.end(), 0.0);
        double trait_array[pop.nAgents];
        std::copy(pop.trait.begin(), pop.trait.end(), trait_array);
        double time = 0.0;
        double feed_time = 0.0;
        double it_t = 0.0;
        size_t id;
        double increment = 0.01;

        // lookup table for discrete distr
        gsl_ran_discrete_t*g = gsl_ran_discrete_preproc(static_cast<size_t>(pop.nAgents), trait_array);

        for (int sc = 0; sc < scenes; sc++) {
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
                        pop.move(id, food, moveCost, collective, 1.0);
                    }
                    it_t = (std::floor(time / increment) * increment) + increment;
                }

                // update network halfway
                if (time > (tmax / 2.0)) {
                    // update population pbsn
                    pop.updatePbsn(pbsn, 2.0, food.dSize);
                }

                // check which food is available
                food.countAvailable();

                // when time has advanced by more than an increment,
                // all agents forage and the PBSN is updated
                if (time > (feed_time + increment)) {
                    feed_time = feed_time + time; // increase time here
                    // pop forages
                    for (size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++) {
                        forage(i, food, pop, 2.0);
                    }
                }
            }
            // scene ends here, make new landscape
            food.initResources();

        }


        // generation ends here
        // update gendata
        thisGenData.updateGenData(pop, gen);
        thisNetworkData.updateNetworkData(pop, gen, pbsn);
        // subtract competition costs
        pop.competitionCosts(competitionCost);
        // reproduce
        pop.Reproduce();
    }
    return Rcpp::List::create(
                Named("trait_data") = thisGenData.getGenData(),
                Named("network_measures") = thisNetworkData.getNetworkData()
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
//' @param collective Whether to move collectively.
//' @param nScenes How many scenes.
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
Rcpp::List do_simulation(int popsize, int genmax, int tmax, 
                         int nFood, int foodClusters, double clusterDispersal, double landsize,
                         double competitionCost, const bool collective,
                         const int nScenes) {

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
    Network pbsn;
    pbsn.initAssociations(pop.nAgents);

    // evolve population and store data
    Rcpp::List evoSimData = evolve_pop(genmax, tmax, pop, food, pbsn, competitionCost, collective, nScenes);

    Rcpp::Rcout << "data prepared\n";

    return evoSimData;
}

//' Export a population.
//'
//' @param popsize The population size.
// [[Rcpp::export]]
DataFrame export_pop(int popsize) {
    Rcpp::Rcout << "in export function";
    Population pop (popsize, 2);
    // pop.initPop(popsize);
    pop.setTrait();

    DataFrame df_pop = DataFrame::create(
                Named("trait") = pop.trait,
                Named("energy") = pop.energy
            );

    return df_pop;
}
