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
               const int nInfected,
               const float costInfect):
        pop (popsize, range_agents, range_food, handling_time, pTransmit),
        food(nItems, landsize, nClusters, clusterSpread, regen_time),
        gen_data (),
        scenario(scenario),
        tmax(tmax),
        genmax(genmax),
        handling_time(handling_time),
        regen_time(regen_time),
        range_food(range_food),
        range_agents(range_agents),
        nInfected(nInfected),
        pTransmit(pTransmit),
        costInfect(costInfect)
    {}
    ~simulation() {}

    Population pop;
    Resources food;
    genData gen_data;
    const int scenario, tmax, genmax, handling_time, regen_time, nInfected;
    const float range_food, range_agents, costInfect;
    float pTransmit;

    // funs
    Rcpp::List do_simulation();

};

Rcpp::List run_pathomove(const int popsize, const int scenario,
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
                        const float costInfect);

#endif // SIMULATIONS_H
