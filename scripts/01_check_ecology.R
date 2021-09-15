# check function
Rcpp::compileAttributes()
devtools::build()
{sink(file = "install_output.log"); devtools::install(upgrade = "never"); sink()}
devtools::document()

detach(package:snevo)
library(snevo)
library(ggplot2)
library(data.table)

a = run_pathomove(
  scenario = 1,
  popsize = 500,
  nItems = 1000,
  landsize = 10,
  nClusters = 8,
  clusterSpread = 2,
  tmax = 50,
  genmax = 1000,
  range_food = 0.5,
  range_agents = 1,
  handling_time = 3,
  regen_time = 10
)
names(a)

#### handle data ####
b = copy(a)
b = Map(function(l, g) {
    l$id = seq(nrow(l))
    l$gen = g
    l
}, b$pop_data, b$gens)
b = rbindlist(b)
b

#### plot data ####
b = melt(b, id.vars = c("gen", "id"))

# energy = b[variable == "energy",]
wts = b[variable != "energy",]

ggplot(wts)+
  geom_bin2d(
    aes(gen, value),
    binwidth = c(1, 0.005)
  )+
  scale_y_continuous(
    trans = ggallin::pseudolog10_trans
  )+
  scale_fill_viridis_c()+
  facet_grid(~variable)
