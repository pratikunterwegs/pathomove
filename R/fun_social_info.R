#' Assign a social strategy
#'
#' @param df The dataframe with agent id and social weights.
#'
#' @return A dataframe with social strategy assigned.
#' @export
#' @import data.table
#'
get_social_strategy = function(df) {
  assertthat::assert_that(
    all(c("sH", "sN") %in% names(df)),
    msg = "get_social_strat: data does not have social weights"
  )
  data.table::setDT(df)
  df[, social_strat := data.table::fcase(
    (sH > 0 & sN > 0), "agent tracking",
    (sH > 0 & sN < 0), "handler tracking",
    (sH < 0 & sN > 0), "non-handler tracking",
    (sH < 0 & sN < 0), "agent avoiding"
  )]
}

#' Importance of social strategy.
#'
#' @param df The dataframe with agent id and social weights.
#'
#' @return A dataframe with social strategy importance.
#' @export
#' @import data.table
#'
get_si_importance = function(df) {
  assertthat::assert_that(
    all(c("sH", "sN") %in% names(df)),
    msg = "get_social_strat: data does not have social weights"
  )
  data.table::setDT(df)
  get_functional_variation(df)
  df[, si_imp := abs(sH) + abs(sN)]
  df
}