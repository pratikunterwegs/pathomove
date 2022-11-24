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

// food_2 parameters
const float landsize = 10.f;
const int nItems = 10;
const int nClusters = 10;
const float clusterSpread = 0.1f;
const int regen_time = 50;

Resources food_2(nItems, landsize, nClusters, clusterSpread, regen_time);

// population parameters
const int popsize = 5;
const float n_samples = 4.f;
const float range_food = 1.f;
const float range_agents = 1.f;
const float range_move = 1.f;
const int handling_time = 5;
const float p_transmit = 0.1;
const float test_mSize = 0.001;
const float vertical = false;
const float reprod_threshold = false;

// make test population
Population pop_3(popsize, range_agents, range_food, range_move,
                 handling_time, p_transmit, vertical,
                 reprod_threshold);

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Population inheritance without threshold") {
  // only a single agent is near food
  pop_3.coordX = {3.3f, 0.f, 0.f, 0.f, 0.f};
  pop_3.coordY = {3.f, 0.f, 0.f, 0.f, 0.f};
  pop_3.updateRtree();
  pop_3.sF = {10.0f, 0.f, 0.f, 0.f, 0.f};
  pop_3.sH = {0.f, 0.f, 0.f, 0.f, 0.f};
  pop_3.sN = {0.f, 0.f, 0.f, 0.f, 0.f};
  pop_3.counter = {0, 0, 0, 0, 0};

  // food_2.initResources();
  food_2.countAvailable();
  food_2.coordX = {4.5f, 4.1f, 4.1f, 4.2f, 4.3f, 4.5f, 4.2f, 4.1f, 4.2f, 4.1f};
  food_2.coordY = {3.0f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f};
  food_2.counter = std::vector<int>(nItems, 0);
  food_2.countAvailable();
  // initialise rtree and set counter value
  bgi::rtree<value, bgi::quadratic<16>> tmpRtree;
  for (int i = 0; i < nItems; ++i) {
    point p = point(food_2.coordX[i], food_2.coordY[i]);
    tmpRtree.insert(std::make_pair(p, i));
  }
  food_2.rtree = tmpRtree;

  test_that("Agents away from items see none") {
    int near_items = pop_3.countFood(food_2, pop_3.coordX[1], pop_3.coordY[1]);
    CATCH_CHECK(near_items == 0);  // found out manually using std::cout
  }

  test_that("Agent inheritance works, no threshold") {
    pop_3.move(food_2, true);
    pop_3.pickForageItem(food_2, true);
    pop_3.doForage(food_2);

    // check that far agents have no items
    CATCH_CHECK(pop_3.counter[1] == 0);
    CATCH_CHECK(pop_3.intake[1] == Approx(0.f).epsilon(1e-3));
    CATCH_CHECK(pop_3.energy[1] == Approx(0.001f).epsilon(1e-3));

    pop_3.energy = pop_3.intake;

    // std::cout << "Agent 0 intake = " << pop_3.intake[0] << "\n";
    // std::cout << "Agent 0 energy = " << pop_3.energy[0] << "\n";

    std::vector<float> vfit = pop_3.handleFitness();
    for (size_t i = 0; i < popsize; i++) {
      // std::cout << "Agent " << i << " fitness = " << vfit[i] << "\n";
      if (i > 0) CATCH_CHECK(vfit[0] > vfit[i]);
    }

    // reproduction with very low dispersal and mutation
    pop_3.Reproduce(food_2, false, 0.001f, 1e-6f, 1e-6f);

    // expect new pop_3 to have same sF as single nearby agent
    for (size_t i = 0; i < popsize; i++) {
      // std::cout << "Agent " << i << " sF = " << pop_3.sF[i] << "\n";
      CATCH_CHECK(pop_3.sF[i] == Approx(10.f).epsilon(1e-2));
    }
  }
}
