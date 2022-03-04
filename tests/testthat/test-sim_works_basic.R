test_that("simulation works", {
  # skip("skipped")
  # parameters
  popsize <- 100

  data <- run_pathomove(
    scenario = 2,
    popsize = popsize,
    nItems = 100,
    landsize = 20,
    nClusters = 10,
    clusterSpread = 0.1,
    tmax = 20,
    genmax = 10,
    range_food = 1,
    range_agents = 1,
    range_move = 1,
    handling_time = 3,
    regen_time = 5,
    pTransmit = 0.1,
    initialInfections = 10,
    costInfect = 0.2,
    nThreads = 2
  )

  # check is list
  testthat::expect_is(
    data, "list"
  )

  # check length is two
  testthat::expect_identical(
    length(data), 4L
  )

  # check names in data
  testthat::expect_true(
    all(
      c("gen_data", "edgeLists", "move_pre", "move_post") %in%
        names(data)
    )
  )

  # check elements are data frames
  invisible(
    lapply(data$trait_data$pop_data,
      testthat::expect_is,
      class = "data.frame"
    )
  )

  # check trait data has popsize rows, checking only one df for now
  testthat::expect_identical(
    nrow(data[["gen_data"]]$pop_data[[1]]), as.integer(popsize)
  )
})
