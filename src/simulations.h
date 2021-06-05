#ifndef SIMULATIONS_H
#define SIMULATIONS_H

#include "data_types.h"

void evolve_pop(int genmax, double tmax,
                Population &pop,
                Resources &food,
                genData &thisGenData,
                double competitionCost,
                float sensoryRange,
                const int scenes,
                const int stopTime);

Rcpp::List do_simulation(int popsize, int genmax, int tmax,
                         int nFood, int foodClusters, double clusterDispersal, double landsize,
                         double competitionCost, float sensoryRange,
                         const int nScenes, const int stopTime);

#endif // SIMULATIONS_H
