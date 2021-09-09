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
    Rcpp::Rcout << "pop with" << pop.nAgents << " agents for " << genmax << " gens " << tmax << " timesteps\n";

    // prepare social network struct
    // Network pbsn;
    // pbsn.initAssociations(pop.nAgents);

    // set seed
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);

    // go over gens
    for(int gen = 0; gen < genmax; gen++) {
        // go over scenes

            food.initResources();
            food.countAvailable();
            // reset population associations and degree
            // pop.associations = std::vector<int> (pop.nAgents, 0);
            // pop.degree = std::vector<int> (pop.nAgents, 0);

            // reset counter and positions
            pop.counter = std::vector<int> (pop.nAgents, 0);
            pop.initPos(food);

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

                // associations not upated

                // PBSN etc
                // pop.updatePbsn(pbsn, sensoryRange);
            }
            // timestep ends here
            // pop.degree = getDegree(pbsn);

            // food.countAvailable();
        // generation ends here
        // update gendata
        if (gen == (genmax - 1)) {
            gen_data.updateGenData(pop, gen);
        }

        // thisNetworkData.updateNetworkData(pop, gen, pbsn);
        // subtract competition costs
        // pop.competitionCosts(competitionCost);
        // reproduce
        pop.Reproduce();
    }
    // all gens end here

    Rcpp::Rcout << "data prepared\n";

    return gen_data.getGenData();
}
