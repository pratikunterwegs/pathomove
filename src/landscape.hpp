#ifndef LANDSCAPE_H
#define LANDSCAPE_H

// [[Rcpp::depends(BH)]]

// Enable C++11 via this plugin to suppress 'long long' errors
// [[Rcpp::plugins("cpp14")]]

#include <vector>
#include <Rcpp.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
using namespace Rcpp;
#include <algorithm>
#include <random>
#include <functional>
#include <chrono>
#include "parameters.hpp"

// apparently some types
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<point, unsigned> value;

// items class
struct Resources {
public:
    Resources(const int nItems,
              const float landsize,
              const int nClusters,
              const float clusterSpread,
              const int regen_time) :
        nItems(nItems),
        dSize(landsize),
        nClusters(nClusters),
        clusterSpread(clusterSpread),
        regen_time(regen_time),
        coordX(nItems, 0.0f),
        coordY(nItems, 0.0f),
        available(nItems, true),
        counter(nItems, 0),
        nAvailable(nItems)
    {}
    ~Resources() {}

    const int nItems;
    const float dSize;
    const int nClusters;
    const float clusterSpread;
    const int regen_time;
    std::vector<float> coordX;
    std::vector<float> coordY;
    std::vector<bool> available;
    std::vector<int> counter;
    int nAvailable;
    // make rtree
    bgi::rtree< value, bgi::quadratic<16> > rtree;

    // funs to init with nCentres
    void initResources();
    void countAvailable();
    void regenerate();
};

#endif // LANDSCAPE_H
