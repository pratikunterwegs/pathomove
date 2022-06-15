# methods to make simple pathomove output plots

#' Plot the outcomes of a _pathomove_ simulation.
#' @param x A `pathomove_output` object.
#' @param type What type of output to print.
#' The current option is `infections` by default, for a plot of infections in
#' each recorded generation.
#'
#' @return Shows a `ggplot` plot.
#' @export
plot_pathomove <- function(x, type = "infections") {

  # check output class
  assertthat::assert_that(
    class(x) == "pathomove_output",
    msg = "plot_pathomove: object is not a pathomove output!"
  )

  # check types and make plot
  if (type == "infections") {
    temp_df_ <- data.frame(
      gen = x@generations,
      infections = x@infections_per_gen
    )
    # when infections begin
    gen_patho_intro <- x@parameters$gen_patho_intro

    plot(gen, infections, type = "b")
    abline(v = gen_patho_init, col="red", lty=2)
}
