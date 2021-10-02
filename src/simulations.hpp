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
               const int regen_time,
               float pTransmit,
               const int initialInfections,
               const float costInfect,
               const int nThreads):
        // population, food, and data structures
        pop (popsize, range_agents, range_food, handling_time, pTransmit),
        food(nItems, landsize, nClusters, clusterSpread, regen_time),
        gen_data (genmax, popsize, 2), // increment hardcoded

        // eco-evolutionary parameters
        scenario(scenario),
        tmax(tmax),
        genmax(genmax),

        // agent perception and behaviour, food growth
        range_food(range_food),
        range_agents(range_agents),
        handling_time(handling_time),
        regen_time(regen_time),

        // disease parameters
        initialInfections(initialInfections),
        costInfect(costInfect),
        pTransmit(pTransmit),

        // parallelisation
        nThreads (nThreads)
    {}
    ~simulation() {}

    Population pop;
    Resources food;
    genData gen_data;
    const int scenario, tmax, genmax;
    const float range_food, range_agents;
    const int handling_time;

    const int regen_time, initialInfections;
    const float costInfect;
    float pTransmit;
    int nThreads;

    // funs
    Rcpp::List do_simulation();

};

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
                        const int nThreads);

#endif // SIMULATIONS_H
