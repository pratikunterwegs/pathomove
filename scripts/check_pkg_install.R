# check function
remove.packages("snevo")

Rcpp::compileAttributes()
devtools::build()
{sink(file = "install_output.log"); devtools::install(upgrade = "never"); sink()}
devtools::document()

detach(package:snevo)
library(snevo)
library(ggplot2)
library(data.table)

l = snevo::get_test_landscape(
  nItems = 1000,
  landsize = 200,
  nClusters = 50, 
  clusterSpread = 5,
  regen_time = 10
)
ggplot(l)+
  geom_point(
    aes(x, y, col = tAvail),
    size = 0.3
  )+
  scale_colour_viridis_b(
    option = "H",
    direction = 1,
    breaks = c(0, 1, 2, 5, 10)
  )

{t1 = Sys.time()
invisible(
  x = {
    a = snevo::run_pathomove(
      scenario = 0,
      popsize = 500,
      nItems = 1000,
      landsize = 200,
      nClusters = 50,
      clusterSpread = 5,
      tmax = 100,
      genmax = 500,
      range_food = 1,
      range_agents = 1,
      handling_time = 5,
      regen_time = 200,
      pTransmit = 0.01,
      initialInfections = 2,
      costInfect = 0.00,
      nThreads = 2
    )
  }
)
t2 = Sys.time()
t2 - t1}

data = a
a = data[[1]]
names(a)

plot(a[["gens"]], a[["n_infected"]], type = "o", pch = 16)
plot(a[["gens"]], a[["diameter"]], type = "o", pch = 16)
plot(a[["gens"]], a[["glob_eff"]], type = "o", pch = 16)

#### handle data ####
b = copy(a)
b = Map(function(l, g) {
    l$id = seq(nrow(l))
    l$gen = g
    l
}, b$pop_data, b$gens)
b = rbindlist(b)
b
# b = b[(gen %% 100 == 0) | (gen == 9999),] 

ggplot(b)+
  geom_bin2d(
    aes(gen, energy),
    binwidth = c(2, 1)
  )

#### plot data ####
b = melt(b, id.vars = c("gen", "id"))

# energy = b[variable == "energy",]
wts = b[!variable %in% c("energy", "assoc", "t_infec", "moved", "degree"),]

wts[, value := tanh(value * 20)]

ggplot(wts)+
  geom_bin2d(
    aes(gen, value),
    binwidth = c(2, 0.02),
    show.legend = F
  )+
  scale_y_continuous(
    # limits = c(-0.3, 0.3),
    # limits = c(-0.5, 0.5)
    # trans = ggallin::ssqrt_trans
  )+
  scale_fill_viridis_c(
    option = "A", direction = -1,
    begin = 0, end = 1
  )+
  theme_test()+
  facet_wrap(~variable, ncol = 2)

# plot associations per gen
ggplot(b[variable == "assoc"])+
  geom_bin2d(
    aes(gen, value),
    binwidth = c(2, 500)
  )+
  scale_fill_viridis_c(
    option = "H", direction = -1,
    limits = c(3, NA),
    na.value = "transparent",
    trans = "sqrt"
  )

# scale weights
wts_wide = copy(b[!variable %in% c("energy", "assoc", "t_infec", "moved", "degree"),])
wts_wide[, scaled_val := value / sum(abs(value)),
         by = c("gen", "id")]
wts_wide = dcast(
  wts_wide[, !("value")], 
  gen + id ~ variable, 
  value.var = "scaled_val"
)

ggplot(wts_wide[gen %% 100 == 0 | gen == max(gen)])+
  geom_hline(
    yintercept = 0, col = 2
  )+
  geom_vline(
    xintercept = 0, col = 2
  )+
  geom_jitter(
    aes(sF, sH,
        fill = sN),
    shape = 21,
    colour = 'black',
    stroke = 0.1,
    size = 2,
    alpha = 0.5
  )+
  colorspace::scale_fill_continuous_diverging(
    palette = "Berlin", rev = T,
    limits = c(-1, 1),
    trans = ggallin::ssqrt_trans
  )+
  scale_x_continuous(
    # trans = ggallin::ssqrt_trans
  )+
  scale_y_continuous(
    # trans = ggallin::ssqrt_trans
  )+
  theme_bw()+
  facet_wrap(~ gen)+
  coord_cartesian(
    xlim = c(-1, 1),
    ylim = c(-1, 1)
  )

#### explore network ####
library(igraph)
g = data[[2]]
g = igraph::graph_from_adjacency_matrix(g, mode = "upper", weighted = TRUE)
