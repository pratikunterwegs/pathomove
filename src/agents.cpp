#define _USE_MATH_DEFINES
/// code to make agents
#include "agents.h"

#include <Rcpp.h>
#include <RcppParallel.h>

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/range/algorithm.hpp>
#include <cassert>
#include <iostream>
#include <vector>

#include "landscape.h"
#include "network.h"

// to shuffle pop id
void Population::shufflePop() {
  if (order[0] == order[nAgents - 1]) {
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
      order[i] = i;
    }
    boost::range::random_shuffle(order);
  } else {
    boost::range::random_shuffle(order);
  }
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
  Rcpp::NumericVector rd_x = Rcpp::runif(nAgents);
  Rcpp::NumericVector rd_y = Rcpp::runif(nAgents);
  for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
    coordX[i] = rd_x(i) * food.dSize;
    initX[i] = coordX[i];
    coordY[i] = rd_y(i) * food.dSize;
    initY[i] = coordY[i];
  }
  updateRtree();
}

// set agent trait
void Population::setTrait(const float &mSize) {
  // create a cauchy distribution, mSize is the scale
  sF = Rcpp::as<std::vector<float>>(Rcpp::rcauchy(nAgents, 0.0, mSize));
  sH = Rcpp::as<std::vector<float>>(Rcpp::rcauchy(nAgents, 0.0, mSize));
  sN = Rcpp::as<std::vector<float>>(Rcpp::rcauchy(nAgents, 0.0, mSize));
}

float get_distance(const float &x1, const float &x2, const float &y1,
                   const float &y2) {
  return std::sqrt(std::pow((x1 - x2), 2) + std::pow((y1 - y2), 2));
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

  BOOST_FOREACH (value const &v, near_agents) {
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

  BOOST_FOREACH (value const &v, near_agents) {
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

    BOOST_FOREACH (value const &v, near_food) {
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

    BOOST_FOREACH (value const &v, near_food) {
      // count only which are available!
      if (food.available[v.second]) {
        food_id.push_back(v.second);
      }
    }
    near_food.clear();
  }

  // first element is number of near entities
  // second is the identity of entities
  return food_id;
}

/// population movement function
void Population::move(const Resources &food, const bool &multithreaded) {
  const float twopi = 2.f * M_PI;

  // what increment for 3 samples in a circle around the agent
  const float increment = twopi / n_samples;
  float angle = 0.f;
  // for this increment what angles to sample at
  std::vector<float> sample_angles(static_cast<int>(n_samples), 0.f);
  for (int i_ = 0; i_ < static_cast<int>(n_samples); i_++) {
    sample_angles[i_] = angle;
    angle += increment;
  }

  // make random noise for each individual and each sample
  Rcpp::NumericMatrix noise_v(nAgents, n_samples);
  for (size_t i_ = 0; i_ < n_samples; i_++) {
    noise_v(_, i_) = Rcpp::rnorm(nAgents, 0.0f, 0.01f);
  }

  shufflePop();
  // loop over agents --- randomise
  if (multithreaded) {
    // any number above 1 will allow automatic n threads
    unsigned int p = tbb::task_scheduler_init::default_num_threads();

    // try parallel
    tbb::parallel_for(
        tbb::blocked_range<unsigned>(1, nAgents),
        [&](const tbb::blocked_range<unsigned> &r) {
          for (unsigned i = r.begin(); i < r.end(); ++i) {
            int id = order[i];
            if (counter[id] > 0) {
              counter[id]--;
            } else {
              // first assess current location
              float sampleX = coordX[id];
              float sampleY = coordY[id];

              float foodHere = 0.f;
              // count local food only if items are available
              if (food.nAvailable > 0) {
                foodHere =
                    static_cast<float>(countFood(food, sampleX, sampleY));
              }
              // count local handlers and non-handlers
              std::pair<int, int> agentCounts = countAgents(sampleX, sampleY);

              // get suitability current
              float suit_origin =
                  ((sF[id] * foodHere) + (sH[id] * agentCounts.first) +
                   (sN[id] * agentCounts.second));

              // add self-isolation coefficient
              if (use_sI) {
                suit_origin += sI[id] * infected[id] *
                               (agentCounts.first + agentCounts.second);
              }

              float newX = sampleX;
              float newY = sampleY;
              // now sample at n locations around
              for (size_t j = 0; j < sample_angles.size(); j++) {
                float t1_ = static_cast<float>(cos(sample_angles[j]));
                float t2_ = static_cast<float>(sin(sample_angles[j]));

                // use range for agents to determine sample locs
                sampleX = coordX[id] + (range_agents * t1_);
                sampleY = coordY[id] + (range_agents * t2_);

                // crudely wrap sampling location
                if ((sampleX > food.dSize) | (sampleX < 0.f)) {
                  sampleX = std::fabs(std::fmod(sampleX, food.dSize));
                }
                if ((sampleY > food.dSize) | (sampleY < 0.f)) {
                  sampleY = std::fabs(std::fmod(sampleY, food.dSize));
                }

                // count food at sample locations if any available
                if (food.nAvailable > 0) {
                  foodHere =
                      static_cast<float>(countFood(food, sampleX, sampleY));
                }

                // count local handlers and non-handlers
                std::pair<int, int> agentCounts = countAgents(sampleX, sampleY);

                float suit_dest =
                    ((sF[id] * foodHere) + (sH[id] * agentCounts.first) +
                     (sN[id] * agentCounts.second) +
                     noise_v(id, j) // add same very very small noise to all
                    );
                if (use_sI) {
                  suit_dest += sI[id] * infected[id] *
                               (agentCounts.first + agentCounts.second);
                }

                if (suit_dest > suit_origin) {
                  // where does the individual really go
                  newX = coordX[id] + (range_move * t1_);
                  newY = coordY[id] + (range_move * t2_);

                  // crudely wrap MOVEMENT location
                  if ((newX > food.dSize) | (newX < 0.f)) {
                    newX = std::fabs(std::fmod(newX, food.dSize));
                  }
                  if ((newY > food.dSize) | (newY < 0.f)) {
                    newY = std::fabs(std::fmod(newY, food.dSize));
                  }

                  assert(newX < food.dSize && newX > 0.f);
                  assert(newY < food.dSize && newY > 0.f);
                  suit_origin = suit_dest;
                }
              }
              // distance to be moved
              moved[id] += range_move;

              // set locations
              coordX[id] = newX;
              coordY[id] = newY;
            }
          }
        });
  } else {
    for (int i = 0; i < nAgents; ++i) {
      int id = order[i];
      if (counter[id] > 0) {
        counter[id]--;
      } else {
        // first assess current location
        float sampleX = coordX[id];
        float sampleY = coordY[id];

        float foodHere = 0.f;
        // count local food only if items are available
        if (food.nAvailable > 0) {
          foodHere = static_cast<float>(countFood(food, sampleX, sampleY));
        }
        // count local handlers and non-handlers
        std::pair<int, int> agentCounts = countAgents(sampleX, sampleY);

        // get suitability current
        float suit_origin =
            ((sF[id] * foodHere) + (sH[id] * agentCounts.first) +
             (sN[id] * agentCounts.second));
        if (use_sI) {
          suit_origin +=
              sI[id] * infected[id] * (agentCounts.first + agentCounts.second);
        }

        float newX = sampleX;
        float newY = sampleY;
        // now sample at three locations around
        for (size_t j = 0; j < sample_angles.size(); j++) {
          float t1_ = static_cast<float>(cos(sample_angles[j]));
          float t2_ = static_cast<float>(sin(sample_angles[j]));

          // use range for agents to determine sample locs
          sampleX = coordX[id] + (range_agents * t1_);
          sampleY = coordY[id] + (range_agents * t2_);

          // crudely wrap sampling location
          if ((sampleX > food.dSize) | (sampleX < 0.f)) {
            sampleX = std::fabs(std::fmod(sampleX, food.dSize));
          }
          if ((sampleY > food.dSize) | (sampleY < 0.f)) {
            sampleY = std::fabs(std::fmod(sampleY, food.dSize));
          }

          // count food at sample locations if any available
          if (food.nAvailable > 0) {
            foodHere = static_cast<float>(countFood(food, sampleX, sampleY));
          }

          // count local handlers and non-handlers
          std::pair<int, int> agentCounts = countAgents(sampleX, sampleY);

          float suit_dest =
              ((sF[id] * foodHere) + (sH[id] * agentCounts.first) +
               (sN[id] * agentCounts.second) +
               noise_v(id, j) // add same very very small noise to all
              );
          if (use_sI) {
            suit_dest += sI[id] * infected[id] *
                         (agentCounts.first + agentCounts.second);
          }

          if (suit_dest > suit_origin) {
            // where does the individual really go
            newX = coordX[id] + (range_move * t1_);
            newY = coordY[id] + (range_move * t2_);

            // crudely wrap MOVEMENT location
            if ((newX > food.dSize) | (newX < 0.f)) {
              newX = std::fabs(std::fmod(newX, food.dSize));
            }
            if ((newY > food.dSize) | (newY < 0.f)) {
              newY = std::fabs(std::fmod(newY, food.dSize));
            }

            assert(newX < food.dSize && newX > 0.f);
            assert(newY < food.dSize && newY > 0.f);
            suit_origin = suit_dest;
          }
        }
        // distance to be moved
        moved[id] += range_move;

        // set locations
        coordX[id] = newX;
        coordY[id] = newY;
      }
    }
  }
}

// function to paralellise choice of forage item
void Population::pickForageItem(const Resources &food,
                                const bool &multithreaded) {
  shufflePop();
  // nearest food
  std::vector<int> idTargetFood(nAgents, -1);

  if (multithreaded) {
    // loop over agents --- no shuffling required here
    unsigned int p = tbb::task_scheduler_init::default_num_threads();

    // try parallel foraging --- agents pick a target item
    tbb::parallel_for(tbb::blocked_range<unsigned>(1, nAgents),
                      [&](const tbb::blocked_range<unsigned> &r) {
                        for (unsigned i = r.begin(); i < r.end(); ++i) {
                          if ((counter[i] > 0) | (food.nAvailable == 0)) {
                            // nothing -- agent cannot forage or there is no
                            // food
                          } else {
                            // find nearest item ids
                            std::vector<int> theseItems =
                                getFoodId(food, coordX[i], coordY[i]);
                            int thisItem = -1;

                            // check near items count
                            if (theseItems.size() > 0) {
                              // take first item by default
                              thisItem = theseItems[0];
                              idTargetFood[i] = thisItem;
                            }
                          }
                        }
                      });
  } else {
    for (int i = 0; i < nAgents; ++i) {
      if ((counter[i] > 0) | (food.nAvailable == 0)) {
        // nothing -- agent cannot forage or there is no food
      } else {
        // find nearest item ids
        std::vector<int> theseItems = getFoodId(food, coordX[i], coordY[i]);
        int thisItem = -1;

        // check near items count
        if (theseItems.size() > 0) {
          // take first item by default
          thisItem = theseItems[0];
          idTargetFood[i] = thisItem;
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
  for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
    int id = order[i];
    if ((counter[id] > 0) | (food.nAvailable == 0)) {
      // nothing
    } else {
      int thisItem = forageItem[id]; // the item picked by this agent
      // check selected item is available
      if (thisItem != -1) {
        counter[id] = handling_time;
        intake[id] += 1.0; // increased here --- not as described.

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
  // sort vec fitness
  std::vector<float> vecFitness = energy;
  std::sort(vecFitness.begin(), vecFitness.end()); // sort to to get min-max
  // scale to max fitness
  float maxFitness = vecFitness[vecFitness.size() - 1];
  float minFitness = vecFitness[0];

  // reset to energy
  vecFitness = energy;
  // rescale copied energy vector by min anx max fitness
  for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
    vecFitness[i] = ((vecFitness[i] - minFitness) / (maxFitness - minFitness));
  }

  return vecFitness;
}

/// prepare function to handle fitness and offer parents when applying a
/// reproduction threshold
std::pair<std::vector<int>, std::vector<float>>
Population::applyReprodThreshold() {
  std::vector<float> energy_pos;
  std::vector<int> id_pos;

  float this_agent_energy = 0.f;
  // add only agents with positive energy
  for (size_t i = 0; i < nAgents; i++) {
    this_agent_energy = energy[i];
    if (this_agent_energy > 0.f) {
      id_pos.push_back(i);
      energy_pos.push_back(this_agent_energy);
    }
  }

  // count agents remaining
  const int agents_remaining = id_pos.size();
  assert(agents_remaining > 0 && "Reprod threshold: no agents remaining");

  // normalise energy between 0 and 1
  std::vector<float> vecFitness = energy_pos;
  std::sort(vecFitness.begin(), vecFitness.end()); // sort to to get min-max
  // scale to max fitness
  float maxFitness = vecFitness[vecFitness.size() - 1];
  float minFitness = vecFitness[0];

  // rescale copied energy vector by min anx max fitness
  for (size_t i = 0; i < static_cast<size_t>(agents_remaining); i++) {
    energy_pos[i] = ((energy_pos[i] - minFitness) / (maxFitness - minFitness));
  }

  return std::pair<std::vector<int>, std::vector<float>>(id_pos, energy_pos);
}

// fun for replication
void Population::Reproduce(const Resources &food, const bool &infect_percent,
                           const float &dispersal, const float &mProb,
                           const float &mSize) {
  // boost random for probability of vertical transmission
  // currently same as prob for horizontal
  boost::random::bernoulli_distribution<> verticalInfect(p_vTransmit);

  // prepare to deal with fitness and reproduction options
  std::pair<std::vector<int>, std::vector<float>> thresholded_parents;
  std::vector<float> vecFitness;

  if (reprod_threshold) {
    thresholded_parents = applyReprodThreshold();
    vecFitness = thresholded_parents.second;
  } else if (infect_percent) {
    vecFitness = energy;
  } else {
    vecFitness = handleFitness();
  }

  // set up weighted lottery based on the vector of fitnesses
  boost::random::discrete_distribution<> weightedLottery(vecFitness.begin(),
                                                         vecFitness.end());

  // get parent trait based on weighted lottery
  std::vector<float> tmp_sF(nAgents, 0.f);
  std::vector<float> tmp_sH(nAgents, 0.f);
  std::vector<float> tmp_sN(nAgents, 0.f);
  std::vector<float> tmp_sI(nAgents, 0.f);

  // infected or not for vertical transmission
  std::vector<bool> infected_2(nAgents, false);

  // reset infection source
  srcInfect = std::vector<int>(nAgents, 0);

  // reset associations
  associations = std::vector<int>(nAgents, 0);

  // reset distance moved
  moved = std::vector<float>(nAgents, 0.f);

  // reset adjacency matrix
  pbsn.adjMat = Rcpp::NumericMatrix(nAgents, nAgents);

  // positions
  std::vector<float> coord_x_2(nAgents, 0.f);
  std::vector<float> coord_y_2(nAgents, 0.f);

  // Get sprout distances
  Rcpp::NumericVector sprout_x = Rcpp::rnorm(nAgents, 0.0f, dispersal);
  Rcpp::NumericVector sprout_y = Rcpp::rnorm(nAgents, 0.0f, dispersal);

  for (int a = 0; a < nAgents; a++) {
    size_t parent_id = static_cast<size_t>(weightedLottery(gen));

    // mod the parent id if a reprod threshold is applied, this helps refer
    // to the id in the thresholded parents vector
    if (reprod_threshold)
      parent_id = thresholded_parents.first[parent_id];

    tmp_sF[a] = sF[parent_id];
    tmp_sH[a] = sH[parent_id];
    tmp_sN[a] = sN[parent_id];

    // sI inherited only after patho intro
    if (use_sI)
      tmp_sI[a] = sI[parent_id];

    // inherit positions from parent
    coord_x_2[a] = coordX[parent_id] + sprout_x(parent_id);
    coord_y_2[a] = coordY[parent_id] + sprout_y(parent_id);

    // robustly wrap positions
    if (coord_x_2[a] < 0.f)
      coord_x_2[a] = food.dSize + coord_x_2[a];
    if (coord_x_2[a] > food.dSize)
      coord_x_2[a] = coord_x_2[a] - food.dSize;

    if (coord_y_2[a] < 0.f)
      coord_y_2[a] = food.dSize + coord_y_2[a];
    if (coord_y_2[a] > food.dSize)
      coord_y_2[a] = coord_y_2[a] - food.dSize;

    // vertical transmission of infection if set to TRUE
    if (vertical) {
      if (infected[parent_id]) {
        if (verticalInfect(gen)) {
          infected_2[a] = true;
          srcInfect[a] = -2; // -2 for parents
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

  if (use_sI) {
    auto mutation_sI = Rcpp::rbinom(nAgents, 1, mProb);
    Rcpp::NumericVector mut_size_sI = Rcpp::rcauchy(nAgents, 0.0f, mSize);

    for (int a = 0; a < nAgents; a++) {
      if (mutation_sI(a)) {
        tmp_sI[a] = tmp_sI[a] + mut_size_sI(a);
      }
    }
  }

  // reset nInfected and count natal infections
  // from vertical transmission
  countInfected();

  // swap trait matrices
  std::swap(sF, tmp_sF);
  std::swap(sH, tmp_sH);
  std::swap(sN, tmp_sN);
  if (use_sI)
    std::swap(sI, tmp_sI);

  tmp_sF.clear();
  tmp_sH.clear();
  tmp_sN.clear();
  tmp_sI.clear();

  // swap energy
  std::vector<float> tmpEnergy(nAgents, 0.001);
  std::swap(energy, tmpEnergy);
  tmpEnergy.clear();

  // swap intake
  std::vector<float> tmpIntake(nAgents, 0.001);
  std::swap(intake, tmpIntake);
  tmpIntake.clear();
}
