#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "simulations.h"

#include <Rcpp.h>

using namespace Rcpp;

Rcpp::List simulation::do_simulation() {

    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);
    
    // prepare landscape and pop
    food.initResources();
    food.countAvailable();
    Rcpp::Rcout << "landscape with " << food.nClusters << " clusters\n";

    pop.setTrait(mSize);
    Rcpp::Rcout << "pop with " << pop.nAgents << " agents for " << genmax << " gens " << tmax << " timesteps\n";

    // prepare scenario
    Rcpp::Rcout << "this is scenario " << scenario << "\n";

    // agent random position in first gen
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
        gen_init = g_patho_init;
    }

    Rcpp::Rcout << "spillover in gen: " << gen_init << "\n";

    // go over gens
    for(int gen = 0; gen < genmax; gen++) {

        // Rcpp::Rcout << "gen = " << gen << "\n";

        // food.initResources();
        food.countAvailable();
        // Rcpp::Rcout << "food available = " << food.nAvailable << "\n";

        // reset counter and positions
        pop.counter = std::vector<int> (pop.nAgents, 0);
        
        if((scenario > 0) && (gen > gen_init)) {
            pop.introducePathogen(initialInfections);
        }
        // Rcpp::Rcout << "introduced pathogen if applicable\n";

        // Rcpp::Rcout << "entering ecological timescale\n";

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
            pop.doForage(food);

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

        //population infection cost by time
        if (gen >= gen_init) {
            pop.pathogenCost(costInfect, infect_percent);
        } else {
            pop.energy = pop.intake;
        }

        // update gendata
        if ((gen == (genmax - 1)) | (gen % increment_log == 0)) {

            // Rcpp::Rcout << "logging data at gen: " << gen << "\n";
            gen_data.updateGenData(pop, gen);
        }

        if((gen == 0) | ((gen % (genmax / 10)) == 0) | (gen == genmax - 1)) {
            edgeLists.push_back(pop.pbsn.getNtwkDf());
            Rcpp::Rcout << "gen: " << gen << " --- logged edgelist\n";
        }

        // reproduce
        pop.Reproduce(food, infect_percent, dispersal, mProb, mSize);

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
//' @param g_patho_init The generation in which to begin introducing the pathogen.
//' @param range_food The sensory range for food.
//' @param range_agents The sensory range for agents.
//' @param range_move The movement range for agents.
//' @param handling_time The handling time.
//' @param regen_time The item regeneration time.
//' @param pTransmit Probability of transmission.
//' @param initialInfections Agents infected per event.
//' @param costInfect The per-timestep cost of pathogen infection.
//' @param nThreads How many threads to parallelise over. Set to 1 to run on
//' the HPC Peregrine cluster.
//' @param dispersal A float value; the standard deviation of a normal
//' distribution centred on zero, which determines how far away from its parent
//' each individual is initialised. The standard value is 5 percent of the
//' landscape size (\code{landsize}), and represents local dispersal.
//' Setting this to 10 percent is already almost equivalent to global dispersal.
//' @param infect_percent A boolean value; whether the infection depletes a
//' percentage of daily energy (\code{TRUE}) or whether a fixed value 
//' (\code{FALSE}) is subtracted from net energy.
//' For \code{infect_percent = TRUE}, the net energy remaining after \code{T} 
//' timesteps of infection is \code{N * (1 - cost_infect) ^ T}, where \code{N}
//' is total intake.
//' For \code{infect_percent = FALSE}, the net energy remaining after \code{T} 
//' timesteps of infection is \code{N - (cost_infect * T)}, where \code{N}
//' is total intake.
//' @param mProb The probability of mutation. The suggested value is 0.01.
//' While high, this may be more appropriate for a small population; change this
//' value and \code{popsize} to test the simulation's sensitivity to these values.
//' @param mSize Controls the mutational step size, and represents the scale
//' parameter of a Cauchy distribution. 
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
Rcpp::List run_pathomove(const int scenario,
                        const int popsize,
                        const int nItems, const float landsize,
                        const int nClusters,
                        const float clusterSpread,
                        const int tmax,
                        const int genmax,
                        const int g_patho_init,
                        const float range_food,
                        const float range_agents,
                        const float range_move,
                        const int handling_time,
                        const int regen_time,
                        float pTransmit,
                        const int initialInfections,
                        const float costInfect,
                        const int nThreads,
                        const float dispersal,
                        const bool infect_percent,
                        const float mProb,
                        const float mSize) {
                            
    // check that intial infections is less than popsize
    assert(initialInfections <= popsize && "more infections than agents!");
                            
    simulation this_sim(popsize, scenario, nItems, landsize,
                        nClusters, clusterSpread, tmax, genmax, g_patho_init,
                        range_food, range_agents, range_move,
                        handling_time, regen_time,
                        pTransmit, initialInfections, 
                        costInfect, nThreads, dispersal, infect_percent,
                        mProb, mSize);
    return this_sim.do_simulation();
}

// [[Rcpp::export]]
S4 run_pathomove_s4(const int scenario,
                        const int popsize,
                        const int nItems, const float landsize,
                        const int nClusters,
                        const float clusterSpread,
                        const int tmax,
                        const int genmax,
                        const int g_patho_init,
                        const float range_food,
                        const float range_agents,
                        const float range_move,
                        const int handling_time,
                        const int regen_time,
                        float pTransmit,
                        const int initialInfections,
                        const float costInfect,
                        const int nThreads,
                        const float dispersal,
                        const bool infect_percent,
                        const float mProb,
                        const float mSize) {

    // check that intial infections is less than popsize
    assert(initialInfections <= popsize && "more infections than agents!");
    // make simulation class with input parameters                            
    simulation this_sim(popsize, scenario, nItems, landsize,
                        nClusters, clusterSpread, tmax, genmax, g_patho_init,
                        range_food, range_agents, range_move,
                        handling_time, regen_time,
                        pTransmit, initialInfections, 
                        costInfect, nThreads, dispersal, infect_percent,
                        mProb, mSize);
    // do the simulation using the simulation class function                        
    Rcpp::List pathomoveOutput = this_sim.do_simulation();
    // get generation data from output
    Rcpp::List gen_data = pathomoveOutput["gen_data"];
    // make dataframe of infections and generations
    Rcpp::DataFrame infections_per_gen = Rcpp::DataFrame::create(
        Named("gen") = gen_data["gens"],
        Named("n_infected") = gen_data["n_infected"]
    );

    S4 x("pathomove_output");
    x.slot("scenario") = scenario;
    x.slot("n_gen") = genmax;
    x.slot("gen_patho_intro") = scenario == 0 ? NA_REAL : g_patho_init;
    x.slot("infections_per_gen") = Rcpp::wrap(infections_per_gen);

    return(x);
}
