// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
#include <Rcpp.h>

// clang-format off
#include "landscape.h"

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
// clang-format on

std::mt19937 rng;

/// simple wrapping function
// because std::fabs + std::fmod is somewhat suspicious
// we assume values that are at most a little larger than max (max + 1) and
// a little smaller than zero (-1)
float wrap_pos(const float &p1, const float &pmax) {
  return p1 - pmax * std::floor(p1 / pmax);
}

void Resources::initResources() {
  // generate n central items
  std::vector<float> centreCoordX(nClusters);
  std::vector<float> centreCoordY(nClusters);
  const float padded_size_max = dSize * 0.95f;
  const float padded_size_min = dSize * 0.05f;

  // make item and cluster coords
  Rcpp::NumericVector cluster_rd_x =
      Rcpp::runif(nClusters, padded_size_min, padded_size_max);
  Rcpp::NumericVector cluster_rd_y =
      Rcpp::runif(nClusters, padded_size_min, padded_size_max);

  Rcpp::NumericVector rd_x = Rcpp::rnorm(nItems, 0.0f, clusterSpread);
  Rcpp::NumericVector rd_y = Rcpp::rnorm(nItems, 0.0f, clusterSpread);

  for (size_t i = 0; i < static_cast<size_t>(nClusters); i++) {
    centreCoordX[i] = cluster_rd_x(i);
    centreCoordY[i] = cluster_rd_y(i);
  }

  // generate items around
  for (int i = 0; i < nItems; i++) {
    coordX[i] = (centreCoordX[(i % nClusters)] + rd_x(i));
    coordY[i] = (centreCoordY[(i % nClusters)] + rd_y(i));

    // wrap
    coordX[i] = wrap_pos(dSize + coordX[i], dSize);
    coordY[i] = wrap_pos(dSize + coordY[i], dSize);
  }

  // initialise rtree and set counter value
  bgi::rtree<value, bgi::quadratic<16>> tmpRtree;
  for (int i = 0; i < nItems; ++i) {
    point p = point(coordX[i], coordY[i]);
    tmpRtree.insert(std::make_pair(p, i));
  }

  // initialise counters
  // mean time
  const int mean_time =
      std::max(1, static_cast<int>(static_cast<float>(regen_time) / 10.f));
  counter = Rcpp::as<std::vector<int>>(Rcpp::rpois(nItems, mean_time));

  std::swap(rtree, tmpRtree);
  tmpRtree.clear();
}

void Resources::countAvailable() {
  nAvailable = 0;
  // counter set to max regeneration value on foraging
  for (size_t i = 0; i < static_cast<size_t>(nItems); i++) {
    if (counter[i] == 0) {
      nAvailable++;
    }
  }
}

void Resources::regenerate() {
  for (int i = 0; i < nItems; i++) {
    counter[i] -= (counter[i] > 0 ? 1 : 0);
    available[i] = (counter[i] == 0);
  }
  // set availability
  countAvailable();
}

/// function to export landscape as matrix
//' Returns a test landscape.
//'
//' @param nItems How many items.
//' @param landsize Size as a numeric (float).
//' @param nClusters How many clusters, an integer value.
//' @param clusterSpread Dispersal of items around cluster centres.
//' @param regen_time Regeneration time, in timesteps.
//' @return A data frame of the evolved population traits.
//' @export
// [[Rcpp::export]]
Rcpp::DataFrame get_test_landscape(const int nItems, const float landsize,
                                   const int nClusters,
                                   const float clusterSpread,
                                   const int regen_time) {
  unsigned seed = static_cast<unsigned>(
      std::chrono::system_clock::now().time_since_epoch().count());
  rng.seed(seed);

  Resources food(nItems, landsize, nClusters, clusterSpread, regen_time);
  food.initResources();

  return Rcpp::DataFrame::create(Rcpp::Named("x") = food.coordX,
                                 Rcpp::Named("y") = food.coordY,
                                 Rcpp::Named("tAvail") = food.counter);
}
