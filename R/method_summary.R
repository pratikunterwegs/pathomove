# methods to summarise and show pathomove_output class objects

#' Summarise a `pathomove_output` object.
#'
#' @name summary.pathomove_output
#' @docType methods
#' @rdname summary
#'
setMethod(
  "summary", signature(object = "pathomove_output"),
  function(object) {
    glue::glue(
      "Scenario: {object@parameters$scenario}
              Popsize: {object@parameters$popsize}
              Pathogen introduced in gen: {object@parameters$gen_patho_intro}"
    )
  }
)


#' Show the structure of a `pathomove_output` object.
#'
#' @name str.pathomove_output
#' @docType methods
#' @rdname str
#'
setMethod(
  "str", signature(object = "pathomove_output"),
  function(object) {
    str(
      object, max.level = 3
    )
  }
)
