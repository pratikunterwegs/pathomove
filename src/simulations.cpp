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

    // agent random position
    pop.initPos(food);

    Rcpp::Rcout << "initialised population positions\n";
    Rcpp::List edgeLists;

    Rcpp::Rcout << "created edge list object\n";

    // agent data logging increment
    int increment_log = std::max((static_cast<int>(static_cast<float>(genmax) * 0.001f)), 2);

    Rcpp::Rcout << "logging data after gens: " << increment_log << "\n";

    if (scenario == 0) {
        pTransmit = 0.f;
    }
    int gen_init = 0;
    if (scenario == 2) {
        gen_init = static_cast<int>(std::round(static_cast<float>(genmax) * 0.667));
        
    }

    Rcpp::Rcout << "spillover in gen: " << gen_init << "\n";

    // go over gens
    for(int gen = 0; gen < genmax; gen++) {

        Rcpp::Rcout << "gen = " << gen << "\n";

        // food.initResources();
        food.countAvailable();
        Rcpp::Rcout << "food available = " << food.nAvailable << "\n";

        // reset counter and positions
        pop.counter = std::vector<int> (pop.nAgents, 0);
        Rcpp::Rcout << "resetting agent counter\n";
        // pop.initPos(food);

        if((scenario > 0) && (gen > gen_init)) {
            pop.introducePathogen(initialInfections);
        }
        Rcpp::Rcout << "introduced pathogen if applicable\n";

        Rcpp::Rcout << "entering ecological timescale\n";

        // timesteps start here
        for (size_t t = 0; t < static_cast<size_t>(tmax); t++)
        {
            // resources regrow
            food.regenerate();
            pop.updateRtree();
            // movement section
            pop.move(food, nThreads);

            // log movement
            if(gen == std::max(gen_init - 1, 2)) {
                mdPre.updateMoveData(pop, t);
            }
            if(gen == (genmax - 1)) {
                mdPost.updateMoveData(pop, t);
            }

            // foraging -- split into parallelised picking
            // and non-parallel exploitation
            pop.pickForageItem(food, nThreads);
            pop.doForage(food, nThreads);

            // count associations
            pop.countAssoc(nThreads);
            if((scenario > 0) && (gen > gen_init)) {
                // disease
                pop.pathogenSpread();
            }

            // timestep ends here
        }
        
        pop.countInfected();

        assert(pop.nInfected <= pop.nAgents);

        // update gendata
        if ((gen == (genmax - 1)) | (gen % increment_log == 0)) {
            // Rcpp::Rcout << "logging data at gen: " << gen << "\n";
            gen_data.updateGenData(pop, gen);
        }
        
        //population infection cost by time
        pop.pathogenCost(costInfect);
        
        Rcpp::Rcout << "gen: " << gen << "\n";

        if((gen == 0) | ((gen % (genmax / 10)) == 0) | (gen == genmax - 1)) {
            edgeLists.push_back(pop.pbsn.getNtwkDf());
            Rcpp::Rcout << "logged edgelist\n";
        }

        // reproduce
        pop.Reproduce();

        // generation ends here
    }
    // all gens end here

    Rcpp::Rcout << "data prepared\n";

    return Rcpp::List::create(
        Named("gen_data") = gen_data.getGenData(),
        Named("edgeLists") = edgeLists,
        Named("move_pre") = mdPre.getMoveData(),
        Named("move_post") = mdPost.getMoveData()
    );
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
//' @param range_move The movement range for agents.
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
                        const float range_move,
                        const int handling_time,
                        const int regen_time,
                        float pTransmit,
                        const int initialInfections,
                        const float costInfect,
                        const int nThreads) {
                            
    simulation this_sim(popsize, scenario, nItems, landsize,
                        nClusters, clusterSpread, tmax, genmax,
                        range_food, range_agents, range_move,
                        handling_time, regen_time,
                        pTransmit, initialInfections, 
                        costInfect, nThreads);
    return this_sim.do_simulation();
}
