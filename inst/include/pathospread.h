// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
#ifndef INST_INCLUDE_PATHOSPREAD_H_
#define INST_INCLUDE_PATHOSPREAD_H_

/// define functions for pathogen spread

// clang-format off
#include <unordered_set>
#include <vector>

#include "parameters.h"
#include "agent_dyn.h"
// clang-format on

namespace pathomove {
/// function to infect n individuals
inline void Population::introducePathogen(const int initialInfections) {
  // recount for safety
  countInfected();
  shufflePop();
  int agents_to_try = initialInfections;
  int agents_remaining = nAgents;

  // loop through the intended number of infections
  for (int i = 0; i < agents_to_try; i++) {
    size_t id = order[i];
    // if agent is NOT ALREADY INFECTED
    if (!infected[id]) {
      // toggle infected agents boolean for infected
      infected[id] = true;
      timeInfected[id] = 1;
      srcInfect[id] = 0;  // count as forced, 0
    } else {
      agents_to_try++;
    }
    // decrease the number of agents remaining to try
    agents_remaining--;
    if (agents_remaining == 0) {
      break;
    }
  }
  // count after
  countInfected();
}

/// function to spread pathogen
inline void Population::pathogenSpread() {
  // looping through agents, query rtree for neighbours
  // pathogen spreads in random order
  shufflePop();
  for (int i = 0; i < nAgents; i++) {
    // spread to neighbours if self infected
    size_t id = order[i];
    if (infected[id]) {
      timeInfected[id]++;  // increase time infecetd
      // get neigbour ids
      std::vector<int> nbrsId = getNeighbourId(coordX[id], coordY[id]);

      if (nbrsId.size() > 0) {
        // draw whether neighbours will be infected
        auto transmission = Rcpp::rbinom(nbrsId.size(), 1, pTransmit);

        // loop through neighbours
        for (size_t j = 0; j < nbrsId.size(); j++) {
          size_t toInfect = nbrsId[j];

          if (!infected[toInfect]) {
            // infect neighbours with prob p
            if (transmission(j)) {
              infected[toInfect] = true;
              srcInfect[toInfect] = id + 1;  // add 1 to id
            }
          }
        }
      }
    }
  }
}

/// function for pathogen cost --- use old formula
inline void Population::pathogenCost(const float costInfect,
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
inline void Population::countInfected() {
  nInfected = 0;
  for (int i = 0; i < nAgents; i++) {
    if (infected[i]) {
      nInfected++;
    }
  }
  assert(nInfected <= nAgents);
}
}  // namespace pathomove

#endif  // INST_INCLUDE_PATHOSPREAD_H_
