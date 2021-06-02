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

void Population::initPos(Resources food) {
    std::uniform_real_distribution<double> agent_ran_pos(0.0, food.dSize);
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        coordX[i] = agent_ran_pos(rng);
        coordY[i] = agent_ran_pos(rng);
    }
    updateRtree();
}

void Population::setTrait() {
    std::uniform_real_distribution<double> agent_ran_trait(0.0, 1.0);
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        for(size_t j = 0; j < traitMatrix.size(); j++) {
            traitMatrix[i][j] = agent_ran_trait(rng);
        }
    }
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
    for (size_t i = 0; i < theseItems.size(); i++)
    {
        if (food.available[theseItems[i]]) near_food_avail++;
    }
    // get distance as a resource selection function
    distance = (traitMatrix[id][1] * near_food_avail) + (traitMatrix[id][2] * neighbours) + traitMatrix[id][3];

    double heading;
    heading = (traitMatrix[id][4] * near_food_avail) + (traitMatrix[id][5] * neighbours) + traitMatrix[id][6];
    heading = heading * M_PI / 180.0;

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
    std::vector<std::vector<float> > tmpTraitMatrix (6, std::vector<float> (nAgents, 0.f));
    for (int a = 0; a < traitMatrix.size(); a++) {
        for (int j = 0; j < nAgents; ++j)
        {
            tmpTraitMatrix[a][j] = traitMatrix[a][static_cast<size_t>(weightedLottery(rng))];
        }
    }
    // check trait matrix size
    assert(tmpTraitMatrix[0].size() == traitMatrix[0].size() && "trait matrices different size");
    
    // reset counter
    counter = std::vector<int> (nAgents, 0);
    assert(static_cast<int>(counter.size()) == nAgents && "counter size wrong");

    // mutate trait: trait shifts up or down with an equal prob
    // trait mutation prob is mProb, in a two step process
    for (size_t a = 0; a < tmpTraitMatrix[a].size(); a++) {
        for (int i = 0; i < nAgents; ++i)
        {
            if(mutation_happens(rng)) {
                tmpTraitMatrix[a][i] = tmpTraitMatrix[a][i] + mutation_size(rng);
            }
        }
    }

    // swap trait matrices
    std::swap(traitMatrix, tmpTraitMatrix);
    tmpTraitMatrix.clear();
    // swap energy
    std::vector<double> tmpEnergy (nAgents, 10.0);
    std::swap(energy, tmpEnergy);
    tmpEnergy.clear();
}

#endif // AGENTS_H
