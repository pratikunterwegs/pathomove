# load libs
library(data.table)
library(ggplot2)
library(colorspace)

# list files and read
file_list = list.files("data/output/", pattern = "Rdata", full.names = T)

# load and add params to data
data = lapply(file_list, function(l) {
  load(l)
  data_tmp = data_evolved_pop$trait_data$pop_data
  gens = data_evolved_pop$trait_data$gens
  data_tmp = Map(function(df, g) {
    df$gen = g
    df
  }, data_tmp, gens)
  params = data_evolved_pop[c("foodClusters", 
                               "clusterDispersal", "replicate")]
  data_tmp = lapply(data_tmp, data.table::setDT)
  
  invisible(
    lapply(data_tmp, function(df) {
      df[, `:=`(
        foodClusters = params[["foodClusters"]],
        clusterDispersal = params[["clusterDispersal"]],
        replicate = params[["replicate"]]
      )]
    })
  )
  data_tmp = data.table::rbindlist(data_tmp)[, energy := NULL]
  data_tmp$trait = log10(data_tmp$trait)
  data_tmp$trait_round = plyr::round_any(data_tmp$trait, 0.02)
  data_summary = data_tmp[, .N, by = c("foodClusters", 
                                   "clusterDispersal", "replicate",
                                   "trait_round", "gen")]
})

# bind and check
data = rbindlist(data)
head(data)
data[, prop := N / sum(N), by = c("replicate", "gen", 
                                  "foodClusters", "clusterDispersal")]

# count morphs
data_morphs = data[prop > 0.005, .N, by = c("foodClusters", "clusterDispersal",
                                           "replicate", "gen")]

# make plot of morphs
plot_morphs = 
  ggplot(data_morphs[gen %% 10 == 0])+
  geom_path(
    aes(gen, N,
        group = interaction(replicate)),
    colour = "steelblue"
  )+
  scale_x_log10()+
  theme_classic()+
  coord_cartesian(
    xlim = c(10, 1000),
    ylim = c(0, 50)
  )+
  theme(
    strip.background = element_blank(),
    strip.text = element_text(face = "italic", hjust = 0)
  )+
  facet_wrap(foodClusters ~ clusterDispersal, 
             labeller = label_wrap_gen(multi_line=FALSE),
             scale = "free", nrow = 3)+
  labs(x = "Generation",
       y = "Morphs (traits > 0.5%)")

# save figures
ggsave(plot = plot_morphs, filename = "figures/figure_morphs.png")

# save data
fwrite(data_morphs, file = "data/results/data_morphs.csv")

#### check associations ####
# load and add params to data
data_network = lapply(file_list, function(l) {
  load(l)
  data_tmp = data_evolved_pop$network_measures
  params = data_evolved_pop[c("foodClusters", 
                              "clusterDispersal", "replicate")]
  data_tmp$foodClusters = params[["foodClusters"]]
  data_tmp$clusterDispersal = params[["clusterDispersal"]]
  data_tmp$replicate = params[["replicate"]]
  setDT(data_tmp)
})

# bind and check
data_network = rbindlist(data_network)
head(data_network)
data_network = melt(data_network,
                    id.vars = c("gen", "foodClusters", 
                                "clusterDispersal", "replicate"))

# plot and check
ggplot(data_network[variable != "interactions",],
       aes(x = gen,
           group = interaction(variable, replicate)
       ))+
  geom_line(
    aes(y = value,
        colour = variable
    )
  )+
  scale_x_sqrt(
  )+
  theme_classic()+
  coord_cartesian(
    xlim = c(5, 250),
    ylim = c(0, 25)
  )+
  theme(
    strip.background = element_blank(),
    strip.text = element_text(face = "italic", hjust = 0)
  )+
  facet_wrap(foodClusters ~ clusterDispersal, 
             labeller = label_wrap_gen(multi_line=FALSE),
             scale = "free", nrow = 3)
