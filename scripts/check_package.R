
library(snevo)
password = readLines("data/password.txt")

# check cluster
snevo::check_prepare_cluster(
  ssh_con = "p284074@peregrine.hpc.rug.nl",
  password = password
)

# prepare parameters
snevo::make_parameter_file(
  genmax = 10000,
  foodClusters = c(1),
  clusterDispersal = seq(5),
  replicates = 10,
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
