args = commandArgs(TRUE)

message(getwd())

param_file = args[1]

row_n = as.numeric(args[2])

params = read.csv(param_file)

library(snevo)

# run simulation
data_evolved_pop = do_simulation(
  popsize = params$popsize[row_n],
  genmax = params$genmax[row_n], 
  tmax = params$tmax[row_n], 
  foodClusters = params$foodClusters[row_n], 
  clusterDispersal = params$clusterDispersal[row_n],
  landsize = 100
)

# get params as named vector
these_params = unlist(params[row_n,])

# append list of params
data_evolved_pop = append(
  data_evolved_pop,
  params = these_params
)

# name of rdata file
output_file = Reduce(
  paste,
  mapply(
    function(p, np) {
      paste(np, p, sep = "_")
    }, these_params, names(these_params)
  )
)
output_file = glue::glue(
  'data/output/{output_file}.Rdata'
)

# save
save(
  data_evolved_pop,
  file = output_file
)
