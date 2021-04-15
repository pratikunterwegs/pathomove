#ifndef DATA_TYPES_H
#define DATA_TYPES_H
/// data types and related functions
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "parameters.h"
#include "landscape.h"
#include "agents.h"
#include "network.h"
#include "network_operations.hpp"

// define a struct holding a vector of data frames which holds generation wise data
struct genData {
public:
    std::vector<std::vector<double> > genEnergyVec;
    std::vector<std::vector<double> > genTraitVec;
    std::vector<std::vector<int> > genAssocVec;
    std::vector<std::vector<int> > genDegreeVec;
    std::vector<int> gens;

    void updateGenData (Population &pop, const int gen);
    Rcpp::List getGenData ();
};

struct networkData {
public:
    std::vector<int> interactions;
    std::vector<double> diameter;
    std::vector<double> global_efficiency;
    std::vector<int> gens;

    void updateNetworkData(Population &pop, const int gen, Network &pbsn);
    Rcpp::DataFrame getNetworkData();
};

struct moveData {
public:
    std::vector<std::vector<int> > id;
    std::vector<std::vector<double> > trait;
    std::vector<std::vector<double> > coordX;
    std::vector<std::vector<double> > coordY;
    std::vector<std::vector<double> > energy;
    std::vector<int> timestep;

    void updateMoveData (Population &pop, const int timestep);
    Rcpp::List getMoveData ();
};

// function to update move
void moveData::updateMoveData (Population &pop, const int timestep_) {
    // get pop data
    std::vector<int> idVec (pop.nAgents);
    for (size_t i = 0; i < pop.nAgents; i++) {
        idVec[i] = i;
    }

    id.push_back(idVec);
    trait.push_back(pop.trait);
    coordX.push_back(pop.coordX);
    coordY.push_back(pop.coordY);
    energy.push_back(pop.energy);
    timestep.push_back(timestep_);
}

// function to return gen data as an rcpp list
Rcpp::List moveData::getMoveData() {
    Rcpp::List moveDataList (timestep.size());
    for (size_t i = 0; i < timestep.size(); i++)
    {
        moveDataList[i] = DataFrame::create(
            Named("id") = id[i],
            Named("energy") = energy[i],
            Named("x") = coordX[i],
            Named("Y") = coordY[i],
            Named("trait") = trait[i]
        );
    }
    List dataToReturn = List::create(
        Named("move_data") = moveDataList,
        Named("timestep") = timestep
    );

    return dataToReturn;
}

// function to update gendata
void genData::updateGenData (Population &pop, const int gen_) {
    // get pop data
    genEnergyVec.push_back(pop.energy);
    genTraitVec.push_back(pop.trait);
    genAssocVec.push_back(pop.associations);
    genDegreeVec.push_back(pop.degree);
    gens.push_back(gen_);
}

void networkData::updateNetworkData(Population &pop, const int gen, Network &pbsn) {

    std::vector<double> tmpNetworkData = networkMeasures(pbsn, pop);

    assert (tmpNetworkData.size() == 3 && "wrong size network measures");

    interactions.push_back(tmpNetworkData[0]);
    diameter.push_back(tmpNetworkData[1]);
    global_efficiency.push_back(tmpNetworkData[2]);
    gens.push_back(gen);
}

// function to return gen data as an rcpp list
Rcpp::List genData::getGenData() {
    Rcpp::List genDataList (gens.size());
    for (size_t i = 0; i < gens.size(); i++)
    {
        genDataList[i] = DataFrame::create(
            Named("energy") = genEnergyVec[i],
            Named("trait") = genTraitVec[i],
            Named("associations") = genAssocVec[i],
            Named("degree") = genDegreeVec[i]
        );
    }
    List dataToReturn = List::create(
        Named("pop_data") = genDataList,
        Named("gens") = gens
    );

    return dataToReturn;
}

// function to return network data as a DataFrame
Rcpp::DataFrame networkData::getNetworkData() {
    Rcpp::DataFrame networkData = Rcpp::DataFrame::create(
                Named("gen") = gens,
                Named("interactions") = interactions,
                Named("diameter") = diameter,
                Named("global_efficiency") = global_efficiency
            );

    return networkData;
}

#endif  //
