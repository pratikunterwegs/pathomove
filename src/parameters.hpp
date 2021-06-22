#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <random>
#include <chrono>

extern std::mt19937 rng;

// landscape
const double foodEnergy = 1.0;

// mutation params
const double mShift = 0.01;
const double mProb = 0.01;

#endif // PARAMETERS_H
