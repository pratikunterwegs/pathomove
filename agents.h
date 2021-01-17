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

// EtaCRW=0.7 #the weight of the CRW component in the BCRW used to model the Indiv movement
// StpSize_ind=7 #Mean step lengths of individuals;
// StpStd_ind=5 # Sandard deviations of step lengths of individuals
// Kappa_ind=3 #oncentration parameters of von Mises directional distributions used for individuals' movement

// Agent class
struct Population {
public:
    Population() :
        nAgents (10000),
        coordX (nAgents, 50.0),
        coordY (nAgents, 50.0),
        energy (nAgents, 0.000001),
        // one trait
        trait(nAgents, 0.0)

    {}
    ~Population() {}

    int nAgents;
    std::vector<double> coordX;
    std::vector<double> coordY;
    std::vector<double> energy;
    std::vector<double> trait;

    // funs for pop
    void setTrait ();
    void initPos(Resources food);
    void move(Resources food);
};

void Population::initPos(Resources food) {
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        coordX[i] = gsl_rng_uniform(r) * food.dSize;
        coordY[i] = gsl_rng_uniform(r) * food.dSize;
    }
}

void Population::setTrait() {
    for (size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        trait[i] = gsl_ran_gamma(r, 1.0, 1.0);
    }
}

void Population::move(Resources food) {

    double heading;
    double landsize = food.dSize;

    for(size_t i = 0; i < static_cast<size_t>(nAgents); i++) {
        double stepSize = gsl_ran_gamma(r, indivStepSize, indivStepSizeSd);

        // get heading
        heading = etaCrw * gsl_ran_gaussian(r, 1.0);

        // get radians
        heading = heading * M_PI / 180.0;

        // figure out the next position
        coordX[i] = coordX[i] + (stepSize * cos(heading));
        coordY[i] = coordY[i] + (stepSize * sin(heading));

        // make the move on the wrapped landscape
        coordX[i] = fmod(landsize + coordX[i], landsize);

        coordY[i] = fmod(landsize + coordY[i], landsize);
    }
}

void findNearestItem(size_t individual, Resources &food, Population &pop){
    // search nearest item
    std::vector<value> nearItems;
    point currentLoc = point(pop.coordX[individual], pop.coordY[individual]);
    box bbox(point(pop.coordX[individual] - range, 
        pop.coordY[individual] - range),
        point(pop.coordX[individual] + range, pop.coordY[individual] + range));

    food.rtree.query(
                bgi::within(bbox) &&
                bgi::satisfies([&](value const& v) {return bg::distance(v.first, currentLoc) < 2;}),
                std::back_inserter(nearItems));

    std::cout << nearItems.size() << " near agent " << individual <<  "\n";
}

// void findNearestItem(size_t individual, Resources &food, Population &pop){
//     double itemRange;
//     // check if food
//     if (food.nAvailable > 0) {
//        // check available items
//         for(size_t j = 0; j < food.whichAvailable.size(); j++) {
//             itemRange = sqrt(
//                         pow(food.coordX[food.whichAvailable[j]] - pop.coordX[individual], 2.0) +
//                     pow(food.coordY[food.whichAvailable[j]] - pop.coordY[individual], 2.0));

//             std::cout << itemRange << " ";

//             if(itemRange <= range) {

//                 pop.energy[individual] += 1.0 ;
//                 food.counter[food.whichAvailable[j]] = regenTime;
//                 food.countAvailable();

//                 break;
//             }

//         }
//     }
// }

#endif // AGENTS_H
