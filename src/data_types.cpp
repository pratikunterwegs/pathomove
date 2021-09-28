
#include "data_types.hpp"

// function to update move
void moveData::updateMoveData (Population &pop, const int timestep_) {
    // get pop data
    std::vector<int> idVec (pop.nAgents);
    for (size_t i = 0; i < pop.nAgents; i++) {
        idVec[i] = i;
    }

    id.push_back(idVec);
    coordX.push_back(pop.coordX);
    coordY.push_back(pop.coordY);
    energy.push_back(pop.energy);
    timestep.push_back(timestep_);
}

// function to return move data as an rcpp list
Rcpp::List moveData::getMoveData() {
    Rcpp::List moveDataList (timestep.size());
    for (size_t i = 0; i < timestep.size(); i++)
    {
        moveDataList[i] = DataFrame::create(
            Named("id") = id[i],
            Named("energy") = energy[i],
            Named("x") = coordX[i],
            Named("Y") = coordY[i]
        );
    }
    List dataToReturn = List::create(
        Named("move_data") = moveDataList,
        Named("timestep") = timestep
    );

    return dataToReturn;
}

// function to update gendata
void genData::updateGenData (Population &pop, const int gen_) {
    // get pop data
    genEnergyVec.push_back(pop.energy);
    genSF.push_back(pop.sF);
    genSH.push_back(pop.sH);
    genSN.push_back(pop.sN);

    genAssocVec.push_back(pop.associations);
    genTimeInfec.push_back(pop.timeInfected);

    genMoved.push_back(pop.moved);

    genDegree.push_back(pop.pbsn.getDegree());
    genInfected.push_back(pop.nInfected);
    gens.push_back(gen_);
    pSrcInfect.push_back(pop.propSrcInfection());

    // update adjacency matrices
    std::vector<float> measures = pop.pbsn.ntwkMeasures();
    pbsn_diameter.push_back(measures[0]);
    pbsn_glob_eff.push_back(measures[1]);

}

// function to return gen data as an rcpp list
Rcpp::List genData::getGenData() {
    Rcpp::List genDataList (gens.size());
    for (size_t i = 0; i < gens.size(); i++)
    {
        genDataList[i] = DataFrame::create(
            Named("energy") = genEnergyVec[i],
            Named("sF") = genSF[i],
            Named("sH") = genSH[i],
            Named("sN") = genSN[i],
            Named("assoc") = genAssocVec[i],
            Named("t_infec") = genTimeInfec[i],
            Named("degree") = genDegree[i],
            Named("moved") = genMoved[i]
            // Named("degree") = genDegreeVec[i]
        );
    }
    List dataToReturn = List::create(
        Named("pop_data") = genDataList,
        Named("gens") = gens,
        Named("n_infected") = genInfected,
        Named("p_src") = pSrcInfect,
        Named("diameter") = pbsn_diameter,
        Named("glob_eff") = pbsn_glob_eff
    );

    return dataToReturn;
}
