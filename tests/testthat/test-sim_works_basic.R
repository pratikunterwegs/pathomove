test_that("simulation works", {
  # skip("skipped")
  # parameters
  popsize <- 100

  data <- run_pathomove(
    scenario = 2,
    popsize = 100,
    nItems = 180,
    landsize = 60,
    nClusters = 60,
    clusterSpread = 1,
    tmax = 100,
    genmax = 20,
    g_patho_init = 15,
    range_food = 1.0,
    range_agents = 1.0,
    range_move = 1.0,
    handling_time = 5,
    regen_time = 50,
    pTransmit = 0.05,
    initialInfections = 40,
    costInfect = 0.25,
    nThreads = 2,
    dispersal = 3.0,
    infect_percent = FALSE,
    mProb = 0.001,
    mSize = 0.001
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
