// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
/*
 * This file uses the Catch unit testing library, alongside
 * testthat's simple bindings, to test a C++ function.
 *
 * For your own packages, ensure that your test files are
 * placed within the `src/` folder, and that you include
 * `LinkingTo: testthat` within your DESCRIPTION file.
 */

#include <pathomove.h>
#include <testthat.h>

// set up parameters for the population and landscape
const int popsize = 100;
const int n_samples = 4;
const float range_food = 1.f;
const float range_agents = 1.f;
const float range_move = 0.5f;
const int handling_time = 5;
const float p_transmit = 0.1;
const float test_mSize = 0.001;
const float vertical = false;

// make test population
pathomove::Population pop(popsize, n_samples, range_agents, range_food,
                          range_move, handling_time, p_transmit, p_transmit,
                          vertical, false);

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Population initialisation works") {
  test_that("Population has right size") {
    CATCH_CHECK(pop.nAgents == popsize);
  }

  test_that("Population vectors have right sizes") {
    CATCH_CHECK(pop.sF.size() == popsize);
  }

  test_that("Population has right ranges") {
    CATCH_CHECK(pop.range_agents == range_agents);
    CATCH_CHECK(pop.range_food == range_food);
    CATCH_CHECK(pop.range_move == range_move);
  }

  // set population traits
  pop.setTrait(test_mSize);

  test_that("Population vector has right value after assignment") {
    CATCH_CHECK(pop.sF[0] != 0.f);  // superfluous, as float comparisons bad
  }
}

const int nItems = 100;
const float landsize = 10.f;
const int nClusters = 20;
const float clusterSpread = 1.f;
const int regen_time = 20;

// a food object
pathomove::Resources land(nItems, landsize, nClusters, clusterSpread,
                          regen_time);

// Population position on the landscape
context("Population position initialisation works") {
  test_that("Population on null island before position set") {
    CATCH_CHECK(pop.coordX[0] == Approx(0.f).epsilon(1e-5));
  }

  pop.initPos(land);

  test_that("Population on random pos after position set") {
    CATCH_CHECK(pop.coordX[0] > 0.f);
  }

  test_that("Population within boundaries") {
    float max_pos_x = 0.f;
    float min_pos_y = landsize;
    for (size_t i = 0; i < pop.coordX.size(); i++) {
      max_pos_x = (pop.coordX[i] > max_pos_x) ? pop.coordX[i] : max_pos_x;
      min_pos_y = (pop.coordY[i] < min_pos_y) ? pop.coordY[i] : min_pos_y;
    }
    // float comparison but a bit less precise
    CATCH_CHECK(max_pos_x < landsize);
    CATCH_CHECK(min_pos_y > 0.f);
  }
}
