#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <random>
// gsl libs
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <chrono>

// make a gsl rng and a regular rng
//const gsl_rng_type * T;
gsl_rng * r = gsl_rng_alloc (gsl_rng_taus);
// set seed
unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());

std::mt19937 rng;

// movement
const double indivStepSize = 2.0;
const double indivStepSizeSd = 0.2;

const double stepSizeArs = 0.5;
const double stepSizeSdArs = 0.05;

const double etaCrw = 50.0; // three degrees heading sd = 2.0 when correlated, fewer turns
const double etaArs = 120.0;
const double stopTime = 2.0;
const double moveCost = 0.001;

// agents
// const double range = 2.0;
const double pCheck = 1.0;

// landscape
const double regenTime = 3.0;
const double foodEnergy = 1.0;

// mutation params
const double mShift = 0.01;
const double mProb = 0.01;

#endif // PARAMETERS_H
