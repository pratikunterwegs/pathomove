# check function
remove.packages("snevo")

Rcpp::compileAttributes()
devtools::build()
{sink(file = "install_output.log"); devtools::install(upgrade = "never"); sink()}
# devtools::document()

detach(package:snevo)
library(snevo)
library(ggplot2)
library(data.table)

# l = snevo::get_test_landscape(
#   nItems = 1000,
#   landsize = 100,
#   nClusters = 200, 
#   clusterSpread = 1,
#   regen_time = 100
# )
# ggplot(l)+
#   geom_point(
#     aes(x, y, col = tAvail)
#     # size = 0.3
#   )+
#   scale_colour_viridis_b(
#     option = "H",
#     direction = 1,
#     breaks = c(0, 1, 2, 5, 10)
#   )+
#   coord_equal()

# {t1 = Sys.time()
# invisible(
#   x = {
    a = run_pathomove(
      scenario = 0,
      popsize = 50,
      nItems = 1000,
      landsize = 50,
      nClusters = 100,
      clusterSpread = 1,
      tmax = 100,
      genmax = 200,
      range_food = 1.0,
      range_agents = 1.0,
      range_move = 1.0,
      handling_time = 5,
      regen_time = 50,
      pTransmit = 0.05,
      initialInfections = 10,
      costInfect = 0.2,
      nThreads = 2
    )
#   }
# )
# t2 = Sys.time()
# t2 - t1}

# movement data
m1= a[["move_pre"]] |> rbindlist()
m2= a[["move_post"]] |> rbindlist()

ggplot(m1)+
  geom_path(
    aes(x, y, group = id, col = id),
    # size = 0.1
  )+
  # geom_point(
  #   aes(x, y, col = id),
  #   # size = 0.1
  # )+
  scale_colour_viridis_c(
    option = "H"
  )+
  coord_equal(
    # xlim = c(0, 50),
    # ylim = c(0, 50)
  )

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

#### plot data ####
b = melt(b, id.vars = c("gen", "id"))

# energy = b[variable == "energy",]
wts = b[!variable %in% c("energy", "assoc", "t_infec", "moved", "degree"),]

wts[, value := tanh(value * 20)]

ggplot(wts)+
  geom_hline(
    yintercept = 0, lty = 2,
    col = "grey", size = 0.2
  )+
  geom_bin2d(
    aes(gen, value),
    binwidth = c(2, 0.02),
    show.legend = F
  )+
  scale_y_continuous(
    trans = ggallin::ssqrt_trans
  )+
  scale_fill_viridis_c(
    option = "A", direction = -1,
    begin = 0, end = 1
  )+
  theme_test()+
  facet_wrap(~variable, ncol = 2)

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
    # colour = "transparent",
    stroke = 0.1,
    size = 4,
    alpha = 0.8
  )+
  colorspace::scale_fill_continuous_sequential(
    palette = "Batlow", rev = T,
    limits = c(-1, 1),
    trans = ggallin::ssqrt_trans
  )+
  scale_x_continuous(
    # trans = ggallin::ssqrt_trans
  )+
  scale_y_continuous(
    # trans = ggallin::ssqrt_trans
  )+
  facet_wrap(~ gen, labeller = label_both)+
  coord_equal(
    xlim = c(-1, 1),
    ylim = c(-1, 1)
  )

#### explore network ####
library(igraph)
# g = data[["matrices"]][4:6]
# g = g[[3]]
setnames(b, "assoc", "weight")
g = igraph::graph_from_data_frame(b[b$weight > 0, ], directed = FALSE)

plot(g, vertex.size = 3, vertex.label=NA)

library(tidygraph)
library(ggraph)

g = tidygraph::as_tbl_graph(g)

ggraph(g, layout = "mds")+
  geom_node_point()+
  geom_edge_link()
