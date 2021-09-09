#ifndef SIMULATIONS_H
#define SIMULATIONS_H

#include "data_types.hpp"
#include "landscape.hpp"
#include "agents.hpp"

class simulation {
public:
    simulation(const int popsize, const int scenario,
               const int nItems, const float landsize,
               const int nClusters,
               const float clusterSpread,
               const int tmax,
               const int genmax,
               const float range_food,
               const float range_agents,
               const int handling_time,
               const int regen_time):
        pop (popsize),
        food(nItems, landsize, nClusters, clusterSpread, regen_time),
        gen_data (),
        scenario(scenario),
        tmax(tmax),
        genmax(genmax),
        handling_time(handling_time),
        regen_time(regen_time),
        range_food(range_food),
        range_agents(range_agents)
    {}
    ~simulation() {}

    Population pop;
    Resources food;
    genData gen_data;
    const int scenario, tmax, genmax, handling_time, regen_time;
    const float range_food, range_agents;

    // funs
    Rcpp::List do_simulation();

};

//' Runs the pathomove simulation.
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
//' @param sensoryRange The sensory range.
//' @param nScenes How many scenes.
//' @param stopTime The handling time.
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
/*Rcpp::List*/ int run_pathomove(const int popsize, const int scenario,
                                  const int nItems, const float landsize,
                                  const int nClusters,
                                  const float clusterSpread,
                                  const int tmax,
                                  const int genmax,
                                  const float range_food,
                                  const float range_agents,
                                  const int handling_time,
                                  const int regen_time) {
    simulation this_sim(popsize, scenario, nItems, landsize,
                        nClusters, clusterSpread, tmax, genmax,
                        range_food, range_agents,
                        handling_time, regen_time);

    return 0;
}

#endif // SIMULATIONS_H
