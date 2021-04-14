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

void Resources::initResources() {
    // generate n central items
    std::vector<double> centreCoordX (nClusters);
    std::vector<double> centreCoordY (nClusters);

    for(size_t i = 0; i < static_cast<size_t>(nClusters); i++) {

        centreCoordX[i] = /*dist(rng);*/gsl_rng_uniform(r) * dSize;
        centreCoordY[i] = /*dist(rng);*/gsl_rng_uniform(r) * dSize;

        // also add to main set
        coordX[i] = centreCoordX[i];
        coordY[i] = centreCoordY[i];
    }

    // generate items around
    for(int i = nClusters; i < nItems; i++) {

        coordX[i] = (centreCoordX[(i % nClusters)] + gsl_ran_gaussian(r, clusterDispersal));
        coordY[i] = (centreCoordY[(i % nClusters)] + gsl_ran_gaussian(r, clusterDispersal));

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
    // counter set to max regeneration value on foraging
    for (size_t i = 0; i < static_cast<size_t>(nItems); i++){
        nAvailable += available[i] ? 1 : 0;
    }
}

/// function to export landscape as matrix
//' Returns a test landscape.
//'
//' @param nItems How many items.
//' @param landsize Size as a numeric (double).
//' @param nClusters How many clusters, an integer value.
//' @param clusterDispersal Dispersal of items around cluster centres.
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
Rcpp::DataFrame get_test_landscape(
        const int nItems, const double landsize,
        const int nClusters, const double clusterDispersal) {
    Resources thisLandscape (nItems, landsize, nClusters, clusterDispersal);
    thisLandscape.initResources();

    return Rcpp::DataFrame::create(
                Rcpp::Named("x") = thisLandscape.coordX,
                Rcpp::Named("y") = thisLandscape.coordY
            );
}

#endif // LANDSCAPE_H
