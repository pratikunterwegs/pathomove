test_that("Landscape export function", {
  land <- get_test_landscape(
    nItems = 1000,
    landsize = 50,
    nClusters = 50,
    clusterSpread = 1,
    regen_time = 100
  )

  testthat::expect_s3_class(
    land, "data.frame"
  )

  testthat::expect_true(
    all(c("x", "y", "tAvail") %in% colnames(land))
  )
})
