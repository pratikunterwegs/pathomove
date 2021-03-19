test_that("simulation works", {
  
  # parameters
  popsize = 100
  
  # check function runes
  data_evolved_pop = snevo::do_simulation(
    popsize = popsize,
    genmax = 10, 
    tmax = 10, 
    foodClusters = 1, 
    clusterDispersal = 0.1,
    landsize = 100
  )
  
  # check is list
  testthat::expect_is(
    data_evolved_pop, "list"
  )
  
  # check length is two
  testthat::expect_identical(
    length(data_evolved_pop), 2L
  )
  
  # check elements are data frames
  invisible(
    lapply(data_evolved_pop, testthat::expect_is, class = "data.frame")
  )
  
  # check trait data has popsize rows
  testthat::expect_identical(
    nrow(data_evolved_pop[["trait_data"]]), as.integer(popsize)
  )
  
})
