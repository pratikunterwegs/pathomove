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
const double indivStepSize = 3.0;
const double indivStepSizeSd = 1.05;
//const float indivKappa = 3.f;
const double etaCrw = 50.0; // three degrees heading sd = 2.0 when correlated, fewer turns

// agents
const double range = 2.0;

// landscape
const int regenTime = 5;

#endif // PARAMETERS_H
