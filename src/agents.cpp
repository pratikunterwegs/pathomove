// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
#define _USE_MATH_DEFINES
/// code to make agents

// clang-format off
#include "agents.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include "landscape.h"
#include "network.h"

#include <Rcpp.h>
#include <RcppParallel.h>
#include <boost/foreach.hpp>
// clang-format on

// wrapper around R's RNG such that we get a uniform distribution over
// [0,n) as required by the STL algorithm
// taken from https://gallery.rcpp.org/articles/stl-random-shuffle/
inline int randWrapper(const int n) { return floor(unif_rand() * n); }

// to shuffle pop id
void Population::shufflePop() {
  Rcpp::IntegerVector order_agents = Rcpp::seq(0, nAgents - 1);

  std::random_shuffle(order_agents.begin(), order_agents.end(), randWrapper);

  order = Rcpp::as<std::vector<int>>(order_agents);
}

// to update agent Rtree
void Population::updateRtree() {
  // initialise rtree
  bgi::rtree<value, bgi::quadratic<16>> tmpRtree;
  for (int i = 0; i < nAgents; ++i) {
    point p = point(coordX[i], coordY[i]);
    tmpRtree.insert(std::make_pair(p, i));
  }
  std::swap(agentRtree, tmpRtree);
  tmpRtree.clear();
}

// function for initial positions
void Population::initPos(const Resources &food) {
  coordX = Rcpp::as<std::vector<float>>(Rcpp::runif(nAgents, 0.f, food.dSize));
  coordY = Rcpp::as<std::vector<float>>(Rcpp::runif(nAgents, 0.f, food.dSize));
  initX = coordX;
  initY = coordY;
  updateRtree();
}

// set agent trait
void Population::setTrait(const float &mSize) {
  // create a cauchy distribution, mSize is the scale
  sF = Rcpp::as<std::vector<float>>(Rcpp::rnorm(nAgents, 0.0, mSize));
  sH = Rcpp::as<std::vector<float>>(Rcpp::rnorm(nAgents, 0.0, mSize));
  sN = Rcpp::as<std::vector<float>>(Rcpp::rnorm(nAgents, 0.0, mSize));
}

// general function for agents within distance
std::pair<int, int> Population::countAgents(const float &xloc,
                                            const float &yloc) {
  int handlers = 0;
  int nonhandlers = 0;
  std::vector<value> near_agents;
  // query for a simple box
  agentRtree.query(bgi::satisfies([&](value const &v) {
                     return bg::distance(v.first, point(xloc, yloc)) <
                            range_agents;
                   }),
                   std::back_inserter(near_agents));

  BOOST_FOREACH (value const &v, near_agents) {  // NOLINT
    if (counter[v.second] > 0)
      handlers++;
    else
      nonhandlers++;
  }
  near_agents.clear();
  // first element is number of near entities
  // second is the identity of entities
  return std::pair<int, int>{handlers, nonhandlers};
}

// function for near agent ids
std::vector<int> Population::getNeighbourId(const float &xloc,
                                            const float &yloc) {
  std::vector<int> agent_id;
  std::vector<value> near_agents;
  // query for a simple box
  // neighbours for associations are counted over the MOVEMENT RANGE
  agentRtree.query(bgi::satisfies([&](value const &v) {
                     return bg::distance(v.first, point(xloc, yloc)) <
                            range_move;
                   }),
                   std::back_inserter(near_agents));

  BOOST_FOREACH (value const &v, near_agents) {  // NOLINT
    agent_id.push_back(v.second);
  }
  near_agents.clear();
  // first element is number of near entities
  // second is the identity of entities
  return agent_id;
}

// general function for items within distance
int Population::countFood(const Resources &food, const float &xloc,
                          const float &yloc) {
  int nFood = 0;
  std::vector<value> near_food;

  // check any available
  if (food.nAvailable > 0) {
    // query for a simple box
    food.rtree.query(bgi::satisfies([&](value const &v) {
                       return bg::distance(v.first, point(xloc, yloc)) <
                              range_food;
                     }),
                     std::back_inserter(near_food));

    BOOST_FOREACH (value const &v, near_food) {  // NOLINT
      // count only which are available!
      if (food.available[v.second]) {
        nFood++;
      }
    }
    near_food.clear();
  }

  return nFood;
}

// function for the nearest available food item
std::vector<int> Population::getFoodId(const Resources &food, const float &xloc,
                                       const float &yloc) {
  std::vector<int> food_id;
  std::vector<value> near_food;
  // check any available
  if (food.nAvailable > 0) {
    // query for a simple box
    // food is accessed over the MOVEMENT RANGE
    food.rtree.query(bgi::satisfies([&](value const &v) {
                       return bg::distance(v.first, point(xloc, yloc)) <
                              range_move;
                     }),
                     std::back_inserter(near_food));

    BOOST_FOREACH (value const &v, near_food) {  // NOLINT
      // count only which are available!
      if (food.available[v.second]) {
        food_id.push_back(v.second);
      }
    }
    near_food.clear();
  }

  return food_id;
}

/// simple wrapping function
// because std::fabs + std::fmod is somewhat suspicious
// we assume values that are at most a little larger than max (max + 1) and
// a little smaller than zero (-1)
float wrap_pos(const float &p1, const float &pmax) {
  if (std::fabs(p1 / pmax) > 2.f) {
    Rcpp::stop("Individuals moving far past boundary!\n");
  }
  if (p1 > pmax) {
    return p1 - pmax;
  }
  if (p1 < 0.f) {
    return pmax + p1;
  }
  return p1;
}

/// population movement function
void Population::move(const Resources &food, const bool &multithreaded) {
  const float twopi = 2.f * M_PI;

  // what increment for n samples in a circle around the agent
  const float increment = twopi / static_cast<float>(n_samples);

  // make random noise for each individual and each sample
  Rcpp::NumericMatrix noise_v(nAgents, n_samples);
  for (size_t i_ = 0; i_ < n_samples; i_++) {
    noise_v(Rcpp::_, i_) = Rcpp::rnorm(nAgents, 0.0f, 1e-5f);
  }

  // loop over agents
  if (multithreaded) {
    // try parallel
    tbb::parallel_for(
        tbb::blocked_range<unsigned>(0, nAgents),
        [&](const tbb::blocked_range<unsigned> &r) {
          for (unsigned i = r.begin(); i < r.end(); ++i) {
            // int id = order[i];
            if (counter[i] > 0) {
              counter[i]--;
            } else {
              // first assess current location, this is angle '-1'
              float choice = -1.f;

              // count local food items
              int foodHere = countFood(food, coordX[i], coordY[i]);
              // count local handlers and non-handlers
              std::pair<int, int> agentCounts =
                  countAgents(coordX[i], coordY[i]);

              // get suitability of current location
              // implicit conversion from int to float as ints are promoted
              float suit_origin = (sF[i] * foodHere) +
                                  (sH[i] * agentCounts.first) +
                                  (sN[i] * agentCounts.second);

              // does the agent move at all? initially set to false
              bool agent_moves = false;

              // now sample at n locations around
              // j.first are angles, j.second are iterators
              for (std::pair<float, size_t> j(0.f, 0); j.first < twopi;
                   j.first += increment, j.second++) {
                // use range for agents to determine sample locs
                float sampleX = coordX[i] + (range_agents * cos(j.first));
                float sampleY = coordY[i] + (range_agents * sin(j.first));

                // count food items at sample location
                foodHere = countFood(food, sampleX, sampleY);
                // count handlers and non-handlers at sample location
                agentCounts = countAgents(sampleX, sampleY);

                float suit_dest =
                    (sF[i] * foodHere) + (sH[i] * agentCounts.first) +
                    (sN[i] * agentCounts.second) + noise_v(i, j.second);

                if (suit_dest > suit_origin) {
                  // the agent moves
                  agent_moves = true;
                  // which angle the agent choses from the angles vector
                  choice = j.first;
                  // update 'suit_origin' to be the highest suitability
                  // encountered
                  suit_origin = suit_dest;
                }
              }
              // distance to be moved // agent_moves promoted to float
              moved[i] += (agent_moves * range_move);

              if (agent_moves && (choice < 0.f)) {
                Rcpp::stop("Error: Agent moved but choice not logged");
              }

              // which angle does the agent move to // agent_moves promoted
              coordX[i] += (agent_moves * range_move * cos(choice));
              coordY[i] += (agent_moves * range_move * sin(choice));

              // wrap locations
              coordX[i] = wrap_pos(coordX[i], food.dSize);
              coordY[i] = wrap_pos(coordY[i], food.dSize);
            }
          }
        });
  } else {
    for (int i = 0; i < nAgents; ++i) {
      // int id = order[i];
      if (counter[i] > 0) {
        counter[i]--;
      } else {
        // first assess current location, this is angle '-1'
        float choice = -1.f;

        // count local food items
        int foodHere = countFood(food, coordX[i], coordY[i]);
        // count local handlers and non-handlers
        std::pair<int, int> agentCounts = countAgents(coordX[i], coordY[i]);

        // get suitability of current location
        // implicit conversion from int to float as ints are promoted
        float suit_origin = (sF[i] * foodHere) + (sH[i] * agentCounts.first) +
                            (sN[i] * agentCounts.second);

        // does the agent move at all? initially set to false
        bool agent_moves = false;

        // now sample at n locations around
        // j.first are angles, j.second are iterators
        for (std::pair<float, size_t> j(0.f, 0); j.first < twopi;
             j.first += increment, j.second++) {
          // use range for agents to determine sample locs
          float sampleX = coordX[i] + (range_agents * cos(j.first));
          float sampleY = coordY[i] + (range_agents * sin(j.first));

          // count food items at sample location
          foodHere = countFood(food, sampleX, sampleY);
          // count handlers and non-handlers at sample location
          agentCounts = countAgents(sampleX, sampleY);

          float suit_dest = (sF[i] * foodHere) + (sH[i] * agentCounts.first) +
                            (sN[i] * agentCounts.second) + noise_v(i, j.second);

          if (suit_dest > suit_origin) {
            // the agent moves
            agent_moves = true;
            // which angle the agent choses from the angles vector
            choice = j.first;
            // update 'suit_origin' to be the highest suitability
            // encountered
            suit_origin = suit_dest;
          }
        }
        // distance to be moved // agent_moves promoted to float
        moved[i] += (agent_moves * range_move);

        if (agent_moves && (choice < 0.f)) {
          Rcpp::stop("Error: Agent moved but choice not logged");
        }

        // which angle does the agent move to // agent_moves promoted
        coordX[i] += (agent_moves * range_move * cos(choice));
        coordY[i] += (agent_moves * range_move * sin(choice));

        // wrap locations
        coordX[i] = wrap_pos(coordX[i], food.dSize);
        coordY[i] = wrap_pos(coordY[i], food.dSize);
      }
    }
  }
}

// function to paralellise choice of forage item
void Population::pickForageItem(const Resources &food,
                                const bool &multithreaded) {
  // nearest food
  std::vector<int> idTargetFood(nAgents, -1);

  if (multithreaded) {
    // loop over agents --- no shuffling required here
    // try parallel foraging --- agents pick a target item
    tbb::parallel_for(tbb::blocked_range<unsigned>(0, nAgents),
                      [&](const tbb::blocked_range<unsigned> &r) {
                        for (unsigned i = r.begin(); i < r.end(); ++i) {
                          if ((counter[i] > 0) || (food.nAvailable == 0)) {
                            // nothing -- agent cannot forage or there is no
                            // food
                          } else {
                            // find nearest item ids
                            std::vector<int> theseItems =
                                getFoodId(food, coordX[i], coordY[i]);
                            // check near items count
                            if (theseItems.size() > 0) {
                              // take random item
                              std::random_shuffle(theseItems.begin(),
                                                  theseItems.end(),
                                                  randWrapper);
                              idTargetFood[i] = theseItems[0];
                            }
                          }
                        }
                      });
  } else {
    for (int i = 0; i < nAgents; ++i) {
      if ((counter[i] > 0) || (food.nAvailable == 0)) {
        // nothing -- agent cannot forage or there is no food
      } else {
        // find nearest item ids
        std::vector<int> theseItems = getFoodId(food, coordX[i], coordY[i]);

        // check near items count
        if (theseItems.size() > 0) {
          // take random item
          std::random_shuffle(theseItems.begin(), theseItems.end(),
                              randWrapper);
          idTargetFood[i] = theseItems[0];
        }
      }
    }
  }

  forageItem = idTargetFood;
}

// function to exploitatively forage on picked forage items
void Population::doForage(Resources &food) {
  // all agents have picked a food item if they can forage
  // now forage in a serial loop --- this cannot be parallelised
  // this order is randomised
  shufflePop();
  for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
    int id = order[i];
    if ((counter[id] > 0) || (food.nAvailable == 0) || (forageItem[id] == -1)) {
      // nothing
    } else {
      // here we assume that the individual can forage, there is food
      // WE MUST CHECK whether the picked item is available
      int thisItem = forageItem[id];  // the item picked by this agent

      // IFF the item is available, then harvest it and mark it as unavailable
      if (food.available[thisItem]) {
        counter[id] = handling_time;
        intake[id] += 1.0;  // increased here --- not as described.

        // reset food availability
        food.available[thisItem] = false;
        food.counter[thisItem] = food.regen_time;
        food.nAvailable--;
      }
    }
  }
}

void Population::countAssoc() {
  for (int i = 0; i < nAgents; ++i) {
    // count nearby agents and update raw associations
    std::vector<int> nearby_agents = getNeighbourId(coordX[i], coordY[i]);
    associations[i] += nearby_agents.size();

    // loop over nearby agents and update association matrix
    for (size_t j = 0; j < nearby_agents.size(); j++) {
      int target_agent = nearby_agents[j];
      pbsn.adjMat(i, target_agent) += 1;
    }
  }
}

/// small function to check whether individuals have a positive energy balance
const bool Population::check_reprod_threshold() {
  return (std::count_if(energy.begin(), energy.end(),
                        [](float energy_f) { return energy_f > 0.f; }) > 0);
}

/// minor function to normalise vector
std::vector<float> Population::handleFitness() {
  Rcpp::NumericVector vecFitness = Rcpp::wrap(energy);
  // random errors in fitness
  Rcpp::NumericVector rd_fitness = Rcpp::rnorm(nAgents, 0.0f, 1e-5f);
  vecFitness =
      vecFitness + rd_fitness;  // add error to avoid all energies equal
  vecFitness = (vecFitness - Rcpp::min(vecFitness)) /
               (Rcpp::max(vecFitness) - Rcpp::min(vecFitness));

  return Rcpp::as<std::vector<float>>(vecFitness);
}

/// prepare function to handle fitness and offer parents when applying a
/// reproduction threshold
std::pair<std::vector<int>, std::vector<float>>
Population::applyReprodThreshold() {
  std::vector<float> energy_pos;
  std::vector<int> id_pos;

  // add only agents with positive energy
  for (size_t i = 0; i < nAgents; i++) {
    if (energy[i] > 0.f) {
      id_pos.push_back(i);
      energy_pos.push_back(energy[i]);
    }
  }

  // count agents remaining
  const int agents_remaining = id_pos.size();

  // normalise energy between 0 and 1
  Rcpp::NumericVector vecFitness = Rcpp::wrap(energy_pos);
  Rcpp::NumericVector rd_fitness = Rcpp::rnorm(agents_remaining, 0.0f, 1e-5f);
  vecFitness =
      vecFitness + rd_fitness;  // add error to avoid all energies equal

  vecFitness = (vecFitness - Rcpp::min(vecFitness)) /
               (Rcpp::max(vecFitness) - Rcpp::min(vecFitness));

  energy_pos = Rcpp::as<std::vector<float>>(vecFitness);

  return std::pair<std::vector<int>, std::vector<float>>(id_pos, energy_pos);
}

// fun for replication
void Population::Reproduce(const Resources &food, const bool &infect_percent,
                           const float &dispersal, const float &mProb,
                           const float &mSize) {
  // draw vertical infectons
  auto v_infect = Rcpp::rbinom(nAgents, 1, pTransmit);

  // prepare to deal with fitness and reproduction options
  std::pair<std::vector<int>, std::vector<float>> thresholded_parents;
  std::vector<float> vecFitness = handleFitness();

  if (infect_percent) {
    vecFitness = energy;
  }
  // handle vecFtiness (parents) in special cases
  if (reprod_threshold && infect_percent) {
    Rcpp::stop(
        "Error: Only one of 'reprod_threshold' and 'infect_percent' can be "
        "true");
  }
  if (reprod_threshold) {
    thresholded_parents = applyReprodThreshold();
    vecFitness = thresholded_parents.second;
  }
  if (infect_percent) {
    vecFitness = energy;
  }

  // set up weighted lottery based on the vector of fitnesses
  std::discrete_distribution<> weightedLottery(vecFitness.begin(),
                                               vecFitness.end());

  // get parent trait based on weighted lottery
  std::vector<float> tmp_sF(nAgents, 0.f);
  std::vector<float> tmp_sH(nAgents, 0.f);
  std::vector<float> tmp_sN(nAgents, 0.f);

  // infected or not for vertical transmission
  std::vector<bool> infected_2(nAgents, false);

  // reset infection source
  srcInfect = Rcpp::IntegerVector(nAgents, NA_INTEGER);

  // reset associations
  associations = std::vector<int>(nAgents, 0);

  // reset distance moved
  moved = std::vector<float>(nAgents, 0.f);

  // reset adjacency matrix
  pbsn.adjMat = Rcpp::IntegerMatrix(nAgents, nAgents);

  // positions
  std::vector<float> coord_x_2(nAgents, 0.f);
  std::vector<float> coord_y_2(nAgents, 0.f);

  // Get sprout distances
  Rcpp::NumericVector sprout_x = Rcpp::rnorm(nAgents, 0.0f, dispersal);
  Rcpp::NumericVector sprout_y = Rcpp::rnorm(nAgents, 0.0f, dispersal);

  for (int a = 0; a < nAgents; a++) {
    size_t parent_id = static_cast<size_t>(weightedLottery(rng));

    // mod the parent id if a reprod threshold is applied, this helps refer
    // to the id in the thresholded parents vector
    if (reprod_threshold) parent_id = thresholded_parents.first[parent_id];

    tmp_sF[a] = sF[parent_id];
    tmp_sH[a] = sH[parent_id];
    tmp_sN[a] = sN[parent_id];

    // inherit positions from parent
    coord_x_2[a] = coordX[parent_id] + sprout_x(a);
    coord_y_2[a] = coordY[parent_id] + sprout_y(a);

    // wrap locations
    coord_x_2[a] = wrap_pos(coord_x_2[a], food.dSize);
    coord_y_2[a] = wrap_pos(coord_y_2[a], food.dSize);

    // vertical transmission of infection if set to TRUE
    if (vertical) {
      if (infected[parent_id]) {
        if (v_infect(a)) {
          infected_2[a] = true;
          srcInfect[a] = -parent_id;  // -id for each parent
        }
      }
    }
  }

  // swap infected and infected_2
  std::swap(infected, infected_2);
  infected_2.clear();

  // swap coords --- this initialises individuals near their parent's position
  std::swap(coordX, coord_x_2);
  std::swap(coordY, coord_y_2);
  coord_x_2.clear();
  coord_y_2.clear();

  // update initial positions!
  initX = coordX;
  initY = coordY;

  // reset counter and time infected
  counter = std::vector<int>(nAgents, 0);
  timeInfected = std::vector<int>(nAgents, 0);
  assert(static_cast<int>(counter.size()) == nAgents && "counter size wrong");

  // mutate trait: trait shifts up or down with an equal prob
  // trait mutation prob is mProb, in a two step process
  auto mutation_sF = Rcpp::rbinom(nAgents, 1, mProb);
  auto mutation_sH = Rcpp::rbinom(nAgents, 1, mProb);
  auto mutation_sN = Rcpp::rbinom(nAgents, 1, mProb);

  Rcpp::NumericVector mut_size_sF = Rcpp::rcauchy(nAgents, 0.0f, mSize);
  Rcpp::NumericVector mut_size_sH = Rcpp::rcauchy(nAgents, 0.0f, mSize);
  Rcpp::NumericVector mut_size_sN = Rcpp::rcauchy(nAgents, 0.0f, mSize);

  for (int a = 0; a < nAgents; a++) {
    if (mutation_sF(a)) {
      tmp_sF[a] = tmp_sF[a] + mut_size_sF(a);
    }
    if (mutation_sH(a)) {
      tmp_sH[a] = tmp_sH[a] + mut_size_sH(a);
    }
    if (mutation_sN(a)) {
      tmp_sN[a] = tmp_sN[a] + mut_size_sN(a);
    }
  }

  // reset nInfected and count natal infections
  // from vertical transmission
  countInfected();

  // swap trait matrices
  std::swap(sF, tmp_sF);
  std::swap(sH, tmp_sH);
  std::swap(sN, tmp_sN);

  tmp_sF.clear();
  tmp_sH.clear();
  tmp_sN.clear();

  // swap energy
  std::vector<float> tmpEnergy(nAgents, 0.001);
  std::swap(energy, tmpEnergy);
  tmpEnergy.clear();

  // swap intake
  std::vector<float> tmpIntake(nAgents, 0.001);
  std::swap(intake, tmpIntake);
  tmpIntake.clear();
}
