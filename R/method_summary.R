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
    print(
      glue::glue(
        "Scenario: {object@parameters$scenario}
        Pathogen introduced in gen: {object@parameters$gen_patho_intro}
        Popsize: {object@parameters$pop_size}
        "
      )
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
    utils::str(
      object,
      max.level = 3
    )
  }
)
