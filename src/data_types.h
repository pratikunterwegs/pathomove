// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
/// data types and related functions
#ifndef SRC_DATA_TYPES_H_
#define SRC_DATA_TYPES_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "agents.h"
#include "network.h"

// define a struct holding a vector of data frames which holds generation wise
// data
struct genData {
  Rcpp::List gDataList;

  std::vector<int> gNInfected, gens;

  void updateGenData(const Population &pop, const int g_);
  Rcpp::List getGenData();
};

struct moveData {
 public:
  moveData(const int tmax, const int popsize)
      : tmax(tmax),
        popsize(popsize),
        timesteps(tmax, std::vector<int>(popsize, 0)),
        x(tmax, std::vector<float>(popsize, 0)),
        y(tmax, std::vector<float>(popsize, 0)) {}
  ~moveData() {}

  const int tmax;
  const int popsize;
  std::vector<std::vector<int>> timesteps;
  std::vector<std::vector<float>> x, y;

  void updateMoveData(const Population &pop, const int t_);
  Rcpp::List getMoveData();
};

#endif  // SRC_DATA_TYPES_H_
