
#include "data_types.h"

// function to update gendata
void genData::updateGenData(Population &pop, const int g_) {

  gDataList.push_back(Rcpp::DataFrame::create(
      // get pop data
      Rcpp::Named("intake") =
          pop.intake, // this returns the intake! not the net energy
      Rcpp::Named("energy") =
          pop.energy, // this returns the net energy, fitness proxy
      Rcpp::Named("sF") = pop.sF, Rcpp::Named("sH") = pop.sH,
      Rcpp::Named("sN") = pop.sN, Rcpp::Named("sI") = pop.sI,
      Rcpp::Named("x") = pop.initX, Rcpp::Named("y") = pop.initY,
      Rcpp::Named("xn") = pop.coordX, Rcpp::Named("yn") = pop.coordY,
      Rcpp::Named("assoc") = pop.associations,
      Rcpp::Named("t_infec") = pop.timeInfected,
      Rcpp::Named("src_infect") = pop.srcInfect,
      Rcpp::Named("moved") = pop.moved));

  gNInfected.push_back(pop.nInfected);
  gens.push_back(g_);
}

// function to return gen data as an rcpp list
Rcpp::List genData::getGenData() {

  Rcpp::List dataToReturn =
      Rcpp::List::create(Named("pop_data") = gDataList, Named("gens") = gens,
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