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
  // loop through the intended number of infections
  for (int i = 0; i < initialInfections; i++) {
    // toggle infected agents boolean for infected
    infected[i] = true;
    timeInfected[i] = 1;
    srcInfect[i] = -2;  // count as forced
  }
  // count after
  countInfected();
  assert(nInfected == initialInfections &&
         "wrong number of initial infections");
}

/// function to spread pathogen
void Population::pathogenSpread() {
  std::bernoulli_distribution transmission(pTransmit);
  // looping through agents, query rtree for neighbours
  for (int i = 0; i < nAgents; i++) {
    // spread to neighbours if self infected
    if (infected[i]) {
      timeInfected[i]++;  // increase time infecetd
      // get neigbour ids
      std::vector<int> nbrsId = getNeighbourId(coordX[i], coordY[i]);

      if (nbrsId.size() > 0) {
        // loop through neighbours
        for (size_t j = 0; j < nbrsId.size(); j++) {
          size_t toInfect = nbrsId[j];
          if (!infected[toInfect]) {
            // infect neighbours with prob p
            if (transmission(rng)) {
              infected[toInfect] = true;
              srcInfect[toInfect] = i;
            }
          }
        }
      }
    }
  }
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
