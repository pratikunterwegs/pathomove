# code to check landscapes

library(snevo)
library(data.table)
library(ggplot2)

landscape = snevo::get_test_landscape(
    nItems = 3000,
    landsize = 1000,
    nClusters = 100,
    clusterDispersal = 2
)

bin_size = 0.01 # 5% landscape size
bin_size = bin_size * nrow(landscape)
setDT(landscape)
landscape[, c("x", "y") := list(
    plyr::round_any(x, bin_size),
    plyr::round_any(y, bin_size)
)]
landscape = landscape[,.N, by = c("x", "y")]

# plot landscape as tiles
ggplot(landscape)+
    geom_tile(
        aes(x, y, fill = N)
    )+
    scale_fill_distiller(
        palette = "Greens"
    )
