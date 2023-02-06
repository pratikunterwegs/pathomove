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

#include <agent_dyn.h>

// landscape parameters
const float landsize = 10.f;
const int nItems = 10;
const int nClusters = 10;
const float clusterSpread = 0.1f;
const int regen_time = 50;

Resources landscape(nItems, landsize, nClusters, clusterSpread, regen_time);

context("Items availability works correctly") {
  // initialise landscape and update Rtree
  landscape.initResources();
  landscape.countAvailable();

  // place items
  landscape.coordX = {5.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
  landscape.coordY = landscape.coordX;
  landscape.counter = std::vector<int>(nItems, 0);

  // count again
  landscape.countAvailable();
  test_that("All items are available") {
    CATCH_CHECK(landscape.nAvailable == nItems);
  }
}

// population parameters
const int popsize = 1;
const int n_samples = 4;
const float range_food = 1.f;
const float range_agents = 1.f;
const float range_move = 1.f;
const int handling_time = 5;
const float p_transmit = 0.1;
const float test_mSize = 0.001;
const float vertical = false;
const float reprod_threshold = false;

// make test population
Population pop_2(popsize, n_samples, range_agents, range_food, range_move,
                 handling_time, p_transmit, p_transmit, vertical,
                 reprod_threshold);

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Population movement works") {
  pop_2.coordX = {3.3f};
  pop_2.coordY = {3.f};
  pop_2.updateRtree();
  pop_2.sF = {10.0f};
  pop_2.sH = {0.f};
  pop_2.sN = {0.f};
  pop_2.counter = {0};

  test_that("There is only one agent") {
    CATCH_CHECK(pop_2.nAgents == 1);
    CATCH_CHECK(pop_2.sF.size() == 1);
  }

  // landscape.initResources();
  landscape.countAvailable();

  // place items
  landscape.coordX = {4.5f, 4.1f, 4.1f, 4.2f, 4.3f,
                      4.5f, 4.2f, 4.1f, 4.2f, 4.1f};
  landscape.coordY = {3.0f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f, 3.f};
  landscape.counter = std::vector<int>(nItems, 0);

  // count again
  landscape.countAvailable();

  // initialise rtree and set counter value
  bgi::rtree<value, bgi::quadratic<16>> tmpRtree;
  for (int i = 0; i < nItems; ++i) {
    point p = point(landscape.coordX[i], landscape.coordY[i]);
    tmpRtree.insert(std::make_pair(p, i));
  }

  landscape.rtree = tmpRtree;

  test_that("Check items in position") {
    CATCH_CHECK(landscape.coordX[0] == Approx(4.5f).epsilon(1e-2));
    CATCH_CHECK(landscape.coordY[0] == Approx(3.f).epsilon(1e-2));
  }

  test_that("Agent counts items correctly at its own position") {
    int near_items =
        pop_2.countFood(landscape, pop_2.coordX[0], pop_2.coordY[0]);
    CATCH_CHECK(near_items == 7);  // found out manually using std::cout
  }

  test_that("Agent counts items correctly at a search position") {
    int near_items = pop_2.countFood(landscape, 4.f, 3.f);
    CATCH_CHECK(near_items == 10);
  }

  test_that("Agent moves correctly without multithreading") {
    pop_2.move(landscape, false);
    // check that agent moves only once
    CATCH_CHECK(pop_2.moved[0] == Approx(1.0).epsilon(1e-3));

    // check agent moves closer to food
    CATCH_CHECK(pop_2.coordX[0] > 3.3f);
  }

  test_that("Agent moves correctly with multithreading") {
    // reset
    pop_2.coordX = {3.3f};
    pop_2.coordY = {3.f};
    pop_2.moved = {0.f};

    pop_2.move(landscape, true);

    // check that agent moves only once
    CATCH_CHECK(pop_2.moved[0] == Approx(1.0).epsilon(1e-3));

    // check agent moves closer to food
    CATCH_CHECK(pop_2.coordX[0] > 3.3f);
  }

  test_that("Agent forages") {
    // reset
    pop_2.coordX = {3.3f};
    pop_2.coordY = {3.f};
    pop_2.moved = {0.f};

    pop_2.move(landscape, true);

    pop_2.pickForageItem(landscape, true);
    pop_2.doForage(landscape);

    // check that agent has item and already has intake
    CATCH_CHECK(pop_2.counter[0] == handling_time);
    CATCH_CHECK(pop_2.intake[0] == Approx(1.f).epsilon(1e-3));
    CATCH_CHECK(pop_2.energy[0] == Approx(0.001f).epsilon(1e-3));

    // check that one food item is unavailable
    CATCH_CHECK(landscape.nAvailable == nItems - 1);
  }
}
