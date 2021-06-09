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
  popsize = 100,
  genmax = 500, 
  tmax = 25, 
  nFood = 500,
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
# summarise trait per gen
d_summary = melt(d, id.vars = "gen", 
                  measure.vars = c("coef_food", "coef_nbrs"),
                  variable.name = "trait")
d_summary[, v_round := plyr::round_any(value, 0.01)]
d_summary = d_summary[,.N, by = c("gen", "trait", "v_round")]

ggplot(d_summary)+
  geom_tile(aes(
    gen, 
    v_round, 
    fill = N
  ))+
  scale_fill_viridis_c(option = "H")+
  facet_grid(~trait)+
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
