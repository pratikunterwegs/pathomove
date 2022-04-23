#' Checks the validity of the `pathomove_output` class.
#'
#' @param object Instance of the `pathomove_output` class.
#'
#' @return Boolean or errors
#' @export
#'
check_pathomove_output <- function(object) {
  errors <- character()
  length_scenario <- length(object@scenario)
  if (length_scenario != 1) {
    msg <- sprintf("scenario is length %i, should be 1.", length_scenario)
    errors <- c(errors, msg)
  }

  if (length(errors) == 0) {
    TRUE
  } else {
    errors
  }
}

#' Defines the `pathomove_output` class.
#'
#' @slot parameters list.
#' @slot generations integer.
#' @slot infections_per_gen integer.
#' @slot trait_data list.
#'
#' @export
setClass(
  # name of the class
  Class = "pathomove_output",

  # define the types of the class
  slots = c(
    parameters = "list",
    generations = "integer",
    infections_per_gen = "integer",
    trait_data = "list"
  ),

  # define the default values of the slots
  prototype = list(
    parameters = list(),
    generations = NA_integer_,
    infections_per_gen = NA_integer_,
    trait_data = list()
  ),

  # check validity of class
  validity = check_pathomove_output
)
