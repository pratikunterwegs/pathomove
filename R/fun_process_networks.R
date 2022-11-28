
#' Get network data from Rds files.
#'
#' @param output A `pathomove_output` object.
#' @param assoc_threshold The association count threshold.
#'
#' @return A list of \code{tidygraph} objects.
#' @export
get_networks <- function(output, assoc_threshold = 5) {
  agent_parameters <- output@agent_parameters
  eco_parameters <- output@eco_parameters

  # edgelist collection and work
  el <- output@edge_lists
  el_gens <- output@gens_edge_lists

  el <- lapply(el, function(le) {
    le <- le[le$assoc > assoc_threshold, ]
    data.table::setDT(le)
    data.table::setnames(le, c("from", "to", "weight"))
    le <- le[from != to, ]
    le$to <- le$to + 1
    le$from <- le$from + 1

    le
  })

  # handle nodes
  nodes <- output@trait_data
  nodes <- nodes[output@generations %in% output@gens_edge_lists] # id data for el

  # work on nodes
  nodes <- Map(nodes, output@gens_edge_lists,
    f = function(n, g) {
      n$gen <- g
      n$id <- seq(nrow(n))
      data.table::setDT(n)

      # add simulation parameter data
      n[, names(agent_parameters) := agent_parameters]
      n[, names(eco_parameters) := eco_parameters]

      n <- get_social_strategy(n)
      n
    }
  )

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
  d <- lapply(data, data.table::as.data.table)
  d <- Map(d, seq(length(d)), f = function(data, repl) {
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
