// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
#ifndef INST_INCLUDE_PARAMETERS_H_
#define INST_INCLUDE_PARAMETERS_H_

// Enable C++14 via this plugin to suppress 'long long' errors
// [[Rcpp::plugins("cpp14")]]
// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppParallel)]]

#include <chrono>
#include <random>

namespace pathomove {
// landscape
const double foodEnergy = 1.0;
}  // namespace pathomove

#endif  // INST_INCLUDE_PARAMETERS_H_
