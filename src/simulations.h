// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
// simulation class for pathomove
#ifndef SRC_SIMULATIONS_H_
#define SRC_SIMULATIONS_H_

// clang-format off
#include <algorithm>

#include "landscape.h"
#include "agents.h"
#include "data_types.h"
// clang-format on

class simulation {
 public:
  simulation(const int popsize, const int scenario, const int nItems,
             const float landsize, const int nClusters,
             const float clusterSpread, const int tmax, const int genmax,
             const int g_patho_init, const float range_food,
             const float range_agents, const float range_move,
             const int handling_time, const int regen_time, float pTransmit,
             const int initialInfections, const float costInfect,
             const bool multithreaded, const float dispersal,
             const bool infect_percent, const bool vertical,
             const bool reprod_threshold, const float mProb, const float mSize,
             const float spillover_rate)
      :  // population, food, and data structures
        pop(popsize, range_agents, range_food, range_move, handling_time,
            pTransmit, vertical, reprod_threshold),
        food(nItems, landsize, nClusters, clusterSpread, regen_time),
        gen_data(),  // increment hardcoded

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
        spillover_rate(spillover_rate),

        // parallelisation
        multithreaded(multithreaded),

        // natal dispersal and pathogen cost structure
        dispersal(dispersal),
        infect_percent(infect_percent),
        vertical(vertical),
        reprod_threshold(reprod_threshold),

        // mutation probability and step size
        mProb(mProb),
        mSize(mSize),

        // movement data
        mdPre(tmax, popsize),
        mdPost(tmax, popsize) {}
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
  const float spillover_rate;

  const bool multithreaded;
  const float dispersal;
  const bool infect_percent;
  const bool vertical, reprod_threshold;

  const float mProb, mSize;

  moveData mdPre, mdPost;

  // funs
  Rcpp::List do_simulation();
};

#endif  // SRC_SIMULATIONS_H_
