#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "parameters.h"
#include "landscape.h"
#include "agents.h"
#include "network.h"

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

//if(gen > (genmax -10)) {

//    // print pbsn
//    for(size_t i = 0; i < static_cast<size_t>(pop.nAgents - 1); i++) {
//        for(size_t j = 0; j < pop.pbsn.associations[i].size(); j++) {

//            if(pop.pbsn.associations[i][j] > 0) {
//                pbsnofs << gen << ","
//                        << i << ","
//                        << j + 1 << ","
//                        << pop.pbsn.associations[i][j] << "\n";
//            }

//        }
//    }
//}

//void export_landscape()
//{
//    // print to file
//    std::ofstream ofs;
//    ofs.open("items.csv", std::ofstream::out);
//    ofs << "x,y\n";

//    for (size_t i = 0; i < static_cast<size_t>(food.nItems); i++)
//    {
//        ofs << food.coordX[i] << "," << food.coordY[i] << "\n";
//    }

//    ofs.close();
//}

//void export_traits() {
//    // position ofs
//    std::ofstream traitofs;
//    traitofs.open("trait.csv", std::ofstream::out);
//    traitofs << "id,gen,trait\n";
//}

//void export_pbsn(){
//    // pbsn ofs
//    std::ofstream pbsnofs;
//    pbsnofs.open("pbsn.csv", std::ofstream::out);
//    pbsnofs << "gen,id1,id2,weight\n";

//    std::cout << "opened ofs\n";
//}

void do_simulation(int genmax, int tmax, int foodClusters, double clusterDispersal)
{
    // set seed
    gsl_rng_set(r, seed);
    // init food
    Resources food;
    food.initResources(foodClusters, clusterDispersal);
    food.countAvailable();

    /// export landscape

    // init pop
    Population pop;
    pop.setTrait();

    for(int gen = 0; gen < genmax; gen++) {

        pop.initPos(food);
        pop.pbsn.initAssociations(pop.nAgents);
        for (int t = 0; t < tmax; t++) {

            pop.move(food);
            pop.updatePbsn();


            for (size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++)
            {
                forage(i, food, pop);
                food.countAvailable();
            }

            //decrement food counter by one
            for (size_t j = 0; j < static_cast<size_t>(food.nItems); j++)
            {
                if(food.counter[j] > 0) {
                    food.counter[j] --;
                }
            }
            // timestep ends here
        }
        // generation ends here
        // reproduce
        pop.Reproduce();

    }
    std::cout << "done";
}
