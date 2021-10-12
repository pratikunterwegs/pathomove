/// functions for network operations

#include "network.hpp"
#include <Rcpp.h>

/// function for dataframe from rcpp matrix
Rcpp::DataFrame Network::getNtwkDf() {
	std::vector<int> focal;
	std::vector<int> subfocal;
	std::vector<int> edgeWeight;

	for (int n = 0; n < nVertices; ++n)
	{
		focal.push_back(n);
		for(int m = n+1; m < nVertices; ++m) {
			focal.push_back(n);
			subfocal.push_back(m);
			edgeWeight.push_back(adjMat(n, m));
		}
	}

	// make df
	Rcpp::DataFrame ntwkDf = Rcpp::DataFrame::create(
		Named("focal") = focal,
		Named("target") = subfocal,
		Named("assoc") = edgeWeight
	);

	return ntwkDf;
}

/// function to get metrics from adjacency matrix
// std::vector<float> Network::ntwkMeasures() {
//     /// including R environment with the igraph package
//     Rcpp::Environment igraph("package:igraph");
//     Rcpp::Function graph_diameter = igraph["diameter"];
//     Rcpp::Function graph_from_adjmat = igraph["graph.adjacency"];
//     Rcpp::Function network_size = igraph["gsize"];


//     SEXP graph = graph_from_adjmat(adjMat, Rcpp::Named("diag")=false, Rcpp::Named("weighted")=true);

//     SEXP diameter = graph_diameter(graph, Rcpp::Named("directed")=false);
//     SEXP size = network_size(graph);

//     Rcpp::NumericVector vDiam (diameter);
//     Rcpp::NumericVector vSize (size);

//     std::vector<float> measures { 
//         static_cast<float>(vDiam[0]), // diameter
//         static_cast<float>(vDiam[0]) * (static_cast<float>(nVertices) / static_cast<float>(vSize[0])) // global efficiency
//     };

//     // Rcpp::Rcout << "pbsn diameter = " << measures[0] << "\n";

//     return measures;
// }

// /// get individual degree from data frame
// std::vector<int> Network::getDegree() {

//     /// including R environment with the igraph package
//     Rcpp::Environment igraph("package:igraph");
//     Rcpp::Function graph_degree = igraph["degree"];
//     Rcpp::Function graph_from_adjmat = igraph["graph.adjacency"];
    

//     SEXP graph = graph_from_adjmat(adjMat, Rcpp::Named("diag")=false, Rcpp::Named("weighted")=true);
    
//     SEXP degree = graph_degree(graph, Rcpp::Named("loops")=false);

//     std::vector<int> vDeg = Rcpp::as< std::vector<int> >(degree);

//     return vDeg;
// }
