# check function
devtools::build()
devtools::install()

library(snevo)
library(ggplot2)
library(data.table)

a = snevo::export_pop(101)
a
# sim works
a = snevo::do_simulation(
  popsize = 500,
  genmax = 250, 
  tmax = 100, 
  nFood = 500,
  foodClusters = 10, 
  clusterDispersal = 0.125,
  landsize = 25,
  competitionCost = 0,
  regenTime = 0, 
  collective = T
)

x11()

b = a[["network_measures"]]

ggplot(b,
  aes(
      gen, global_efficiency
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
  scale_fill_viridis_c(option = "E")+
  coord_cartesian(ylim = c(0, 1))

## check associations and trait
d_sn = d[, list(
  mean_a = mean(associations)
), by = c("gen", "trait_round")]

ggplot(d_sn)+
  geom_tile(aes(
    gen, 
    trait_round, 
    fill = mean_a
  ))+
  scale_fill_viridis_c(
    trans = "sqrt"
  )+
  coord_cartesian(ylim = c(0, 1))
