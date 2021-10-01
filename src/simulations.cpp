#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "simulations.hpp"

#include <Rcpp.h>

using namespace Rcpp;

Rcpp::List simulation::do_simulation() {

    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);
    
    // prepare landscape and pop
    food.initResources();
    food.countAvailable();
    Rcpp::Rcout << "landscape with " << food.nClusters << " clusters\n";

    pop.setTrait();
    Rcpp::Rcout << "pop with " << pop.nAgents << " agents for " << genmax << " gens " << tmax << " timesteps\n";

    // prepare scenario
    Rcpp::Rcout << "this is scenario " << scenario << "\n";

    if (scenario == 0) {
        pTransmit = 0.f;
    }
    // go over gens
    for(int gen = 0; gen < genmax; gen++) {
        Rcpp::Rcout << "gen: " << gen << "\n";
        // food.initResources();
        food.countAvailable();

        Rcpp::Rcout << "counted food" << gen << "\n";
        
        // reset counter and positions
        pop.counter = std::vector<int> (pop.nAgents, 0);
        pop.initPos(food);

        Rcpp::Rcout << "randomised agent pos" << gen << "\n";

        if(scenario > 0) {
            pop.introducePathogen(initialInfections);
            Rcpp::Rcout << "introduced pathogen" << gen << "\n";
        }

        // timesteps start here
        for (size_t t = 0; t < static_cast<size_t>(tmax); t++)
        {
            // resources regrow
            food.regenerate();

            pop.updateRtree();

            // movement section
            pop.move(food, nThreads);

            // foraging
            pop.forage(food, nThreads);

            // count associations
            pop.countAssoc(nThreads);

            if(scenario > 0) {
                // disease
                pop.pathogenSpread();
            }

            // timestep ends here
        }
        
        pop.countInfected();
        assert(pop.nInfected <= pop.nAgents);

        // update gendata
        if ((gen == (genmax - 1)) | (gen % 2 == 0)) {
            Rcpp::Rcout << "logging data at gen: " << gen << "\n";
            gen_data.updateGenData(pop, gen);
        }

        // thisNetworkData.updateNetworkData(pop, gen, pbsn);
        
        //population infection cost by time
        pop.pathogenCost(costInfect);

        // reproduce
        pop.Reproduce();

        // generation ends here
    }
    // all gens end here

    Rcpp::Rcout << "data prepared\n";

    return gen_data.getGenData();
}

//' Runs the pathomove simulation.
//'
//' @description Run the simulation using parameters passed as
//' arguments to the corresponding R function.
//'
//' @param scenario The pathomove scenario: 1 for ancestral pathogen, 2 for spillover pathogen.
//' @param popsize The population size.
//' @param nItems How many food items on the landscape.
//' @param landsize The size of the landscape as a numeric (double).
//' @param nClusters Number of clusters around which food is generated.
//' @param clusterSpread How dispersed food is around the cluster centre.
//' @param tmax The number of timesteps per generation.
//' @param genmax The maximum number of generations per simulation.
//' @param range_food The sensory range for food.
//' @param range_agents The sensory range for agents.
//' @param handling_time The handling time.
//' @param regen_time The item regeneration time.
//' @param pTransmit Probability of transmission.
//' @param intialInfections Agents infected per event.
//' @param costInfect Cost infection.
//' @param nThreads How many threads to parallelise over.
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
Rcpp::List run_pathomove(const int scenario,
                        const int popsize,
                        const int nItems, const float landsize,
                        const int nClusters,
                        const float clusterSpread,
                        const int tmax,
                        const int genmax,
                        const float range_food,
                        const float range_agents,
                        const int handling_time,
                        const int regen_time,
                        float pTransmit,
                        const int initialInfections,
                        const float costInfect,
                        const int nThreads) {
                            
    simulation this_sim(popsize, scenario, nItems, landsize,
                        nClusters, clusterSpread, tmax, genmax,
                        range_food, range_agents,
                        handling_time, regen_time,
                        pTransmit, initialInfections, 
                        costInfect, nThreads);
    return this_sim.do_simulation();
}
