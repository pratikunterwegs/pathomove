// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
#ifndef SRC_LANDSCAPE_H_
#define SRC_LANDSCAPE_H_

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
  bgi::rtree<value, bgi::quadratic<16> > rtree;

  // funs to init with nCentres
  void initResources();
  void countAvailable();
  void regenerate();
};

float wrap_pos(const float &p1, const float &pmax);

/// function to set the simulation seed for R RNG
void set_seed(const int &seed);

#endif  // SRC_LANDSCAPE_H_
