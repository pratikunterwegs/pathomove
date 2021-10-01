
library(snevo)
password = readLines("data/password")

# check cluster
# snevo::check_prepare_cluster(
#   ssh_con = "p284074@peregrine.hpc.rug.nl",
#   password = password
# )

# prepare parameters
snevo::make_parameter_file(
  scenario = 1,
  popsize = 500,
  nItems = 2000,
  landsize = 60,
  nClusters = 30,
  clusterSpread = 1,
  tmax = 100,
  genmax = 100,
  range_food = 0.5,
  range_agents = 1,
  handling_time = 5,
  regen_time = 3,
  pTransmit = 0.0001,
  initialInfections = 2,
  costInfect = 0.05,
  nThreads = 2,
  replicates = 3,
  which_file = "data/parameters/parameters_test.csv"
)

# try sending in a job
snevo::use_cluster(
  ssh_con = "p284074@peregrine.hpc.rug.nl",
  password = password, 
  script = "scripts/do_sim_cluster.R", 
  template_job = "bash/main_job_maker.sh", 
  parameter_file = "data/parameters/parameters_test.csv"
)
