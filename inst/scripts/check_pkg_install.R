# check function
remove.packages("pathomove")

Rcpp::compileAttributes()
devtools::build(vignettes = FALSE)

sink(file = "install_output.log")
devtools::install(upgrade = "never", build_vignettes = FALSE)
sink()

devtools::document()

detach(package:pathomove)
library(pathomove)
library(data.table)
library(ggplot2)

l <- get_test_landscape(
  nItems = 1800,
  landsize = 60,
  nClusters = 60,
  clusterSpread = 1,
  regen_time = 50
)
ggplot(l) +
  geom_point(
    aes(x, y)
  ) +
  geom_segment(
    x = 0, y = 0,
    xend = 2, yend = 0
  ) +
  coord_equal()

# check basic simulation run
a = run_pathomove(
  scenario = 1,
  popsize = 500,
  nItems = 180,
  landsize = 60,
  nClusters = 60,
  clusterSpread = 1,
  tmax = 100,
  genmax = 500,
  g_patho_init = 300,
  range_food = 1.0,
  range_agents = 1.0,
  range_move = 1.0,
  handling_time = 5,
  regen_time = 50,
  pTransmit = 0.05,
  initialInfections = 20,
  costInfect = 0.25,
  multithreaded = TRUE,
  # nThreads = 2,
  dispersal = 2.0,
  infect_percent = FALSE,
  vertical = FALSE,
  reprod_threshold = FALSE,
  mProb = 0.01,
  mSize = 0.01,
  spillover_rate = 1.0
)

plot(a@generations, a@infections_per_gen, type = "b")

a@gens_patho_intro

b <- pathomove::get_trait_data(a)
pathomove::get_social_strategy(b)

d <- b[, .N, by = c("gen", "social_strat")]

ggplot(d) +
  geom_area(
    aes(
      gen, N,
      fill = social_strat
    )
  )

ggplot(b) +
  stat_summary(
    aes(
      gen, energy
    ),
    binwidth = c(100, NA)
  )

ggplot(b[gen > a@gens_patho_intro]) +
  geom_jitter(
    aes(moved, assoc, col = social_strat)
  ) +
  scale_y_log10()
