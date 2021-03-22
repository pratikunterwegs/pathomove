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

#include <Rcpp.h>

using namespace Rcpp;

// define a struct holding a vector of data frames which holds generation wise data
struct genData {
public:
    genData():
        std::vector<Rcpp::DataFrame> genDataVec,
        std:::vector<int> gen
    {}
    ~genData() {}

    void updateGenData (Population &pop, const int gen);
};

// function to update gendata
void genData::updateGenData (Population &pop, const int gen) {

    // get pop data
    // create data frame of evolved traits and return
    DataFrame pop_data = DataFrame::create(
        Named("energy") = pop.energy,
        Named("trait") = pop.trait
    );

    genDataVec.push_back(df_evolved_pop);
    gen.push_back(gen);
}