#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include "parameters.h"
#include <vector>
#include <algorithm>
#include <functional>

// boost geometry libraries using boost headers R
// [[Rcpp::plugins(cpp14)]]
// [[Rcpp::depends(BH)]]

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <Rcpp.h>

// apparently some types
typedef boost::geometry::model::point<float, 2, boost::geometry::cs::cartesian> point;
typedef boost::geometry::model::box<point> box;
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
    boost::geometry::index::rtree< value, boost::geometry::index::quadratic<16> > rtree;

    // funs to init with nCentres
    void initResources();
    void countAvailable();
};

void Resources::initResources() {
    // generate n central items
    std::vector<double> centreCoordX (nClusters);
    std::vector<double> centreCoordY (nClusters);

    std::uniform_real_distribution<double> item_ran_pos(0.0, dSize);
    std::normal_distribution<double> item_cluster_spread(0.0, clusterDispersal);

    for(size_t i = 0; i < static_cast<size_t>(nClusters); i++) {

        centreCoordX[i] = item_ran_pos(rng);
        centreCoordY[i] = item_ran_pos(rng);

        // also add to main set
        coordX[i] = centreCoordX[i];
        coordY[i] = centreCoordY[i];
    }

    // generate items around
    for(int i = nClusters; i < nItems; i++) {

        coordX[i] = (centreCoordX[(i % nClusters)] + item_cluster_spread(rng));
        coordY[i] = (centreCoordY[(i % nClusters)] + item_cluster_spread(rng));

        // wrap
        coordX[i] = fmod(dSize + coordX[i], dSize);
        coordY[i] = fmod(dSize + coordY[i], dSize);
    }

    // initialise rtree
    boost::geometry::index::rtree< value, boost::geometry::index::quadratic<16> > tmpRtree;
    for (int i = 0; i < nItems; ++i)
    {
        point p = point(coordX[i], coordY[i]);
        tmpRtree.insert(std::make_pair(p, i));
    }
    std::swap(rtree, tmpRtree);
    tmpRtree.clear();

    // set all to available
    available = std::vector<bool> (nItems, true);
}

void Resources::countAvailable() {
    nAvailable = 0;
    // counter set to max regeneration value on foraging
    for (size_t i = 0; i < static_cast<size_t>(nItems); i++){
        if(available[i]) {
            nAvailable ++;
        }
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
