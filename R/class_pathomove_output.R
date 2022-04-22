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
    infections_per_gen = "integer"
  ),

  # define the default values of the slots
  prototype = list(
    scenario = NA_real_,
    n_gen = NA_real_,
    gen_patho_intro = NA_real_,
    infections_per_gen = NA_real_
  ),

  # check validity of class
  validity = check_pathomove_output
)

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

  # length_status <- length(object@status)
  # if (length_status != 1) {
  #   msg <- paste("status is length ", length_status, ". Should be 1", sep = "")
  #   errors <- c(errors, msg)
  # }
  # 
  # length_missing_species <- length(object@missing_species)
  # if (length_missing_species != 1) {
  #   msg <- paste(
  #     "missing_species is length ", length_missing_species, ". Should be 1",
  #     sep = ""
  #   )
  #   errors <- c(errors, msg)
  # }
  # 
  # length_min_age <- length(object@min_age)
  # if (length_min_age != 1) {
  #   msg <- paste(
  #     "min_age is length ", length_min_age, ". Should be 1",
  #     sep = ""
  #   )
  #   errors <- c(errors, msg)
  # }

  if (length(errors) == 0) {
    TRUE
  } else {
    errors
  }
}
