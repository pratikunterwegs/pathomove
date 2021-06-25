# check function
# check function
Rcpp::compileAttributes()
devtools::build()
sink(file = "install_output.log"); devtools::install(); sink()

library(snevo)
library(ggplot2)
library(data.table)

# sim work=e
a = snevo::do_simulation(
  popsize = 2500,
  genmax = 50, 
  tmax = 25, 
  nFood = 1000,
  foodClusters = 16, 
  clusterDispersal = 4,
  landsize = 10,
  competitionCost = 2,
  sensoryRange = 2,
  nScenes = 1,
  stopTime = 3
)

names(a)

d = Map(function(df, g) {
  df$gen = g
  return(df)
}, a[["pop_data"]], a[["gens"]])

d = rbindlist(d)
hist(d$energy)

# explore evo
ggplot(d)+
  geom_bin2d(
    aes(gen, coef_food),
    binwidth = c(2, 0.01)
  )+
  coord_cartesian(
    xlim = c(0, 1e3)
  )+
  scale_fill_viridis_c(
    option = "F",
    direction = -1
  )
