// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
/// functions for network operations

#include "network.h"

#include <Rcpp.h>

/// function for dataframe from rcpp matrix
Rcpp::DataFrame Network::getNtwkDf() {
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
