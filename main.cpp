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
    // init food
    Resources food;
    food.initResources(30, 1.5);
    food.countAvailable();

    //    std::cout << food.nAvailable << "\n";

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

    std::ofstream moveofs;
    moveofs.open("moves.csv", std::ofstream::out);
    moveofs << "id,gen,x,y,energy,trait\n";

    for(int gen = 0; gen < 10500; gen++) {


        std::cout << "gen = " << gen << "\n";

        for (size_t t = 0; t < 100; t++) {

            pop.initPos(food);
            pop.move(food);

            for (size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++)
            {
                forage(i, food, pop);
                food.countAvailable();
            }

            // decrement food counter by one
            for (size_t j = 0; j < static_cast<size_t>(food.nItems); j++){
                if(food.counter[j] > 0) {
                    food.counter[j] --;
                }

            }
        }

        if(gen % 10 == 0) {
            // print evolved pop
            for(size_t i = 0; i < static_cast<size_t>(pop.nAgents); i++){
                moveofs << i << ","
                        << gen << ","
                        << pop.coordX[i] << ","
                        << pop.coordY[i] << ","
                        << pop.energy[i] << ","
                        << pop.trait[i] << "\n";
            }
        }
        // reproduce
        pop.Reproduce();

    }

    moveofs.close();
    std::cout << "done";
}
