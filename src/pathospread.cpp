/// define functions for pathogen spread

#include "parameters.hpp"
#include "agents.hpp"
#include <unordered_set>

/// function to infect n individuals
void Population::popIntroPathogen(const int nInfected) {

    shufflePop();
    // loop through the intended number of infections
    for (int i = 0; i < nInfected; i++)
    {       
        // toggle infected agents boolean for infected
        pop[ order[i] ].infected = true;
        pop[ order[i] ].timeInfected = 0;        
    }
}

/// function to spread pathogen
void Population::popPathogenSpread() {

    // looping through agents, query rtree for neighbours
    for (size_t i = 0; i < pop.size(); i++)
    {
        // spread to neighbours if self infected
        if (pop[i].infected) 
        {
            // get neigbour ids
            std::vector<int> nbrsId = countAgents(
                coordX[i], coordY[i]
            ).second();

            if (nbrsId.size() > 0) 
            {
                // loop through neighbours
                for(size_t j = 0; j < nbrsId.size(); j++) 
                {
                    size_t toInfect = nbrsId[j];
                    if (!pop[toInfect].infected) 
                    {
                        // infect neighbours with prob p
                        if(pathogenTransmits(rng))
                        {
                            pop[toInfect].infected = true;
                            pop[toInfect].timeInfected = t_;
                        }
                    }
                }
            }
        }   
    }
}

/// function for pathogen cost
void Population::popPathogenCost(const float costInfect) {
    for (size_t i = 0; i < pop.size(); i++)
    {
        if(pop[i].infected) {
            pop[i].energy -= (costInfection * static_cast<float>(pop[i].timeInfected));
        }
    }
}
