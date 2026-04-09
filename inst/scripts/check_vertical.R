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
    aes(x, y),
    shape = 1,
    alpha = 0.3
  ) +
  geom_segment(
    x = 0,
    y = 0,
    xend = 2,
    yend = 0
  ) +
  coord_equal()

# check basic simulation run
a <- run_pathomove(
  scenario = 3,
  popsize = 500,
  nItems = 1800,
  landsize = 60,
  nClusters = 60,
  clusterSpread = 1,
  tmax = 100,
  genmax = 350,
  g_patho_init = 200,
  range_food = 1.0,
  range_agents = 1.0,
  range_move = 1.0,
  handling_time = 5,
  regen_time = 50,
  pTransmit = 0.05,
  p_v_transmit = 0.33,
  initialInfections = 20,
  costInfect = 0.25,
  multithreaded = TRUE,
  dispersal = 2.0,
  infect_percent = FALSE,
  vertical = TRUE,
  reprod_threshold = FALSE,
  mProb = 0.01,
  mSize = 0.01,
  spillover_rate = 0.01
)

plot(a@infections_per_gen, type = "b")

a@gens_patho_intro

plot(a@landscape)

b <- pathomove::get_trait_data(a)
pathomove::get_social_strategy(b)

d <- b[, .N, by = c("gen", "social_strat")]

ggplot() +
  geom_col(
    data = d,
    aes(
      gen,
      N,
      fill = social_strat
    ),
    width = a@eco_parameters$genmax / 100
  ) +
  geom_line(
    aes(
      x = seq(min(a@generations), max(a@generations)),
      y = a@infections_per_gen
    )
  ) +
  geom_vline(
    xintercept = a@gens_patho_intro,
    linewidth = 0.2,
    lty = 1,
    alpha = 0.4
  ) +
  coord_cartesian(
    xlim = c(min(a@gens_patho_intro) - 50, NA)
  )

ggplot(b) +
  stat_summary(
    aes(
      gen,
      energy
    ),
    binwidth = c(100, NA)
  )

ggplot(b) +
  stat_summary(
    aes(
      gen,
      intake
    ),
    binwidth = c(100, NA)
  )

ggplot(b) +
  stat_summary(
    aes(
      gen,
      moved
    ),
    binwidth = c(100, NA)
  )

ggplot(b) +
  stat_summary(
    aes(
      gen,
      assoc
    )
  )

ggplot(b[, list(total_intake = sum(intake)), by = gen]) +
  geom_path(
    aes(gen, total_intake)
  )

ggplot(b[gen > a@gens_patho_intro]) +
  geom_jitter(
    aes(moved, assoc, col = social_strat),
    shape = 1,
    size = 0.2,
    alpha = 0.5
  ) +
  scale_y_log10()

#### initial positions ####
# ggplot(b[gen == min(gen), c("x", "y")]) +
#   geom_point(
#     aes(x, y)
#   )

#### plot movement ####
md <- get_move_data(a)

ggplot(md[id %in% seq(100)]) +
  geom_point(
    data = a@landscape,
    aes(x, y),
    shape = 1,
    alpha = 0.5
  ) +
  geom_path(
    aes(x, y, col = as.factor(id), group = id),
    size = 0.2,
    show.legend = FALSE
  ) +
  facet_wrap(
    ~type
  )

#### get network data ####
networks <- get_networks(a)
library(ggraph)
ggraph(networks[["210"]], x = x, y = y) +
  geom_point(
    data = a@landscape,
    aes(x, y),
    size = 0.2,
    col = "darkgreen",
    alpha = 0.5
  ) +
  geom_node_point(
    aes(
      fill = t_infec,
      size = assoc
    ),
    shape = 21,
    show.legend = TRUE
  )
scale_size_continuous(
  range = c(0.5, 3)
) +
  colorspace::scale_fill_continuous_sequential(
    palette = "Inferno",
    limit = c(1, 100),
    breaks = c(1, 10, 30, 100),
    na.value = "lightblue",
    trans = "sqrt"
  ) +
  coord_equal(
    expand = TRUE,
    xlim = c(0, 60),
    ylim = c(0, 60)
  ) +
  theme_graph(
    background = "white",
    border = TRUE,
    base_size = 8,
    plot_margin = margin(rep(0, 3))
  ) +
  theme(
    legend.margin = margin(rep(0, 4)),
    legend.position = "top",
    legend.title = element_text(size = 6),
    legend.key.height = unit(1, units = "mm"),
    legend.key.width = unit(3, units = "mm"),
    plot.background = element_blank()
  ) +
  labs(
    fill = "Time infected"
  ) +
  guides(
    size = "none",
    edge_alpha = "none"
  )


#### plotting transmission trees ####
df <- b[gen >= a@gens_patho_intro & gen %% 50 == 0, ]
df[, src_infect := fifelse(src_infect < 0, NA_real_, src_infect)]
df[, src_infect := fifelse(t_infec == 0, NA_real_, src_infect + 1)]

df <- split(df[, c("src_infect", "t_infec", "assoc", "gen")], by = "gen")

chains <- lapply(df, get_transmission_chain)

library(ggraph)
plots <- lapply(chains, function(chain) {
  ggraph(chain, layout = "circlepack") +
    # geom_edge_link(
    #   edge_colour = "grey"
    # ) +
    geom_node_circle(
      aes(fill = t_infec),
      colour = "darkgrey"
    ) +
    coord_fixed() +
    scale_fill_viridis_c(
      option = "C",
      direction = -1,
      limits = c(1, NA),
      na.value = "lightblue"
    ) +
    coord_equal() +
    theme_void()
})

library(patchwork)
wrap_plots(plots, nrow = 2) +
  plot_layout(guides = "collect")
