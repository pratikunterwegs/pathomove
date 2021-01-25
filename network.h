#ifndef NETWORK_H
#define NETWORK_H

#include <vector>

struct Network {
public:
    Network():
        nAgents(100)
    {}
    ~Network() {}

    int nAgents;
    std::vector<std::vector<int> > associations;

    void initAssociations(int nVertices);
};

// initialise associations
void Network::initAssociations(int nVertices){

    nAgents = nVertices;
    for(size_t i = 0; nAgents - (static_cast<int>(i) + 1) > 0; i ++) {
        associations.push_back(std::vector<int> (nAgents - (static_cast<int>(i) + 1), 0));
    }
}

#endif // NETWORK_H
