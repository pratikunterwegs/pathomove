#ifndef AGENTS_H
#define AGENTS_H

#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <boost/foreach.hpp>
#include "landscape.h"

// Agent class
struct Population {
public:
    Population(const int popsize, const double beginTrait) :
        nAgents (popsize),
        coordX (popsize, 0.0),
        coordY (popsize, 0.0),
        energy (popsize, 1.0),
        // trait matrix -- 6 rows, n agent traits
        traitMatrix (6, std::vector<float>(popsize)),
        // count stationary behaviour
        counter (popsize, 0),
        // associations
        associations(popsize, 0),
        degree(popsize, 0)

    {}
    ~Population() {}

    int nAgents = 0;
    std::vector<double> coordX;
    std::vector<double> coordY;
    std::vector<double> energy;
    std::vector<std::vector<float> > traitMatrix;
    std::vector<int> counter;
    std::vector<int> associations; // number of total interactions
    std::vector<int> degree;

    // position rtree
    bgi::rtree< value, bgi::quadratic<16> > agentRtree;

    // funs for pop
    void initPop (int popsize);
    void setTrait ();
    void initPos(Resources food);

    void competitionCosts(const double competitionCost);
    void updateRtree();
    std::pair<int, std::vector<int> > countNearby (
        bgi::rtree< value, bgi::quadratic<16> > treeToQuery,
        size_t id, float sensoryRange);

    void move(size_t id, Resources food, const double moveCost, float sensoryRange);
    void forage(size_t individual, Resources &food, float distance, const int stopTime);
    
    std::vector<double> normaliseIntake();
    void Reproduce();
    
    // for network
    // void updatePbsn(Network &pbsn, const double range);
};

float get_distance(float x1, float x2);

#endif // AGENTS_H
