#ifndef NETWORK_OPERATIONS_HPP
#define NETWORK_OPERATIONS_HPP

// #include "network.h"
// #include <Rcpp.h>
// #include "agents.h"

// Rcpp::Environment igraph("package:igraph");
// Rcpp::Function graph_from_df = igraph["graph.data.frame"];
// Rcpp::Function graph_diameter = igraph["diameter"];
// Rcpp::Function graph_degree = igraph["degree"];
// Rcpp::Function graph_from_adjmat = igraph["graph.adjacency"];

// /// function to make igraph network from pbsn
// std::vector<double> networkMeasures(Network &pbsn, Population &pop) {

//     Rcpp::DataFrame pbsn_data = returnPbsn(pop, pbsn);

//     List graph = graph_from_df(pbsn_data, Named("directed")=false);

//     SEXP diameter = graph_diameter(graph, Named("directed")=false);

//     Rcpp::NumericVector vDiam (diameter);

//     return std::vector<double> { static_cast<double>(pbsn_data.nrows()), // interactions
//         vDiam[0], // diameter
//         vDiam[0] * (pop.nAgents / static_cast<double>(pbsn_data.nrows())) // global efficiency
//     };
// }

// std::vector<int> getDegree(Network &pbsn) {
//     // Rcpp::DataFrame pbsn_data = returnPbsn(pop, pbsn);
//     List graph = graph_from_adjmat(pbsn.adjacencyMatrix, Named("diag")=false, Named("weighted")=true);

//     SEXP degree = graph_degree(graph, Named("loops")=false);

//     std::vector<int> vDeg = Rcpp::as< std::vector<int> >(degree);

//     return vDeg;
// }

#endif // NETWORK_OPERATIONS_HPP
