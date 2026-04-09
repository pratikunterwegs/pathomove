# see `help(run_script, package = 'touchstone')` on how to run this
# interactively

# installs branches to benchmark
touchstone::branch_install()

# some setup
# benchmark a function call from your package (two calls per branch)
touchstone::benchmark_run(
  expr_before_benchmark = {
    genmax <- 100L
    g_patho_init <- genmax / 2
    initial_infections <- 5L
    tmax = 100
    popsize = 100
    scenario = 1
  }, #<-- TODO OTPIONAL setup before benchmark
  simple_case = pathomove::run_pathomove(
    scenario = scenario,
    popsize = popsize,
    initialInfections = initial_infections,
    tmax = tmax,
    genmax = genmax,
    g_patho_init = g_patho_init
  ), #<- TODO put the call you want to benchmark here
  n = 2
)

# create artifacts used downstream in the GitHub Action
touchstone::benchmark_analyze()
