#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <cassert>
#include <Rcpp.h>

/// including R environment with the igraph package
Rcpp::Environment igraph("package:igraph");
Rcpp::Function graph_diameter = igraph["diameter"];
Rcpp::Function graph_degree = igraph["degree"];
Rcpp::Function graph_from_adjmat = igraph["graph.adjacency"];
Rcpp::Function df_from_graph = igraph["as_data_frame"]
Rcpp::Function network_size = igraph["gsize"]

/// the network structure, which holds an adjacency matrix
// network should be a member of population later
// network has funs to return network metrics and the adj matrix
struct Network {
public:
    Network(const int popsize):
        adjMat (popsize, popsize),
        graph
        
    {}
    ~Network() {}
    Rcpp::NumericMatrix adjMat;
    Rcpp::List graph;

    // functions
    void adjMat_to_graph();
    std::vector<double> ntwkMeasures();
    std::vector<int> getDegree();
    Rcpp::DataFrame returnDF();
};

#endif // NETWORK_H
