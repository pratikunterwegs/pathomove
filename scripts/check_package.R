
library(snevo)
password = readLines("data/password.txt")

# check cluster
snevo::check_prepare_cluster(
  ssh_con = "p284074@peregrine.hpc.rug.nl",
  password = password
)
