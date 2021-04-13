#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include "parameters.h"
#include <vector>
#include <algorithm>
#include <functional>
// boost geometry libraries
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <Rcpp.h>

// make namespaces
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// apparently some types
typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<point, unsigned> value;

// items class
struct Resources {
public:
    Resources(const int nItemsInit, const double landsize,
        double regenTime) :
    nItems(nItemsInit),
    dSize(landsize),
    coordX(nItems, 0.0),
    coordY(nItems, 0.0),
    counter(nItems, 0.0),
    nAvailable(nItems),
    regenTime(2.0),
    whichAvailable(nItems)
    {}
    ~Resources() {}

    int nItems;
    double dSize;
    std::vector<double> coordX;
    std::vector<double> coordY;
    std::vector<int> counter;
    int nAvailable;
    double regenTime;
    std::vector<size_t> whichAvailable;
    // make rtree
    bgi::rtree< value, bgi::quadratic<16> > rtree;

    // funs to init with nCentres
    void initResources(const int nCentres, const double dDispersal);
    void countAvailable();
    void setRegenTime(double newRegenTime);
};

void Resources::initResources(const int nCentres, const double dDispersal) {
    // generate n central items
    std::vector<double> centreCoordX (nCentres);
    std::vector<double> centreCoordY (nCentres);

    for(size_t i = 0; i < static_cast<size_t>(nCentres); i++) {

        centreCoordX[i] = /*dist(rng);*/gsl_rng_uniform(r) * dSize;
        centreCoordY[i] = /*dist(rng);*/gsl_rng_uniform(r) * dSize;

        // also add to main set
        coordX[i] = centreCoordX[i];
        coordY[i] = centreCoordY[i];
    }

    // generate items around
    for(int i = nCentres; i < nItems; i++) {

        coordX[i] = (centreCoordX[(i % nCentres)] + gsl_ran_gaussian(r, dDispersal));
        coordY[i] = (centreCoordY[(i % nCentres)] + gsl_ran_gaussian(r, dDispersal));

        // wrap
        coordX[i] = fmod(dSize + coordX[i], dSize);
        coordY[i] = fmod(dSize + coordY[i], dSize);
    }

    // initialise rtree
    bgi::rtree< value, bgi::quadratic<16> > tmpRtree;
    for (int i = 0; i < nItems; ++i)
    {
        point p = point(coordX[i], coordY[i]);
        tmpRtree.insert(std::make_pair(p, i));
    }
    std::swap(rtree, tmpRtree);
    tmpRtree.clear();
}

void Resources::countAvailable() {
    nAvailable = 0;
    whichAvailable.clear();
    // counter set to max regeneration value on foraging
    for (size_t i = 0; i < static_cast<size_t>(nItems); i++){
        nAvailable += counter[i] == 0 ? 1 : 0;
        whichAvailable.push_back(i);
    }
}

void Resources::setRegenTime (double newRegenTime) {
    regenTime = newRegenTime;
}

/// function to export landscape as matrix
//' @param nItems How many items.
//' @param landsize Size as a numeric (double).
//' @param nClusters How many clusters, an integer value.
//' @param clusterDispersal Dispersal of items around cluster centres.
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
Rcpp::DataFrame get_test_landscape(
    const int nItems, const double landsize,
    const int nClusters, const double clusterDispersal) {
    Resources thisLandscape (nItems, landsize, 0.0);
    thisLandscape.initResources(nClusters, clusterDispersal);

    return Rcpp::DataFrame::create(
        Rcpp::Named("x") = thisLandscape.coordX,
        Rcpp::Named("y") = thisLandscape.coordY
    );
}

#endif // LANDSCAPE_H
