#ifndef NETWORK_H
#define NETWORK_H

// #include <vector>
// #include <cassert>
// #include <Rcpp.h>
// struct Network {
// public:

//     int nAgents = 0;
//     std::vector<std::vector<int> > associations;
//     Rcpp::NumericMatrix adjacencyMatrix;

//     void initAssociations(int nVertices);
//     void initAdjMat (int nVertices);
// };

// // initialise associations
// void Network::initAssociations(int nVertices){

//     nAgents = nVertices;
//     associations = std::vector<std::vector<int> > (nAgents, std::vector<int> (nAgents, 0)); // a square-ish matrix
//     for(size_t i = 0; i < static_cast<size_t>(nAgents); i ++) {
//         associations[i] = (std::vector<int> (nAgents - (i), 0));
//     }
//     // check size along top
//     assert(static_cast<int>(associations[0].size()) == (nAgents - 1) && "association triangle is wrong");
// }

// void Network::initAdjMat (int nVertices) {
//     adjacencyMatrix = Rcpp::NumericMatrix (nVertices, nVertices);
// }

#endif // NETWORK_H
