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

            pop.move(food, 0.0001);

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

        // subtract competition costs
        pop.competitionCosts(0.0001);
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
//' @param popsize The population size.
//' @param genmax The maximum number of generations per simulation.
//' @param tmax The number of timesteps per generation.
//' @param foodClusters Number of clusters around which food is generated.
//' @param clusterDispersal How dispersed food is around the cluster centre.
//' @param landsize The size of the landscape as a numeric (double).
//' @return A data frame of the evolved population traits.
// [[Rcpp::export]]
DataFrame do_simulation(int popsize, int genmax, int tmax, int foodClusters, double clusterDispersal, double landsize) {

    // prepare landscape
    Resources food;
    food.initResources(foodClusters, clusterDispersal, landsize);
    food.countAvailable();
    Rcpp::Rcout << "landscape with " << foodClusters << " clusters\n";
     /// export landscape

    // prepare population
    Population pop (popsize);
    // pop.initPop(popsize);
    pop.setTrait();
    Rcpp::Rcout << pop.nAgents << " agents over " << genmax << " gens of " << tmax << " timesteps\n";

    // evolve population
    evolve_pop(genmax, tmax, pop, food);

    // create data frame and return
    DataFrame df_evolved_pop = DataFrame::create(
        Named("energy") = pop.energy,
        Named("p_ars") = pop.trait
     );


    Rcpp::Rcout << "done evolving\n";

    return df_evolved_pop;
}

//' Export a population.
//'
//' @param popsize The population size.
// [[Rcpp::export]]
DataFrame export_pop(int popsize) {
    Rcpp::Rcout << "in export function";
    Population pop (popsize);
    // pop.initPop(popsize);
    pop.setTrait();

    DataFrame df_pop = DataFrame::create(
                Named("trait") = pop.trait,
                Named("energy") = pop.energy
            );

    return df_pop;
}
