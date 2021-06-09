#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <random>
#include <boost/foreach.hpp>
#include "agents.h"

std::cauchy_distribution<double> def_move_angle(0.0, 20.0);

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
    for(size_t i = 0; i < nAgents; i++) {
        coef_food[i] = agent_ran_trait(rng);
        coef_nbrs[i] = agent_ran_trait(rng);
    }
}

float get_distance(float x1, float x2) {
    return fabs(x1 - x2);
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
    
    // std::cout << "id = " << id << " at " << bg::wkt<point> (point(coordX[id], coordY[id])) << "\n";

    // query for a simple box
    treeToQuery.query(bgi::satisfies([&](value const& v) {
        return bg::distance(v.first, point(coordX[id], coordY[id])) < sensoryRange;}),
        std::back_inserter(nearEntities));

    BOOST_FOREACH(value const& v, nearEntities) {
        // std::cout << bg::wkt<point> (v.first) << " - " << v.second << "\n";
        entityId.push_back(v.second);
    }

    nearEntities.clear();

    // std::cout << "near agents = " << entityId.size() << "\n\n";

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
    distance = (coef_food[id] * near_food_avail) + (coef_nbrs[id] * neighbours) + 0.1;

    double heading;
    heading = def_move_angle(rng);
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
    // energy[id] = static_cast<double> (theseItems.size());
    // counter[id] = stopTime;
    int thisItem = -1;

    // check near items count
    if(theseItems.size() > 0) {
        // now check them
        for (size_t i = 0; i < theseItems.size(); i++){
            if(food.available[theseItems[i]]) {
                thisItem = theseItems[i]; // if available pick this item
                break;
            }
        }

        if (thisItem != -1) {
            // check selected item is available
            assert(food.available[thisItem] && "forage error: item not available");
            counter[id] = stopTime;
            energy[id] += 1.0;

            // remove the food item from the landscape
            food.available[thisItem] = false;
        }
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
    std::vector<float> tmp_coef_food (nAgents, 0.f);
    std::vector<float> tmp_coef_nbrs (nAgents, 0.f);
    for (int a = 0; a < nAgents; a++) {
        tmp_coef_nbrs[a] = coef_nbrs[static_cast<size_t>(weightedLottery(rng))];
        tmp_coef_food[a] = coef_food[static_cast<size_t>(weightedLottery(rng))];
    }
    
    // reset counter
    counter = std::vector<int> (nAgents, 0);
    assert(static_cast<int>(counter.size()) == nAgents && "counter size wrong");

    // mutate trait: trait shifts up or down with an equal prob
    // trait mutation prob is mProb, in a two step process
    for (int a = 0; a < nAgents; a++) {
        if(mutation_happens(rng)) {
            tmp_coef_food[a] = tmp_coef_food[a] + mutation_size(rng);
        }
        if(mutation_happens(rng)) {
            tmp_coef_nbrs[a] = tmp_coef_nbrs[a] + mutation_size(rng);
        }
    }

    // swap trait matrices
    coef_food =  tmp_coef_food;
    coef_nbrs = tmp_coef_nbrs;
    tmp_coef_nbrs.clear(); tmp_coef_food.clear();
    // swap energy
    std::vector<double> tmpEnergy (nAgents, 0.001);
    std::swap(energy, tmpEnergy);
    tmpEnergy.clear();
}
