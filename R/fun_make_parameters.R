#' Write parameters as CSV.
#'
#' @param replicates How many replicates.
#' @param which_file Where to save these parameters.
#' @param ... Pass arguments to \code{run\_noisewalker}.
#'
#' @return Writes a parameter file.
#' @export
#'
make_parameter_file <- function(
  replicates,
  ...,
  which_file = "this_file.csv"
) {

  # make combinations
  parameters <- data.table::CJ(
    ...,
    replicate = seq(replicates)
  )

  # write file
  data.table::fwrite(
    parameters,
    file = which_file
  )
}