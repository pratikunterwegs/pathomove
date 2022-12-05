// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
/// define functions for pathogen spread

// clang-format off
#include <unordered_set>

#include "parameters.h"
#include "agents.h"
// clang-format on

/// function to infect n individuals
void Population::introducePathogen(const int initialInfections) {
  // recount for safety
  countInfected();
  shufflePop();
  // loop through the intended number of infections
  for (int i = 0; i < initialInfections; i++) {
    size_t id = order[i];
    // toggle infected agents boolean for infected
    infected[id] = true;
    timeInfected[id] = 1;
    srcInfect[id] = -2;  // count as forced
  }
  // count after
  countInfected();
  assert(nInfected == initialInfections &&
         "wrong number of initial infections");
}

/// function to spread pathogen
void Population::pathogenSpread() {
  std::bernoulli_distribution transmission(pTransmit);
  std::vector<bool> new_infections (nAgents, false);  // new infections
  // looping through agents, query rtree for neighbours
  for (int i = 0; i < nAgents; i++) {
    // spread to neighbours if self infected
    if (infected[i]) {
      new_infections[i] = true;
      timeInfected[i]++;  // increase time infecetd
      // get neigbour ids
      std::vector<int> nbrsId = getNeighbourId(coordX[i], coordY[i]);

      if (nbrsId.size() > 0) {
        // draw whether neighbours will be infected
        auto transmission = Rcpp::rbinom(nbrsId.size(), 1, pTransmit);

        // loop through neighbours
        for (size_t j = 0; j < nbrsId.size(); j++) {
          size_t toInfect = nbrsId[j];

          if (!infected[toInfect]) {
            // infect neighbours with prob p
            if (transmission(rng)) {
              new_infections[toInfect] = true;
              // infected[toInfect] = true;
              srcInfect[toInfect] = i;
            }
          }
        }
      }
    }
  }
  std::swap(infected, new_infections);
  new_infections.clear();
}

/// function for pathogen cost --- use old formula
void Population::pathogenCost(const float costInfect,
                              const bool infect_percent) {
  for (int i = 0; i < nAgents; i++) {
    if (infect_percent) {
      energy[i] = intake[i] * (std::pow((1.f - costInfect),
                                        static_cast<float>(timeInfected[i])));
    } else {
      energy[i] =
          intake[i] - (costInfect * static_cast<float>(timeInfected[i]));
    }
  }
}

/// count infected agents
void Population::countInfected() {
  nInfected = 0;
  for (int i = 0; i < nAgents; i++) {
    if (infected[i]) {
      nInfected++;
    }
  }
  assert(nInfected <= nAgents);
}
