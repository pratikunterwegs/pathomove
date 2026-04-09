#### Check that network ids and structures are correct ####
# get some simulation data
nAgents <- 10L
tmax <- 10L
g_patho_init <- 5L
data <- run_pathomove(
  scenario = 1,
  popsize = nAgents,
  genmax = 10,
  g_patho_init = g_patho_init,
  initialInfections = nAgents / 2,
  pTransmit = 1.0,
  dispersal = 0.0,
  tmax = tmax
) # use default values but reduce generations and time

# get trait data from pathomove output
trait_data <- get_trait_data(data)
# get trait data for after g_patho_init
trait_data <- trait_data[gen >= g_patho_init, ]

# check that trait data has source infect correctly
test_that("Infection source data is correct", {
  # expect all infection sources are 0 or above, as no parental infection
  expect_true(
    all(trait_data$src_infect[!is.na(trait_data$src_infect)] >= 0)
  )
  expect_true(
    all(trait_data$src_infect[!is.na(trait_data$src_infect)] <= nAgents)
  )
})

# check network function returns id
networks <- get_networks(data)
last_network <- networks[[length(networks)]]

test_that("Networks are returned with correct infection sources", {
  infection_sources <- as.data.frame(last_network)$src_infect
  expect_true(all(infection_sources[!is.na(infection_sources)] >= 0))
  expect_true(all(infection_sources[!is.na(infection_sources)] <= nAgents))
})

test_that("Transmission chain edges have valid nodes", {
  trans_chain <- get_transmission_chain(trait_data[gen == max(gen), ])
  trans_chain <- tidygraph::activate(trans_chain, "edges")
  trans_chain_edges <- as.data.frame(trans_chain)
  expect_true(
    all(trans_chain_edges$from > 0)
  )
  expect_true(
    all(trans_chain_edges$from <= nAgents)
  )
})

#### Check for infection sources with vertical transmission ####
data <- run_pathomove(
  scenario = 1,
  popsize = nAgents,
  genmax = 10,
  g_patho_init = g_patho_init,
  initialInfections = nAgents,
  costInfect = 0.0, # to allow agent to have offspring
  pTransmit = 1.0,
  dispersal = 0.0,
  vertical = TRUE,
  p_v_transmit = 1.0,
  tmax = tmax
) # use default values but reduce generations and time

# get trait data from pathomove output
trait_data <- get_trait_data(data)
# get trait data for after g_patho_init
trait_data <- trait_data[gen >= g_patho_init, ]

# Check that no agents are forced infections
test_that("Vertical infection is not overridden by manual infection", {
  # infection sources in the last generation
  # in the first spillover gen, we would expect to find srcInfect = 0
  infection_sources <- trait_data[
    trait_data$gen == max(trait_data$gen),
  ]$src_infect
  expect_true(
    all(infection_sources < 0) # all are infected from parents
  )
  expect_true(
    all(infection_sources >= -nAgents)
  )
})
