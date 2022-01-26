args = commandArgs(TRUE)

message(getwd())

param_file = args[1]

row_n = as.numeric(args[2])

message(
  paste("which file = ", param_file)
)

message(
  paste("which row = ", row_n)
)

params = read.csv(param_file)

library(pathomove)

# run simulation
data = pathomove::run_pathomove(
  scenario = params$scenario[row_n],

  popsize = params$popsize[row_n],
  
  nItems = params$popsize[row_n],
  landsize = params$landsize[row_n],
  nClusters = params$nClusters[row_n],
  clusterSpread = params$clusterSpread[row_n],

  tmax = params$tmax[row_n],
  genmax = params$genmax[row_n], 
  g_patho_init = params$g_patho_init[row_n],
  
  range_food = params$range_food[row_n],
  range_agents = params$range_agents[row_n], 
  handling_time = params$handling_time[row_n],

  regen_time = params$regen_time[row_n],
  pTransmit = params$pTransmit[row_n],

  initialInfections = params$initialInfections[row_n],
  costInfect = params$costInfect[row_n],
  nThreads = params$nThreads[row_n]
)

# get params as named vector
these_params = unlist(params[row_n,])

# append list of params
data = append(
  data,
  these_params
)

output_file_index = params$ofi[row_n]

# name of rdata file
output_file = glue::glue(
  'data/output/{output_file_index}.Rds'
)

# save
save(
  data,
  file = output_file
)
