# check function
Rcpp::compileAttributes()
devtools::build()
# devtools::document()
devtools::install()

library(snevo)
library(ggplot2)
library(data.table)

a = snevo::do_eco_sim(
    popsize = 500,
    landsize = 100,
    nFood = 500,
    nClusters = 256,
    clusterDispersal = 8,
    maxAct = 1,
    activityRatio = 0.5,
    pInactive = 0.5,
    collective = FALSE,
    sensoryRange = 1.0,
    stopTime = 2,
    tmax = 100.0,
    scenes = 1
)
names(a)

b = a[["pbsn"]]

library(igraph)
g = graph.adjacency(b, weighted = TRUE, mode = "undirected")
g = simplify(g, remove.loops = TRUE)
plot(g, vertex.size = 1)

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
            energy,
            colour = factor(trait)),
        shape = 1
    )+
    scale_y_log10()

# melt
data = melt(d, id.vars = c("trait", "scene"))

data_summary = data[,.(median = median(value), 
          sd = sd(value)), 
    by = c("scene", "trait", "variable")]

ggplot(data_summary)+
    geom_pointrange(
        aes(
            scene, median,
            ymin = median - sd,
            ymax = median + sd,
            colour = factor(trait)
    ),
    position = position_dodge(width = 0.2)
)+
facet_wrap(~variable, scales = "free")
