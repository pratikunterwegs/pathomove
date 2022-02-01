# check function
remove.packages("pathomove")

Rcpp::compileAttributes()
devtools::build()
{sink(file = "install_output.log"); devtools::install(upgrade = "never"); sink()}
devtools::document()

detach(package:pathomove)
library(pathomove)
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
a = pathomove::run_pathomove(
  scenario = 2,
  popsize = 500,
  nItems = 1000,
  landsize = 50,
  nClusters = 100,
  clusterSpread = 1,
  tmax = 100,
  genmax = 200,
  g_patho_init = 170,
  range_food = 1.0,
  range_agents = 1.0,
  range_move = 1.0,
  handling_time = 5,
  regen_time = 50,
  pTransmit = 0.05,
  initialInfections = 10,
  costInfect = 0.3,
  nThreads = 2,
  local_dispersal = TRUE,
  infect_percent = FALSE
)
#   }
# )
# t2 = Sys.time()
# t2 - t1}

# movement data
m1 = a[["move_pre"]] |> rbindlist()
m2 = a[["move_post"]] |> rbindlist()

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

ggplot(b[variable %in% c("intake", "energy")])+
  stat_summary(
    aes(
      gen, value
    )
  )+
  facet_wrap(~variable)

# energy = b[variable == "energy",]
wts = b[!variable %in% c("energy", "assoc", "t_infec", "moved", "degree"),]



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
