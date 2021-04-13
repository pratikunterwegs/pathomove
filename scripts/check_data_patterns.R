# load libs
library(data.table)
library(ggplot2)
library(colorspace)
library(viridisLite)

# list files and read
file_list <- list.files("data/output/", pattern = "Rdata", full.names = T)

# load and add params to data
data <- lapply(file_list, function(l) {
  load(l)
  data_tmp <- data_evolved_pop$trait_data$pop_data
  gens <- data_evolved_pop$trait_data$gens
  data_tmp <- Map(function(df, g) {
    df$gen <- g
    df
  }, data_tmp, gens)
  params <- data_evolved_pop[c(
    "foodClusters",
    "clusterDispersal", "replicate"
  )]
  data_tmp <- lapply(data_tmp, data.table::setDT)

  invisible(
    lapply(data_tmp, function(df) {
      df[, `:=`(
        foodClusters = params[["foodClusters"]],
        clusterDispersal = params[["clusterDispersal"]],
        replicate = params[["replicate"]]
      )]
    })
  )
  data_tmp <- data.table::rbindlist(data_tmp)[, energy := NULL]
  data_tmp$trait <- log10(data_tmp$trait)
  data_tmp$trait_round <- plyr::round_any(data_tmp$trait, 0.02)
  data_summary <- data_tmp[, list(
    .N,
    mean_assoc = mean(associations)
  ), by = c(
    "foodClusters",
    "clusterDispersal", "replicate",
    "trait_round", "gen"
  )]
})

# bind and check
data <- rbindlist(data)
head(data)
data[, prop := N / sum(N), by = c(
  "replicate", "gen",
  "foodClusters", "clusterDispersal"
)]

# count morphs
data_morphs <- data[prop > 0.01, .N, by = c(
  "foodClusters", "clusterDispersal",
  "replicate", "gen"
)]

# plot associations by trait
ggplot(data[replicate == 1,])+
  geom_tile(
    aes(gen, trait_round,
        fill = prop)
  )+
  facet_grid(
    foodClusters ~ clusterDispersal
  )

# make plot of morphs
plot_morphs <-
  ggplot(data_morphs[gen %% 10 == 0]) +
  geom_hline(
    yintercept = 2,
    colour = "red"
  ) +
  geom_jitter(
    aes(gen, N,
      group = interaction(replicate)
    ),
    colour = "steelblue",
    alpha = 0.5,
    # size = 0.3,
    shape = 1
  ) +
  scale_x_log10() +
  theme_classic() +
  coord_cartesian(
    xlim = c(10, 500),
    ylim = c(0, 50)
  ) +
  theme(
    strip.background = element_blank(),
    strip.text = element_text(face = "italic", hjust = 0)
  ) +
  facet_grid(foodClusters ~ clusterDispersal,
    labeller = label_both,
    # labeller = label_wrap_gen(multi_line=FALSE),
    scale = "free"
  ) +
  labs(
    x = "Generation",
    y = "Morphs (traits > 0.5%)"
  )

# save figures
ggsave(plot = plot_morphs, filename = "figures/figure_morphs.png")

# save data
fwrite(data_morphs, file = "data/results/data_morphs.csv")

#### check associations ####
# load and add params to data
data_network <- lapply(file_list, function(l) {
  load(l)
  data_tmp <- data_evolved_pop$network_measures
  params <- data_evolved_pop[c(
    "foodClusters",
    "clusterDispersal", "replicate"
  )]
  data_tmp$foodClusters <- params[["foodClusters"]]
  data_tmp$clusterDispersal <- params[["clusterDispersal"]]
  data_tmp$replicate <- params[["replicate"]]
  setDT(data_tmp)
})

# bind and check
data_network <- rbindlist(data_network)
head(data_network)
# data_network = melt(data_network,
#                     id.vars = c("gen", "foodClusters",
#                                 "clusterDispersal", "replicate"))

# plot and check
ggplot(
  data_network,
  aes(
    x = gen, y = global_efficiency,
    group = replicate
  )
) +
  geom_line(
    colour = "steelblue"
  ) +
  # scale_x_sqrt() +
  theme_test() +
  coord_cartesian(
    # xlim = c(0, 100)
    # ylim = c(0, 25)
  ) +
  theme(
    strip.background = element_blank(),
    strip.text = element_text(face = "italic", hjust = 0)
  ) +
  facet_grid(foodClusters ~ clusterDispersal,
    labeller = label_both,
    # labeller = label_wrap_gen(multi_line=FALSE),
    scale = "free"
  )

# link network and morphs
data_total = merge(
  data_morphs, data_network
)

ggplot(data_total[gen > 900,])+
  geom_boxplot(
    aes(factor(N), global_efficiency,
        colour = gen),
    shape = 1
  )+
  scale_colour_viridis_c(
    option = "H"
  )+
  facet_grid(foodClusters ~ clusterDispersal,
             labeller = label_both,
             # labeller = label_wrap_gen(multi_line=FALSE),
             scale = "free"
  )
