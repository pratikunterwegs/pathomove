/// functions for network operations

#include "network.hpp"

/// make graph from adjacency matrix
void Network::adjMat_to_graph() {
    graph = graph_from_adjmat(adjMat, Named("diag")=false, Named("weighted")=true);
}

/// function to get metrics from adjacency matrix
std::vector<double> Network::ntwkMeasures() {

    SEXP diameter = graph_diameter(graph, Named("directed")=false);
    SEXP size = network_size(graph);

    Rcpp::NumericVector vDiam (diameter);
    Rcpp::NumericVector vSize (size);

    return std::vector<double> { 
        vDiam[0], // diameter
        vDiam[0] * (pop.nAgents / vSize[0]) // global efficiency
    };
}

/// get individual degree from data frame
std::vector<int> Network::getDegree() {
    
    SEXP degree = graph_degree(graph, Named("loops")=false);

    std::vector<int> vDeg = Rcpp::as< std::vector<int> >(degree);

    return vDeg;
}

/// get graph as dataframe
Rcpp::DataFrame returnDF(){
    
    return Rcpp::DataFrame( returnDF(graph) );
}