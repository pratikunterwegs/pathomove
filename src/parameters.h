#ifndef PARAMETERS_H
#define PARAMETERS_H

// Enable C++14 via this plugin to suppress 'long long' errors
// [[Rcpp::plugins("cpp14")]]
// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppParallel)]]
//' @importFrom RcppParallel RcppParallelLibs

#include <boost/random/mersenne_twister.hpp>
#include <chrono>

extern boost::mt19937 gen;

// landscape
const double foodEnergy = 1.0;

#endif // PARAMETERS_H
