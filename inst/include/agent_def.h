// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.

#pragma once

#define _USE_MATH_DEFINES
/// code to make agents

// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppParallel)]]

// clang-format off
#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include <boost/foreach.hpp>

#include "landscape.h"
#include "network.h"
// clang-format on

namespace pathomove {
// Agent class
struct Population {
  // TODO: add a default constructor with sensible values
 public:
  Population(const int popsize, const int n_samples, const float range_agents,
             const float range_food, const float range_move,
             const int handling_time, const float pTransmit,
             const float p_v_transmit, const bool vertical,
             const bool reprod_threshold)
      :  // agents, positions, energy and traits
        nAgents(popsize),
        coordX(popsize, 0.0f),
        coordY(popsize, 0.0f),
        initX(popsize, 0.0f),
        initY(popsize, 0.0f),
        intake(popsize, 0.001f),
        energy(popsize, 0.001f),
        sF(popsize, 0.f),
        sH(popsize, 0.f),
        sN(popsize, 0.f),

        // counters for handling and social metrics
        counter(popsize, 0),
        associations(popsize, 0),
        // degree(popsize, 0),

        // agent sensory parameters
        n_samples(n_samples),
        range_agents(range_agents),
        range_food(range_food),
        range_move(range_move),
        handling_time(handling_time),

        // vectors for agent order, infection status, time infected
        order(popsize, 1),
        forageItem(popsize, -1),
        infected(popsize, false),  //,
        timeInfected(popsize, 0),

        // disease parameters and total pop infected
        pTransmit(pTransmit),
        p_v_transmit(p_v_transmit),
        vertical(vertical),
        reprod_threshold(reprod_threshold),
        nInfected(0),

        // infection source and distance moved
        srcInfect(popsize, NA_INTEGER),
        moved(popsize, 0.f),

        // a network object
        pbsn(popsize) {}
  ~Population() {}

  // agent count, coords, and energy
  const int nAgents;
  std::vector<float> coordX, coordY, initX, initY, intake, energy;
  // weights
  std::vector<float> sF, sH, sN;  // TODO: make this a 2D tensor

  // counter and metrics
  std::vector<int> counter, associations;  // number of total interactions

  // sensory range and foraging
  const int n_samples;
  const float range_agents, range_food, range_move;
  const int handling_time;

  // shuffle vector and transmission
  Rcpp::IntegerVector order;
  std::vector<int> forageItem;
  std::vector<bool> infected;
  std::vector<int> timeInfected;
  const float pTransmit, p_v_transmit;
  const bool vertical, reprod_threshold;

  // the number of infected agents
  int nInfected;
  Rcpp::IntegerVector srcInfect;

  // movement distances
  std::vector<float> moved;

  // position rtree
  bgi::rtree<value, bgi::quadratic<16>> agentRtree;  // TODO: remove spatial index based dist calc

  // network object
  Network pbsn;  // TODO: build off a tensor

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
  void doForage(Resources &food);  // NOLINT

  // funs to handle fitness and reproduce
  Rcpp::NumericVector handleFitness();
  void Reproduce(const Resources &food, const bool &infect_percent,
                 const float &dispersal, const float &mProb,
                 const float &mSize);

  // pathogen dynamics -- initial infections, spread, and costs
  void introducePathogen(const int nAgInf);
  void pathogenSpread();
  void pathogenCost(const float costInfect, const bool infect_percent);

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
}  // namespace pathomove
