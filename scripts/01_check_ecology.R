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
    nFood = 100,
    nClusters = 20,
    clusterDispersal = 0.1,
    maxAct = 1,
    activityRatio = 0.1,
    pInactive = 0.8,
    collective = FALSE,
    sensoryRange = 1,
    tmax = 10,
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

d_summary = d[,list(
    mean_assoc = mean(associations),
    sd_assoc = sd(associations)
), by = c("scene", "trait")]

ggplot(d_summary)+
geom_pointrange(
    aes(
        scene, mean_assoc,
        ymin = mean_assoc - sd_assoc,
        ymax = mean_assoc + sd_assoc,
        colour = factor(trait)
    ),
    position = position_dodge(width = 0.2)
)
facet_grid(~scene)
