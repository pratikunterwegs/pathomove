# check function

library(socialitymodel)
library(data.table)

# sim works
a = socialitymodel::do_simulation(genmax = 1000, 
              tmax = 100, foodClusters = 10, clusterDispersal = 0.1,
              landsize = 100)

landsize = 100

# make parameter file
params = CJ(
  genmax = 1000,
  tmax = 100,
  foodClusters = c(1, seq(25, 100, 25)),
  clusterDispersal = 5,
  replicate = seq(5)
)

data = Map(function(g, tm, cl, d) {
  do_simulation(
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
  geom_histogram(aes(V1))+
  facet_grid(foodClusters ~ clusterDispersal,
             labeller = label_both)+
  xlim(0, 1)
