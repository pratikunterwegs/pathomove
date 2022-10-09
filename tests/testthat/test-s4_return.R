test_that("Pathomove returns S4 output", {
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
    multithreaded = FALSE,
    dispersal = 3.0,
    infect_percent = FALSE,
    vertical = FALSE,
    mProb = 0.001,
    mSize = 0.001,
    spillover_rate = 0.01
  )

  # check is list
  testthat::expect_s4_class(
    data, "pathomove_output"
  )

  # expect equal lengths
  testthat::expect(
    length(data@trait_data) == length(data@generations),
    failure_message = "not as many trait dataframes as generations"
  )

  # expect equal lengths
  testthat::expect(
    length(data@edge_lists) == length(data@gens_edge_lists),
    failure_message = "not as many trait dataframes as generations"
  )

  # check elements are data frames
  invisible(
    lapply(data@trait_data,
      testthat::expect_s3_class,
      class = "data.frame"
    )
  )
})

# test for simple simulation failure when there are more infections
# than agents
test_that("pathomove fails when infections > agents", {
  # parameters
  popsize <- 20

  testthat::expect_error(
    object = {
      run_pathomove(
        scenario = 2,
        popsize = popsize,
        nItems = 180,
        landsize = 60,
        nClusters = 60,
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
        initialInfections = popsize + 1,
        costInfect = 0.25,
        multithreaded = FALSE,
        dispersal = 3.0,
        infect_percent = FALSE,
        vertical = FALSE,
        mProb = 0.001,
        mSize = 0.001,
        spillover_rate = 0.01
      )
    }
  )
})

test_that("Pathomove multithreading works", {
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
    multithreaded = TRUE,
    dispersal = 3.0,
    infect_percent = FALSE,
    vertical = FALSE,
    mProb = 0.001,
    mSize = 0.001,
    spillover_rate = 0.01
  )

  # check is list
  testthat::expect_s4_class(
    data, "pathomove_output"
  )
})
