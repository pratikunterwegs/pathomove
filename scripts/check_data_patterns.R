# load libs
library(data.table)
library(ggplot2)

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
  data_tmp$trait = tanh(data_tmp$trait * 20)
  data_tmp$trait_round = plyr::round_any(data_tmp$trait, 0.01)
  data_summary = data_tmp[, .N, by = c("foodClusters", 
                                   "clusterDispersal", "replicate",
                                   "trait_round", "gen")]
})

# bind and check
data = rbindlist(data)
head(data)
# 
# # count trait by replicate and dispersal
# data[, trait_round := cut(trait, seq(0, max(trait), 0.01))]
# data_summary = data[, .N, by = c("foodClusters", 
#                                  "clusterDispersal", "replicate",
#                                  "trait_round", "gen")]

# # get low
# data_summary$trait_round = stringi::stri_extract_last(
#   data_summary$trait_round, regex = "[-0-9]+\\.\\d{2}"
# )
# data_summary$trait_round = as.numeric(data_summary$trait_round)

# plot and check
ggplot(data[gen %% 10 == 0 && trait_round < 2])+
  geom_tile(
    aes(gen, trait_round,
        fill = N)
  )+
  coord_cartesian(
    ylim = c(0, 2)
  )+
  scale_fill_distiller(
    palette = "Reds", direction = 1
  )+
  facet_wrap(~replicate, labeller = label_both)

#### check associations ####
# load and add params to data
data = lapply(file_list, function(l) {
  load(l)
  data_tmp = data_evolved_pop$pbsn
  params = data_evolved_pop[c("foodClusters", 
                              "clusterDispersal", "replicate")]
  data_tmp$foodClusters = params[["foodClusters"]]
  data_tmp$clusterDispersal = params[["clusterDispersal"]]
  data_tmp$replicate = params[["replicate"]]
  setDT(data_tmp)
})

# bind and check
data = rbindlist(data)
head(data)

# plot and check
ggplot(data)+
  geom_bar(
    aes(associations,
        group = replicate
    ),
    alpha = 0.1,
    position = position_identity()
  )+
  coord_cartesian(xlim = c(0, 10))+
  facet_grid(foodClusters~clusterDispersal)
