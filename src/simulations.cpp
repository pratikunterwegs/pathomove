#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "simulations.hpp"

#include <Rcpp.h>

using namespace Rcpp;

Rcpp::List simulation::do_simulation() {

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


    // prepare social network struct
    // Network pbsn;
    // pbsn.initAssociations(pop.nAgents);

    // set seed
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);

    // go over gens
    for(int gen = 0; gen < genmax; gen++) {
        // go over scenes

            // food.initResources();
            food.countAvailable();
            // reset population associations and degree
            // pop.associations = std::vector<int> (pop.nAgents, 0);
            // pop.degree = std::vector<int> (pop.nAgents, 0);

            // reset counter and positions
            pop.counter = std::vector<int> (pop.nAgents, 0);
            pop.initPos(food);

            if(scenario > 0) {
                pop.introducePathogen(nInfected);
            }

            // reset pbsn
            // pbsn.initAssociations(pop.nAgents);
            // pbsn.initAdjMat(pop.nAgents);

            // timesteps start here
            for (size_t t = 0; t < static_cast<size_t>(tmax); t++)
            {
                // resources regrow
                food.regenerate();

                pop.updateRtree();

                // movement section
                pop.move(food);

                // foraging
                pop.forage(food);

                // disease
                pop.pathogenSpread();

                // PBSN etc
                // pop.updatePbsn(pbsn, sensoryRange);
            }
            // timestep ends here
            // pop.degree = getDegree(pbsn);

            // food.countAvailable();
        // generation ends here
        // update gendata
        if ((gen == (genmax - 1)) | (gen % 10 == 0)) {
            gen_data.updateGenData(pop, gen);
        }

        // thisNetworkData.updateNetworkData(pop, gen, pbsn);
        
        //population infection cost by time
        pop.pathogenCost(costInfect);

        // reproduce
        pop.Reproduce();
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
//' @param nInfected Agents infected per event.
//' @param costInfect Cost infection.
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
                        const int nInfected,
                        const float costInfect) {
                            
    simulation this_sim(popsize, scenario, nItems, landsize,
                        nClusters, clusterSpread, tmax, genmax,
                        range_food, range_agents,
                        handling_time, regen_time,
                        pTransmit, nInfected, costInfect);
    return this_sim.do_simulation();
}
