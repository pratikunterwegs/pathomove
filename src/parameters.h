// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
#ifndef SRC_PARAMETERS_H_
#define SRC_PARAMETERS_H_

// Enable C++14 via this plugin to suppress 'long long' errors
// [[Rcpp::plugins("cpp14")]]
// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppParallel)]]

#include <chrono>
#include <random>

extern std::mt19937 rng;

// landscape
const double foodEnergy = 1.0;

#endif  // SRC_PARAMETERS_H_
