
#' Get network data from Rds files.
#'
#' @param datafile Rds data file.
#' @param assoc_threshold The association count threshold.
#'
#' @return A list of \code{tidygraph} objects.
#' @export
get_networks <- function(datafile, assoc_threshold = 5) {
  load(datafile)

  # scenario
  repl <- data[["replicate"]]
  cost <- data[["costInfect"]]
  regen <- data[["regen_time"]]
  infect_percent <- data[["infect_percent"]]
  dispersal <- data[["dispersal"]]

  # edgelist collection and work
  el <- data[["edgeLists"]]
  el <- el[-1] # all edgelists except first

  el <- lapply(el, function(le) {
    le <- le[le$assoc > assoc_threshold, ]
    data.table::setDT(le)
    setnames(le, c("from", "to", "weight"))
    le <- le[from != to, ]
    le$to <- le$to + 1
    le$from <- le$from + 1

    le
  })

  # handle generations
  genmax <- data[["genmax"]]
  genseq <- seq(genmax / 10, genmax, by = (genmax / 10))
  genseq[length(genseq)] <- last(genseq) - 1 # generations of edgelists

  # handle nodes
  nodes <- data[["gen_data"]][["pop_data"]]
  nodes <- nodes[data[["gen_data"]][["gens"]] %in% genseq] # id data for el

  # work on nodes
  nodes <- Map(nodes, genseq, f = function(n, g) {
    n$gen <- g
    n$id <- seq(nrow(n))
    setDT(n)

    # assign scenario etc
    n$repl <- repl
    n$cost <- cost
    n$regen <- regen
    n$infect_percent <- infect_percent
    n$dispersal <- dispersal
    assign_movement_types(n)
    get_social_strategy(n)
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
  names(g) <- genseq

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
}
