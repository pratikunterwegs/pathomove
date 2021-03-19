#' Write parameters as CSV.
#'
#' @param generations Max generations.
#' @param popsize Max population size.
#' @param tmax Timesteps.
#' @param clusters Food clusters.
#' @param spread Spread of food clusters.
#' @param replicates How many replicates.
#' @param which_file Which file to write to.
#'
#' @return Writes a parameter file.
#' @export
#'
make_parameter_file <- function(
                                generations = 1000,
                                popsize = 1000,
                                tmax = 100,
                                clusters = seq(1, 10),
                                spread = seq(1, 5),
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
