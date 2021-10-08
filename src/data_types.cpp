
#include "data_types.hpp"

// function to update gendata
void genData::updateGenData (Population &pop, const int g_) {
    
    int i = g_ / increment;
    // get social network measures
    // std::vector<float> measures = pop.pbsn.ntwkMeasures();
    
    // get pop data
    gEnergy[i] = pop.energy;
    gSF[i] = pop.sF;
    gSH[i] = pop.sH;
    gSN[i] = pop.sN;
    gAssoc[i] = pop.associations;
    gTInfected[i] = pop.timeInfected;
    // gDegree[i] = pop.pbsn.getDegree();
    gNInfected[i] = pop.nInfected;
    gMoved[i] = pop.moved;

    gens[i] = g_;
    // gPbsnDiameter[i] = measures[0];
    // gPbsnGlobEff[i] = measures[1];
}

// function to return gen data as an rcpp list
Rcpp::List genData::getGenData() {
    Rcpp::List gDataList (gSampled);
    for (int i = 0; i < gSampled; i++)
    {
        gDataList[i] = DataFrame::create(
            Named("energy") = gEnergy[i],
            Named("sF") = gSF[i],
            Named("sH") = gSH[i],
            Named("sN") = gSN[i],
            Named("assoc") = gAssoc[i],
            Named("t_infec") = gTInfected[i],
            // Named("degree") = gDegree[i],
            Named("moved") = gMoved[i]
        );
    }
    List dataToReturn = List::create(
        Named("pop_data") = gDataList,
        Named("gens") = gens,
        Named("n_infected") = gNInfected//,
        // Named("diameter") = gPbsnDiameter,
        // Named("glob_eff") = gPbsnGlobEff
    );

    return dataToReturn;
}
