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
               const bool infect_percent):
        // population, food, and data structures
        pop (popsize, range_agents, range_food, range_move, handling_time, pTransmit),
        food(nItems, landsize, nClusters, clusterSpread, regen_time),
        gen_data (genmax, popsize, std::max(static_cast<int>(static_cast<float>(genmax) * 0.001f), 2)), // increment hardcoded

        // eco-evolutionary parameters
        scenario(scenario),
        tmax(tmax),
        genmax(genmax),
        g_patho_init(g_patho_init),

        // agent perception and behaviour, food growth
        range_food(range_food),
        range_agents(range_agents),
        range_move(range_move),
        handling_time(handling_time),
        regen_time(regen_time),

        // disease parameters
        initialInfections(initialInfections),
        costInfect(costInfect),
        pTransmit(pTransmit),

        // parallelisation
        nThreads (nThreads),

        // natal dispersal
        dispersal(dispersal),
        infect_percent(infect_percent),

        // movement data
        mdPre(tmax, popsize),
        mdPost(tmax, popsize)
    {}
    ~simulation() {}

    Population pop;
    Resources food;
    genData gen_data;
    const int scenario, tmax, genmax, g_patho_init;
    const float range_food, range_agents, range_move;
    const int handling_time;

    const int regen_time, initialInfections;
    const float costInfect;
    float pTransmit;
    int nThreads;
    const float dispersal;
    const bool infect_percent;

    moveData mdPre, mdPost;

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
                        const bool infect_percent);

#endif // SIMULATIONS_H
