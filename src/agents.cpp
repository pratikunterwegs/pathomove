#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <random>
#include <boost/foreach.hpp>
#include "landscape.hpp"
#include "agents.hpp"

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

// uniform distribution for agent position
std::uniform_real_distribution<float> agent_ran_pos(0.0f, 1.f);

// function for initial positions
void Population::initPos(Resources food) {
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        coordX[i] = agent_ran_pos(rng) * food.dSize;
        coordY[i] = agent_ran_pos(rng) * food.dSize;
    }
    updateRtree();
}

// unifrom distribution for agent trait
std::uniform_real_distribution<float> agent_ran_trait(0.0, 1.0);

// set agent trait
void Population::setTrait() {
    for(size_t i = 0; i < nAgents; i++) {
        coef_food[i] = agent_ran_trait(rng);
        coef_nbrs[i] = agent_ran_trait(rng);
    }
}

float get_distance(float x1, float x2, float y1, float y2) {
    return std::sqrt(std::pow((x1 - x2), 2) + std::pow((y1 - y2), 2));
}

// to update pbsn
// void Population::updatePbsn(Network &pbsn, const float range) {

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

// function for competition costs
void Population::competitionCosts(const float competitionCost) {
    
    // reduce energy by competition cost
    for(int i = 0; i < nAgents; i++) {
        energy[i] -= (associations[i] * competitionCost);
    }
}

// general function for items or agents within distance
std::pair<int, std::vector<int> > Population::countNearby (
    bgi::rtree< value, bgi::quadratic<16> > treeToQuery,
    size_t id, float sensoryRange,
    const float xloc, const float yloc) {

    std::vector<int> entityId;
    std::vector<value> nearEntities;
    
    // std::cout << "id = " << id << " at " << bg::wkt<point> (point(coordX[id], coordY[id])) << "\n";

    // query for a simple box
    treeToQuery.query(bgi::satisfies([&](value const& v) {
        return bg::distance(v.first, point(xloc, yloc)) < sensoryRange;}),
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
void Population::move(size_t id, Resources food, const float moveCost, float sensoryRange) {

    float angle = 0.f;
    float twopi = 2.f * M_PI;
    
    // what increment for 4 samples in a circle around the agent
    float increment = twopi / 4.0f;

    // first assess current location
    float sampleX = coordX[id];
    float sampleY = coordY[id]; 
    float foodHere = static_cast<float>(countNearby(
            food.rtree, id, sensoryRange, sampleX, sampleY
        ).first);
    float nbrsHere = static_cast<float>(countNearby(
            agentRtree, id, sensoryRange, sampleX, sampleY
        ).first);;

    // get suitability current
    float suitabilityHere = (coef_food[id] * foodHere) + (coef_nbrs[id] * nbrsHere);

    // new location is initially current location
    float newX = coordX[id];
    float newY = coordY[id];

    // now sample at four locations around
    for(float theta = 0.f; theta < twopi - increment; theta += increment) {

        sampleX = coordX[id] + (sensoryRange * static_cast<float>(cos(theta)));
        sampleY = coordY[id] + (sensoryRange * static_cast<float>(sin(theta)));

        foodHere = static_cast<float>(countNearby(
            food.rtree, id, sensoryRange, sampleX, sampleY
        ).first);

        nbrsHere = static_cast<float>(countNearby(
            agentRtree, id, sensoryRange, sampleX, sampleY
        ).first);

        float new_suitabilityHere = (coef_food[id] * foodHere) + (coef_nbrs[id] * nbrsHere);

        if (new_suitabilityHere > suitabilityHere) {
            
            newX = sampleX; newY = sampleY;
            suitabilityHere = new_suitabilityHere;
        }
    }

    // crudely wrap movement
    if((newX > food.dSize) | (newX < 0.f)) {
        newX = std::fabs(std::fmod(newX, food.dSize));
    }
    if((newY > food.dSize) | (newY < 0.f)) {
        newY = std::fabs(std::fmod(newY, food.dSize));
    }

    if(get_distance(newX, coordX[id], newY, coordY[id]) > 0.f) {
        energy[id] -= moveCost;
    }
    // set locations
    coordX[id] = newX; coordY[id] = newY;
}

void Population::forage(size_t id, Resources &food, float sensoryRange, const int stopTime){
    // find nearest item ids
    std::vector<int> theseItems = (countNearby(food.rtree, id, sensoryRange, coordX[id], coordY[id])).second;
    // energy[id] = static_cast<float> (theseItems.size());
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
std::vector<float> Population::normaliseIntake() {
    // sort vec fitness
    std::vector<float> vecFitness = energy;
    std::sort(vecFitness.begin(), vecFitness.end());
    // scale to max fitness
    float maxFitness = vecFitness[vecFitness.size()-1];
    float minFitness = vecFitness[0];
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
    std::vector<float> vecFitness = normaliseIntake();

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
    std::vector<float> tmpEnergy (nAgents, 0.001);
    std::swap(energy, tmpEnergy);
    tmpEnergy.clear();
}
