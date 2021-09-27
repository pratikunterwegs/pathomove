#ifndef DATA_TYPES_H
#define DATA_TYPES_H
/// data types and related functions
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "agents.hpp"
#include "network.hpp"

// define a struct holding a vector of data frames which holds generation wise data
struct genData {
public:
    std::vector<std::vector<float> > genEnergyVec;
    std::vector<std::vector<float> > genSF;
    std::vector<std::vector<float> > genSH;
    std::vector<std::vector<float> > genSN;

    std::vector<std::vector<int> > genAssocVec;
    std::vector<std::vector<int> > genTimeInfec;
    std::vector<std::vector<int> > genDegree;
    // std::vector<Rcpp::NumericMatrix> genPbsn;

    std::vector<std::vector<float> > genMoved;
    std::vector<int> genInfected;
    std::vector<int> gens;
    std::vector<float> pSrcInfect;

    // network metrics
    std::vector<float> pbsn_diameter;
    std::vector<float> pbsn_glob_eff;

    void updateGenData (Population &pop, const int gen);
    Rcpp::List getGenData ();
};

struct moveData {
public:
    std::vector<std::vector<int> > id;
    std::vector<std::vector<float> > coordX;
    std::vector<std::vector<float> > coordY;
    std::vector<std::vector<float> > energy;
    std::vector<int> timestep;

    void updateMoveData (Population &pop, const int timestep);
    Rcpp::List getMoveData ();
};

#endif  //
