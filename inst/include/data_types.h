// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
/// data types and related functions
#ifndef INST_INCLUDE_DATA_TYPES_H_
#define INST_INCLUDE_DATA_TYPES_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "agent_dyn.h"

namespace pathomove {
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

/// function to update gendata
inline void genData::updateGenData(const Population &pop, const int g_) {
  gDataList.push_back(Rcpp::DataFrame::create(
      // get pop data
      Rcpp::Named("intake") =
          pop.intake,  // this returns the intake! not the net energy
      Rcpp::Named("energy") =
          pop.energy,  // this returns the net energy, fitness proxy
      Rcpp::Named("sF") = pop.sF, Rcpp::Named("sH") = pop.sH,
      Rcpp::Named("sN") = pop.sN, Rcpp::Named("x") = pop.initX,
      Rcpp::Named("y") = pop.initY, Rcpp::Named("xn") = pop.coordX,
      Rcpp::Named("yn") = pop.coordY, Rcpp::Named("assoc") = pop.associations,
      Rcpp::Named("t_infec") = pop.timeInfected,
      Rcpp::Named("src_infect") = pop.srcInfect,
      Rcpp::Named("moved") = pop.moved));

  gNInfected.push_back(pop.nInfected);
  gens.push_back(g_);
}

// function to return gen data as an rcpp list
inline Rcpp::List genData::getGenData() {
  Rcpp::List dataToReturn = Rcpp::List::create(
      Rcpp::Named("pop_data") = gDataList, Rcpp::Named("gens") = gens,
      Rcpp::Named("n_infected") = gNInfected);

  return dataToReturn;
}

inline void moveData::updateMoveData(const Population &pop, const int t_) {
  assert(t_ <= tmax && "too many timesteps logged");

  timesteps[t_] = std::vector<int>(popsize, t_);
  x[t_] = pop.coordX;
  y[t_] = pop.coordY;
}

inline Rcpp::List moveData::getMoveData() {
  Rcpp::List mDataList(tmax);
  std::vector<int> id(popsize, 0);
  std::iota(std::begin(id), std::end(id), 0);

  for (int i = 0; i < tmax; i++) {
    mDataList[i] = Rcpp::DataFrame::create(
        Rcpp::Named("time") = timesteps[i], Rcpp::Named("x") = x[i],
        Rcpp::Named("y") = y[i], Rcpp::Named("id") = id);
  }

  return mDataList;
}
}  // namespace pathomove

#endif  // INST_INCLUDE_DATA_TYPES_H_
