// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.

// Enable C++14 via this plugin to suppress 'long long' errors
// [[Rcpp::plugins("cpp14")]]
// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppParallel)]]

// clang-format off
#include <pathomove.h>

#include <Rcpp.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
// clang-format on

/// function to export landscape as matrix
//' Returns a test landscape.
//'
//' @param nItems How many items.
//' @param landsize Size as a numeric (float).
//' @param nClusters How many clusters, an integer value.
//' @param clusterSpread Dispersal of items around cluster centres.
//' @param regen_time Regeneration time, in timesteps.
//' @return A data frame of the evolved population traits.
//' @export
// [[Rcpp::export]]
Rcpp::DataFrame get_test_landscape(const int nItems, const float landsize,
                                   const int nClusters,
                                   const float clusterSpread,
                                   const int regen_time) {
  pathomove::Resources food(nItems, landsize, nClusters, clusterSpread,
                            regen_time);
  food.initResources();

  return Rcpp::DataFrame::create(Rcpp::Named("x") = food.coordX,
                                 Rcpp::Named("y") = food.coordY,
                                 Rcpp::Named("tAvail") = food.counter);
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
//' @param n_samples Integer. The number of directions around the current
//' location to scan for cues.
//' @param range_food The sensory range for food.
//' @param range_agents The sensory range for agents.
//' @param range_move The movement range for agents.
//' @param handling_time The handling time.
//' @param regen_time The item regeneration time.
//' @param pTransmit Probability of transmission among individuals.
//' @param p_v_transmit Probability of transmission from parents to offspring.
//' @param initialInfections Agents infected per event.
//' @param costInfect The per-timestep cost of pathogen infection.
//' @param multithreaded Boolean. Whether multithreading using TBB to be used.
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
//' set to `TRUE` for a realistic implementation of scenario 3,
//' _single spillover_.
//' @param reprod_threshold Boolean, should individuals with negative energy
//' reproduce.
//' @param mProb The probability of mutation. The suggested value is 0.01.
//' While high, this may be more appropriate for a small population; change this
//' value and \code{popsize} to test the simulation's sensitivity to these
//' parameters.
//' @param mSize Controls the mutational step size, and represents the scale
//' parameter of a Cauchy distribution.
//' @param spillover_rate For scenario 3, the probability parameter _p_ of a
//' geometric distribution from which the number of generations until the next
//' pathogen introduction are drawn.
//' @param seed An integer number that is the seed for the R RNG. Defaults to
//' zero.
//' @export
//' @return An S4 class, `pathomove_output`, with simulation outcomes.
// [[Rcpp::export]]
Rcpp::S4 run_pathomove(
    const int scenario = 1, const int popsize = 100, const int nItems = 1800,
    const float landsize = 60.0, const int nClusters = 60,
    const float clusterSpread = 1.0, const int tmax = 100,
    const int genmax = 100, const int g_patho_init = 70,
    const int n_samples = 5, const float range_food = 1.0,
    const float range_agents = 1.0, const float range_move = 1.0,
    const int handling_time = 5, const int regen_time = 50,
    float pTransmit = 0.05, const float p_v_transmit = 0.05,
    const int initialInfections = 20, const float costInfect = 0.25,
    const bool multithreaded = true, const float dispersal = 2.0,
    const bool infect_percent = false, const bool vertical = false,
    const bool reprod_threshold = false, const float mProb = 0.01,
    const float mSize = 0.01, const float spillover_rate = 1.0,
    const int seed = 0) {
  // check that intial infections is less than popsize
  if (initialInfections > popsize) {
    Rcpp::stop("Error: Initial infections must be less than/equal to popsize");
  }
  if (g_patho_init >= genmax) {
    Rcpp::stop("Error: G_patho_init must be less than genmax");
  }
  if (genmax < 10) {
    Rcpp::warning("Simulation often crashes when 1 < genmax < 10");
  }
  if (multithreaded) {
    Rcpp::warning(
        "Multithreading is on and the simulation is NOT reproducible!\nSet "
        "`multithreaded = FALSE` for a much slower but completely reproducible "
        "simulation.");
  }

  // Prepare R and RNG seed
  pathomove::set_seed(seed);

  // Prepare data for simulation messages
  // return scenario as string
  std::string scenario_str("scenario_here");
  std::string scenario_message = "this message";
  switch (scenario) {
    case 1:
      if (g_patho_init == 0)
        scenario_str = "Endemic pathogen";
      else
        scenario_str = "Novel pathogen";
      scenario_message =
          "Pathogen introduced from gen: " + std::to_string(g_patho_init) +
          "\n";
      break;
    case 2:
      scenario_str = "Single spillover";
      scenario_message =
          "Pathogen introduced once in gen: " + std::to_string(g_patho_init) +
          "\n";
      break;
    case 3:
      scenario_str = "Sporadic spillover";
      scenario_message =
          "Pathogen introduced with prob = " + std::to_string(spillover_rate) +
          "from gen: " + std::to_string(g_patho_init) + "\n" +
          " Generations with pathogen introduction stored in slot "
          "`gens_patho_intro`\n";
      break;
    default:
      Rcpp::stop(
          "Unrecognised scenario option, choose from `1`, `2`, or `3`\n");
      break;  // unnecessary
  }

  Rcpp::String infection_cost_type = infect_percent ? "percent" : "absolute";

  Rcpp::String vertical_infection = vertical ? "vertical" : "no_vertical";

  /* Section for simulation messages */
  Rcpp::Rcout << "Running a `pathomove` simulation...\n";
  Rcpp::Rcout << " Generations: " << genmax << " | Timesteps: " << tmax << "\n";
  Rcpp::Rcout << " Scenario: " << scenario << ": " << scenario_str << "\n";
  Rcpp::Rcout << " " + scenario_message;
  Rcpp::Rcout << "Landscape:\n Size: " << landsize
              << " | Food items: " << nItems << " | Clusters: " << nClusters
              << " | Spread: " << clusterSpread << "\n";
  Rcpp::Rcout << "Population:\n "
              << "Population size: " << popsize
              << " | Movement range: " << range_move << "\n"
              << " Reproduction threshold: "
              << (reprod_threshold ? "On" : "Off") << "\n";
  Rcpp::Rcout << "Pathogen:\n "
              << "p(Transmit): " << pTransmit
              << " | p(Vertical transmit): " << (vertical ? p_v_transmit : 0.0)
              << "\n "
              << "Cost: " << costInfect
              << " | Initial infections: " << initialInfections << "\n\n";
  /* Messages section ends here */

  // make simulation class with input parameters
  pathomove::simulation this_sim(
      popsize, scenario, nItems, landsize, nClusters, clusterSpread, tmax,
      genmax, g_patho_init, n_samples, range_food, range_agents, range_move,
      handling_time, regen_time, pTransmit, p_v_transmit, initialInfections,
      costInfect, multithreaded, dispersal, infect_percent, vertical,
      reprod_threshold, mProb, mSize, spillover_rate);
  // do the simulation using the simulation class function
  Rcpp::List pathomoveOutput = this_sim.do_simulation();
  // get generation data from output
  Rcpp::List gen_data = pathomoveOutput["gen_data"];
  // make list of dataframes of population traits
  Rcpp::List pop_data = gen_data["pop_data"];

  // agents parameter list --- limit of 20 elements for manual lists!
  Rcpp::List agents_param_list = Rcpp::List::create(
      Rcpp::Named("popsize") = popsize, Rcpp::Named("range_food") = range_food,
      Rcpp::Named("range_agents") = range_agents,
      Rcpp::Named("range_move") = range_move,
      Rcpp::Named("handling_time") = handling_time,
      Rcpp::Named("pTransmit") = pTransmit,
      Rcpp::Named("p_v_transmit") = p_v_transmit,
      Rcpp::Named("initialInfections") = initialInfections,
      Rcpp::Named("costInfect") = costInfect,
      Rcpp::Named("infect_percent") = infection_cost_type,
      Rcpp::Named("vertical_infection") = vertical_infection,
      Rcpp::Named("reprod_threshold") = reprod_threshold,
      Rcpp::Named("dispersal") = dispersal, Rcpp::Named("mProb") = mProb,
      Rcpp::Named("mSize") = mSize);

  // ecological parameters list
  Rcpp::List eco_param_list = Rcpp::List::create(
      Rcpp::Named("scenario") = Rcpp::wrap(scenario_str),
      Rcpp::Named("genmax") = genmax,
      Rcpp::Named("g_patho_init") = (scenario == 0 ? NA_REAL : g_patho_init),
      Rcpp::Named("spillover_rate") =
          (scenario == 3 ? NA_REAL : spillover_rate),
      Rcpp::Named("nItems") = nItems, Rcpp::Named("landsize") = landsize,
      Rcpp::Named("nClusters") = nClusters,
      Rcpp::Named("clusterSpread") = clusterSpread, Rcpp::Named("tmax") = tmax,
      Rcpp::Named("regen_time") = regen_time);

  // create S4 class pathomove output and fill slots
  Rcpp::S4 x("pathomove_output");
  x.slot("agent_parameters") = Rcpp::wrap(agents_param_list);
  x.slot("eco_parameters") = Rcpp::wrap(eco_param_list);
  x.slot("generations") = Rcpp::wrap(gen_data["gens"]);
  x.slot("gens_patho_intro") = pathomoveOutput["gens_patho_intro"];
  x.slot("infections_per_gen") = Rcpp::wrap(pathomoveOutput["n_infected_gen"]);
  x.slot("trait_data") = Rcpp::wrap(pop_data);
  x.slot("edge_lists") = Rcpp::wrap(pathomoveOutput["edgeLists"]);
  x.slot("gens_edge_lists") = Rcpp::wrap(pathomoveOutput["gens_edge_lists"]);
  x.slot("landscape") = Rcpp::DataFrame::create(
      Rcpp::Named("x") = Rcpp::wrap(this_sim.food.coordX),
      Rcpp::Named("y") = Rcpp::wrap(this_sim.food.coordY));
  x.slot("move_data") = Rcpp::List::create(
      Rcpp::Named("pre") = pathomoveOutput["move_data_pre"],
      Rcpp::Named("post") = pathomoveOutput["move_data_post"]);

  return (x);
}
