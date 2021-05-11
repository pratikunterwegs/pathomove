# script to replicate simulations for homogeneous populations

# load libraries
library(data.table)
library(snevo)

# make parameter combinations
# select landscapes of size 100, 500 items, 16 clusters
# cluster dispersal is 2, 4, 8
params = CJ(
  popsize = c(250L, 500L, 1000L),
  landsize = 100,
  nFood = 500L,
  nClusters = 16L,
  cluster_dispersal = c(2, 4, 8),
  max_activity = c(0.12, seq(0.2, 1, 0.2)),
  activity_ratio = 0.98,
  p_inactive = 0.5,
  collective = FALSE,
  sensory_range = c(0.1, 1, 2),
  stop_time = c(0.5, 1.0, 2.0),
  t_max = c(10, 25, 100),
  scenes = 10L
)

# go over parameters and save data
invisible(
  lapply(seq(nrow(params)), function(row) {
    data = do_eco_sim(
      popsize = params[row]$popsize,
      landsize = params[row]$landsize,
      nFood = params[row]$nFood,
      nClusters = params[row]$nClusters,
      clusterDispersal = params[row]$cluster_dispersal,
      maxAct = params[row]$max_activity,
      activityRatio = params[row]$activity_ratio,
      pInactive = params[row]$p_inactive,
      collective = params[row]$collective,
      sensoryRange = params[row]$sensory_range,
      stopTime = params[row]$stop_time,
      tmax = params[row]$t_max,
      scenes = params[row]$scenes
    )
    
    these_params = params[row, ]
    
    to_save = list(
      data = data,
      sim_params = these_params
    )
    
    # save data
    save(
      to_save,
      file = sprintf("data/output/data_sim_%i.Rdata", row)
    )
    
  } )
)
