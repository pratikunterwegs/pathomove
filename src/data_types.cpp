
#include "data_types.h"

// function to update gendata
void genData::updateGenData(Population &pop, const int g_) {
  int i = g_ / increment;
  // get social network measures
  // std::vector<float> measures = pop.pbsn.ntwkMeasures();

  // get pop data
  gIntake[i] = pop.intake; // this returns the intake! not the net energy
  gEnergy[i] = pop.energy; // this returns the net energy, fitness proxy
  gSF[i] = pop.sF;
  gSH[i] = pop.sH;
  gSN[i] = pop.sN;
  gX[i] = pop.initX;
  gY[i] = pop.initY;
  gXn[i] = pop.coordX;
  gYn[i] = pop.coordY;
  gAssoc[i] = pop.associations;
  gTInfected[i] = pop.timeInfected;
  gSrc[i] = pop.srcInfect;
  // gDegree[i] = pop.pbsn.getDegree();
  gNInfected[i] = pop.nInfected;
  gMoved[i] = pop.moved;

  gens[i] = g_;
  // gPbsnDiameter[i] = measures[0];
  // gPbsnGlobEff[i] = measures[1];
}

// function to return gen data as an rcpp list
Rcpp::List genData::getGenData() {
  Rcpp::List gDataList(gSampled);
  for (int i = 0; i < gSampled; i++) {
    gDataList[i] = DataFrame::create(
        Named("intake") = gIntake[i], Named("energy") = gEnergy[i],
        Named("sF") = gSF[i], Named("sH") = gSH[i], Named("sN") = gSN[i],
        Named("x") = gX[i], Named("y") = gY[i], Named("xn") = gXn[i],
        Named("yn") = gYn[i], Named("assoc") = gAssoc[i],
        Named("t_infec") = gTInfected[i], Named("infect_src") = gSrc[i],
        Named("moved") = gMoved[i]);
  }
  List dataToReturn =
      List::create(Named("pop_data") = gDataList, Named("gens") = gens,
                   Named("n_infected") = gNInfected);

  return dataToReturn;
}

void moveData::updateMoveData(Population &pop, const int t_) {
  assert(t_ <= tmax && "too many timesteps logged");

  timesteps[t_] = std::vector<int>(popsize, t_);
  x[t_] = pop.coordX;
  y[t_] = pop.coordY;
}

Rcpp::List moveData::getMoveData() {
  Rcpp::List mDataList(tmax);
  std::vector<int> id(popsize, 0);
  std::iota(std::begin(id), std::end(id), 0);

  for (int i = 0; i < tmax; i++) {
    mDataList[i] =
        DataFrame::create(Named("time") = timesteps[i], Named("x") = x[i],
                          Named("y") = y[i], Named("id") = id);
  }

  return mDataList;
}