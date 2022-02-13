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

    genData(const int genmax, const int popsize, const int increment) :
        genmax (genmax),
        increment (increment),
        gSampled (genmax / increment),
        // generation wise dataframe
        // energy
        gIntake (gSampled, std::vector<float>(popsize, 0.f)),
        gEnergy (gSampled, std::vector<float>(popsize, 0.f)),
        // weights
        gSF (gSampled, std::vector<float>(popsize, 0.f)),
        gSH (gSampled, std::vector<float>(popsize, 0.f)),
        gSN (gSampled, std::vector<float>(popsize, 0.f)),
        // initial positions
        gX (gSampled, std::vector<float>(popsize, 0.f)),
        gY (gSampled, std::vector<float>(popsize, 0.f)),
        gXn (gSampled, std::vector<float>(popsize, 0.f)),
        gYn (gSampled, std::vector<float>(popsize, 0.f)),
        // ecological interactions
        gAssoc (gSampled, std::vector<int>(popsize, 0.f)),
        gTInfected (gSampled, std::vector<int>(popsize, 0.f)),
        // gDegree (gSampled, std::vector<int>(popsize, 0.f)),
        gMoved (gSampled, std::vector<float>(popsize, 0.f)),
        // generation specific data
        gNInfected (gSampled, 0),
        gens (gSampled, 0)//,
        // gPbsnDiameter (gSampled, 0.f),
        // gPbsnGlobEff (gSampled, 0.f)
    {}
    ~genData() {}

    const int genmax;
    const int increment;
    const int gSampled;
    std::vector<std::vector<float> > gIntake;
    std::vector<std::vector<float> > gEnergy;
    std::vector<std::vector<float> > gSF;
    std::vector<std::vector<float> > gSH;
    std::vector<std::vector<float> > gSN;

    std::vector<std::vector<float> > gX;
    std::vector<std::vector<float> > gY;
    std::vector<std::vector<float> > gXn;
    std::vector<std::vector<float> > gYn;

    std::vector<std::vector<int> > gAssoc;
    std::vector<std::vector<int> > gTInfected;
    // std::vector<std::vector<int> > gDegree;
    std::vector<std::vector<float> > gMoved;

    std::vector<int> gNInfected;
    std::vector<int> gens;

    // network metrics
    // std::vector<float> gPbsnDiameter;
    // std::vector<float> gPbsnGlobEff;

    void updateGenData (Population &pop, const int g_);
    Rcpp::List getGenData ();
};

struct moveData {
public:

    moveData(const int tmax, const int popsize) :
        tmax(tmax),
        popsize(popsize),
        timesteps(tmax, std::vector<int>(popsize, 0)),
        x(tmax, std::vector<float>(popsize, 0)),
        y(tmax, std::vector<float>(popsize, 0))
    {}
    ~moveData() {}

    const int tmax;
    const int popsize;
    std::vector<std::vector<int> > timesteps;
    std::vector<std::vector<float> > x;
    std::vector<std::vector<float> > y;

    void updateMoveData (Population &pop, const int t_);
    Rcpp::List getMoveData();
};

#endif  //
