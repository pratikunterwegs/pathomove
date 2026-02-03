// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.

#pragma once

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
