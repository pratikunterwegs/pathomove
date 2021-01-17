#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <random>
// gsl libs
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

// make a gsl rng and a regular rng
gsl_rng * r = gsl_rng_alloc (gsl_rng_taus);
std::mt19937 rng;

// movement
const double indivStepSize = 2.0;
const double indivStepSizeSd = 0.2;
//const float indivKappa = 3.f;
const double etaCrw = 50.0; // three degrees heading sd = 2.0 when correlated, fewer turns
const double etaArs = 120.0;
const int searchTime = 5;
const double moveCost = 0.01;

// agents
const double range = 2.0;

// landscape
const int regenTime = 5;

// mutation params
const double mShift = 0.01f;
const double mProb = 0.01f;

#endif // PARAMETERS_H
