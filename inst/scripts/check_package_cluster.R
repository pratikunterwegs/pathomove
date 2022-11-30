
library(pathomove)
password <- readLines("data/password")

# check cluster
# pathomove::check_prepare_cluster(
#   ssh_con = "p284074@peregrine.hpc.rug.nl",
#   password = password
# )

# prepare parameters
pathomove::make_parameter_file(
  scenario = 2,
  popsize = 500,
  nItems = 1800,
  landsize = 60,
  nClusters = 60,
  clusterSpread = 1,
  tmax = 300,
  genmax = 1000,
  g_patho_init = 700,
  range_food = 1,
  range_agents = 1,
  range_move = 1,
  handling_time = 5,
  regen_time = c(20, 50, 100),
  pTransmit = "0.05",
  initialInfections = 20,
  costInfect = c(0.005, 0.01, 0.02),
  multithreaded = TRUE,
  replicates = 1,
  which_file = "data/parameters/parameters_test.csv"
)

# try sending in a job
pathomove::use_cluster(
  ssh_con = "p284074@peregrine.hpc.rug.nl",
  password = password,
  folder = "snevo",
  script = "scripts/do_sim_cluster.R",
  template_job = "bash/main_job_maker.sh",
  parameter_file = "data/parameters/parameters_test.csv"
)
