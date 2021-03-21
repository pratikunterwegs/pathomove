#' Write parameters as CSV.
#'
#' @param popsize Max population size.
#' @param tmax Timesteps.
#' @param replicates How many replicates.
#' @param which_file Which file to write to.
#' @param genmax Max generation.
#' @param foodClusters How many clusters.
#' @param clusterDispersal How spread out food is.
#'
#' @return Writes a parameter file.
#' @export
#'
make_parameter_file <- function(
                                genmax = 1000,
                                popsize = 1000,
                                tmax = 100,
                                foodClusters = seq(1, 10),
                                clusterDispersal = seq(1, 5),
                                replicates = 10,
                                which_file = "this_file.csv") {

  # make combinations
  parameters <- data.table::CJ(
    generations,
    popsize,
    tmax,
    clusters,
    spread,
    replicate = seq(replicates)
  )

  # write file
  data.table::fwrite(
    parameters,
    file = which_file
  )
}
