#ifndef PARAMETERS_H
#define PARAMETERS_H

// Enable C++14 via this plugin to suppress 'long long' errors
// [[Rcpp::plugins("cpp14")]]
// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppParallel)]]

#include <random>
#include <chrono>

extern std::mt19937 rng;

// landscape
const double foodEnergy = 1.0;

#endif // PARAMETERS_H
