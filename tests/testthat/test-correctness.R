#### Check for aspects of simulation correctness ####
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

# Check that all individuals have the same infection source (-2, artificial)
test_that("Individuals infected artificially are correctly marked", {
  data = get_trait_data(data)
  expect_identical(
    unique(data[data$gen == max(data$gen), ]$src_infect), -2L
  )
})
