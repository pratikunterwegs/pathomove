#' Write parameters as CSV.
#'
#' @param replicates How many replicates.
#' @param which_file Where to save these parameters.
#' @param ... Pass arguments to \code{run\_noisewalker}.
#'
#' @return Writes a parameter file.
#' @export
#'
make_parameter_file <- function(replicates,
                                ...,
                                which_file = "this_file.csv") {

  # make combinations
  parameters <- data.table::CJ(
    ...,
    replicate = seq(replicates)
  )

  # assign unique id called ofi
  parameters$ofi <- as.integer(Sys.time()) + seq(nrow(parameters))

  # write file
  data.table::fwrite(
    parameters,
    file = which_file
  )
}
