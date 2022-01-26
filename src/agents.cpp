#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <random>

#include <boost/foreach.hpp>

#include <Rcpp.h>
#include <RcppParallel.h>

#include "network.hpp"
#include "landscape.hpp"
#include "agents.hpp"

// to shuffle pop id
void Population::shufflePop() {
    if (order[0] == order[nAgents - 1])
    {
        for (size_t i = 0; i < static_cast<size_t>(nAgents); i++)
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
        initX[i] = coordX[i];
        coordY[i] = agent_ran_pos(rng) * food.dSize;
        initY[i] = coordY[i];
    }
    updateRtree();
}

// unifrom distribution for agent trait
std::uniform_real_distribution<float> agent_ran_trait(-0.001, 0.001);

// set agent trait
void Population::setTrait() {
    for(int i = 0; i < nAgents; i++) {
        sF[i] = agent_ran_trait(rng);
        sH[i] = agent_ran_trait(rng);
        sN[i] = agent_ran_trait(rng);
    }
}

float get_distance(float x1, float x2, float y1, float y2) {
    return std::sqrt(std::pow((x1 - x2), 2) + std::pow((y1 - y2), 2));
}

// general function for agents within distance
std::pair<int, int> Population::countAgents (
    const float xloc, const float yloc) {
    
    int handlers = 0;
    int nonhandlers = 0;
    std::vector<value> near_agents;
    // query for a simple box
    agentRtree.query(bgi::satisfies([&](value const& v) {
        return bg::distance(v.first, point(xloc, yloc)) < range_agents;}),
        std::back_inserter(near_agents));

    BOOST_FOREACH(value const& v, near_agents) {
        
        if(counter[v.second] > 0) handlers ++; else nonhandlers ++;
    }
    near_agents.clear();
    // first element is number of near entities
    // second is the identity of entities
    return std::pair<int, int> {handlers, nonhandlers};
}

// function for near agent ids
std::vector<int> Population::getNeighbourId (
    const float xloc, const float yloc) {
    
    std::vector<int> agent_id;
    std::vector<value> near_agents;
    // query for a simple box
    // neighbours for associations are counted over the MOVEMENT RANGE
    agentRtree.query(bgi::satisfies([&](value const& v) {
        return bg::distance(v.first, point(xloc, yloc)) < range_move;}),
        std::back_inserter(near_agents));

    BOOST_FOREACH(value const& v, near_agents) {
        agent_id.push_back(v.second);
    }
    near_agents.clear();
    // first element is number of near entities
    // second is the identity of entities
    return agent_id;
}

// general function for items within distance
int Population::countFood (
    const Resources &food,
    const float xloc, const float yloc) {

    int nFood = 0;
    std::vector<value> near_food;

    // check any available
    if (food.nAvailable > 0) {
        // query for a simple box
        food.rtree.query(bgi::satisfies([&](value const& v) {
            return bg::distance(v.first, point(xloc, yloc)) < range_food;}),
            std::back_inserter(near_food));

        BOOST_FOREACH(value const& v, near_food) {
            // count only which are available!
            if (food.available[v.second]) {
                nFood++;
            }
        }
        near_food.clear();
    }

    return nFood;
}

// function for the nearest available food item
std::vector<int> Population::getFoodId (
    const Resources &food,
    const float xloc, const float yloc) {
        
    std::vector<int> food_id;
    std::vector<value> near_food;
    // check any available
    if (food.nAvailable > 0) {
        // query for a simple box
        // food is accessed over the MOVEMENT RANGE
        food.rtree.query(bgi::satisfies([&](value const& v) {
            return bg::distance(v.first, point(xloc, yloc)) < range_move;}), 
            std::back_inserter(near_food));

        BOOST_FOREACH(value const& v, near_food) {
            // count only which are available!
            if (food.available[v.second]) {
                food_id.push_back(v.second);
            }
        }
        near_food.clear();
    }

    // first element is number of near entities
    // second is the identity of entities
    return food_id;
}

/// rng for suitability
std::normal_distribution<float> noise(0.f, 0.0001f);
std::cauchy_distribution<float> noise_cauchy(0.f, 0.001f);

/// population movement function
void Population::move(const Resources &food, const int nThreads) {

    float twopi = 2.f * M_PI;
    
    // what increment for 3 samples in a circle around the agent
    float increment = twopi / n_samples;
    float angle = 0.f;
    // for this increment what angles to sample at
    std::vector<float> sample_angles (static_cast<int>(n_samples), 0.f);
    for (int i_ = 0; i_ < static_cast<int>(n_samples); i_++)
    {
        sample_angles[i_] = angle;
        angle += increment;
    }

    // make random noise for each individual and each sample
    std::vector<std::vector<float> > noise_v (nAgents, std::vector<float>(static_cast<int>(n_samples), 0.f));
    for (size_t i_ = 0; i_ < noise_v.size(); i_++)
    {
        for (size_t j_ = 0; j_ < static_cast<size_t>(n_samples); j_++)
        {
            noise_v[i_][j_] = noise_cauchy(rng);
        }
    }    

    shufflePop();
    // loop over agents --- randomise
    if (nThreads > 1) {
        // any number above 1 will allow automatic n threads
        tbb::task_scheduler_init _tbb(tbb::task_scheduler_init::automatic); // automatic for now
        // try parallel
        tbb::parallel_for(
            tbb::blocked_range<unsigned>(1, order.size()),
            [&](const tbb::blocked_range<unsigned>& r) {
                for (unsigned i = r.begin(); i < r.end(); ++i) {
                    int id = order[i];
                    if (counter[id] > 0) {
                        counter[id] --;
                    }
                    else {
                        // first assess current location
                        float sampleX = coordX[id];
                        float sampleY = coordY[id]; 

                        float foodHere = 0.f;
                        // count local food only if items are available
                        if(food.nAvailable > 0) {
                            foodHere = static_cast<float>(countFood(
                                food, sampleX, sampleY
                            ));
                        }
                        // count local handlers and non-handlers
                        std::pair<int, int> agentCounts = countAgents(sampleX, sampleY);
                        
                        // get suitability current
                        float suit_origin = (
                            (sF[id] * foodHere) + (sH[id] * agentCounts.first) +
                            (sN[id] * agentCounts.second)
                        );

                        float newX = sampleX;
                        float newY = sampleY;
                        // now sample at three locations around
                        for(size_t j = 0; j < sample_angles.size(); j++) {
                            float t1_ = static_cast<float>(cos(sample_angles[j]));
                            float t2_ = static_cast<float>(sin(sample_angles[j]));
                            
                            // use range for agents to determine sample locs
                            sampleX = coordX[id] + (range_agents * t1_);
                            sampleY = coordY[id] + (range_agents * t2_);

                            // crudely wrap sampling location
                            if((sampleX > food.dSize) | (sampleX < 0.f)) {
                                sampleX = std::fabs(std::fmod(sampleX, food.dSize));
                            }
                            if((sampleY > food.dSize) | (sampleY < 0.f)) {
                                sampleY = std::fabs(std::fmod(sampleY, food.dSize));
                            }

                            // count food at sample locations if any available
                            if(food.nAvailable > 0) {
                                foodHere = static_cast<float>(countFood(
                                    food, sampleX, sampleY
                                ));
                            }
                            
                            // count local handlers and non-handlers
                            std::pair<int, int> agentCounts = countAgents(sampleX, sampleY);

                            float suit_dest = (
                                (sF[id] * foodHere) + (sH[id] * agentCounts.first) +
                                (sN[id] * agentCounts.second) +
                                noise_v[id][j] // add same very very small noise to all
                            );

                            if (suit_dest > suit_origin) {
                                // where does the individual really go
                                newX = coordX[id] + (range_move * t1_);
                                newY = coordY[id] + (range_move * t2_);

                                // crudely wrap MOVEMENT location
                                if((newX > food.dSize) | (newX < 0.f)) {
                                    newX = std::fabs(std::fmod(newX, food.dSize));
                                }
                                if((newY > food.dSize) | (newY < 0.f)) {
                                    newY = std::fabs(std::fmod(newY, food.dSize));
                                }

                                assert(newX < food.dSize && newX > 0.f);
                                assert(newY < food.dSize && newY > 0.f);
                                suit_origin = suit_dest;
                            }
                        }
                        // distance to be moved
                        moved[id] += range_move;

                        // set locations
                        coordX[id] = newX; coordY[id] = newY;
                    }
                }
            }
        );
    } else if (nThreads == 1) {
        for (int i = 0; i < nAgents; ++i) {
            int id = order[i];
            if (counter[id] > 0) {
                counter[id] --;
            }
            else {
                // first assess current location
                float sampleX = coordX[id];
                float sampleY = coordY[id]; 

                float foodHere = 0.f;
                // count local food only if items are available
                if(food.nAvailable > 0) {
                    foodHere = static_cast<float>(countFood(
                        food, sampleX, sampleY
                    ));
                }
                // count local handlers and non-handlers
                std::pair<int, int> agentCounts = countAgents(sampleX, sampleY);
                
                // get suitability current
                float suit_origin = (
                    (sF[id] * foodHere) + (sH[id] * agentCounts.first) +
                    (sN[id] * agentCounts.second)
                );

                float newX = sampleX;
                float newY = sampleY;
                // now sample at three locations around
                for(size_t j = 0; j < sample_angles.size(); j++) {
                    float t1_ = static_cast<float>(cos(sample_angles[j]));
                    float t2_ = static_cast<float>(sin(sample_angles[j]));
                    
                    // use range for agents to determine sample locs
                    sampleX = coordX[id] + (range_agents * t1_);
                    sampleY = coordY[id] + (range_agents * t2_);

                    // crudely wrap sampling location
                    if((sampleX > food.dSize) | (sampleX < 0.f)) {
                        sampleX = std::fabs(std::fmod(sampleX, food.dSize));
                    }
                    if((sampleY > food.dSize) | (sampleY < 0.f)) {
                        sampleY = std::fabs(std::fmod(sampleY, food.dSize));
                    }

                    // count food at sample locations if any available
                    if(food.nAvailable > 0) {
                        foodHere = static_cast<float>(countFood(
                            food, sampleX, sampleY
                        ));
                    }
                    
                    // count local handlers and non-handlers
                    std::pair<int, int> agentCounts = countAgents(sampleX, sampleY);

                    float suit_dest = (
                        (sF[id] * foodHere) + (sH[id] * agentCounts.first) +
                        (sN[id] * agentCounts.second) +
                        noise_v[id][j] // add same very very small noise to all
                    );

                    if (suit_dest > suit_origin) {
                        // where does the individual really go
                        newX = coordX[id] + (range_move * t1_);
                        newY = coordY[id] + (range_move * t2_);

                        // crudely wrap MOVEMENT location
                        if((newX > food.dSize) | (newX < 0.f)) {
                            newX = std::fabs(std::fmod(newX, food.dSize));
                        }
                        if((newY > food.dSize) | (newY < 0.f)) {
                            newY = std::fabs(std::fmod(newY, food.dSize));
                        }

                        assert(newX < food.dSize && newX > 0.f);
                        assert(newY < food.dSize && newY > 0.f);
                        suit_origin = suit_dest;
                    }
                }
                // distance to be moved
                moved[id] += range_move;

                // set locations
                coordX[id] = newX; coordY[id] = newY;
            }
        }

    }
    
}

// function to paralellise choice of forage item
void Population::pickForageItem(const Resources &food, const int nThreads){
    shufflePop();
    // nearest food
    std::vector<int> idTargetFood (nAgents, -1);

    if (nThreads > 1)
    {
        // loop over agents --- no shuffling required here
        tbb::task_scheduler_init _tbb(tbb::task_scheduler_init::automatic); // automatic for now
        // try parallel foraging --- agents pick a target item
        tbb::parallel_for(
            tbb::blocked_range<unsigned>(1, order.size()),
                [&](const tbb::blocked_range<unsigned>& r) {
                for (unsigned i = r.begin(); i < r.end(); ++i) {
                    if ((counter[i] > 0) | (food.nAvailable == 0)) { 
                        // nothing -- agent cannot forage or there is no food
                    }
                    else {
                        // find nearest item ids
                        std::vector<int> theseItems = getFoodId(food, coordX[i], coordY[i]);
                        int thisItem = -1;

                        // check near items count
                        if(theseItems.size() > 0) {
                            // take first item by default
                            thisItem = theseItems[0];
                            idTargetFood[i] = thisItem;
                        }
                    }
                }
            }
        );
    } else if (nThreads == 1)
    {
        for (int i = 0; i < nAgents; ++i) {
            if ((counter[i] > 0) | (food.nAvailable == 0)) { 
                // nothing -- agent cannot forage or there is no food
            }
            else {
                // find nearest item ids
                std::vector<int> theseItems = getFoodId(food, coordX[i], coordY[i]);
                int thisItem = -1;

                // check near items count
                if(theseItems.size() > 0) {
                    // take first item by default
                    thisItem = theseItems[0];
                    idTargetFood[i] = thisItem;
                }
            }
        }
    }

    forageItem = idTargetFood;
}

// function to exploitatively forage on picked forage items
void Population::doForage(Resources &food, const int nThreads) {
    // all agents have picked a food item if they can forage
    // now forage in a serial loop --- this cannot be parallelised
    // this order is randomised
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++)
    {
        int id = order[i];
        if ((counter[id] > 0) | (food.nAvailable == 0)) {
            // nothing
        } else {
            int thisItem = forageItem[id]; //the item picked by this agent
            // check selected item is available
            if (thisItem != -1)
            {
                counter[id] = handling_time;
                energy[id] += 1.0; // increased here --- not as described.

                // reset food availability
                food.available[thisItem] = false;
                food.counter[thisItem] = food.regen_time;
                food.nAvailable --;
            }
        }
    }
}

void Population::countAssoc(const int nThreads) {
    for (int i = 0; i < nAgents; ++i) {
        // count nearby agents and update raw associations
        std::vector<int> nearby_agents = getNeighbourId(coordX[i], coordY[i]);
        associations[i] += nearby_agents.size();

        // loop over nearby agents and update association matrix
        for (size_t j = 0; j < nearby_agents.size(); j++)
        {
            int target_agent = nearby_agents[j];
            pbsn.adjMat (i, target_agent) += 1;
        }
    }
}

/// minor function to normalise vector
std::vector<float> Population::handleFitness() {
    // sort vec fitness
    std::vector<float> vecFitness = energy;
    std::sort(vecFitness.begin(), vecFitness.end()); // sort to to get min-max
    // scale to max fitness
    float maxFitness = vecFitness[vecFitness.size()-1];
    float minFitness = vecFitness[0];

    // reset to energy
    vecFitness = energy;
    // rescale copied energy vector by min anx max fitness
    for(size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        vecFitness[i] = ((vecFitness[i]  - minFitness) / (maxFitness - minFitness)) +
         noise(rng);
    }
    
    return vecFitness;
}

// mutation probability and size distribution
std::bernoulli_distribution mutation_happens(mProb);
std::cauchy_distribution<float> mutation_size(0.0, mShift);

// fun for replication
void Population::Reproduce() {
    // std::bernoulli_distribution verticalInfect(0.01f);
    std::normal_distribution<float> sprout(0.f, 3.f);

    //normalise intake
    std::vector<float> vecFitness = handleFitness();

    // set up weighted lottery
    std::discrete_distribution<> weightedLottery(vecFitness.begin(), vecFitness.end());

    // get parent trait based on weighted lottery
    std::vector<float> tmp_sF (nAgents, 0.f);
    std::vector<float> tmp_sH (nAgents, 0.f);
    std::vector<float> tmp_sN (nAgents, 0.f);
    
    // infected or not for vertical transmission
    std::vector<bool> infected_2 (nAgents, false);

    // reset infection source
    srcInfect = std::vector<int> (nAgents, 0);

    // reset associations
    associations = std::vector<int> (nAgents, 0);

    // reset distance moved
    moved = std::vector<float> (nAgents, 0.f);

    // reset adjacency matrix
    pbsn.adjMat = Rcpp::NumericMatrix(nAgents, nAgents);

    // positions
    std::vector<float> coord_x_2 (nAgents, 0.f);
    std::vector<float> coord_y_2 (nAgents, 0.f);
    
    for (int a = 0; a < nAgents; a++) {
        size_t parent_id = static_cast<size_t>(weightedLottery(rng));

        tmp_sF[a] = sF[parent_id];
        tmp_sH[a] = sH[parent_id];
        tmp_sN[a] = sN[parent_id];

        coord_x_2[a] = coordX[parent_id] + sprout(rng);
        coord_y_2[a] = coordY[parent_id] + sprout(rng);

        // edit initial positions
        initX[a] = coord_x_2[a];
        initY[a] = coord_y_2[a];

        // // vertical transmission of infection.
        // if(infected[parent_id]) {
        //     if(verticalInfect(rng)) {
        //         infected_2[a] = true;
        //         srcInfect[a] = 1;
        //     }
        // }
    }

    // swap infected and infected_2
    std::swap(infected, infected_2);
    infected_2.clear();

    // swap coords
    std::swap(coordX, coord_x_2);
    std::swap(coordY, coord_y_2);
    coord_x_2.clear(); coord_y_2.clear();
    
    // reset counter and time infected
    counter = std::vector<int> (nAgents, 0);
    timeInfected = std::vector<int> (nAgents, 0);
    assert(static_cast<int>(counter.size()) == nAgents && "counter size wrong");

    // mutate trait: trait shifts up or down with an equal prob
    // trait mutation prob is mProb, in a two step process
    for (int a = 0; a < nAgents; a++) {
        if(mutation_happens(rng)) {
            tmp_sF[a] = tmp_sF[a] + mutation_size(rng);
        }
        if(mutation_happens(rng)) {
            tmp_sH[a] = tmp_sH[a] + mutation_size(rng);
        }
        if(mutation_happens(rng)) {
            tmp_sN[a] = tmp_sN[a] + mutation_size(rng);
        }
    }
    
    // reset nInfected and count natal infections
    // from vertical transmission
    countInfected();

    // swap trait matrices
    std::swap(sF, tmp_sF);
    std::swap(sH, tmp_sH);
    std::swap(sN, tmp_sN);

    tmp_sF.clear(); tmp_sH.clear(); tmp_sN.clear();
    
    // swap energy
    std::vector<float> tmpEnergy (nAgents, 0.001);
    std::swap(energy, tmpEnergy);
    tmpEnergy.clear();
}
