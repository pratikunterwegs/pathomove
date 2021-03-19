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

// EtaCRW=0.7 #the weight of the CRW component in the BCRW used to model the Indiv movement
// StpSize_ind=7 #Mean step lengths of individuals;
// StpStd_ind=5 # Sandard deviations of step lengths of individuals
// Kappa_ind=3 #oncentration parameters of von Mises directional distributions used for individuals' movement

// Agent class
struct Population {
public:
   Population(const int popsize) :
       nAgents (popsize),
       coordX (popsize, 50.0),
       coordY (popsize, 50.0),
       energy (popsize, 0.000001),
       // one trait
       trait(popsize, 0.0),
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
    void move(Resources food, const double moveCost);
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

    // scale trait
    sort(trait.begin(), trait.end());
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        trait[i] = trait[i] / trait[nAgents - 1];
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

void Population::move(Resources food, const double moveCost) {

    double heading;
    double landsize = food.dSize;
    double stepSize;

    for(size_t i = 0; i < static_cast<size_t>(nAgents); i++) {

        stepSize = gsl_ran_gamma(r, indivStepSize, trait); // individual strategy is the deviation in step size
        heading = etaCrw * gsl_ran_gaussian(r, 1.0);

        // get radians
        heading = heading * M_PI / 180.0;

        // figure out the next position
        coordX[i] = coordX[i] + (stepSize * cos(heading));
        coordY[i] = coordY[i] + (stepSize * sin(heading));

        // make the move on the wrapped landscape
        coordX[i] = fmod(landsize + coordX[i], landsize);

        coordY[i] = fmod(landsize + coordY[i], landsize);
        
        // add a cost
        energy[i] -= (stepSize * moveCost);
    }
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

        // if item available
        if (thisItem > -1) {
            pop.energy[individual] += 1.0;
            food.counter[thisItem] = regenTime;
        }
    }
}

/// minor function to normalise vector
void Population::normaliseIntake() {
    // sort vec fitness
    std::vector<double> vecFitness = energy;
    std::sort(vecFitness.begin(), vecFitness.end());

    // get min and max fitness
    double minFitness = vecFitness[0];

    // add abs min to all to remove negatives
    for(size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        energy[i] += std::fabs(minFitness);
    }

    // resort to get new max
    vecFitness = energy;
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

    // mutate trait
    for (size_t a = 0; static_cast<int>(a) < nAgents; a++) {
        if (gsl_ran_bernoulli(r, mProb) == 1) {
            newTrait[a] += gsl_ran_cauchy(r, mShift);

            if (newTrait[a] <= 0.0) {
                newTrait[a] = 0.00001;
            }
        }
    }

    // swap vectors
    std::swap(trait, newTrait);
    newTrait.clear();


}

#endif // AGENTS_H
