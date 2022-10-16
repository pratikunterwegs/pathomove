#ifndef DATA_TYPES_H
#define DATA_TYPES_H
/// data types and related functions
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "agents.h"
#include "network.h"

// define a struct holding a vector of data frames which holds generation wise
// data
struct genData {
  // public:
  // genData() : () {}
  // ~genData() {}

  Rcpp::List gDataList;

  // const int genmax;
  // const int increment;
  // const int gSampled;
  // std::vector<std::vector<float>> gIntake, gEnergy, gSF, gSH, gSN, gSI;

  // std::vector<std::vector<float>> gX, gY, gXn, gYn;

  // std::vector<std::vector<int>> gAssoc, gTInfected, gSrc;
  // std::vector<std::vector<float>> gMoved;

  std::vector<int> gNInfected, gens;

  void updateGenData(Population &pop, const int g_);
  Rcpp::List getGenData();
};

struct moveData {
public:
  moveData(const int tmax, const int popsize)
      : tmax(tmax), popsize(popsize),
        timesteps(tmax, std::vector<int>(popsize, 0)),
        x(tmax, std::vector<float>(popsize, 0)),
        y(tmax, std::vector<float>(popsize, 0)) {}
  ~moveData() {}

  const int tmax;
  const int popsize;
  std::vector<std::vector<int>> timesteps;
  std::vector<std::vector<float>> x, y;

  void updateMoveData(Population &pop, const int t_);
  Rcpp::List getMoveData();
};

#endif //
