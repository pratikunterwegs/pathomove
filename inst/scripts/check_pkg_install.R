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

l <- pathomove::get_test_landscape(
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

a = pathomove::run_pathomove(
  scenario = 1,
  popsize = 500,
  nItems = 180,
  landsize = 60,
  nClusters = 60,
  clusterSpread = 1,
  tmax = 100,
  genmax = 2000,
  g_patho_init = 1000,
  range_food = 1.0,
  range_agents = 1.0,
  range_move = 1.0,
  handling_time = 5,
  regen_time = 50,
  pTransmit = 0.05,
  initialInfections = 20,
  costInfect = 0.25,
  multithreaded = TRUE,
  dispersal = 2.0,
  infect_percent = FALSE,
  vertical = FALSE,
  evolve_sI = TRUE,
  mProb = 0.01,
  mSize = 0.01,
  spillover_rate = 1.0
)

microbenchmark::microbenchmark(
  pathomove::run_pathomove(
    scenario = 2,
    popsize = 500,
    nItems = 180,
    landsize = 60,
    nClusters = 60,
    clusterSpread = 1,
    tmax = 100,
    genmax = 100,
    g_patho_init = 10,
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
    spillover_rate = 1.0
  ),
  times = 10
)

# movement data
m1 <- a[["move_pre"]] |> rbindlist()
m2 <- a[["move_post"]] |> rbindlist()

m1_summary <- m1[, unlist(lapply(.SD, function(x) {
  list(
    first = first(x),
    last = last(x)
  )
}), recursive = FALSE), by = c("id"), .SDcols = c("x", "y")]

m2_summary <- m2[, unlist(lapply(.SD, function(x) {
  list(
    first = first(x),
    last = last(x)
  )
}), recursive = FALSE), by = c("id"), .SDcols = c("x", "y")]

ggplot(m1) +
  geom_point(
    aes(x, y, group = id, col = id)
  ) +
  scale_colour_viridis_c(
    option = "H"
  ) +
  coord_equal()

data <- a
a <- data[[1]]
names(a)

plot(a[["gens"]], a[["n_infected"]], type = "o", pch = 16)

#### handle data ####
b <- copy(a)
b <- Map(function(l, g) {
  l$id <- seq_len(nrow(l))
  l$gen <- g
  l
}, b$pop_data, b$gens)
b <- rbindlist(b)
b

#### examine strategies ####
d <- copy(b)
d[, social_strat := fcase(
  (sH > 0 & sN > 0), "agent tracking",
  (sH > 0 & sN <= 0), "handler tracking",
  (sH <= 0 & sN > 0), "non-handler tracking",
  (sH <= 0 & sN <= 0), "agent avoiding"
)]

df <- d[, .N, by = c("gen", "social_strat")]

ggplot(df) +
  geom_point(
    aes(
      gen, N,
      col = social_strat
    )
  )

#### plot data ####
b <- melt(b, id.vars = c("gen", "id"))

ggplot(b[variable %in% c("intake", "moved")]) +
  stat_summary(
    aes(
      gen, value
    )
  ) +
  facet_wrap(~variable, scales = "free")

wts <- b[!variable %in% c("energy", "assoc", "t_infec", "moved", "degree"), ]

#### explore network ####
library(igraph)
setnames(b, "assoc", "weight")
g <- igraph::graph_from_data_frame(b[b$weight > 0, ], directed = FALSE)

plot(g, vertex.size = 3, vertex.label = NA)

library(tidygraph)
library(ggraph)

g <- tidygraph::as_tbl_graph(g)

ggraph(g, layout = "mds") +
  geom_node_point() +
  geom_edge_link()

#### check S4 class output ####
a <- pathomove::run_pathomove(
  scenario = 2,
  popsize = 500,
  nItems = 1800,
  landsize = 60,
  nClusters = 60,
  clusterSpread = 1,
  tmax = 100,
  genmax = 1000,
  g_patho_init = 700,
  range_food = 1,
  range_agents = 1,
  range_move = 1,
  handling_time = 5,
  regen_time = 50,
  pTransmit = 0.05,
  initialInfections = 20,
  costInfect = 0.25,
  multithreaded = TRUE,
  dispersal = 3.0, # for local-ish dispersal
  infect_percent = FALSE,
  vertical = FALSE,
  mProb = 0.01,
  mSize = 0.01
)

pathomove::plot_pathomove(a)

trait <- Map(
  a@trait_data, a@generations,
  f = function(df, g) {
    df$gen <- g
    df
  }
) |> rbindlist()

trait |>
  ggplot(aes(gen, intake)) +
  stat_summary(geom = "line") +
  geom_vline(
    aes(xintercept = 700),
    col = "red"
  )
annotate(
  geom = "vline",
  xintercept = 700, col = 2
)
geom_bin_2d(
  binwidth = c(2, 1)
) +
  scale_fill_viridis_c(
    option = "A", direction = -1
  )

trait |>
  ggplot(aes(gen, moved)) +
  geom_bin_2d(
    binwidth = c(2, 5)
  ) +
  scale_fill_viridis_c(
    option = "A", direction = -1
  )

get_social_strategy(trait)

tdf <- trait[, .N, by = c("gen", "social_strat")]

ggplot(tdf) +
  geom_col(
    aes(gen, N, fill = social_strat)
  ) +
  scale_fill_viridis_d(
    direction = -1
  )

sdf <- trait[, .N, by = c("gen", "infect_src")]
ggplot(sdf) +
  geom_col(
    aes(gen, N, fill = as.factor(infect_src))
  )

#### check sporadic spillover ####
a <- pathomove::run_pathomove(
  scenario = 1,
  popsize = 500,
  nItems = 1800,
  landsize = 60,
  nClusters = 60,
  clusterSpread = 1,
  tmax = 100,
  genmax = 500,
  g_patho_init = 300,
  range_food = 1,
  range_agents = 1,
  range_move = 1,
  handling_time = 5,
  regen_time = 50,
  pTransmit = 0.05,
  initialInfections = 20,
  costInfect = 0.5,
  multithreaded = TRUE,
  dispersal = 2.0, # for local-ish dispersal
  infect_percent = FALSE,
  vertical = FALSE,
  mProb = 0.01,
  mSize = 0.01,
  spillover_rate = 0.01
)

plot(a@generations, a@infections_per_gen, type = "b")

a@gens_patho_intro

b <- pathomove::get_trait_data(a)
pathomove::get_social_strategy(b)

d <- b[, .N, by = c("gen", "social_strat")]

ggplot(d) +
  geom_col(
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
