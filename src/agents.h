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
// make namespaces
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// apparently some types
typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<point, unsigned> value;

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
       counter (popsize, 0.0),
       // associations
       associations(popsize, 0)

   {}
   ~Population() {}

    int nAgents = 0;
    std::vector<double> coordX;
    std::vector<double> coordY;
    std::vector<double> energy;
    std::vector<double> trait;
    std::vector<double> counter;
    std::vector<int> associations;

    // position rtree
    bgi::rtree< value, bgi::quadratic<16> > agentRtree;

    // funs for pop
    void initPop (int popsize);
    void setTrait ();
    void setTraitBimodal (const double maxAct, const double ratio, const double proportion);
    void initPos(Resources food);
    void move(size_t id, Resources food, const double moveCost, const bool collective,
        const double sensoryRange);
    void normaliseIntake();
    void Reproduce();
    // for network
    void updatePbsn(Network &pbsn, const double range, const double landsize);
    void competitionCosts(const double competitionCost);
    void updateRtree();
    void countNeighbours (size_t id, const double sensoryRange, const double landsize);
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

void Population::setTraitBimodal(const double maxAct, const double ratio, const double proportion) {
    trait = std::vector<double> (nAgents, maxAct);
    // int nInactive = static_cast<int>(std::floor(proportion * static_cast<double>(nAgents)));
    for (int z = 0; z < nAgents; z++)
    {
        if(gsl_ran_bernoulli(r, proportion) == 1) {
            trait[z] = ratio * maxAct;
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
void Population::updatePbsn(Network &pbsn, const double range, const double landsize) {

    // focal agents
    for(size_t i = 0; i < static_cast<size_t>(nAgents - 1); i++) {
        // make vector of proximate agents
        // move j along the size of associations expected for i
        for(size_t j = i + 1; j < pbsn.associations[i].size(); j++) {

            if(wrappedDistanceAgents(coordX[i], coordY[i], coordX[j], coordY[j], landsize) < range) {
                pbsn.associations[i][j]++;
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

double wrappedDistance(boost::geometry::model::point<float, 2, bg::cs::cartesian> rTreeLoc,
                       double queryX, double queryY, double landsize) {
    double rtreeX = rTreeLoc.get<0>();
    double rtreeY = rTreeLoc.get<1>();

    double distanceX = fabs( fmod( (rtreeX - queryX), landsize ) );
    double distanceY = fabs( fmod( (rtreeY - queryY), landsize ) );

    double wrD = std::sqrt( (distanceX * distanceX) + (distanceY * distanceY) );

    return wrD;
}

void Population::move(size_t id, Resources food, const double moveCost,
    const bool collective, const double sensoryRange) {

    double heading;
    heading = etaCrw * gsl_ran_gaussian(r, 3.0);
    // get radians
    heading = heading * M_PI / 180.0;
    double landsize = food.dSize;
    double stepSize;

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
                    bgi::satisfies([&](value const& v) {return wrappedDistance(v.first, coordX[id],
                                                        coordY[id], food.dSize) < sensoryRange;}),
                    std::back_inserter(nearAgents));
        
        if (nearAgents.size() > 0) {
            size_t neighbour = nearAgents[0].second;
            static const double TWOPI = 6.2831853071795865;
            // static const double RAD2DEG = 57.2957795130823209;
            // if (a1 = b1 and a2 = b2) throw an error 
            double theta = atan2(coordX[id] - coordX[neighbour], 
                coordY[id] - coordY[neighbour]);
            if (theta < 0.0)
                theta += TWOPI;
            heading = theta;
        }
    }

    stepSize = gsl_ran_gamma(r, indivStepSize, indivStepSizeSd); // individual strategy is the 

    // figure out the next position
    coordX[id] = coordX[id] + (stepSize * cos(heading));
    coordY[id] = coordY[id] + (stepSize * sin(heading));

    // make the move on the wrapped landscape
    coordX[id] = fmod(landsize + coordX[id], landsize);

    coordY[id] = fmod(landsize + coordY[id], landsize);

    // add a cost
    energy[id] -= (stepSize * moveCost);
}

// check neighbours
void Population::countNeighbours (size_t id,
                                  const double sensoryRange,
                                  const double landsize) {
    updateRtree();
    std::vector<int> agentId;
    std::vector<value> nearAgents;
    box bbox(point(coordX[id] - sensoryRange,
                    coordY[id] - sensoryRange),
                point(coordX[id] + sensoryRange, coordY[id] + sensoryRange));
    agentRtree.query(
                bgi::within(bbox) &&
                bgi::satisfies([&](value const& v) {return wrappedDistance(v.first, coordX[id],
                                                    coordY[id], landsize) < sensoryRange;}),
                std::back_inserter(nearAgents));
    for(size_t i = 0; i < nearAgents.size(); i++){
            agentId.push_back(nearAgents[i].second); // store item ids
        }
    associations[id] += agentId.size();
}

std::vector<int> findNearItems(size_t individual, Resources &food, Population &pop,
    const double distance){
    // search nearest item only if any are available
    std::vector<int> itemID;

    if (food.nAvailable > 0) {
        std::vector<value> nearItems;
        box bbox(point(pop.coordX[individual] - distance,
                       pop.coordY[individual] - distance),
                 point(pop.coordX[individual] + distance, pop.coordY[individual] + distance));

        food.rtree.query(
                    bgi::within(bbox) &&
                    bgi::satisfies([&](value const& v) {return wrappedDistance(v.first, pop.coordX[individual],
                                                        pop.coordY[individual], food.dSize) < distance;}),
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
            if(food.available[i]) {
                thisItem = theseItems[i]; // if available pick this item

                break;
            }
        }

        // if item available then consume it
        // also stop the agent here for as many steps as its trait determines
        if (thisItem > -1) {
            pop.counter[individual] = stopTime;
            pop.energy[individual] += foodEnergy;

            // agent moves to where item was
            pop.coordX[individual] = food.coordX[thisItem];
            pop.coordY[individual] = food.coordY[thisItem];

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
    counter = std::vector<double> (nAgents);
    assert(static_cast<int>(counter.size()) == nAgents && "counter size wrong");

    // mutate trait: trait shifts up or down with an equal prob
    // trait mutation prob is mProb, in a two step process
    for (size_t a = 0; static_cast<int>(a) < nAgents; a++) {
        if (gsl_ran_bernoulli(r, mProb) == 1) {
            // mutation set, now increase or decrease
            newTrait[a] = trait[a] + gsl_ran_cauchy(r, mShift);
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
