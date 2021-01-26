#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "parameters.h"
#include "landscape.h"
#include "agents.h"
#include "network.h"

/// construct agent output filename
std::vector<std::string> identifyOutpath(const int clusters,
                                         const double dispersal){
    // assumes path/type already prepared
    std::string path = "data/";
    // output filename as milliseconds since epoch
    // std::string output_id = std::to_string(static_cast<long>(gsl_rng_uniform_int(r, 10000)));
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch();

    // add a random number to be sure of discrete values
    long duration = value.count() + static_cast<long>(gsl_rng_uniform_int(r, 10000));
    std::string output_id = std::to_string(duration);
    // output_id = output_id + "_f" + std::to_string(frequency) + "ft" +
    // std::to_string(frequencyTransfer) + "rep" + rep;

    // write summary with filename to agent data
    // and parameter files
    // start with output id
    const std::string summary_out = path + "/lookup.csv";
    std::ofstream summary_ofs;

    // if not exists write col names
    std::ifstream f2(summary_out.c_str());
    if (!f2.good()) {
        summary_ofs.open(summary_out, std::ofstream::out);
        summary_ofs << "filename,clusters,dispersal\n";
        summary_ofs.close();
    }
    // append if not
    summary_ofs.open(summary_out, std::ofstream::out | std::ofstream::app);
    summary_ofs << output_id << ","
                << clusters << ","
                << dispersal << "\n";
    summary_ofs.close();

    return std::vector<std::string> {path, output_id};
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

void evolve_pop(int genmax, int tmax,
                Population &pop, Resources &food)
{
    // set seed
    gsl_rng_set(r, seed);

    for(int gen = 0; gen < genmax; gen++) {

        pop.initPos(food);
        Network pbsn;
        pbsn.initAssociations(pop.nAgents);
        for (int t = 0; t < tmax; t++) {

            pop.move(food);
            pop.updatePbsn(pbsn);


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
    std::cout << "done evolving";
}

void do_simulation(int genmax, int tmax, int foodClusters, double clusterDispersal) {

    // prepare output paths etc
    identifyOutpath(foodClusters, clusterDispersal);

    // prepare landscape
    Resources food;
    food.initResources(foodClusters, clusterDispersal);
    food.countAvailable();

     /// export landscape

    // prepare population
    Population pop;
    pop.setTrait();

    // evolve population
    evolve_pop(genmax, tmax, pop, food);
}
