/*
  * This file uses the Catch unit testing library, alongside
* testthat's simple bindings, to test a C++ function.
 *
 * For your own packages, ensure that your test files are
 * placed within the `src/` folder, and that you include
 * `LinkingTo: testthat` within your DESCRIPTION file.
 */

// All test files should include the <testthat.h>
// header file.
#include <testthat.h>

// Normally this would be a function from your package's
// compiled library -- you might instead just include a header
// file providing the definition, and let R CMD INSTALL
// handle building and linking.

#include "agents.hpp"

const int popsize = 100;
const float range_food = 1.f;
const float range_agents = 1.f;
const float range_move = 0.5f;
const int handling_time = 5;
const float p_transmit = 0.1;

// make test population
Population pop (popsize, range_agents, range_food, range_move, 
                handling_time, p_transmit);

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Population initialisation works") {
  
  test_that("Population has right size") {
    expect_true(pop.nAgents == popsize);
  }
  
  test_that("Population vectors have right sizes") {
    expect_true(pop.sF.size() == popsize);
  }
  
  test_that("Population has right ranges") {
    expect_true(pop.range_agents == range_agents);
    expect_true(pop.range_food == range_food);
    expect_true(pop.range_move == range_move);
  }
  
  test_that("Population vector have right value before assignment") {
    expect_true((pop.sF[0] - 0.f) < 1e-5);
  }
  
  // set population traits
  pop.setTrait();
  
  test_that("Population vector have right value before assignment") {
    expect_false((pop.sF[0] - 0.f) < 1e-5);
  }

}

const int nItems = 100;
const float landsize = 10.f;
const int nClusters = 20;
const float clusterSpread = 1.f;
const int regen_time = 20;

// a food object
Resources land(nItems, landsize, nClusters, clusterSpread, regen_time);

// Population position on the landscape
context("Population position initialisation works") {
  
  test_that("Population on null island before position set") {
    expect_true((pop.coordX[0] - 0.f) < 1e-5);
  }
  
  pop.initPos(land);
  
  test_that("Population on random pos after position set") {
    expect_false((pop.coordX[0] - 0.f) < 1e-5);
  }
  
  test_that("Population within boundaries") {
    
    float max_pos_x = 0.f;
    float min_pos_y = 0.f;
    for (size_t i = 0; i < pop.coordX.size(); i++) {
      max_pos_x = (pop.coordX[i] > max_pos_x) ? pop.coordX[i] : max_pos_x;
      min_pos_y = (pop.coordY[i] < min_pos_y) ? pop.coordY[i] : min_pos_y;
    }
    // float comparison but a bit less precise
    expect_true(max_pos_x - landsize < 1e-2);
    expect_true(min_pos_y - 0.f < 1e2);
  }
  
}
