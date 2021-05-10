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
// Rcpp::List evolve_pop(int genmax, double tmax,
//                       Population &pop, Resources &food, Network &pbsn,
//                       double competitionCost,
//                       const bool collective,
//                       const int scenes)
// {
//     // make generation data
//     genData thisGenData;
//     networkData thisNetworkData;
//     // set seed
//     // gsl_rng_set(r, seed);
//     // random starting position only in first generation
//     pop.initPos(food);
//     for(int gen = 0; gen < genmax; gen++) {
        
//         }
//         // generation ends here
//         // update gendata
//         // thisGenData.updateGenData(pop, gen);
//         // thisNetworkData.updateNetworkData(pop, gen, pbsn);
//         // // subtract competition costs
//         // pop.competitionCosts(competitionCost);
//         // // reproduce
//         // pop.Reproduce();
// //    }
//     return Rcpp::List::create(
//                 Named("trait_data") = thisGenData.getGenData(),
//                 Named("network_measures") = thisNetworkData.getNetworkData()
//             );
// }

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
// Rcpp::List do_simulation(int popsize, int genmax, int tmax, 
//                          int nFood, int foodClusters, double clusterDispersal, double landsize,
//                          double competitionCost, const bool collective,
//                          const int nScenes) {

//     // prepare landscape
//     Resources food (nFood, landsize, foodClusters, clusterDispersal);
//     food.initResources();
//     food.countAvailable();
//     Rcpp::Rcout << "landscape with " << foodClusters << " clusters\n";
//     /// export landscape

//     // prepare population
//     Population pop (popsize, 0);
//     // pop.initPop(popsize);
//     pop.setTrait();
//     Rcpp::Rcout << pop.nAgents << " agents over " << genmax << " gens of " << tmax << " timesteps\n";

//     // prepare social network struct
//     Network pbsn;
//     pbsn.initAssociations(pop.nAgents);

//     // evolve population and store data
//     Rcpp::List evoSimData = evolve_pop(genmax, tmax, pop, food, pbsn, competitionCost, collective, nScenes);

//     Rcpp::Rcout << "data prepared\n";

//     return evoSimData;
// }

//' Export a population.
//'
//' @param popsize The population size.
// DataFrame export_pop(int popsize) {
//     Rcpp::Rcout << "in export function";
//     Population pop (popsize, 2);
//     // pop.initPop(popsize);
//     pop.setTrait();

//     DataFrame df_pop = DataFrame::create(
//                 Named("trait") = pop.trait,
//                 Named("energy") = pop.energy
//             );

//     return df_pop;
// }
