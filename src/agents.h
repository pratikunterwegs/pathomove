#ifndef AGENTS_H
#define AGENTS_H

#define _USE_MATH_DEFINES
/// code to make agents
#include <algorithm>
#include <boost/foreach.hpp>
#include <cassert>
#include <iostream>
#include <vector>

#include "landscape.h"
#include "network.h"

// Agent class
struct Population {
public:
  Population(const int popsize, const float n_samples, const float range_agents,
             const float range_food, const float range_move,
             const int handling_time, float pTransmit, const float p_vTransmit,
             const bool vertical, const bool reprod_threshold)
      :

        // agents, positions, energy and traits
        nAgents(popsize), coordX(popsize, 0.0f), coordY(popsize, 0.0f),
        initX(popsize, 0.0f), initY(popsize, 0.0f), intake(popsize, 0.001f),
        energy(popsize, 0.001f), sF(popsize, 0.f), sH(popsize, 0.f),
        sN(popsize, 0.f), sI(popsize, 0.f),

        // counters for handling and social metrics
        counter(popsize, 0), associations(popsize, 0),

        // agent sensory parameters
        n_samples(n_samples), range_agents(range_agents),
        range_food(range_food), range_move(range_move),
        handling_time(handling_time),

        // vectors for agent order, infection status, time infected
        order(popsize, 1), forageItem(popsize, -1),
        infected(popsize, false), //,
        timeInfected(popsize, 0),

        // disease parameters and total pop infected
        pTransmit(pTransmit), p_vTransmit(p_vTransmit), vertical(vertical),
        reprod_threshold(reprod_threshold), nInfected(0),

        // infection source and distance moved
        srcInfect(popsize, 0), moved(popsize, 0.f),

        // a network object
        pbsn(popsize) {}
  ~Population() {}

  // agent count, coords, and energy
  const int nAgents;
  std::vector<float> coordX, coordY, initX, initY, intake, energy;
  // weights
  std::vector<float> sF, sH, sN, sI;

  // counter and metrics
  std::vector<int> counter, associations;

  // sensory range and foraging
  const float n_samples, range_agents, range_food, range_move;
  const int handling_time;

  // shuffle vector and transmission
  std::vector<int> order, forageItem;
  std::vector<bool> infected;
  std::vector<int> timeInfected;
  float pTransmit, p_vTransmit;
  const bool vertical, reprod_threshold;
  bool use_sI = false; // initially false

  // the number of infected agents
  int nInfected;
  std::vector<int> srcInfect;

  // movement distances
  std::vector<float> moved;

  // position rtree
  bgi::rtree<value, bgi::quadratic<16>> agentRtree;

  // network object
  Network pbsn;

  /// functions for the population ///
  // population order, trait and position randomiser
  void shufflePop();
  void setTrait(const float &mSize);
  void initPos(const Resources &food);

  // make rtree and get nearest agents and food
  void updateRtree();

  int countFood(const Resources &food, const float &xloc, const float &yloc);

  std::vector<int> getFoodId(const Resources &food, const float &xloc,
                             const float &yloc);

  std::pair<int, int> countAgents(const float &xloc, const float &yloc);

  std::vector<int> getNeighbourId(const float &xloc, const float &yloc);

  // functions to move and forage on a landscape
  void move(const Resources &food, const bool &multithreaded);
  void pickForageItem(const Resources &food, const bool &multithreaded);
  void doForage(Resources &food);

  // funs to handle fitness and reproduce
  std::vector<float> handleFitness();
  void Reproduce(const Resources &food, const bool &infect_percent,
                 const float &dispersal, const float &mProb,
                 const float &mSize);

  // pathogen dynamics -- initial infections, spread, and costs
  void introducePathogen(const int &nAgInf);
  void pathogenSpread();
  void pathogenCost(const float &costInfect, const bool &infect_percent);

  // count infected agents, infection source
  void countInfected();
  float propSrcInfection();

  // counting proximity based interactions
  void countAssoc();

  // check whether any agents pass the reproduction threshold
  const bool check_reprod_threshold();
  std::pair<std::vector<int>, std::vector<float>> applyReprodThreshold();

  // functions for the network
  // there is no function to update the network, this is handled in countAssoc
};

// a dinky function for distance and passed to catch test
float get_distance(const float &x1, const float &x2, const float &y1,
                   const float &y2);

#endif // AGENTS_H
