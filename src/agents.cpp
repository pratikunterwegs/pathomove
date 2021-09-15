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

// to shuffle pop id
void Population::shufflePop() {
    if (order[0] == order[nAgents - 1])
    {
        for (size_t i = 0; i < nAgents; i++)
        {
            order[i] = i;
        }
        std::random_shuffle ( order.begin(), order.end() );
    }
    else {
        std::random_shuffle ( order.begin(), order.end() );
    }
    
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
std::uniform_real_distribution<float> agent_ran_trait(-0.001, 0.001);

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

// general function for agents within distance
std::pair<int, std::vector<int> > Population::countAgents (
    const float xloc, const float yloc) {
    
    std::vector<int> agent_id;
    std::vector<value> near_agents;
    // query for a simple box
    agentRtree.query(bgi::satisfies([&](value const& v) {
        return bg::distance(v.first, point(xloc, yloc)) < range_agents;}),
        std::back_inserter(near_agents));

    BOOST_FOREACH(value const& v, near_agents) {
        // std::cout << bg::wkt<point> (v.first) << " - " << v.second << "\n";
        agent_id.push_back(v.second);
    }
    near_agents.clear();
    // first element is number of near entities
    // second is the identity of entities
    return std::pair<int, std::vector<int> > {agent_id.size(), agent_id};
}

// general function for items within distance
std::pair<int, std::vector<int> > Population::countFood (
    Resources &food,
    const float xloc, const float yloc) {
    std::vector<int> food_id;
    std::vector<value> near_food;

    // check any available
    if (food.nAvailable > 0) {
        // query for a simple box
        food.rtree.query(bgi::satisfies([&](value const& v) {
            return bg::distance(v.first, point(xloc, yloc)) < range_food;}),
            std::back_inserter(near_food));

        BOOST_FOREACH(value const& v, near_food) {
            if (food.available[v.second]) {
                food_id.push_back(v.second);
            }
        }
        near_food.clear();
    }

    // first element is number of near entities
    // second is the identity of entities
    return std::pair<int, std::vector<int> > {food_id.size(), food_id};
}

/// population movement function
void Population::move(Resources food) {
    float angle = 0.f;
    float twopi = 2.f * M_PI;
    
    // what increment for 4 samples in a circle around the agent
    float increment = twopi / 4.0f;
    shufflePop();
    // loop over agents --- randomise
    for (size_t i = 0; i < order.size(); i++)
    {
        int id = order[i];
        if (counter[id] > 0) {
            counter[id] --;
        }
        else {
            // first assess current location
            float sampleX = coordX[id];
            float sampleY = coordY[id]; 
            float foodHere = static_cast<float>(countFood(
                    food, sampleX, sampleY
                ).first);
            float nbrsHere = static_cast<float>(countAgents(
                    sampleX, sampleY
                ).first);
            
            // get suitability current
            float suitabilityHere = (coef_food[id] * foodHere) + (coef_nbrs[id] * nbrsHere);

            // new location is initially current location
            float newX = coordX[id];
            float newY = coordY[id];
            // now sample at four locations around
            for(float theta = 0.f; theta < twopi - increment; theta += increment) {
                float t1_ = static_cast<float>(cos(theta));
                float t2_ = static_cast<float>(sin(theta));
                // range for food
                sampleX = coordX[id] + (range_food * t1_);
                sampleY = coordY[id] + (range_food * t2_);
                foodHere = static_cast<float>(countFood(
                    food, sampleX, sampleY
                ).first);
                // use range for agents
                sampleX = coordX[id] + (range_agents * t1_);
                sampleY = coordY[id] + (range_agents * t2_);
                nbrsHere = static_cast<float>(countAgents(
                    sampleX, sampleY
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
            // set locations
            coordX[id] = newX; coordY[id] = newY;
        }
    }
}

void Population::forage(Resources &food){
    shufflePop();
    // loop over agents --- randomise
    for (size_t i = 0; i < order.size(); i++)
    {
        int id = order[i];
        if ((counter[id] > 0) | (food.nAvailable == 0)) { 

        }
        else {
            // find nearest item ids
            std::vector<int> theseItems = (countFood(food, coordX[id], coordY[id])).second;
            // energy[id] = static_cast<float> (theseItems.size());
            // counter[id] = stopTime;
            int thisItem = -1;

            // check near items count
            if(theseItems.size() > 0) {
                // now check them
                // for (size_t i = 0; i < theseItems.size(); i++){
                //     if(food.available[theseItems[i]]) {
                //         thisItem = theseItems[i]; // if available pick this item
                //         break;
                //     }
                // }
                // take first items by default
                thisItem = theseItems[0];

                if (thisItem != -1) {
                    // check selected item is available
                    assert(food.available[thisItem] && "forage error: item not available");
                    counter[id] = handling_time;
                    energy[id] += 1.0;

                    // reset food availability
                    food.available[thisItem] = false;
                    food.counter[thisItem] = food.regen_time;
                }
            }
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
std::vector<float> Population::handleFitness() {
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
std::cauchy_distribution<float> mutation_size(0.0, mShift);

// fun for replication
void Population::Reproduce() {
    //normalise intake
    std::vector<float> vecFitness = handleFitness();

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
