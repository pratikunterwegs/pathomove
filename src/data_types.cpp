// Copyright 2022 Pratik R Gupte. See repository licence in LICENSE.md.
#include "data_types.h"

// function to update gendata
void genData::updateGenData(const Population &pop, const int &g_) {
  int i = g_ / increment;
  // get social network measures
  // std::vector<float> measures = pop.pbsn.ntwkMeasures();

  // get pop data
  gIntake[i] = pop.intake;  // this returns the intake! not the net energy
  gEnergy[i] = pop.energy;  // this returns the net energy, fitness proxy
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
    gDataList[i] = Rcpp::DataFrame::create(
        Rcpp::Named("intake") = gIntake[i], Rcpp::Named("energy") = gEnergy[i],
        Rcpp::Named("sF") = gSF[i], Rcpp::Named("sH") = gSH[i],
        Rcpp::Named("sN") = gSN[i], Rcpp::Named("x") = gX[i],
        Rcpp::Named("y") = gY[i], Rcpp::Named("xn") = gXn[i],
        Rcpp::Named("yn") = gYn[i], Rcpp::Named("assoc") = gAssoc[i],
        Rcpp::Named("t_infec") = gTInfected[i],
        Rcpp::Named("infect_src") = gSrc[i], Rcpp::Named("moved") = gMoved[i]);
  }
  Rcpp::List dataToReturn = Rcpp::List::create(
      Rcpp::Named("pop_data") = gDataList, Rcpp::Named("gens") = gens,
      Rcpp::Named("n_infected") = gNInfected);

  return dataToReturn;
}

void moveData::updateMoveData(const Population &pop, const int &t_) {
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
    mDataList[i] = Rcpp::DataFrame::create(
        Rcpp::Named("time") = timesteps[i], Rcpp::Named("x") = x[i],
        Rcpp::Named("y") = y[i], Rcpp::Named("id") = id);
  }

  return mDataList;
}
