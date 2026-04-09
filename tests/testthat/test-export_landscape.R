test_that("Landscape export function", {
  nItems <- 1000L
  landsize <- 50.0
  nClusters <- 50L
  clusterSpread <- 1.0
  regen_time <- 100L

  land <- get_test_landscape(
    nItems = nItems,
    landsize = landsize,
    nClusters = nClusters,
    clusterSpread = clusterSpread,
    regen_time = regen_time
  )

  expect_s3_class(
    land,
    "data.frame"
  )

  expect_identical(
    colnames(land),
    c("x", "y", "tAvail")
  )

  expect_equal(
    nrow(land),
    nItems
  )

  expect_gte(
    min(land$x),
    0.0
  )
  expect_lte(
    min(land$x),
    landsize
  )
  expect_gte(
    min(land$y),
    0.0
  )
  expect_lte(
    min(land$y),
    landsize
  )
})
