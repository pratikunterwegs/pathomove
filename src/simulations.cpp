#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "parameters.h"
#include "landscape.h"
#include "agents.h"
#include "network.h"

#include <Rcpp.h>

using namespace Rcpp;

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

// export evolved traits
void exportTraits(const int gen, Population &pop, std::vector<std::string> outputPath) {

    // trait ofs
    std::ofstream traitofs;
    traitofs.open(outputPath[0] + "trait/" + outputPath[1] + ".csv", std::ofstream::out | std::ofstream::app);

    // expect output at gen = 0 else no colnames
    if(gen > 0) {
        traitofs << "id,gen,x,y,energy,trait\n";
    }

    // loop over and write
    for(size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++){
        traitofs << i << ","
                 << gen << ","
                 << pop.coordX[i] << ","
                 << pop.coordY[i] << ","
                 << pop.energy[i] << ","
                 << pop.trait[i] << "\n";
    }
    traitofs.close();
}

// export pbsn
void exportPbsn(const int gen, Network &pbsn, std::vector<std::string> outputPath, double maxtime) {
    // trait ofs
    std::ofstream pbsnofs;
    pbsnofs.open(outputPath[0] + "pbsn/" + outputPath[1] + ".csv", std::ofstream::out | std::ofstream::app);

    // expect output at gen = 0 else no colnames
    if(gen > 0) {
        pbsnofs << "gen,id1,id2,weight\n";
    }

    // loop over and write
    for(size_t i = 0; i < pbsn.associations.size(); i++) {
        for(size_t j = 0; j < pbsn.associations[i].size(); j++) {
            // export if associations > 0, will zero fill in R
            if(pbsn.associations[i][j] > 0) {
                pbsnofs << gen << ","
                        << i << ","
                        << j+1 << ","
                        << static_cast<double>(pbsn.associations[i][j]) / maxtime << "\n";
            }
        }
    }
    pbsnofs.close();
}

// function to evolve population
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

            // update pbsn only in last n gens
            if(gen == (genmax - 1)) {
                pop.updatePbsn(pbsn);
            }

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

        // write traits to file
        // if(gen == genmax - 1) {
        //     exportTraits(gen, pop, outpath);
        //     exportPbsn(gen, pbsn, outpath, static_cast<double>(tmax));
        // }

        // reproduce
        pop.Reproduce();

    }
}

//' Make landscapes with discrete food items in clusters.
//'
//' @description Makes landscape and writes them to file with unique id.
//'
//' @param foodClusters Number of clusters around which food is generated.
//' @param clusterDispersal How dispersed food is around the cluster centre.
//' @param landsize The size of the landscape as a numeric (double).
//' @param replicates How many replicates.
//' @return Nothing. Runs simulation.
// [[Rcpp::export]]
void export_test_landscapes(int foodClusters, double clusterDispersal, double landsize, int replicates) {
    // outpath is data/test_landscape

    // assumes path/type already prepared
    std::string path = "data/test_landscape";
    // output filename as milliseconds since epoch

    for(int i = 0; i < replicates; i++) {

        // make a landscape
        Resources tmpFood;
        tmpFood.initResources(foodClusters, clusterDispersal, landsize);

        // get unique id
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto value = now_ms.time_since_epoch();

        // add a random number to be sure of discrete values
        long duration = value.count() + static_cast<long>(gsl_rng_uniform_int(r, 10000));
        std::string output_id = std::to_string(duration);
        output_id = "test_landscape" + output_id;

        // write summary of test landscapes
        const std::string summary_out = path + "/test_landscape_lookup.csv";
        std::ofstream summary_ofs;

        // if not exists write col names
        std::ifstream f2(summary_out.c_str());
        if (!f2.good()) {
            summary_ofs.open(summary_out, std::ofstream::out);
            summary_ofs << "filename,clusters,dispersal,replicate\n";
            summary_ofs.close();
        }
        // append if not
        summary_ofs.open(summary_out, std::ofstream::out | std::ofstream::app);
        summary_ofs << output_id << ","
                    << foodClusters << ","
                    << clusterDispersal << ","
                    << i << "\n";
        summary_ofs.close();

        // write the test landscape
        std::ofstream test_land_ofs;
        test_land_ofs.open(path + "/" + output_id + ".csv", std::ofstream::out);
        test_land_ofs << "x,y\n";

        for (size_t i = 0; i < static_cast<size_t>(tmpFood.nItems); i++)
        {
            test_land_ofs << tmpFood.coordX[i] << "," << tmpFood.coordY[i] << "\n";
        }

        test_land_ofs.close();

    }
}

//' Runs the sociality model simulation.
//'
//' @description Run the simulation using parameters passed as
//' arguments to the corresponding R function.
//' 
//' @param genmax The maximum number of generations per simulation.
//' @param tmax The number of timesteps per generation.
//' @param foodClusters Number of clusters around which food is generated.
//' @param clusterDispersal How dispersed food is around the cluster centre.
//' @param landsize The size of the landscape as a numeric (double).
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
DataFrame do_simulation(int genmax, int tmax, int foodClusters, double clusterDispersal, double landsize) {

    // prepare landscape
    Resources food;
    food.initResources(foodClusters, clusterDispersal, landsize);
    food.countAvailable();
    std::cout << "landscape with " << foodClusters << " clusters\n";
     /// export landscape

    // prepare population
    Population pop;
    pop.setTrait();
    std::cout << pop.nAgents << " agents over " << genmax << " gens of " << tmax << " timesteps\n";

    // evolve population
    evolve_pop(genmax, tmax, pop, food);

    // create data frame and return
    DataFrame df_evolved_pop = DataFrame::create(
        Named("gen") = std::vector<int> (pop.nAgents, genmax),
        Named("energy") = pop.energy,
        Named("p_ars") = pop.trait);


    std::cout << "done evolving\n";

    return df_evolved_pop;
}
