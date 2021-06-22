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
#include "parameters.h"

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
              const double landsize,
              const int nClusters,
              const double clusterDispersal) :
        nItems(nItemsInit),
        dSize(landsize),
        nClusters(nClusters),
        clusterDispersal(clusterDispersal),
        coordX(nItems, 0.0),
        coordY(nItems, 0.0),
        available(nItems, true),
        nAvailable(nItems)
    {}
    ~Resources() {}

    int nItems;
    double dSize;
    int nClusters;
    double clusterDispersal;
    std::vector<double> coordX;
    std::vector<double> coordY;
    std::vector<bool> available;
    int nAvailable;
    // make rtree
    bgi::rtree< value, bgi::quadratic<16> > rtree;

    // funs to init with nCentres
    void initResources();
    void countAvailable();
};

#endif // LANDSCAPE_H
