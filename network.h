#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <cassert>

struct Network {
public:
    Network():
        nAgents(100),
        associations(nAgents - 1)
    {}
    ~Network() {}

    int nAgents;
    std::vector<std::vector<int> > associations;

    void initAssociations(int nVertices);
};

// initialise associations
void Network::initAssociations(int nVertices){

    nAgents = nVertices;
    for(size_t i = 0; i < static_cast<size_t>(nAgents - 1); i ++) {
        associations[i] = (std::vector<int> (nAgents - (i + 1), 0));
    }

    // check size
    assert(static_cast<int>(associations[0].size()) == (nAgents - 1) && "association triangle is wrong");
}

#endif // NETWORK_H
