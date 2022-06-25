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
#' @slot agent_parameters list.
#' @slot eco_parameters list.
#' @slot generations integer.
#' @slot gens_patho_intro integer.
#' @slot infections_per_gen integer.
#' @slot trait_data list.
#' @slot edge_list list.
#' @slot gens_edge_list list.
#'
#' @export
setClass(
  # name of the class
  Class = "pathomove_output",

  # define the types of the class
  slots = c(
    agent_parameters = "list",
    eco_parameters = "list",
    generations = "integer",
    gens_patho_intro = "integer",
    infections_per_gen = "integer",
    trait_data = "list",
    edge_lists = "list",
    gens_edge_lists = "integer"
  ),

  # define the default values of the slots
  prototype = list(
    agent_parameters = list(),
    eco_parameters = list(),
    generations = NA_integer_,
    gens_patho_intro = NA_integer_,
    infections_per_gen = NA_integer_,
    trait_data = list(),
    edge_lists = list(),
    gens_edge_lists = NA_integer_
  ),

  # check validity of class
  validity = check_pathomove_output
)
