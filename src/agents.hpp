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
#include "network.hpp"

// Agent class
struct Population {
public:
    Population(const int popsize, const float range_agents,
    const float range_food, const int handling_time,
    float pTransmit) :
        // agents, positions, energy and traits
        nAgents (popsize),
        coordX (popsize, 0.0f),
        coordY (popsize, 0.0f),
        energy (popsize, 0.001),
        sF (popsize, 0.f),
        sH (popsize, 0.f),
        sN (popsize, 0.f),
        
        // counters for handling and social metrics
        counter (popsize, 0),
        associations(popsize, 0),
        degree(popsize, 0),

        // agent sensory parameters
        n_samples (3.f),
        range_agents(range_agents),
        range_food(range_food),
        handling_time(handling_time),

        // vectors for agent order, infection status, time infected
        order(popsize, 1),
        infected(popsize, false),//,
        timeInfected(popsize, 0),

        // disease parameters and total pop infected
        pTransmit (pTransmit),
        nInfected(0),

        // infection source and distance moved
        srcInfect(popsize, 0),
        moved(popsize, 0.f),

        // a network object
        pbsn(popsize)
    {}
    ~Population() {}

    // agent count, coords, and energy
    const int nAgents;
    std::vector<float> coordX;
    std::vector<float> coordY;
    std::vector<float> energy;
    // weights
    std::vector<float> sF;
    std::vector<float> sH;
    std::vector<float> sN;

    // counter and metrics
    std::vector<int> counter;
    std::vector<int> associations; // number of total interactions
    std::vector<int> degree;

    // sensory range and foraging
    const float n_samples, range_agents, range_food;
    const int handling_time;

    // shuffle vector and transmission
    std::vector<int> order;
    std::vector<bool> infected;
    std::vector<int> timeInfected;
    float pTransmit;

    // the number of infected agents
    int nInfected;
    std::vector<int> srcInfect;

    // movement distances
    std::vector<float> moved;

    // position rtree
    bgi::rtree< value, bgi::quadratic<16> > agentRtree;

    // network object
    Network pbsn;

    /// functions for the population ///
    // population order, trait and position randomiser
    void shufflePop();
    void setTrait ();
    void initPos(Resources food);

    // make rtree and get nearest agents and food
    void updateRtree();

    int countFood (
        Resources &food, const float xloc, const float yloc);
    
    std::vector<int> getFoodId (
        Resources &food,
        const float xloc, const float yloc
    );
    
    std::pair<int, int > countAgents (
        const float xloc, const float yloc);
    
    std::vector<int> getNeighbourId (
        const float xloc, const float yloc
    );

    // functions to move and forage on a landscape
    void move(Resources &food, const int nThreads);
    void forage(Resources &food, const int nThreads);
    
    // funs to handle fitness and reproduce
    std::vector<float> handleFitness();
    void Reproduce();
    
    // pathogen dynamics -- initial infections, spread, and costs
    void introducePathogen(const int nAgInf);
    void pathogenSpread();
    void pathogenCost(const float costInfect);

    // count infected agents, infection source
    void countInfected();
    float propSrcInfection();

    // counting proximity based interactions
    void countAssoc();

    // functions for the network
    // there is no function to update the network, this is handled in countAssoc
};

// a dinky function for distance and passed to catch test
float get_distance(float x1, float x2, float y1, float y2);

#endif // AGENTS_H
