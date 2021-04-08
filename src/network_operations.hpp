#ifndef NETWORK_OPERATIONS_HPP
#define NETWORK_OPERATIONS_HPP

#include "network.h"
#include <Rcpp.h>
#include "agents.h"

/// function to make igraph network from pbsn
std::vector<double> networkMeasures(Network &pbsn, Population &pop) {

    Rcpp::Environment igraph("package:igraph");
    Rcpp::Function graph_from_df = igraph["graph.data.frame"];
    Rcpp::Function graph_diameter = igraph["diameter"];

    Rcpp::DataFrame pbsn_data = returnPbsn(pop, pbsn);

    List graph = graph_from_df(pbsn_data, Named("directed")=false);

    SEXP diameter = graph_diameter(graph, Named("directed")=false);
    Rcpp::NumericVector vDiam(diameter);

    return std::vector<double> { static_cast<double>(pbsn_data.nrows()), 
        vDiam[0],
        vDiam[0] * (pop.nAgents / static_cast<double>(pbsn_data.nrows()))
    };
}

#endif // NETWORK_OPERATIONS_HPP
