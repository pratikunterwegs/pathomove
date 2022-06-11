test_that("Pathomove returns S4 output", {
  # skip("skipped")
  # parameters
  data_s4 <- run_pathomove_s4(
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
    mProb = 0.001,
    mSize = 0.001
  )

  # check is list
  testthat::expect_s4_class(
    data_s4, "pathomove_output"
  )

  # check for slots
  testthat::expect(
    methods::.hasSlot(object = data_s4, name = "parameters"),
    failure_message = "no parameters slot"
  )
  testthat::expect(
    methods::.hasSlot(object = data_s4, name = "generations"),
    failure_message = "no generations slot"
  )
  testthat::expect(
    methods::.hasSlot(object = data_s4, name = "infections_per_gen"),
    failure_message = "no 'infections_per_gen' slot"
  )
  testthat::expect(
    methods::.hasSlot(object = data_s4, name = "trait_data"),
    failure_message = "no 'trait_data' slot"
  )

  # expect equal lengths
  testthat::expect(
    length(data_s4@trait_data) == length(data_s4@generations),
    failure_message = "not as many trait dataframes as generations"
  )

  # check elements are data frames
  invisible(
    lapply(data_s4@trait_data,
      testthat::expect_s3_class,
      class = "data.frame"
    )
  )
})
