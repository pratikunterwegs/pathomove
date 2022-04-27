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
