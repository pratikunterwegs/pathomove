# check function
# check function
Rcpp::compileAttributes()
devtools::build()
sink(file = "install_output.log"); devtools::install(); sink()

library(snevo)
library(ggplot2)
library(data.table)

# sim work=e
a = do_simulation(
  popsize = 5,
  genmax = 2, 
  tmax = 2, 
  nFood = 1000,
  foodClusters = 16, 
  clusterDispersal = 2,
  landsize = 5,
  competitionCost = 0,
  sensoryRange = 5,
  collective = F,
  nScenes = 1,
  stopTime = 3
)

names(a)

b = a[["trait_data"]]
d = Map(function(df, g) {
  df$gen = g
  return(df)
}, b[["pop_data"]], b[["gens"]])

d = rbindlist(d)
hist(d$energy)

# explore evo
d[, sprintf("trait%i", seq(6)) := lapply(.SD, round, 1),
  .SDcols = sprintf("trait%i", seq(6))]
# summarise trait per gen
d_summary = melt(d, id.vars = "gen", 
                  measure.vars = sprintf("trait%i", seq(6)),
                  variable.name = "trait")
d_summary = d_summary[,.N, by = c("gen", "trait", "value")]

ggplot(d_summary)+
  geom_tile(aes(
    gen, 
    value, 
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
