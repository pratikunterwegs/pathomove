#ifndef AGENTS_H
#define AGENTS_H

#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include "parameters.h"
#include "landscape.h"
#include "network.h"

// Agent class
struct Population {
public:
    Population(const int popsize, const double beginTrait) :
        nAgents (popsize),
        coordX (popsize, 50.0),
        coordY (popsize, 50.0),
        energy (popsize, 0.000001),
        // one trait
        trait_1(popsize, beginTrait),
        trait_2(popsize, beginTrait),
        trait_3(popsize, beginTrait),
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
    std::vector<double> trait_1;
    std::vector<double> trait_2;
    std::vector<double> trait_3;
    std::vector<double> counter;
    std::vector<int> associations; // number of total interactions
    std::vector<int> degree;

    // position rtree
    bgi::rtree< value, bgi::quadratic<16> > agentRtree;

    // funs for pop
    void initPop (int popsize);
    void setTrait ();
    void setTraitBimodal (const double maxAct, const double ratio, const double proportion);
    void initPos(Resources food);
    void move(size_t id, Resources food, const double moveCost, const bool collective,
              const double sensoryRange);
    std::vector<int> findNearItems(size_t individual, Resources &food, const double distance);
    void forage(size_t individual, Resources &food, const double distance, const int stopTime);
    void normaliseIntake();
    void Reproduce();
    // for network
    void updatePbsn(Network &pbsn, const double range);
    void competitionCosts(const double competitionCost);
    void updateRtree();
    void countNeighbours (size_t id, const double sensoryRange);
};

void Population::initPos(Resources food) {
    std::uniform_real_distribution<double> agent_ran_pos(0.0, food.dSize);
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        coordX[i] = agent_ran_pos(rng);
        coordY[i] = agent_ran_pos(rng);
    }
}

void Population::setTrait() {
    std::uniform_real_distribution<double> agent_ran_trait(0.0, 1.0);
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        trait_1[i] = agent_ran_trait(rng);
        trait_2[i] = agent_ran_trait(rng);
        trait_3[i] = agent_ran_trait(rng);
    }
}

void Population::setTraitBimodal(const double maxAct, const double ratio, const double proportion) {
    trait = std::vector<double> (nAgents, maxAct);
    std::bernoulli_distribution is_inactive(proportion);
    for (int z = 0; z < nAgents; z++)
    {
        if(is_inactive(rng)) {
            trait_1[z] = ratio * maxAct;
        }
        if(is_inactive(rng)) {
            trait_2[z] = ratio * maxAct;
        }
        if(is_inactive(rng)) {
            trait_3[z] = ratio * maxAct;
        }
    }
}

// distance function without wrapping
double wrappedDistanceAgents(double x1, double y1, double x2, double y2, double landsize) {

    double distanceX = fabs( fmod( (x1 - x2), landsize ) );
    double distanceY = fabs( fmod( (y1 - y2), landsize ) );

    double wrD = std::sqrt( (distanceX * distanceX) + (distanceY * distanceY) );

    return wrD;
}

// distance without wrapping
double distanceAgents(double x1, double y1, double x2, double y2) {

    double distanceX = x1 - x2;
    double distanceY = y1 - y2;

    double wrD = std::sqrt( (distanceX * distanceX) + (distanceY * distanceY) );

    return wrD;
}

// to update agent Rtree
void Population::updateRtree () {
    // initialise rtree
    bgi::rtree< value, bgi::quadratic<16> > tmpRtree;
    for (int i = 0; i < nAgents; ++i)
    {
        point p = point(coordX[i], coordY[i]);
        tmpRtree.insert(std::make_pair(p, i));
    }
    std::swap(agentRtree, tmpRtree);
    tmpRtree.clear();
}

// to update pbsn
void Population::updatePbsn(Network &pbsn, const double range) {

    updateRtree();

    // focal agents
    for(size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        // make vector of proximate agents
        // move j along the size of associations expected for i
        // returns the upper right triangle
        // no problems for now with the simple network measures required here
        // but may become an issue later
        for(size_t j = i; j < pbsn.associations[i].size(); j++) {

            if(distanceAgents(coordX[i], coordY[i], coordX[j], coordY[j]) < range) {
                pbsn.associations[i][j]++;
                pbsn.adjacencyMatrix (i, j) += 1;
            }
        }
    }
}

void Population::competitionCosts(const double competitionCost) {
    
    // reduce energy by competition cost
    for(int i = 0; i < nAgents; i++) {
        energy[i] -= (associations[i] * competitionCost);
    }
}

// function for wrapped distance agents using rtree
double wrappedDistance(bg::model::point<float, 2, bg::cs::cartesian> rTreeLoc,
                       double queryX, double queryY, double landsize) {
    double rtreeX = rTreeLoc.get<0>();
    double rtreeY = rTreeLoc.get<1>();

    double distanceX = fabs( fmod( (rtreeX - queryX), landsize ) );
    double distanceY = fabs( fmod( (rtreeY - queryY), landsize ) );

    double wrD = std::sqrt( (distanceX * distanceX) + (distanceY * distanceY) );

    return wrD;
}

// angle distribution
std::uniform_real_distribution<double> agent_move_angle(-90.0, 90.0);

/// population movement function
void Population::move(size_t id, Resources food, const double moveCost,
                      const bool collective, const double sensoryRange) {

    double heading;
    heading = agent_move_angle(rng);
    // get radians
    heading = heading * M_PI / 180.0;

    double distance;
    // count neighbours
    int neighbours = countNeighbours(id, sensoryRange);
    // find nearby food
    std::vector<int> theseItems = findNearItems(individual, food, distance);

    // count available and not
    int near_food_avail;
    int near_food_latent;
    for (size_t i = 0; i < theseItems.size(); i++)
    {
        if (food.available[theseItems[i]]) near_food_avail++;
        else near_food_latent++;
    }

    // get distance as a resource selection function
    distance = (trait_1 * near_food_avail) + (trait_2 * near_food_latent) + (trait_3 * neighbours);

    // if collective, move towards a random agent (the first) within range
    if (collective) {
        updateRtree();
        std::vector<int> agentId;
        std::vector<value> nearAgents;
        box bbox(point(coordX[id] - sensoryRange,
                       coordY[id] - sensoryRange),
                 point(coordX[id] + sensoryRange, coordY[id] + sensoryRange));
        agentRtree.query(
                    bgi::within(bbox) &&
                    bgi::satisfies([&](value const& v) {return bg::distance(v.first, point(coordX[id],
                                                        coordY[id])) < sensoryRange;}),
                std::back_inserter(nearAgents));
        
        if (nearAgents.size() > 0) {
            size_t neighbour = nearAgents[0].second;
            // static const double RAD2DEG = 57.2957795130823209;
            // if (a1 = b1 and a2 = b2) throw an error
            double theta = atan2(coordX[id] - coordX[neighbour],
                                 coordY[id] - coordY[neighbour]);
            if (theta < 0.0)
                theta += TWOPI;
            heading = theta;
        }
    }

    // figure out the next position
    coordX[id] = coordX[id] + (sensoryRange * std::cos(heading));
    coordY[id] = coordY[id] + (sensoryRange * std::sin(heading));

    // bounce agents off the landscape limits
    coordX[id] = (coordX[id] > food.dSize) ? (food.dSize - (food.dSize / 100.0)) : coordX[id];
    coordY[id] = (coordY[id] > food.dSize) ? (food.dSize - (food.dSize / 100.0)) : coordY[id];

    // add a cost
    energy[id] -= (sensoryRange * moveCost);
}

// check neighbours
void Population::countNeighbours (size_t id,
                                  const double sensoryRange) {
    updateRtree();
    std::vector<int> agentId;
    std::vector<value> nearAgents;
    box bbox(point(coordX[id] - sensoryRange,
                   coordY[id] - sensoryRange),
             point(coordX[id] + sensoryRange, coordY[id] + sensoryRange));
    agentRtree.query(
                bgi::within(bbox) &&
                bgi::satisfies([&](value const& v) {return bg::distance(v.first, point(coordX[id], coordY[id]))
                                                    < sensoryRange;}),
            std::back_inserter(nearAgents));
    associations[id] += nearAgents.size();
}

std::vector<int> Population::findNearItems(size_t individual, Resources &food, 
                               const double distance){
    // search nearest item only if any are available
    std::vector<int> itemID;

    if (food.nAvailable > 0) {
        std::vector<value> nearItems;
        box bbox(point(coordX[individual] - distance,
                       coordY[individual] - distance),
                 point(coordX[individual] + distance, coordY[individual] + distance));

        food.rtree.query(
                    bgi::within(bbox) &&
                    bgi::satisfies([&](value const& v) {return bg::distance(v.first, point(coordX[individual],                                      coordY[individual])) < distance;}),
                std::back_inserter(nearItems));

        for(size_t i = 0; i < nearItems.size(); i++){
            itemID.push_back(nearItems[i].second); // store item ids
        }
    }
    return itemID;
}

void Population::forage(size_t individual, Resources &food, const double distance, const int stopTime){
    // find nearest item ids
    std::vector<int> theseItems = findNearItems(individual, food, distance);

    // check near items count
    if(theseItems.size() > 0) {
        // which to pick
        int thisItem = -1;

        // now check them
        for (size_t i = 0; i < theseItems.size(); i++){
            if(food.available[i]) {
                thisItem = theseItems[i]; // if available pick this item
                break;
            }
        }

        // if item available then consume it
        // also stop the agent here for as many steps as its trait determines
        if (thisItem > -1) {
            counter[individual] = stopTime;
            energy[individual] += foodEnergy;

            // agent moves to where item was
            coordX[individual] = food.coordX[thisItem];
            coordY[individual] = food.coordY[thisItem];

            // remove the food item from the landscape for a brief time
            food.available[thisItem] = false;
        }
    }
}

DataFrame returnPbsn (Population &pop, Network &pbsn) {

    std::vector<int> focalAgent;
    std::vector<int> subfocalAgent;
    std::vector<int> pbsnAssociations;

    // focal agents
    for(size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++) {
        // make vector of proximate agents
        // move j along the size of associations expected for i
        for(size_t j = i; j < pbsn.associations[i].size(); j++) {
            // if(pbsn.associations[i][j] > 0) {
            focalAgent.push_back(i);
            subfocalAgent.push_back(j);
            pbsnAssociations.push_back(pbsn.associations[i][j]);

        }
    }

    DataFrame pbsnData = DataFrame::create(
                Named("id_x") = focalAgent,
                Named("id_y") = subfocalAgent,
                Named("associations") = pbsnAssociations
            );

    return pbsnData;
}

/// minor function to normalise vector
void Population::normaliseIntake() {
    // deal with negatives
    for(size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        if (energy[i] < 0.000001) {
            energy[i] = 0.000001;
        } else {
            energy[i] += 0.000001;
        }
    }
    // sort vec fitness
    std::vector<double> vecFitness = energy;
    std::sort(vecFitness.begin(), vecFitness.end());
    // scale to max fitness
    double maxFitness = vecFitness[vecFitness.size()-1];
    // rescale
    for(size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        energy[i] = energy[i] / maxFitness;
    }
}

// mutation probability and size distribution
std::bernoulli_distribution mutation_happens(mProb);
std::gamma_distribution<double> mutation_size(0.01, mShift);

// fun for replication
void Population::Reproduce() {
    //normalise intake
    normaliseIntake();

    // set up weighted lottery
    std::discrete_distribution<> weightedLottery(energy.begin(), energy.end());

    // get parent trait based on weighted lottery
    std::vector<double> newTrait;
    for (size_t a = 0; static_cast<int>(a) < nAgents; a++) {
        newTrait.push_back(
                    trait[static_cast<size_t>(weightedLottery(rng))]);
    }
    // reset counter
    assert(newTrait.size() == trait.size() && "traits different size");
    counter = std::vector<int> (nAgents);
    assert(static_cast<int>(counter.size()) == nAgents && "counter size wrong");

    // mutate trait: trait shifts up or down with an equal prob
    // trait mutation prob is mProb, in a two step process
    for (size_t a = 0; static_cast<int>(a) < nAgents; a++) {
        if (mutation_happens(rng)) {
            // mutation set, now increase or decrease
            newTrait[a] = trait[a] + mutation_size(rng);
            // no negative traits
            if (newTrait[a] < 0) {
                newTrait[a] = 0.0;
            }
        }
    }
    // swap vectors
    std::swap(trait, newTrait);
    newTrait.clear();

    // swap energy
    std::vector<double> tmpEnergy (nAgents, 0.000001);
    std::swap(energy, tmpEnergy);
    tmpEnergy.clear();
}

#endif // AGENTS_H
