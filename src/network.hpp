#pragma once

#include <vector>
#include <cassert>
#include <Rcpp.h>

using namespace Rcpp;

/// the network structure, which holds an adjacency matrix
// network should be a member of population later
// network has funs to return network metrics and the adj matrix
struct Network {
public:
    Network(const int popsize) :
        nVertices(popsize),
        adjMat (popsize, popsize)
    {}
    ~Network() {}

    // members
    const int nVertices;
    Rcpp::NumericMatrix adjMat;

    // functions
    std::vector<float> ntwkMeasures();
    std::vector<int> getDegree();
};

