#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "parameters.h"
#include "landscape.h"
#include "agents.h"
#include "network.h"

int main()
{
    // set seed
    gsl_rng_set(r, seed);
    // init food

    std::cout << "preparing food landscape\n";
    Resources food;
    food.initResources(30, 1.5);
    food.countAvailable();

    // print to file
    std::ofstream ofs;
    ofs.open("items.csv", std::ofstream::out);
    ofs << "x,y\n";

    for (size_t i = 0; i < static_cast<size_t>(food.nItems); i++)
    {
        ofs << food.coordX[i] << "," << food.coordY[i] << "\n";
    }

    ofs.close();


    // init pop
    Population pop;
    pop.initPos(food);
    pop.setTrait();

    // position ofs
    //    std::ofstream traitofs;
    //    traitofs.open("trait.csv", std::ofstream::out);
    //    traitofs << "id,gen,trait\n";

    // pbsn ofs
    std::ofstream pbsnofs;
    pbsnofs.open("pbsn.csv", std::ofstream::out);
    pbsnofs << "gen,id1,id2,weight\n";

    std::cout << "opened ofs\n";

    int genmax = 1;

    for(int gen = 0; gen < genmax; gen++) {

        //        std::cout << "here before init assoc";
        pop.initPos(food);
        std::cout << "positions initialised\n";
        pop.pbsn.initAssociations(pop.nAgents);
        std::cout << "associations intialised\n";

        for (size_t t = 0; t < 105; t++) {

            std::cout << "timestep = " << t << "\n";

            pop.move(food);

            std::cout << "pop moved\n";
            pop.updatePbsn();

            std::cout << "updated pbsn\n\n";

            //            for (size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++)
            //            {
            //                forage(i, food, pop);
            //                food.countAvailable();
            //            }

            // decrement food counter by one
            //            for (size_t j = 0; j < static_cast<size_t>(food.nItems); j++){
            //                if(food.counter[j] > 0) {
            //                    food.counter[j] --;
            //                }

            //            }
        }

        //        if(gen % 10 == 0) {
        //            std::cout << "gen = " << gen << "\n";

        //            // print evolved pop
        ////            for(size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++){
        ////                std::cout << i << "\n";
        ////                traitofs << i << ","
        ////                        << gen << ","
        //////                        << pop.coordX[i] << ","
        //////                        << pop.coordY[i] << ","
        //////                        << pop.energy[i] << ","
        ////                        << pop.trait[i] << "\n";
        ////            }
        //        }

        if(gen > (genmax -10)) {

            // print pbsn
            for(size_t i = 0; i < (pop.nAgents - 1); i++) {
                for(size_t j = 0; j < pop.pbsn.associations[i].size(); j++) {

                    if(pop.pbsn.associations[i][j] > 0) {
                        pbsnofs << gen << ","
                                << i << ","
                                << j + 1 << ","
                                << pop.pbsn.associations[i][j] << "\n";
                    }

                }
            }
        }

        // reproduce
        pop.Reproduce();

    }

    //    traitofs.close();
    pbsnofs.close();
    std::cout << "done";
}
