# code to check landscapes

library(snevo)
library(data.table)
library(ggplot2)

# make parameters
params = CJ(
    landsize = 100.0,
    nItems = 500L,
    nClusters = as.integer(2 ^ seq(4, 8, 1)),
    clusterDispersal = 2 ^ seq(-3, 3, 1)
)

data = copy(params)
# make landscape
landscape = Map(function(l, n, nc, cd) {
    setDT(snevo::get_test_landscape(
        nItems = n,
        landsize = l,
        nClusters = nc,
        clusterDispersal = cd
    ))
}, data$landsize, data$nItems, data$nClusters, data$clusterDispersal)

data$landscape = landscape
data$code = seq(nrow(data))

## unnest and plot
data = data[,unlist(landscape, recursive = F),
    by = c("nItems", "landsize", "nClusters", "clusterDispersal", "code")]

## plot
landscape_grid = ggplot(data)+
    geom_point(
        aes(x, y),
        shape = 1
    )+
    facet_grid(nClusters ~ clusterDispersal, 
        labeller = label_both
    )+
    coord_equal()

ggsave(plot = landscape_grid, 
    filename = "figures/fig_landscape_characters_500items.png")
