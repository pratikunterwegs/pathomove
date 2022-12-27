#### Test all scenarios with minimal run times ####
test_that("Pathomove basic runs with scenario 1", {
  # simulation runs with introduced pathogen
  g_patho_init <- 5L
  genmax <- 10L
  initial_infections <- 5L
  data <- run_pathomove(
    scenario = 1,
    popsize = 10,
    initialInfections = initial_infections,
    tmax = 10,
    genmax = genmax,
    g_patho_init = g_patho_init
  )

  # check for clas pathomove_output
  expect_s4_class(
    data, "pathomove_output"
  )
  expect_snapshot(
    slotNames(data)
  )

  # expect equal lengths
  expect(
    length(data@trait_data) == length(data@generations),
    failure_message = "Not as many trait dataframes as generations"
  )

  # expect equal lengths
  expect_identical(
    length(data@edge_lists), length(data@gens_edge_lists),
    "Not as many trait dataframes as generations"
  )

  # expect a full sequence of pathogen introductions
  expect_identical(
    seq(g_patho_init, genmax - 1),
    data@gens_patho_intro
  )

  # expect a full sequence of pathogen introductions
  expect_identical(
    seq(g_patho_init, genmax - 1),
    data@gens_patho_intro
  )

  # expect that there are infections in generations after g_patho_init
  infections_per_gen <- data@infections_per_gen
  infections_per_gen <- infections_per_gen[
    # starting at i + 1 because Cpp counting
    seq(min(data@gens_patho_intro) + 1, length(infections_per_gen))
  ]
  expect_true(
    all(infections_per_gen >= initial_infections)
  )
})

#### Check that pathomove scenario 2 works ####
test_that("Pathomove runs scenario 2", {
  # simulation runs with single spillover
  g_patho_init <- 5L
  genmax <- 10L
  data <- run_pathomove(
    scenario = 2,
    popsize = 10,
    initialInfections = 5,
    tmax = 10,
    genmax = genmax,
    g_patho_init = g_patho_init
  )
  # expect a single element vector
  expect_identical(
    g_patho_init,
    data@gens_patho_intro
  )
})

#### Check that pathomove scenario 3 works ####
test_that("Pathomove runs scenario 3", {
  # simulation runs with sporadic spillover
  expect_no_error(
    run_pathomove(
      scenario = 3,
      popsize = 10,
      initialInfections = 5,
      tmax = 10,
      genmax = 10,
      g_patho_init = 5,
      spillover_rate = 0.5
    )
  )
})

#### Pathomove runs with reproduction threshold ####
test_that("Pathomove with reproduction threshold", {
  popsize <- 10
  # simulation runs with continuous disease introduction
  expect_no_error(
    run_pathomove(
      scenario = 1,
      popsize = popsize,
      initialInfections = popsize,
      tmax = 10,
      genmax = 10,
      g_patho_init = 5,
      costInfect = 0.01,
      reprod_threshold = TRUE
    )
  )
  # simulation ends correctly with warning
  expect_warning(
    run_pathomove(
      scenario = 1,
      popsize = popsize,
      initialInfections = popsize,
      tmax = 2,
      genmax = 10,
      g_patho_init = 2,
      costInfect = 10,
      reprod_threshold = TRUE
    ),
    "(Warning)*(all agents)*(energy)*(ending simulation at gen)"
  )
})

#### Check for Pathomove argument checks ####
# simulation must quit early when conditions are not met
# test for simple simulation failure when there are more infections
# than agents
test_that("Pathomove quits when infections > agents", {
  popsize <- 10

  expect_error(
    run_pathomove(
      popsize = popsize,
      genmax = 10,
      g_patho_init = 5,
      initialInfections = popsize + 1
    ),
    regex = "Error: Initial infections must be less than/equal to popsize"
  )
})

# test for failure when g_patho_init is after genmax
test_that("Pathomove quits when g_patho_init > genmax", {
  popsize <- 10
  genmax <- 10
  expect_error(
    run_pathomove(
      popsize = popsize,
      genmax = genmax,
      g_patho_init = genmax + 1,
      initialInfections = popsize
    ),
    regex = "Error: G_patho_init must be less than genmax"
  )
})
