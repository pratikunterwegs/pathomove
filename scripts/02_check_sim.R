# check function

library(snevo)
library(ggplot2)
library(data.table)

a = snevo::export_pop(101)

# sim works
a = snevo::do_simulation(
  popsize = 100,
  genmax = 1000, 
  tmax = 100, 
  nFood = 5000,
  foodClusters = 1, 
  clusterDispersal = 10,
  landsize = 100
)

b = a[["network_measures"]]

ggplot(b,
  aes(
      gen, interactions
    )
  )+
  geom_point()+
  geom_path()

b = a[["trait_data"]]
d = Map(function(df, g) {
  df$gen = g
  return(df)
}, b[["pop_data"]], b[["gens"]])

d = rbindlist(d)
hist(d$energy)

# explore evo
d[, trait_round := round(trait, 2)]
d_summary = d[,.N, by = c("gen", "trait_round")]

ggplot(d_summary)+
  geom_tile(aes(
    gen, 
    trait_round, 
    fill = N
  ))+
  scale_fill_distiller(
    direction = 1
  )

sn = a[["network_measures"]]

# run more sims

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
    nFood = 3000,
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
