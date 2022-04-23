# functions to print a summary of the output class
#' Summarise a `pathomove_output` object.
#'
#' @param x A `pathomove_output` object.
#' @param ... 
#'
#' @return Prints a summary of the `pathomove_output` object.
#' @export
#'
summary <- function(x, ...) {
  UseMethod("summary", x)
}

#' @export
#' @rdname summary
summary.pathomove_output <- function(x) {
  glue::glue(
    "Class: {methods::is(x)}
                  Scenario: {x@scenario}"
  )
}
