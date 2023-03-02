// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
/// the network structure, which holds an adjacency matrix
#ifndef INST_INCLUDE_NETWORK_H_
#define INST_INCLUDE_NETWORK_H_

// clang-format off
#include <cassert>
#include <vector>

#include <Rcpp.h>

// clang-format on

namespace pathomove {
// network should be a member of population later
// network has funs to return network metrics and the adj matrix
struct Network {
 public:
  explicit Network(const int popsize)
      : nVertices(popsize), adjMat(popsize, popsize) {}
  ~Network() {}

  // members
  const int nVertices;
  Rcpp::IntegerMatrix adjMat;

  // functions
  Rcpp::DataFrame getNtwkDf();
};

/// function for dataframe from rcpp matrix
inline Rcpp::DataFrame Network::getNtwkDf() {
  std::vector<int> focal;
  std::vector<int> subfocal;
  std::vector<int> edgeWeight;

  for (int n = 0; n < nVertices; ++n) {
    for (int m = n + 1; m < nVertices; ++m) {
      focal.push_back(n);
      subfocal.push_back(m);
      edgeWeight.push_back(adjMat(n, m));
    }
  }

  assert(focal.size() == subfocal.size() && "ntwkdf: unequal lengths");

  // make df
  Rcpp::DataFrame ntwkDf = Rcpp::DataFrame::create(
      Rcpp::Named("focal") = focal, Rcpp::Named("target") = subfocal,
      Rcpp::Named("assoc") = edgeWeight);

  return ntwkDf;
}
}  // namespace pathomove

#endif  // INST_INCLUDE_NETWORK_H_
