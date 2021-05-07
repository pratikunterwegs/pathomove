# check function
Rcpp::compileAttributes()
devtools::build()
# devtools::document()
devtools::install()

library(snevo)
library(ggplot2)
library(data.table)

a = snevo::do_eco_sim(
    popsize = 200,
    landsize = 10,
    nFood = 1000,
    nClusters = 200,
    clusterDispersal = 0.1,
    maxAct = 0.8,
    activityRatio = 0.125,
    pInactive = 0.5,
    collective = FALSE,
    sensoryRange = 50 / 100,
    stopTime = 1.0,
    tmax = 100,
    scenes = 15
)

b = a[["pbsn"]]

library(igraph)
g = graph.adjacency(b, weighted = TRUE, mode = "undirected")
g = simplify(g, remove.loops = TRUE)
plot(g, vertex.size = 10)

d = degree(g, loops = F)
length(d)
hist(d)
# trait to gen
b = a[["trait_data"]]
b$gens
d = Map(function(df, sc) {
    df$scene = sc
    df
}, b$pop_data, b$gens)
d = rbindlist(d)

ggplot(d)+
    geom_jitter(
        aes(as.factor(scene),
            associations,
            colour = factor(trait))
    )

# melt
data = melt(d, id.vars = c("trait", "scene"))

data_summary = data[,.(mean = mean(value), 
          sd = sd(value)), 
    by = c("scene", "trait", "variable")]

ggplot(data_summary)+
    geom_pointrange(
        aes(
            scene, mean,
            ymin = mean - sd,
            ymax = mean + sd,
            colour = factor(trait)
    ),
    position = position_dodge(width = 0.2)
)+
facet_wrap(~variable, scales = "free")
