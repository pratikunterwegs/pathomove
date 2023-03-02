#### Check that the single threaded implementation is reproducible ####
test_that("Setting seed gives reproducible results without multithreading", {
  # original run
  run_original <- run_pathomove(
    scenario = 2,
    genmax = 10,
    tmax = 10,
    g_patho_init = 1,
    p_v_transmit = 0.2,
    costInfect = 0.25,
    multithreaded = FALSE,
    vertical = TRUE,
    mProb = 0.01,
    mSize = 0.01,
    spillover_rate = 0.01,
    seed = 123
  )

  # identical replicate with same seed
  run_replicate <- run_pathomove(
    scenario = 2,
    genmax = 10,
    tmax = 10,
    g_patho_init = 1,
    p_v_transmit = 0.2,
    costInfect = 0.25,
    multithreaded = FALSE,
    vertical = TRUE,
    mProb = 0.01,
    mSize = 0.01,
    spillover_rate = 0.01,
    seed = 123
  )

  # expect identical landscapes
  expect_identical(
    run_replicate@landscape,
    run_original@landscape
  )

  # expect identical infections over time vectors
  expect_identical(
    run_replicate@infections_per_gen,
    run_original@infections_per_gen
  )

  # expect identical evolved populations in the final generation
  pop_original <- get_trait_data(run_original)[gen == max(gen), ]
  pop_replicate <- get_trait_data(run_replicate)[gen == max(gen), ]
  expect_identical(
    pop_replicate,
    pop_original
  )

  # check that different seeds give different outputs
  run_replicate <- run_pathomove(
    scenario = 2,
    genmax = 10,
    tmax = 10,
    g_patho_init = 1,
    p_v_transmit = 0.2,
    costInfect = 0.25,
    multithreaded = FALSE,
    vertical = TRUE,
    mProb = 0.01,
    mSize = 0.01,
    spillover_rate = 0.01,
    seed = 0
  )

  # expect different infections over time vectors
  expect_error(
    expect_identical(
      run_replicate@infections_per_gen,
      run_original@infections_per_gen
    )
  )
})

#### Check that the multithreaded implementation is not reproducible ####
test_that("Multithreaded simulation is not reproducible", {
  # original run
  run_original <- run_pathomove(
    popsize = 500,
    scenario = 2,
    genmax = 1,
    tmax = 100,
    g_patho_init = 0,
    p_v_transmit = 0.2,
    costInfect = 0.25,
    multithreaded = TRUE,
    vertical = TRUE,
    mProb = 0.01,
    mSize = 0.01,
    spillover_rate = 0.01,
    seed = 123
  )

  # identical replicate with same seed
  run_replicate <- run_pathomove(
    popsize = 500,
    scenario = 2,
    genmax = 1,
    tmax = 100,
    g_patho_init = 0,
    p_v_transmit = 0.2,
    costInfect = 0.25,
    multithreaded = TRUE,
    vertical = TRUE,
    mProb = 0.01,
    mSize = 0.01,
    spillover_rate = 0.01,
    seed = 123
  )

  # expect different infections over time vectors
  expect_error(
    expect_identical(
      run_replicate@infections_per_gen,
      run_original@infections_per_gen
    )
  )
})
