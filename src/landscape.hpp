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
    Resources(const int nItemsInit,
              const float landsize,
              const int nClusters,
              const float clusterDispersal) :
        nItems(nItemsInit),
        dSize(landsize),
        nClusters(nClusters),
        clusterDispersal(clusterDispersal),
        coordX(nItems, 0.0f),
        coordY(nItems, 0.0f),
        available(nItems, true),
        nAvailable(nItems)
    {}
    ~Resources() {}

    int nItems;
    float dSize;
    int nClusters;
    float clusterDispersal;
    std::vector<float> coordX;
    std::vector<float> coordY;
    std::vector<bool> available;
    int nAvailable;
    // make rtree
    bgi::rtree< value, bgi::quadratic<16> > rtree;

    // funs to init with nCentres
    void initResources();
    void countAvailable();
};

#endif // LANDSCAPE_H
