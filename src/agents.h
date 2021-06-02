#ifndef AGENTS_H
#define AGENTS_H

#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <boost/foreach.hpp>
#include "parameters.h"
#include "landscape.h"
#include "network.h"

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
    void setTraitBimodal (const double maxAct, const double ratio, const double proportion);
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
    void competitionCosts(const double competitionCost);
    void updateRtree();
    int countNeighbours (size_t id, const double sensoryRange);
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
        for(size_t j = 0; j < traitMatrix.size(); j++) {
            traitMatrix[i][j] = agent_ran_trait(rng);
        }
    }

void Population::setTraitBimodal(const double maxAct, const double ratio, const double proportion) {
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
        if(is_inactive(rng)) {
            trait_4[z] = ratio * maxAct;
        }
        if(is_inactive(rng)) {
            trait_5[z] = ratio * maxAct;
        }
        if(is_inactive(rng)) {
            trait_6[z] = ratio * maxAct;
        }
    }
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
// void Population::updatePbsn(Network &pbsn, const double range) {

//     updateRtree();

//     // focal agents
//     for(size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
//         // make vector of proximate agents
//         // move j along the size of associations expected for i
//         // returns the upper right triangle
//         // no problems for now with the simple network measures required here
//         // but may become an issue later
//         for(size_t j = i; j < pbsn.associations[i].size(); j++) {

//             if(distanceAgents(coordX[i], coordY[i], coordX[j], coordY[j]) < range) {
//                 pbsn.associations[i][j]++;
//                 pbsn.adjacencyMatrix (i, j) += 1;
//             }
//         }
//     }
// }

void Population::competitionCosts(const double competitionCost) {
    
    // reduce energy by competition cost
    for(int i = 0; i < nAgents; i++) {
        energy[i] -= (associations[i] * competitionCost);
    }
}

// general function for items or agents within distance
std::pair<int, std::vector<int> > Population::countNearby (
    bgi::rtree< value, bgi::quadratic<16> > treeToQuery,
    size_t id, float sensoryRange) {

    std::vector<int> entityId;
    std::vector<value> nearEntities;
    
    Rcpp::Rcout << "id = " << id << " at " << bg::wkt<point> (point(coordX[id], coordY[id])) << "\n";

    // query for a simple box
    treeToQuery.query(bgi::satisfies([&](value const& v) {
        return bg::distance(v.first, point(coordX[id], coordY[id])) < sensoryRange;}),
        std::back_inserter(nearEntities));

    BOOST_FOREACH(value const& v, nearEntities) {
        Rcpp::Rcout << bg::wkt<point> (v.first) << " - " << v.second << "\n";
        entityId.push_back(v.second);
    }
    associations[id] += nearAgents.size();

    nearEntities.clear();

    Rcpp::Rcout << "near agents = " << entityId.size() << "\n";

    // first element is number of near entities
    // second is the identity of entities
    return std::pair<int, std::vector<int> > {entityId.size(), entityId};
}

/// population movement function
void Population::move(size_t id, Resources food, const double moveCost, float sensoryRange) {

    double distance;
    // count neighbours
    int neighbours = (countNearby(agentRtree, id, sensoryRange)).first;
    // find nearby food
    std::vector<int> theseItems = (countNearby(food.rtree, id, sensoryRange)).second;

    // count available and not
    int near_food_avail;
    int near_food_latent;
    for (size_t i = 0; i < theseItems.size(); i++)
    {
        if (food.available[theseItems[i]]) near_food_avail++;
        else near_food_latent++;
    }
    // get distance as a resource selection function
    distance = (traitMatrix[id][1] * near_food_avail) + (traitMatrix[id][2] * neighbours) + traitMatrix[id][3];

    double heading;
    heading = (traitMatrix[id][4] * near_food_avail) + (traitMatrix[id][5] * neighbours) + traitMatrix[id][6];
    heading = heading * M_PI / 180.0;

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
                theta += (M_PI * 2.0);
            heading = theta;
        }
    }

    // figure out the next position
    coordX[id] = coordX[id] + (distance * std::cos(heading));
    coordY[id] = coordY[id] + (distance * std::sin(heading));

    // wrap agents
    if((coordX[id] > food.dSize) | (coordX[id] < 0.0)) coordX[id] = fabs(fmod(coordX[id], food.dSize));
    if((coordY[id] > food.dSize) | (coordY[id] < 0.0)) coordY[id] = fabs(fmod(coordY[id], food.dSize));

    // add a cost
    energy[id] -= (distance * moveCost);
}

void Population::forage(size_t id, Resources &food, float sensoryRange, const int stopTime){
    // find nearest item ids
    std::vector<int> theseItems = (countNearby(food.rtree, id, sensoryRange)).second;

    int thisItem;

    // check near items count
    if(theseItems.size() > 0) {
        // now check them
        for (size_t i = 0; i < theseItems.size(); i++){
            if(food.available[theseItems[i]]) {
                thisItem = theseItems[i]; // if available pick this item
                break;
            }
        }

        // check selected item is available
        assert(food.available[thisItem] && "forage error: item not available");
        counter[id] = stopTime;
        energy[id] += 10.0;

        // remove the food item from the landscape
        food.available[thisItem] = false;
    }
}

// DataFrame returnPbsn (Population &pop, Network &pbsn) {

//     std::vector<int> focalAgent;
//     std::vector<int> subfocalAgent;
//     std::vector<int> pbsnAssociations;

//     // focal agents
//     for(size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++) {
//         // make vector of proximate agents
//         // move j along the size of associations expected for i
//         for(size_t j = i; j < pbsn.associations[i].size(); j++) {
//             // if(pbsn.associations[i][j] > 0) {
//             focalAgent.push_back(i);
//             subfocalAgent.push_back(j);
//             pbsnAssociations.push_back(pbsn.associations[i][j]);

//         }
//     }

//     DataFrame pbsnData = DataFrame::create(
//                 Named("id_x") = focalAgent,
//                 Named("id_y") = subfocalAgent,
//                 Named("associations") = pbsnAssociations
//             );

//     return pbsnData;
// }

/// minor function to normalise vector
std::vector<double> Population::normaliseIntake() {
    // sort vec fitness
    std::vector<double> vecFitness = energy;
    std::sort(vecFitness.begin(), vecFitness.end());
    // scale to max fitness
    double maxFitness = vecFitness[vecFitness.size()-1];
    double minFitness = vecFitness[0];
    // rescale
    for(size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        vecFitness[i] = (vecFitness[i] + fabs(minFitness)) / maxFitness;
    }

    return vecFitness;
}

// mutation probability and size distribution
std::bernoulli_distribution mutation_happens(mProb);
std::normal_distribution<float> mutation_size(0.0, mShift);

// fun for replication
void Population::Reproduce() {
    //normalise intake
    std::vector<double> vecFitness = normaliseIntake();

    // set up weighted lottery
    std::discrete_distribution<> weightedLottery(vecFitness.begin(), vecFitness.end());

    // get parent trait based on weighted lottery
    std::vector<double> newTrait_1;
    std::vector<double> newTrait_2;
    std::vector<double> newTrait_3;
    std::vector<double> newTrait_4;
    std::vector<double> newTrait_5;
    std::vector<double> newTrait_6;
    for (size_t a = 0; static_cast<int>(a) < nAgents; a++) {
        newTrait_1.push_back(
                    trait_1[static_cast<size_t>(weightedLottery(rng))]);
        newTrait_2.push_back(
                    trait_2[static_cast<size_t>(weightedLottery(rng))]);
        newTrait_3.push_back(
                    trait_3[static_cast<size_t>(weightedLottery(rng))]);
        newTrait_4.push_back(
                    trait_4[static_cast<size_t>(weightedLottery(rng))]);
        newTrait_5.push_back(
                    trait_5[static_cast<size_t>(weightedLottery(rng))]);
        newTrait_6.push_back(
                    trait_6[static_cast<size_t>(weightedLottery(rng))]);
    }
    // reset counter
    assert(newTrait_1.size() == trait_1.size() && "traits different size");
    counter = std::vector<int> (nAgents, 0);
    assert(static_cast<int>(counter.size()) == nAgents && "counter size wrong");

    // mutate trait: trait shifts up or down with an equal prob
    // trait mutation prob is mProb, in a two step process
    for (size_t a = 0; static_cast<int>(a) < nAgents; a++) {
        // weight for available food
        if (mutation_happens(rng)) {
            // mutation set, now increase or decrease
            newTrait_1[a] = trait_1[a] + mutation_size(rng);
        }
        // weight for unavailable food
        if (mutation_happens(rng)) {
            // mutation set, now increase or decrease
            newTrait_2[a] = trait_2[a] + mutation_size(rng);
        }
        // weight for agents
        if (mutation_happens(rng)) {
            // mutation set, now increase or decrease
            newTrait_3[a] = trait_3[a] + mutation_size(rng);
        }
        // angle weight for food avail
        if (mutation_happens(rng)) {
            // mutation set, now increase or decrease
            newTrait_4[a] = trait_4[a] + mutation_size(rng);
        }
        // angle weight for food n avail
        if (mutation_happens(rng)) {
            // mutation set, now increase or decrease
            newTrait_5[a] = trait_5[a] + mutation_size(rng);
        }
        // angle weight for agents
        if (mutation_happens(rng)) {
            // mutation set, now increase or decrease
            newTrait_6[a] = trait_6[a] + mutation_size(rng);
        }
    }
    // swap vectors
    std::swap(trait_1, newTrait_1);
    std::swap(trait_2, newTrait_2);
    std::swap(trait_3, newTrait_3);
    std::swap(trait_4, newTrait_4);
    std::swap(trait_5, newTrait_5);
    std::swap(trait_6, newTrait_6);
    newTrait_1.clear(); newTrait_2.clear(); newTrait_3.clear();
    newTrait_4.clear(); newTrait_5.clear(); newTrait_6.clear();

    // swap energy
    std::vector<double> tmpEnergy (nAgents, 10.0);
    std::swap(energy, tmpEnergy);
    tmpEnergy.clear();
}

#endif // AGENTS_H
