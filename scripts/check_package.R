
library(snevo)
password = readLines("data/password.txt")

# check cluster
snevo::check_prepare_cluster(
  ssh_con = "p284074@peregrine.hpc.rug.nl",
  password = password
)

# prepare parameters
snevo::make_parameter_file(
  nFood = 1000,
  popsize = 3000,
  landsize = c(100, 300),
  genmax = 5000,
  foodClusters = 100,
  clusterDispersal = 2,
  collective = c(TRUE, FALSE),
  competitionCost = 0.1,
  nScenes = 10,
  replicates = 25,
  which_file = "data/parameters/parameters_full.csv"
)

# try sending in a job
snevo::use_cluster(
  ssh_con = "p284074@peregrine.hpc.rug.nl",
  password = password, 
  script = "scripts/check_sim_cluster.R", 
  template_job = "bash/main_job_maker.sh", 
  parameter_file = "data/parameters/parameters_full.csv"
)
