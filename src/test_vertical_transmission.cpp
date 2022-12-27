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

#include "agents.h"

// food_4 parameters, only one item
const float landsize = 10.f;
const int nItems = 1;
const int nClusters = 1;
const float clusterSpread = 0.1f;
const int regen_time = 50;

Resources food_4(nItems, landsize, nClusters, clusterSpread, regen_time);

// population parameters
const int popsize = 5;
const int n_samples = 4;
const float range_food = 1.f;
const float range_agents = 1.f;
const float range_move = 1.f;
const int handling_time = 5;
const float p_transmit = 0.1f;
const float p_v_transmit = 1.f;
const float test_mSize = 0.001;
const float vertical = true;
const float reprod_threshold = true;

// make test population
Population pop_5(popsize, n_samples, range_agents, range_food, range_move,
                 handling_time, p_transmit, p_v_transmit, vertical,
                 reprod_threshold);

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Vertical transmission of infection") {
  // only a single agent is near food
  pop_5.intake = std::vector<float>(popsize, 10.f);
  pop_5.infected = std::vector<bool>(popsize, true);

  test_that("Offspring are infected by parents") {
    pop_5.energy = pop_5.intake;

    // std::cout << "Agent 0 intake = " << pop_4.intake[0] << "\n";
    // std::cout << "Agent 0 energy = " << pop_4.energy[0] << "\n";

    // reproduction with very low dispersal and mutation
    // vertical transmission is turned on, and p_v_transmit is 1.0
    pop_5.Reproduce(food_4, false, 0.f, 0.f, 0.f);

    // expect new pop_5 to all be infected
    for (size_t i = 0; i < popsize; i++) {
      CATCH_CHECK(pop_5.infected[i]);
      // std::cout << "Agent " << i << " src_infect = " << pop_5.srcInfect[i] <<
      // "\n";
      CATCH_CHECK(pop_5.srcInfect[i] < 0);  // check that srcInfect = neg parent
    }
  }
}
