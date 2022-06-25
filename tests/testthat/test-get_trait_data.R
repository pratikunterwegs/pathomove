test_that("Test getting trait data", {
  # skip("skipped")
  # parameters
  data <- run_pathomove(
    scenario = 2,
    popsize = 10,
    nItems = 180,
    landsize = 10,
    nClusters = 10,
    clusterSpread = 1,
    tmax = 100,
    genmax = 10,
    g_patho_init = 5,
    range_food = 1.0,
    range_agents = 1.0,
    range_move = 1.0,
    handling_time = 5,
    regen_time = 50,
    pTransmit = 0.05,
    initialInfections = 4,
    costInfect = 0.25,
    nThreads = 1,
    dispersal = 3.0,
    infect_percent = FALSE,
    vertical = FALSE,
    mProb = 0.001,
    mSize = 0.001,
    spillover_rate = 0.01
  )

  # get trait data from pathomove output
  trait_data <- get_trait_data(
    data
  )

  # check for class, at least data.frame
  testthat::expect_s3_class(
    trait_data, "data.frame"
  )
  
  # check network function
  networks = get_networks(
    data
  )
  invisible(
    lapply(networks,
      testthat::expect_s3_class,
      class = "tbl_graph"
    )
  )
  
})
