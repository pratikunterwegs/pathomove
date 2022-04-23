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
#' @slot scenario integer.
#' @slot n_gen integer.
#' @slot gen_patho_intro integer.
#' @slot infections_per_gen integer.
#'
#' @export
setClass(
  # name of the class
  Class = "pathomove_output",

  # define the types of the class
  slots = c(
    scenario = "integer",
    n_gen = "integer",
    gen_patho_intro = "integer",
    infections_per_gen = "data.frame"
  ),

  # define the default values of the slots
  prototype = list(
    scenario = NA_integer_,
    n_gen = NA_integer_,
    gen_patho_intro = NA_integer_,
    infections_per_gen = data.frame()
  ),

  # check validity of class
  validity = check_pathomove_output
)