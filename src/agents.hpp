#ifndef AGENTS_H
#define AGENTS_H

#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <boost/foreach.hpp>
#include "landscape.hpp"

// Agent class
struct Population {
public:
    Population(const int popsize, const float range_agents,
    const float range_food, const int handling_time,
    const int nInfected,
    const float pTransmit) :
        // agents and traits
        nAgents (popsize),
        coordX (popsize, 0.0),
        coordY (popsize, 0.0),
        energy (popsize, 0.001),
        coef_nbrs (popsize, 1.f),
        coef_food (popsize, 1.f),
        
        // count stationary behaviour
        counter (popsize, 0),
        // associations
        associations(popsize, 0),
        degree(popsize, 0),
        range_agents(range_agents),
        range_food(range_food),
        handling_time(handling_time),
        order(popsize, 1),
        infected(popsize, false),
        nInfected (nInfected),
        pTransmit (pTransmit)

    {}
    ~Population() {}

    const int nAgents;
    std::vector<float> coordX;
    std::vector<float> coordY;
    std::vector<float> energy;
    std::vector<float> coef_nbrs;
    std::vector<float> coef_food;
    std::vector<int> counter;
    std::vector<int> associations; // number of total interactions
    std::vector<int> degree;
    const float range_agents, range_food;
    const int handling_time;
    std::vector<int> order;
    std::vector<bool> infected;
    const int nInfected;
    const float pTransmit;

    // position rtree
    bgi::rtree< value, bgi::quadratic<16> > agentRtree;

    // funs for pop
    void shufflePop();
    void setTrait ();
    void initPos(Resources food);

    void updateRtree();
    std::pair<int, std::vector<int> > countNearby (
        bgi::rtree< value, bgi::quadratic<16> > treeToQuery,
        size_t id, float sRange,
        const float xloc, const float yloc);

    void move(Resources food);
    void forage(Resources &food);
    
    std::vector<float> handleFitness();
    void Reproduce();
    
    // for network
    // void updatePbsn(Network &pbsn, const double range);
};

float get_distance(float x1, float x2, float y1, float y2);

#endif // AGENTS_H
