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

#include "landscape.h"

context("Position wrapping works") {
  const float landsize = 10.f;
  const float pos_over = 11.13f;
  const float pos_under = -1.13f;
  float wrapped_over = wrap_pos(pos_over, landsize);
  float wrapped_under = wrap_pos(pos_under, landsize);
  test_that("Position wrapping works") {
    CATCH_CHECK(wrapped_over == Approx(pos_over - landsize).epsilon(1e-3));
    CATCH_CHECK(wrapped_under == Approx(landsize + pos_under).epsilon(1e-3));
  }
}