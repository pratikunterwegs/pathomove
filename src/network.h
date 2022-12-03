// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
/// the network structure, which holds an adjacency matrix
#ifndef SRC_NETWORK_H_
#define SRC_NETWORK_H_

// clang-format off
#include <cassert>
#include <vector>

#include <Rcpp.h>

// clang-format on

// network should be a member of population later
// network has funs to return network metrics and the adj matrix
struct Network {
 public:
  explicit Network(const int popsize)
      : nVertices(popsize), adjMat(popsize, popsize) {}
  ~Network() {}

  // members
  const int nVertices;
  Rcpp::NumericMatrix adjMat;

  // functions
  Rcpp::DataFrame getNtwkDf();
};

#endif  // SRC_NETWORK_H_
