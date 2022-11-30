#' Assign a social strategy
#'
#' @param df The dataframe with agent id and social weights.
#'
#' @return A dataframe with social strategy assigned.
#' @export
#' @import data.table
#'
get_social_strategy <- function(df) {
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

#' Get functional variation in movement weights.
#'
#' @param df A dataframe with generation, id, and movement weights.
#'
#' @return Nothing. Transforms weights by reference. See data.table.
#' @export
get_functional_variation <- function(df) {
  data.table::setDT(df)

  assertthat::assert_that(
    all(
      c("sF", "sH", "sN", "gen") %in% colnames(df)
    )
  )

  # transform weights
  df[, c("sF", "sH", "sN") := lapply(.SD, function(x) {
    x / (abs(sF) + abs(sH) + abs(sN))
  }),
  .SDcols = c("sF", "sH", "sN")
  ]
}

#' Importance of social strategy.
#'
#' @param df The dataframe with agent id and social weights.
#'
#' @return A dataframe with social strategy importance.
#' @export
#' @import data.table
#'
get_si_importance <- function(df) {
  assertthat::assert_that(
    all(c("sH", "sN") %in% names(df)),
    msg = "get_social_strat: data does not have social weights"
  )
  data.table::setDT(df)
  get_functional_variation(df)
  df[, si_imp := abs(sH) + abs(sN)]
}

#' Get agent avoidance.
#'
#' @param df A dataframe with scaled decision weights.
#'
#' @return A column of agent avoidance value, where agent avoidance is
#' the sum of negative scaled agent weights.
#' @export
get_agent_avoidance <- function(df) {
  assertthat::assert_that(
    all(c("sH", "sN") %in% names(df)),
    msg = "get_agent_avoidance: data does not have social weights"
  )
  data.table::setDT(df)
  d_ <- copy(df)
  # avoidance is sum of negative agent weights
  d_[, agent_avoidance := (fifelse(sH < 0, sH, 0) + fifelse(sN < 0, sN, 0)) /
    (abs(sH) + abs(sN) + abs(sF))]
  # assign to original df, modified by reference
  df[, agent_avoidance := d_$agent_avoidance]
}
