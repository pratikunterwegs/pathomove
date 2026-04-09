#' Get network data from Rds files.
#'
#' @param output A `pathomove_output` object.
#' @param assoc_threshold The association count threshold.
#'
#' @return A list of \code{tidygraph} objects.
#' @export
get_networks <- function(output, assoc_threshold = 5) {
  # set variables to NULL
  from <- NULL
  to <- NULL

  agent_parameters <- output@agent_parameters
  eco_parameters <- output@eco_parameters

  # edgelist collection and work
  el <- output@edge_lists

  el <- lapply(el, function(le) {
    le <- le[le$assoc > assoc_threshold, ]
    data.table::setDT(le)
    data.table::setnames(le, c("from", "to", "weight"))
    le <- le[from != to, ]
    le$to <- le$to + 1
    le$from <- le$from + 1

    le
  })

  # handle nodes when there are more node data than edgelists, as with default
  nodes <- output@trait_data
  nodes <- nodes[output@generations %in% output@gens_edge_lists] # id data fr el

  # handle if there are more edge lists than node lists
  if (length(output@generations) < length(output@gens_edge_lists)) {
    keep_indices <- output@gens_edge_lists %in%
      output@generations
    output@gens_edge_lists <- output@gens_edge_lists[keep_indices]
    el <- el[keep_indices]
  }

  # work on nodes
  nodes <- Map(nodes, output@gens_edge_lists, f = function(n, g) {
    n$gen <- g
    n$id <- seq_len(nrow(n))
    data.table::setDT(n)

    # add simulation parameter data
    n[, names(agent_parameters) := agent_parameters]
    n[, names(eco_parameters) := eco_parameters]

    n <- get_social_strategy(n)
    n
  })

  assertthat::assert_that(
    length(el) == length(nodes),
    msg = "make networks: nodes and edgelists have different lengths"
  )

  # make tidygraph objects
  g <- Map(nodes, el, f = function(n, edges) {
    tidygraph::tbl_graph(
      nodes = n,
      edges = edges,
      directed = FALSE
    )
  })
  names(g) <- output@gens_edge_lists

  g
}

#' Handle SIR data.
#'
#' @param data A list of SIR model replicates, output of the \code{igraph::sir}
#' function.
#' @param digits Pass digits to round function.
#'
#' @return A data.table of SIR data.
#' @export
handle_sir_data <- function(data, digits = 1) {
  # set variables to NULL
  time <- NULL
  times <- NULL
  agents <- NULL

  d <- lapply(data, data.table::as.data.table)
  d <- Map(d, seq_along(d), f = function(data, repl) {
    data$repl <- repl
    data
  })
  d <- data.table::rbindlist(d)
  d <- data.table::melt(
    d,
    id.vars = c("times", "repl"),
    value.name = "agents",
    variable.name = "class"
  )
  d[, time := round(times, digits)]

  # summarise over time bins
  d <- d[, list(mean = mean(agents)), by = c("time", "class", "repl")]
  d
}

#' Get pathogen transmission chains
#'
#' @param df A dataframe with individual data, typically part of a
#' `pathomove_output` object.
#'
#' @return A `tidygraph` object with a directed network showing which individual
#' was infected by which other individual, for all infected individuals, except
#' the index cases (initial infections).
#' @export
get_transmission_chain <- function(df) {
  df$id <- seq_len(nrow(df))
  assertthat::assert_that(
    all(c("id", "src_infect") %in% colnames(df)),
    msg = "Data does not have infection source and agent id"
  )
  tidygraph::tbl_graph(
    nodes = df,
    edges = df[
      !(is.na(df$src_infect) | (df$src_infect <= 0)),
      c("src_infect", "id")
    ],
    directed = TRUE,
    node_key = "id"
  )
}

#' Get transmission chain sizes
#'
#' @param graph An object inheriting from class `igraph`, typically the output
#' of `get_transmission_chains`.
#'
#' @return A vector of transmission chain sizes.
#' @export
get_chain_size <- function(graph) {
  g_ <- igraph::decompose(graph)
  unlist(lapply(g_, igraph::ecount))
}
