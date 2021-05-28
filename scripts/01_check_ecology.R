# check function
Rcpp::compileAttributes()
devtools::build()
sink(file = "install_output.log"); devtools::install(); sink()

library(snevo)
library(ggplot2)
library(data.table)

a = snevo::do_eco_sim(
    popsize = 50,
    landsize = 1000,
    nFood = 10,
    nClusters = 1,
    clusterDispersal = 8,
    maxAct = 0.01,
    activityRatio = 0.2,
    pInactive = 0.5,
    collective = FALSE,
    sensoryRange = 1.0,
    stopTime = 3,
    tmax = 100,
    scenes = 1
)
names(a)

b = a[["movedata"]]
b = Map(function(l, t) {
    l$time = t
    l
}, b$move_data, seq(length(b$move_data)))
b = rbindlist(b)
b
ggplot(b)+
    # geom_path(
    #     aes(x,Y,
    #         group = factor(id)),
    #     show.legend = F
    # )+
    geom_path(
        aes(x,Y, col = interaction(trait1, trait2, trait3)),
        show.legend = F,
        # size = 2,
        shape = 16)+
    facet_wrap(~id)+
    scale_colour_viridis_d(option = "H", begin = 0.2, end = 0.8)

b = a[["pbsn"]]

g = graph.adjacency(b, weighted = TRUE, mode = "undirected")
g = simplify(g, remove.loops = TRUE)
plot.igraph(g, vertex.size = 5, edge.width = 2,layout=layout_with_fr, vertex.label = NA)

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
    )

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
    position = position_dodge(width = 0.2),
    shape = 0,
    stroke = 2
)+
facet_wrap(~variable, scales = "free")
