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
  Rcpp::DataFrame ntwkDf = Rcpp::DataFrame::create(Named("focal") = focal,
                                                   Named("target") = subfocal,
                                                   Named("assoc") = edgeWeight);

  return ntwkDf;
}
