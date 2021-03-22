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

// define a struct holding a vector of data frames which holds generation wise data
struct genData {
public:
    std::vector<std::vector<double> > genEnergyVec;
    std::vector<std::vector<double> > genTraitVec;
    std::vector<int> gens;

    void updateGenData (Population &pop, const int gen);
    Rcpp::List getGenData ();
};

// function to update gendata
void genData::updateGenData (Population &pop, const int gen_) {
    // get pop data
    genEnergyVec.push_back(pop.energy);
    genTraitVec.push_back(pop.trait);
    gens.push_back(gen_);
}

// function to return gen data as an rcpp list
Rcpp::List genData::getGenData() {
    Rcpp::List genDataList (gens.size());
    for (size_t i = 0; i < gens.size(); i++)
    {
        genDataList[i] = DataFrame::create(
            Named("energy") = genEnergyVec[i],
            Named("trait") = genTraitVec[i]
        );
    }
    List dataToReturn = List::create(
        Named("pop_data") = genDataList,
        Named("gens") = gens
    );

    return dataToReturn;
}
