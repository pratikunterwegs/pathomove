// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
// simulation class for pathomove
#ifndef INST_INCLUDE_SIMULATIONS_H_
#define INST_INCLUDE_SIMULATIONS_H_

// clang-format off
#include <algorithm>
#include <string>
#include <vector>

#include "data_types.h"
// clang-format on

namespace pathomove {
class simulation {
 public:
  simulation(const int popsize, const int scenario, const int nItems,
             const float landsize, const int nClusters,
             const float clusterSpread, const int tmax, const int genmax,
             const int g_patho_init, const int n_samples,
             const float range_food, const float range_agents,
             const float range_move, const int handling_time,
             const int regen_time, const float pTransmit,
             const float p_v_transmit, const int initialInfections,
             const float costInfect, const bool multithreaded,
             const float dispersal, const bool infect_percent,
             const bool vertical, const bool reprod_threshold,
             const float mProb, const float mSize,
             const float spillover_rate)
      :  // population, food, and data structures
        pop(popsize, n_samples, range_agents, range_food, range_move,
            handling_time, pTransmit, p_v_transmit, vertical, reprod_threshold),
        food(nItems, landsize, nClusters, clusterSpread, regen_time),
        gen_data(),

        // eco-evolutionary parameters
        scenario(scenario),
        tmax(tmax),
        genmax(genmax),
        g_patho_init(g_patho_init),

        // agent perception and behaviour, food growth
        n_samples(n_samples),
        range_food(range_food),
        range_agents(range_agents),
        range_move(range_move),
        handling_time(handling_time),
        regen_time(regen_time),

        // disease parameters
        initialInfections(initialInfections),
        costInfect(costInfect),
        pTransmit(pTransmit),
        p_v_transmit(p_v_transmit),
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
  const int scenario, tmax, genmax, g_patho_init, n_samples;
  const float range_food, range_agents, range_move;
  const int handling_time;

  const int regen_time, initialInfections;
  const float costInfect;
  const float pTransmit, p_v_transmit;
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

/// @brief
/// @return
inline Rcpp::List simulation::do_simulation() {
  // prepare landscape and pop
  food.initResources();
  food.countAvailable();
  pop.setTrait(mSize);
  pop.initPos(food);
  Rcpp::List edgeLists;

  // vector to hold generations in which edgelists are logged
  std::vector<int> gens_edge_lists;
  // create object to count infections
  Rcpp::IntegerVector n_infected(genmax, 0);

  // agent data logging increment
  int increment_log =
      std::max((static_cast<int>(static_cast<float>(genmax) * 0.001f)), 2);

  // get sequence of generations in which spillover happens
  // when do introductions occur
  Rcpp::IntegerVector gens_patho_intro = Rcpp::seq(g_patho_init, genmax - 1);
  auto gen_spillover_happens =
      Rcpp::rbinom(genmax - g_patho_init, 1, spillover_rate);
  switch (scenario) {
    case 1:
      // do nothing as sequence is already prepared
      break;
    case 2:
      // single spillover scenario
      gens_patho_intro = Rcpp::IntegerVector::create(g_patho_init);
      break;
    case 3:
      // sporadic spillover scenario
      gens_patho_intro = gens_patho_intro[gen_spillover_happens > 0];
      break;
    default:
      break;
  }

  // go over gens
  for (int gen = 0; gen < genmax; gen++) {
    // food.initResources();
    food.countAvailable();

    // reset counter and positions
    pop.counter = std::vector<int>(pop.nAgents, 0);

    // switch for pathogen introductions
    switch (scenario) {
      case 1:  // maintained for backwards compatibility but not necessary
        if (gen >= g_patho_init) {
          pop.introducePathogen(initialInfections);
        }
        break;
      case 2:
        if (gen == g_patho_init) {
          pop.introducePathogen(initialInfections);
        }
        break;
      case 3:
        if ((gen == g_patho_init) || (gen > g_patho_init)) {
          if (gen_spillover_happens(gen - g_patho_init)) {
            pop.introducePathogen(initialInfections);
          }
        }
        break;
      default:
        Rcpp::stop(
            "Unrecognised scenario option, choose from `1`, `2`, or `3`");
        break;
    }

    // timesteps start here
    for (size_t t = 0; t < static_cast<size_t>(tmax); t++) {
      // resources regrow
      food.regenerate();
      pop.updateRtree();
      // movement section
      pop.move(food, multithreaded);

      // log movement
      if (gen == std::max(g_patho_init - 1, 2)) {
        mdPre.updateMoveData(pop, t);
      }
      if (gen == (genmax - 1)) {
        mdPost.updateMoveData(pop, t);
      }

      // foraging -- split into parallelised picking
      // and non-parallel exploitation
      pop.pickForageItem(food, multithreaded);
      pop.doForage(food);

      // count associations
      pop.countAssoc();

      // relate to g_patho_init, which is the point of regime shift
      if ((scenario > 0) && (gen >= g_patho_init)) {
        // disease spread
        pop.pathogenSpread();
      }

      // timestep ends here
    }

    pop.countInfected();
    // log n infected
    n_infected(gen) = pop.nInfected;
    assert(pop.nInfected <= pop.nAgents);

    // population infection cost by time, if infected
    pop.energy = pop.intake;                       // first make energy = intake
    pop.pathogenCost(costInfect, infect_percent);  // now energy minus costs

    // check if any agents can reproduce if reproduction threshold is applied
    bool reprod_threshold_met = true;
    if (reprod_threshold) {
      reprod_threshold_met = pop.check_reprod_threshold();
      if (!reprod_threshold_met) {
        std::string no_energy_warning =
            "All agents' energy < 0, ending simulation at gen = " +
            std::to_string(gen) + "\n";
        Rcpp::warning(no_energy_warning);
      }
    }

    // update gendata
    if ((gen == (genmax - 1)) || (gen % increment_log == 0) ||
        (!reprod_threshold_met)) {
      // Rcpp::Rcout << "logging data at gen: " << gen << "\n";
      gen_data.updateGenData(pop, gen);
    }

    if ((gen == 0) || ((gen % (genmax / 10)) == 0) || (gen == genmax - 1) ||
        (!reprod_threshold_met)) {
      edgeLists.push_back(pop.pbsn.getNtwkDf());
      gens_edge_lists.push_back(gen);
      Rcpp::Rcout << "gen: " << gen << " --- logged edgelist\n";
    }

    // break here if population is extinct
    if (!reprod_threshold_met) {
      break;
    }

    // reproduce
    pop.Reproduce(food, infect_percent, dispersal, mProb, mSize);

    // generation ends here
  }
  // all gens end here

  Rcpp::Rcout << "Data prepared as an S4 class `pathomove_output`\n";

  return Rcpp::List::create(
      Rcpp::Named("gen_data") = gen_data.getGenData(),
      Rcpp::Named("gens_patho_intro") = gens_patho_intro,
      Rcpp::Named("n_infected_gen") = n_infected,
      Rcpp::Named("move_data_pre") = mdPre.getMoveData(),
      Rcpp::Named("move_data_post") = mdPost.getMoveData(),
      Rcpp::Named("edgeLists") = edgeLists,
      Rcpp::Named("gens_edge_lists") = gens_edge_lists);
}
}  // namespace pathomove

#endif  // INST_INCLUDE_SIMULATIONS_H_
