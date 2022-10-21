test_that("Pathomove works with percentage infections", {
  # parameters
  data <- run_pathomove(
    scenario = 1,
    popsize = 10,
    initialInfections = 2,
    tmax = 100,
    genmax = 10,
    g_patho_init = 5,
    multithreaded = FALSE,
    infect_percent = TRUE
    # vertical = FALSE
  )

  # check is list
  testthat::expect_s4_class(
    data, "pathomove_output"
  )
})

test_that("Pathomove works with vertical transmission", {
  # parameters
  data <- run_pathomove(
    scenario = 1,
    popsize = 10,
    initialInfections = 2,
    tmax = 100,
    genmax = 10,
    g_patho_init = 5,
    multithreaded = FALSE,
    vertical = TRUE
  )

  # check is list
  testthat::expect_s4_class(
    data, "pathomove_output"
  )
})

test_that("Pathomove works with evolution of self isolation", {
  # parameters
  data <- run_pathomove(
    scenario = 1,
    popsize = 10,
    initialInfections = 2,
    tmax = 100,
    genmax = 10,
    g_patho_init = 5,
    multithreaded = FALSE,
    evolve_sI = TRUE
  )

  # check is list
  testthat::expect_s4_class(
    data, "pathomove_output"
  )
})

test_that("Pathomove works with reproduction threshold", {
  # parameters
  data <- run_pathomove(
    scenario = 1,
    popsize = 10,
    initialInfections = 2,
    tmax = 100,
    genmax = 10,
    g_patho_init = 5,
    costInfect = 0.001,
    reprod_threshold = TRUE
  )

  # check is list
  testthat::expect_s4_class(
    data, "pathomove_output"
  )
})

test_that("Pathomove ends safely with high reproduction thresold", {
  expect_output(
    pathomove::run_pathomove(
      scenario = 1,
      popsize = 500,
      nItems = 180,
      landsize = 60,
      nClusters = 60,
      clusterSpread = 1,
      tmax = 100,
      genmax = 10,
      g_patho_init = 3,
      p_vTransmit = 0.75,
      initialInfections = 500,
      costInfect = 1.0,
      infect_percent = FALSE,
      reprod_threshold = TRUE
    ),
    regexp = "(Warning)*(agent)*(energy)*(ending simulation)"
  )
})
