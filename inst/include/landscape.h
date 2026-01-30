// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.

#pragma once

// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppParallel)]]

// clang-format off
#include <algorithm>
#include <chrono>
#include <functional>
#include <random>
#include <utility>
#include <vector>

#include <Rcpp.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "parameters.h"
// clang-format on

namespace pathomove {
// apparently some types
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<point, unsigned> value;

// items class
struct Resources {
 public:
  Resources(const int nItems, const float landsize, const int nClusters,
            const float clusterSpread, const int regen_time)
      : nItems(nItems),
        dSize(landsize),
        nClusters(nClusters),
        clusterSpread(clusterSpread),
        regen_time(regen_time),
        coordX(nItems, 0.0f),
        coordY(nItems, 0.0f),
        available(nItems, true),
        counter(nItems, 0),
        nAvailable(nItems) {}
  ~Resources() {}

  const int nItems;
  const float dSize;
  const int nClusters;
  const float clusterSpread;
  const int regen_time;
  std::vector<float> coordX, coordY;
  std::vector<bool> available;
  std::vector<int> counter;
  int nAvailable;
  // make rtree
  bgi::rtree<value, bgi::quadratic<16>> rtree;

  // funs to init with nCentres
  void initResources();
  void countAvailable();
  void regenerate();
};

/// @brief Function to set the R RNG seed
/// @param seed An integer passed to run_pathomve
inline void set_seed(const int &seed) {
  Rcpp::Environment base_env("package:base");
  Rcpp::Function set_seed_r = base_env["set.seed"];
  set_seed_r(seed);
}

/// simple wrapping function
// because std::fabs + std::fmod is somewhat suspicious
// we assume values that are at most a little larger than max (max + 1) and
// a little smaller than zero (-1)
inline float wrap_pos(const float &p1, const float &pmax) {
  return p1 - pmax * std::floor(p1 / pmax);
}

inline void Resources::initResources() {
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

inline void Resources::countAvailable() {
  nAvailable = 0;
  // counter set to max regeneration value on foraging
  for (size_t i = 0; i < static_cast<size_t>(nItems); i++) {
    if (counter[i] == 0) {
      nAvailable++;
    }
  }
}

inline void Resources::regenerate() {
  for (int i = 0; i < nItems; i++) {
    counter[i] -= (counter[i] > 0 ? 1 : 0);
    available[i] = (counter[i] == 0);
  }
  // set availability
  countAvailable();
}
}  // namespace pathomove
