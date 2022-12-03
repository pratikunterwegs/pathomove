#### Check that R functions can get data from simulation output ####
# get some simulation data
nAgents <- 10L
tmax <- 10L
data <- run_pathomove(
  scenario = 1,
  popsize = nAgents,
  genmax = 10,
  g_patho_init = 5,
  initialInfections = nAgents,
  tmax = tmax
) # use default values but reduce generations and time

# get trait data from pathomove output
trait_data <- get_trait_data(data)

# check network function
networks <- get_networks(data)

# check get movement function
movement <- get_move_data(data)

test_that("Get data from simulation output", {
  # check for class, at least data.frame
  expect_s3_class(
    trait_data, "data.frame"
  )
  # check for nAgents in one (and hence each) generation
  expect_identical(
    nrow(trait_data[trait_data$gen == min(trait_data$gen), ]),
    nAgents
  )
  # check that trait data column names are the same
  expect_snapshot(
    colnames(trait_data)
  )
  # check for tidygraph class
  expect_s3_class(
    networks[[1]], "tbl_graph"
  )
  # check all network list elements are tidygraphs
  expect_identical(
    unique(unlist(lapply(networks, class))),
    c("tbl_graph", "igraph")
  )
  # check network data column names
  expect_snapshot(
    colnames(as.data.frame(networks[[1]]))
  )

  # check the movement data
  expect_snapshot(
    colnames(movement)
  )
  expect_identical(
    max(movement$time), tmax - 1L
  )
})

#### Check that social strategy function works ####
# assign social strategy
get_social_strategy(trait_data)

test_that("Social strategy data is added", {
  expect_true(
    "social_strat" %in% colnames(trait_data)
  )
})

# get social information importance
get_si_importance(trait_data)

test_that("Social information use is calculated", {
  expect_true(
    "si_imp" %in% colnames(trait_data)
  )
  # check that weights are scaled -1 -- +1
  # check only for sF
  expect_gte(
    min(trait_data$sF), -1.0
  )
  expect_lte(
    max(trait_data$sF), 1.0
  )
})
