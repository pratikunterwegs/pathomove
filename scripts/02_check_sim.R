# check function

library(snevo)
library(ggplot2)

a = snevo::export_pop(101)

# sim works
a = snevo::do_simulation(
  popsize = 100,
  genmax = 100, 
  tmax = 10, 
  foodClusters = 1, 
  clusterDispersal = 0.1,
  landsize = 100
)

ggplot(a)+
  geom_jitter(aes(p_ars, energy), size = 0.1,
              alpha = 0.2)+
  scale_y_log10()

ggplot(a)+
  geom_histogram(aes(p_ars))+
  coord_cartesian(xlim = c(0, 1))

landsize = 100
popsize = 1000
# make parameter file
library(data.table)
params = CJ(
  genmax = 1000,
  tmax = 100,
  foodClusters = c(1, seq(25, 100, 25)),
  clusterDispersal = 5,
  replicate = seq(5)
)

data = Map(function(g, tm, cl, d) {
  do_simulation(
    popsize = popsize,
    genmax = g,
    tmax = tm,
    foodClusters = cl,
    clusterDispersal = d,
    landsize = landsize
  )
}, params$genmax, params$tmax, params$foodClusters, params$clusterDispersal)

# get traits
data2 = copy(params)
data2$trait = lapply(data, `[[`, "p_ars")

# plot
library(ggplot2)

# unlits
data2 = data2[, unlist(trait, recursive = F),
              by = names(params)]

ggplot(data2)+
  geom_histogram(aes(V1, fill = factor(replicate)))+
  facet_grid(replicate ~ foodClusters,
             labeller = label_both)+
  xlim(0, 1)
