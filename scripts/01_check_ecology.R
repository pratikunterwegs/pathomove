# check function
devtools::build()
Rcpp::compileAttributes()
devtools::document()
devtools::install()

library(snevo)
library(ggplot2)
library(data.table)

a = snevo::do_eco_sim(
    popsize = 100,
    landsize = 10,
    nFood = 500,
    nClusters = 100,
    clusterDispersal = 0.1,
    maxAct = 0.99,
    activityRatio = 0.2,
    pInactive = 0.8,
    collective = FALSE,
    sensoryRange = 1,
    tmax = 25,
    scenes = 10
)

b = a[["trait_data"]]
b$gens
d = Map(function(df, sc) {
    df$scene = sc
    df
}, b$pop_data, b$gens)
d = rbindlist(d)
hist(d$trait)

ggplot(d)+
geom_boxplot(
    aes(
        factor(trait), associations,
        fill = factor(trait)
    )
)+
facet_grid(~scene)
