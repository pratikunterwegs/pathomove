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

#include <Rcpp.h>

using namespace Rcpp;

// EtaCRW=0.7 #the weight of the CRW component in the BCRW used to model the Indiv movement
// StpSize_ind=7 #Mean step lengths of individuals;
// StpStd_ind=5 # Sandard deviations of step lengths of individuals
// Kappa_ind=3 #oncentration parameters of von Mises directional distributions used for individuals' movement

// Agent class
struct Population {
public:
   Population(const int popsize, const double beginTrait) :
       nAgents (popsize),
       coordX (popsize, 50.0),
       coordY (popsize, 50.0),
       energy (popsize, 0.000001),
       // one trait
       trait(popsize, beginTrait),
       // count stationary behaviour
       counter (popsize, 0),
       // associations
       associations(popsize, 0)

   {}
   ~Population() {}

    int nAgents = 0;
    std::vector<double> coordX;
    std::vector<double> coordY;
    std::vector<double> energy;
    std::vector<double> trait;
    std::vector<int> counter;
    std::vector<int> associations;

    // position rtree
    bgi::rtree< value, bgi::quadratic<16> > agentRtree;

    // funs for pop
    void initPop (int popsize);
    void setTrait ();
    void initPos(Resources food);
    void move(size_t id, Resources food, const double moveCost);
    void normaliseIntake();
    void Reproduce();
    // for network
    void updatePbsn(Network &pbsn, const double range);
    void competitionCosts(const double competitionCost);
};

void Population::initPos(Resources food) {
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        coordX[i] = gsl_rng_uniform(r) * food.dSize;
        coordY[i] = gsl_rng_uniform(r) * food.dSize;
    }
}

void Population::setTrait() {
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        trait[i] = gsl_rng_uniform(r);
    }
}

// distance function without wrapping
double distance(double x1, double y1, double x2, double y2) {

    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// to update pbsn
void Population::updatePbsn(Network &pbsn, const double range) {

    // focal agents
    for(size_t i = 0; i < static_cast<size_t>(nAgents - 1); i++) {
        // make vector of proximate agents
        // move j along the size of associations expected for i
        for(size_t j = i + 1; j < pbsn.associations[i].size(); j++) {

            if(distance(coordX[i], coordY[i], coordX[j], coordY[j]) < range) {
                pbsn.associations[i][j]++;
                // add to associations
                associations[i]++;
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

void Population::move(size_t id, Resources food, const double moveCost) {

    double heading;
    double landsize = food.dSize;
    double stepSize;

    stepSize = gsl_ran_gamma(r, indivStepSize, indivStepSizeSd); // individual strategy is the deviation in step size
    heading = etaCrw * gsl_ran_gaussian(r, 3.0);

    // get radians
    heading = heading * M_PI / 180.0;

    // figure out the next position
    coordX[id] = coordX[id] + (stepSize * cos(heading));
    coordY[id] = coordY[id] + (stepSize * sin(heading));

    // make the move on the wrapped landscape
    coordX[id] = fmod(landsize + coordX[id], landsize);

    coordY[id] = fmod(landsize + coordY[id], landsize);

    // add a cost
    energy[id] -= (stepSize * moveCost);
}

std::vector<int> findNearItems(size_t individual, Resources &food, Population &pop,
    const double distance){
    // search nearest item only if any are available
    std::vector<int> itemID;

    if (food.nAvailable > 0) {
        std::vector<value> nearItems;
        point currentLoc = point(pop.coordX[individual], pop.coordY[individual]);
        box bbox(point(pop.coordX[individual] - distance,
                       pop.coordY[individual] - distance),
                 point(pop.coordX[individual] + distance, pop.coordY[individual] + distance));

        food.rtree.query(
                    bgi::within(bbox) &&
                    bgi::satisfies([&](value const& v) {return bg::distance(v.first, currentLoc) < 2;}),
                    std::back_inserter(nearItems));

        for(size_t i = 0; i < nearItems.size(); i++){
            itemID.push_back(nearItems[i].second); // store item ids
        }
    }
    return itemID;
}

void forage(size_t individual, Resources &food, Population &pop, const double distance){
    // find nearest item ids
    std::vector<int> theseItems = findNearItems(individual, food, pop, distance);

    // check near items count
    if(theseItems.size() > 0) {
        // which to pick
        int thisItem = -1;

        // now check them
        for (size_t i = 0; i < theseItems.size(); i++){
            if(food.counter[theseItems[i]] == 0) {
                thisItem = theseItems[i]; // if available pick this item

                break;
            }
        }

        // if item available then consume it
        // also stop the agent here for as many steps as its trait determines
        if (thisItem > -1) {
            pop.counter[individual] = pop.trait[individual];
            pop.energy[individual] += foodEnergy;
            // remove the food item from the landscape for a brief time
            food.counter[thisItem] = regenTime;
        }
    }
}

DataFrame returnPbsn (Population &pop, Network &pbsn) {

    std::vector<int> focalAgent;
    std::vector<int> subfocalAgent;
    std::vector<int> pbsnAssociations;

    // focal agents
    for(size_t i = 0; i < static_cast<size_t>(pop.nAgents - 1); i++) {
        // make vector of proximate agents
        // move j along the size of associations expected for i
        for(size_t j = i + 1; j < pbsn.associations[i].size(); j++) {

            if(pbsn.associations[i][j] > 0) {
                focalAgent.push_back(i);
                subfocalAgent.push_back(j);
                pbsnAssociations.push_back(pbsn.associations[i][j]);
            }
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
        if (gsl_ran_bernoulli(r, mProb) == 1) {
            // mutation set, now increase or decrease
            newTrait[a] = gsl_ran_gaussian(r, mShift);
            // no negative traits
            if (newTrait[a] < 0) {
                newTrait[a] = 0;
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
