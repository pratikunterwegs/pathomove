# functions to conveniently handle data

#' Get trait data from _pathomove_ output.
#'
#' @param object A `pathomove_output` object.
#' @param scaled_preferences Boolean, whether to scale agents' movement
#' preferences before returning the trait (i.e., preference) data.
#'
#' @return A `data.table` with all reported generations.
#' @export
#'
get_trait_data <- function(object,
                           scaled_preferences = TRUE) {
  # setting variables to NULL
  sF <- NULL
  sH <- NULL
  sN <- NULL
  assertthat::assert_that(
    class(object) == "pathomove_output",
    msg = "pmv get traits: object is not pathomove output"
  )

  assertthat::assert_that(
    methods::.hasSlot(object = object, name = "trait_data"),
    msg = "pmv get traits: object has no 'trait_data' slot"
  )

  assertthat::assert_that(
    methods::.hasSlot(object = object, name = "generations"),
    msg = "pmv get traits: object has no 'generations' slot"
  )

  trait_data_ <- object@trait_data

  generations_ <- object@generations

  assertthat::assert_that(
    length(trait_data_) == length(generations_),
    msg = "pmv get traits: not as many trait dataframes as generations"
  )

  trait_data_ <- Map(
    trait_data_, generations_,
    f = function(td_, g_) {
      td_$gen <- g_
      data.table::as.data.table(td_)
    }
  )

  # set each to data.table
  trait_data_ <- data.table::rbindlist(trait_data_)

  # scale weights if required
  if (scaled_preferences) {
    trait_data_[, c("sF", "sH", "sN") := lapply(
      .SD, function(tr_) {
        tr_ / (abs(sF) + abs(sH) + abs(sN))
      }
    ), .SDcols = c("sF", "sH", "sN")][] # add [] for printing output
  }

  return(trait_data_)
}

#' Get sample movement data from _pathomove_ output.
#'
#' @param object A `pathomove_output` object.
#'
#' @return A `data.table` with the positions of all individuals in two
#' generations: "pre", for the generation immediately before pathogen
#' introduction (`g_patho_init` - 1 in `run_pathomove`); and "post", for the
#' last generation of the simulation (`genmax` - 1 in `run_pathomove`).
#' @export
#'
get_move_data <- function(object) {
  assertthat::assert_that(
    class(object) == "pathomove_output",
    msg = "Object must be of the class `pathomove_output`"
  )

  move_data <- object@move_data
  move_data <- Map(
    move_data,
    names(move_data),
    f = function(le, name) {
      df_ <- data.table::rbindlist(le)
      df_$type <- name
      df_
    }
  )
  data.table::rbindlist(move_data)
}
