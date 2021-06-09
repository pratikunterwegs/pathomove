#ifndef DATA_TYPES_H
#define DATA_TYPES_H
/// data types and related functions
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "agents.h"
#include "network.h"

// define a struct holding a vector of data frames which holds generation wise data
struct genData {
public:
    std::vector<std::vector<double> > genEnergyVec;
    std::vector<std::vector<float> > genCoefFoodVec;
    std::vector<std::vector<float> > genCoefNbrsVec;
    // std::vector<std::vector<int> > genAssocVec;
    // std::vector<std::vector<int> > genDegreeVec;
    std::vector<int> gens;

    void updateGenData (Population &pop, const int gen);
    Rcpp::List getGenData ();
};

struct moveData {
public:
    std::vector<std::vector<int> > id;
    std::vector<std::vector<double> > coordX;
    std::vector<std::vector<double> > coordY;
    std::vector<std::vector<double> > energy;
    std::vector<int> timestep;

    void updateMoveData (Population &pop, const int timestep);
    Rcpp::List getMoveData ();
};

#endif  //
