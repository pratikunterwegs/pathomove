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

// food_3 parameters
const float landsize = 10.f;
const int nItems = 10;
const int nClusters = 10;
const float clusterSpread = 0.1f;
const int regen_time = 50;

Resources food_3(nItems, landsize, nClusters, clusterSpread, regen_time);

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
const float reprod_threshold = true;

// make test population
Population pop_4(popsize, range_agents, range_food, range_move,
                 handling_time, p_transmit, vertical,
                 reprod_threshold);

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Population inheritance with a threshold") {
  // only a single agent is near food
  pop_4.coordX = {3.3f, 0.f, 0.f, 0.f, 0.f};
  pop_4.coordY = {3.f, 0.f, 0.f, 0.f, 0.f};
  pop_4.updateRtree();
  pop_4.sF = {10.0f, 0.f, 0.f, 0.f, 0.f};
  pop_4.sH = {0.f, 0.f, 0.f, 0.f, 0.f};
  pop_4.sN = {0.f, 0.f, 0.f, 0.f, 0.f};
  pop_4.counter = {0, 0, 0, 0, 0};

  // food_3.initResources();
  food_3.countAvailable();
  food_3.coordX = {4.5f, 4.1f, 4.1f, 4.2f, 4.3f, 4.5f, 4.2f, 4.1f, 4.2f, 4.1f};
  food_3.coordY = {3.0f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f};
  food_3.counter = std::vector<int>(nItems, 0);
  food_3.countAvailable();
  // initialise rtree and set counter value
  bgi::rtree<value, bgi::quadratic<16>> tmpRtree;
  for (int i = 0; i < nItems; ++i) {
    point p = point(food_3.coordX[i], food_3.coordY[i]);
    tmpRtree.insert(std::make_pair(p, i));
  }
  food_3.rtree = tmpRtree;

  test_that("Agent inheritance works with threshold") {
    pop_4.coordX = {3.3f, 0.f, 0.f, 0.f, 0.f};
    pop_4.coordY = {3.f, 0.f, 0.f, 0.f, 0.f};
    pop_4.counter = {0, 0, 0, 0, 0};

    pop_4.move(food_3, true);
    pop_4.pickForageItem(food_3, true);
    pop_4.doForage(food_3);

    // check that far agents have no items
    CATCH_CHECK(pop_4.counter[1] == 0);
    CATCH_CHECK(pop_4.intake[1] == Approx(0.f).epsilon(1e-3));
    CATCH_CHECK(pop_4.energy[1] == Approx(0.001f).epsilon(1e-3));

    // assign tinfected
    pop_4.infected = {true, false, false, false, false};
    pop_4.timeInfected = {100, 0, 0, 0, 0};

    pop_4.energy = pop_4.intake;
    pop_4.pathogenCost(0.25f, false);

    // std::cout << "Agent 0 intake = " << pop_4.intake[0] << "\n";
    // std::cout << "Agent 0 energy = " << pop_4.energy[0] << "\n";

    // check that diseased agent has lowest fitness
    std::vector<float> vfit = pop_4.handleFitness();
    for (size_t i = 0; i < popsize; i++) {
      // std::cout << "Agent " << i << " fitness = " << vfit[i] << "\n";
      if (i > 0) CATCH_CHECK(vfit[0] < vfit[i]);
    }

    // check application of reprod threshold
    std::pair<std::vector<int>, std::vector<float>> parents =
        pop_4.applyReprodThreshold();

    // check only four agents make it and 0 is not among them
    CATCH_CHECK(parents.first.size() == 4);
    CATCH_CHECK(parents.first[0] == 1);

    // reproduction with very low dispersal and mutation
    pop_4.Reproduce(food_3, false, 0.001f, 1e-6f, 1e-6f);

    // expect new pop_4 to have same sF as single nearby agent
    for (size_t i = 0; i < popsize; i++) {
      // std::cout << "Agent " << i << " sF = " << pop_4.sF[i] << "\n";
      CATCH_CHECK(pop_4.sF[i] == Approx(0.f).epsilon(1e-2));
    }
  }
}
