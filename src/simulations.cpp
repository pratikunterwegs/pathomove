#include "simulations.h"

#include <Rcpp.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace Rcpp;

Rcpp::List simulation::do_simulation() {
  unsigned seed = static_cast<unsigned>(
      std::chrono::system_clock::now().time_since_epoch().count());
  gen.seed(seed);

  // prepare landscape and pop
  food.initResources();
  food.countAvailable();
  Rcpp::Rcout << "landscape with " << food.nClusters << " clusters\n";

  pop.setTrait(mSize);
  Rcpp::Rcout << "pop with " << pop.nAgents << " agents for " << genmax
              << " gens " << tmax << " timesteps\n";

  // prepare scenario
  Rcpp::Rcout << "this is scenario " << scenario << "\n";

  // agent random position in first gen
  pop.initPos(food);

  Rcpp::Rcout << "initialised population positions\n";
  Rcpp::List edgeLists;

  // vector to hold generations in which edgelists are logged
  std::vector<int> gens_edge_lists;

  Rcpp::Rcout << "created edge list object\n";

  // agent data logging increment
  int increment_log =
      std::max((static_cast<int>(static_cast<float>(genmax) * 0.001f)), 2);

  Rcpp::Rcout << "logging data after gens: " << increment_log << "\n";

  // when do spillovers occur
  auto gen_spillover_happens =
      Rcpp::rbinom(genmax - g_patho_init, 1, spillover_rate);
  
  Rcpp::IntegerVector gens_patho_intro = Rcpp::seq(g_patho_init, genmax - 1);

  gens_patho_intro = gens_patho_intro[gen_spillover_happens > 0];

  // go over gens
  for (int gen = 0; gen < genmax; gen++) {
    // food.initResources();
    food.countAvailable();

    // reset counter and positions
    pop.counter = std::vector<int>(pop.nAgents, 0);

    // switch for pathogen introductions
    switch (scenario) {
    case 0:
      break;
    case 1:
      if (gen >= g_patho_init) {
        if(evolve_sI) pop.use_sI = true; // turn on use and evol of sI
        pop.introducePathogen(initialInfections);
      }
      break;
    case 2:
      if (gen == g_patho_init) {
        if(evolve_sI) pop.use_sI = true;
        pop.introducePathogen(initialInfections);
        Rcpp::Rcout << "Single spillover event occurring at gen:" << gen
                    << "\n";
      }
      break;
    case 3:
      if ((gen == g_patho_init) || gen_spillover_happens(genmax - gen)) {
        if(evolve_sI) pop.use_sI = true;
        pop.introducePathogen(initialInfections);
        Rcpp::Rcout << "New spillover event occurring at gen:" << gen << "\n";
      }
      break;
    default:
      break;
    }

    // timesteps start here
    for (size_t t = 0; t < static_cast<size_t>(tmax); t++) {
      // resources regrow
      food.regenerate();
      pop.updateRtree();
      // movement section
      pop.move(food, multithreaded);

      // // log movement
      // if(gen == std::max(g_patho_init - 1, 2)) {
      //     mdPre.updateMoveData(pop, t);
      // }
      // if(gen == (genmax - 1)) {
      //     mdPost.updateMoveData(pop, t);
      // }

      // foraging -- split into parallelised picking
      // and non-parallel exploitation
      pop.pickForageItem(food, multithreaded);
      pop.doForage(food);

      // count associations
      pop.countAssoc();

      // relate to g_patho_init, which is the point of regime shift
      // NOT g_patho_init, which is when pathogens are introduced
      // this allows for vertical transmission
      if ((scenario > 0) && (gen >= g_patho_init)) {
        // disease spread
        pop.pathogenSpread();
      }

      // timestep ends here
    }

    pop.countInfected();

    assert(pop.nInfected <= pop.nAgents);

    // population infection cost by time, if infected
    pop.energy = pop.intake;                      // first make energy = intake
    pop.pathogenCost(costInfect, infect_percent); // now energy minus costs

    // update gendata
    if ((gen == (genmax - 1)) | (gen % increment_log == 0)) {
      // Rcpp::Rcout << "logging data at gen: " << gen << "\n";
      gen_data.updateGenData(pop, gen);
    }

    if ((gen == 0) | ((gen % (genmax / 10)) == 0) | (gen == genmax - 1)) {
      edgeLists.push_back(pop.pbsn.getNtwkDf());
      gens_edge_lists.push_back(gen);
      Rcpp::Rcout << "gen: " << gen << " --- logged edgelist\n";
    }

    // reproduce
    pop.Reproduce(food, infect_percent, dispersal, mProb, mSize);

    // generation ends here
  }
  // all gens end here

  Rcpp::Rcout << "data prepared\n";

  return Rcpp::List::create(Named("gen_data") = gen_data.getGenData(),
                            Named("gens_patho_intro") = gens_patho_intro,
                            Named("edgeLists") = edgeLists,
                            Named("gens_edge_lists") = gens_edge_lists);
}

//' Runs the pathomove simulation and return a `pathomove_output` object.
//'
//' @description Run the simulation using parameters passed as
//' arguments to the corresponding R function.
//'
//' @param scenario The pathomove scenario: 0 for no pathogen, 1 for
//' persistent introduction across generations,
//' 2 for a single introduction,
//' and 3 for sporadic introductions drawn from a geometric distribution
//' specified by `spillover_rate`.
//' @param popsize The population size.
//' @param nItems How many food items on the landscape.
//' @param landsize The size of the landscape as a numeric (double).
//' @param nClusters Number of clusters around which food is generated.
//' @param clusterSpread How dispersed food is around the cluster centre.
//' @param tmax The number of timesteps per generation.
//' @param genmax The maximum number of generations per simulation.
//' @param g_patho_init The generation in which to begin introducing the
//' pathogen.
//' @param n_samples How many points to sample in the neighbourhood.
//' @param range_food The sensory range for food.
//' @param range_agents The sensory range for agents.
//' @param range_move The movement range for agents.
//' @param handling_time The handling time.
//' @param regen_time The item regeneration time.
//' @param pTransmit Probability of transmission.
//' @param initialInfections Agents infected per event.
//' @param costInfect The per-timestep cost of pathogen infection.
//' @param multithreaded Boolean. Whether to use TBB multithreading.
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
//' @param vertical Should the pathogen be transmitted vertically? Should be
//' set to `TRUE` for a realistic implementation of scenario 3, _single
//' spillover_.
//' @param evolve_sI Should individuals be able to sense infection status and
//' evolve a weight `sI` that affects suitability based on the number of
//' infected neighbours. Initially set to 0, and allowed to evolve only after
//' the pathogen is introduced.
//' @param mProb The probability of mutation. The suggested value is 0.01.
//' While high, this may be more appropriate for a small population;
//' change this value and \code{popsize} to test the simulation's sensitivity to
//' these values.
//' @param mSize Controls the mutational step size, and represents
//' the scale parameter of a Cauchy distribution.
//' @param spillover_rate For scenario 3, the probability parameter _p_ of a
//' geometric distribution from which the number of generations until the next
//' pathogen introduction are drawn.
//'
//' @export
//' @return An S4 class, `pathomove_output`, with simulation outcomes.
// [[Rcpp::export]]
S4 run_pathomove(const int scenario = 2, const int popsize = 500,
                 const int nItems = 1800, const float landsize = 60,
                 const int nClusters = 60, const float clusterSpread = 1,
                 const int tmax = 100, const int genmax = 100,
                 const int g_patho_init = 3000, 
                 const float n_samples = 5.0, const float range_food = 1.0,
                 const float range_agents = 1.0, const float range_move = 1.0,
                 const int handling_time = 5, const int regen_time = 50,
                 float pTransmit = 0.05, const int initialInfections = 20,
                 const float costInfect = 0.25, const bool multithreaded = true,
                 const float dispersal = 2.0, const bool infect_percent = false,
                 const bool vertical = false, 
                 const bool evolve_sI = false, const float mProb = 0.01,
                 const float mSize = 0.01, const float spillover_rate = 1.0) {
  // check that intial infections is less than popsize
  if (initialInfections > popsize) {
    Rcpp::stop("Error: Initial infections must be less than popsize");
  }
  if (g_patho_init > genmax) {
    Rcpp::stop("Error: G_patho_init must be less than genmax");
  }
  // make simulation class with input parameters
  simulation this_sim(
      popsize, scenario, nItems, landsize, nClusters, clusterSpread, tmax,
      genmax, g_patho_init, n_samples, range_food, range_agents, range_move, handling_time,
      regen_time, pTransmit, initialInfections, costInfect, multithreaded,
      dispersal, infect_percent, vertical, evolve_sI, 
      mProb, mSize, spillover_rate);
  // do the simulation using the simulation class function
  Rcpp::List pathomoveOutput = this_sim.do_simulation();

  // get generation data from output
  Rcpp::List gen_data = pathomoveOutput["gen_data"];
  // make list of dataframes of population traits
  Rcpp::List pop_data = gen_data["pop_data"];

  // return scenario as string
  Rcpp::String scenario_str("scenario_here");
  switch (scenario) {
  case 0:
    scenario_str = "no pathogen";
    Rcpp::Rcout << "No pathogen introduction\n";
    break;
  case 1:
    if (g_patho_init == 0)
      scenario_str = "endemic pathogen";
    else
      scenario_str = "novel pathogen";
    Rcpp::Rcout << "Pathogen introduced from gen:" << g_patho_init << "\n";
    break;
  case 2:
    scenario_str = "single spillover";
    break;
  case 3:
    scenario_str = "sporadic spillover";
    break;
  default:
    scenario_str = "unknown scenario";
    break;
  }

  Rcpp::String infection_cost_type = infect_percent ? "percent" : "absolute";

  Rcpp::String vertical_infection = vertical ? "vertical" : "no_vertical";

  // agents parameter list
  Rcpp::List agents_param_list = Rcpp::List::create(
      Named("popsize") = popsize, Named("range_food") = range_food,
      Named("range_agents") = range_agents, Named("range_move") = range_move,
      Named("handling_time") = handling_time, Named("pTransmit") = pTransmit,
      Named("initialInfections") = initialInfections,
      Named("costInfect") = costInfect,
      Named("infect_percent") = infection_cost_type,
      Named("vertical_infection") = vertical_infection,
      Named("evolve_sI") = evolve_sI,
      Named("dispersal") = dispersal, Named("mProb") = mProb,
      Named("mSize") = mSize);

  // ecological parameters list
  Rcpp::List eco_param_list = Rcpp::List::create(
      Named("scenario") = scenario_str, Named("genmax") = genmax,
      Named("g_patho_init") = (scenario == 0 ? NA_REAL : g_patho_init),
      Named("spillover_rate") = (scenario == 3 ? NA_REAL : spillover_rate),
      Named("nItems") = nItems, Named("landsize") = landsize,
      Named("nClusters") = nClusters, Named("clusterSpread") = clusterSpread,
      Named("tmax") = tmax, Named("regen_time") = regen_time);

  // create S4 class pathomove output and fill slots
  S4 x("pathomove_output");
  x.slot("agent_parameters") = Rcpp::wrap(agents_param_list);
  x.slot("eco_parameters") = Rcpp::wrap(eco_param_list);
  x.slot("generations") = Rcpp::wrap(gen_data["gens"]);
  x.slot("gens_patho_intro") = Rcpp::wrap(pathomoveOutput["gens_patho_intro"]);
  x.slot("infections_per_gen") = Rcpp::wrap(gen_data["n_infected"]);
  x.slot("trait_data") = Rcpp::wrap(pop_data);
  x.slot("edge_lists") = Rcpp::wrap(pathomoveOutput["edgeLists"]);
  x.slot("gens_edge_lists") = Rcpp::wrap(pathomoveOutput["gens_edge_lists"]);

  return (x);
}
