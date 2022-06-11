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

    ggplot2::ggplot(
      temp_df_,
      ggplot2::aes(
        x = gen, y = infections
      )
    ) +
      ggplot2::geom_line(
        size = 0.3,
        col = "grey30"
      ) +
      ggplot2::geom_point(
        shape = 16
      ) +
      ggplot2::geom_vline(
        xintercept = gen_patho_intro,
        col = "indianred",
        lty = 3
      ) +
      ggplot2::labs(
        x = "Generations",
        y = "# individuals infected"
      ) +
      ggplot2::theme_classic()
  }
}
