/// functions for network operations

#include "network.hpp"
#include <Rcpp.h>

/// including R environment with the igraph package
Rcpp::Environment igraph("package:igraph");
Rcpp::Function graph_diameter = igraph["diameter"];
Rcpp::Function graph_degree = igraph["degree"];
Rcpp::Function graph_from_adjmat = igraph["graph.adjacency"];
Rcpp::Function df_from_graph = igraph["as_data_frame"];
Rcpp::Function network_size = igraph["gsize"];

/// make graph from adjacency matrix
void Network::adjMat_to_graph() {
    graph = graph_from_adjmat(adjMat, Rcpp::Named("diag")=false, Rcpp::Named("weighted")=true);
}

/// function to get metrics from adjacency matrix
std::vector<float> Network::ntwkMeasures() {

    SEXP diameter = graph_diameter(graph, Rcpp::Named("directed")=false);
    SEXP size = network_size(graph);

    Rcpp::NumericVector vDiam (diameter);
    Rcpp::NumericVector vSize (size);

    std::vector<float> measures { 
        vDiam[0], // diameter
        vDiam[0] * (nVertices / vSize[0]) // global efficiency
    };

    return measures;
}

/// get individual degree from data frame
std::vector<int> Network::getDegree() {
    
    SEXP degree = graph_degree(graph, Rcpp::Named("loops")=false);

    std::vector<int> vDeg = Rcpp::as< std::vector<int> >(degree);

    return vDeg;
}

/// get graph as dataframe
Rcpp::DataFrame Network::returnDF(){
    
    return Rcpp::DataFrame( df_from_graph(graph) );
}