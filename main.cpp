#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "parameters.h"
#include "landscape.h"
#include "network.h"
#include "agents.h"
#include "simulations.cpp"

int main()
{
    do_simulation(100, 100, 20, 1.5);
}
