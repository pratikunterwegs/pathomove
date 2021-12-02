
#include "data_types.hpp"

// function to update gendata
void genData::updateGenData (Population &pop, const int g_) {
    
    int i = g_ / increment;
    // get social network measures
    // std::vector<float> measures = pop.pbsn.ntwkMeasures();
    
    // get pop data
    gIntake[i] = pop.energy; // this returns the intake! not the net energy
    // depending on where the data logging function is applied
    gSF[i] = pop.sF;
    gSH[i] = pop.sH;
    gSN[i] = pop.sN;
    gX[i] = pop.initX;
    gY[i] = pop.initY;
    gXn[i] = pop.coordX;
    gYn[i] = pop.coordY;
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
            Named("intake") = gIntake[i],
            Named("sF") = gSF[i],
            Named("sH") = gSH[i],
            Named("sN") = gSN[i],
            Named("x") = gX[i],
            Named("y") = gY[i],
            Named("xn") = gXn[i],
            Named("yn") = gYn[i],
            Named("assoc") = gAssoc[i],
            Named("t_infec") = gTInfected[i],
            Named("moved") = gMoved[i]
        );
    }
    List dataToReturn = List::create(
        Named("pop_data") = gDataList,
        Named("gens") = gens,
        Named("n_infected") = gNInfected
    );

    return dataToReturn;
}
