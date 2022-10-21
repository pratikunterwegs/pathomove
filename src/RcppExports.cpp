// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// get_test_landscape
Rcpp::DataFrame get_test_landscape(const int& nItems, const float& landsize, const int& nClusters, const float& clusterSpread, const int& regen_time);
RcppExport SEXP _pathomove_get_test_landscape(SEXP nItemsSEXP, SEXP landsizeSEXP, SEXP nClustersSEXP, SEXP clusterSpreadSEXP, SEXP regen_timeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const int& >::type nItems(nItemsSEXP);
    Rcpp::traits::input_parameter< const float& >::type landsize(landsizeSEXP);
    Rcpp::traits::input_parameter< const int& >::type nClusters(nClustersSEXP);
    Rcpp::traits::input_parameter< const float& >::type clusterSpread(clusterSpreadSEXP);
    Rcpp::traits::input_parameter< const int& >::type regen_time(regen_timeSEXP);
    rcpp_result_gen = Rcpp::wrap(get_test_landscape(nItems, landsize, nClusters, clusterSpread, regen_time));
    return rcpp_result_gen;
END_RCPP
}
// run_pathomove
S4 run_pathomove(const int scenario, const int popsize, const int nItems, const float landsize, const int nClusters, const float clusterSpread, const int tmax, const int genmax, const int g_patho_init, const float n_samples, const float range_food, const float range_agents, const float range_move, const int handling_time, const int regen_time, float pTransmit, const float p_vTransmit, const int initialInfections, const float costInfect, const bool multithreaded, const float dispersal, const bool infect_percent, const bool vertical, const bool evolve_sI, const bool reprod_threshold, const float mProb, const float mSize, const float spillover_rate);
RcppExport SEXP _pathomove_run_pathomove(SEXP scenarioSEXP, SEXP popsizeSEXP, SEXP nItemsSEXP, SEXP landsizeSEXP, SEXP nClustersSEXP, SEXP clusterSpreadSEXP, SEXP tmaxSEXP, SEXP genmaxSEXP, SEXP g_patho_initSEXP, SEXP n_samplesSEXP, SEXP range_foodSEXP, SEXP range_agentsSEXP, SEXP range_moveSEXP, SEXP handling_timeSEXP, SEXP regen_timeSEXP, SEXP pTransmitSEXP, SEXP p_vTransmitSEXP, SEXP initialInfectionsSEXP, SEXP costInfectSEXP, SEXP multithreadedSEXP, SEXP dispersalSEXP, SEXP infect_percentSEXP, SEXP verticalSEXP, SEXP evolve_sISEXP, SEXP reprod_thresholdSEXP, SEXP mProbSEXP, SEXP mSizeSEXP, SEXP spillover_rateSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const int >::type scenario(scenarioSEXP);
    Rcpp::traits::input_parameter< const int >::type popsize(popsizeSEXP);
    Rcpp::traits::input_parameter< const int >::type nItems(nItemsSEXP);
    Rcpp::traits::input_parameter< const float >::type landsize(landsizeSEXP);
    Rcpp::traits::input_parameter< const int >::type nClusters(nClustersSEXP);
    Rcpp::traits::input_parameter< const float >::type clusterSpread(clusterSpreadSEXP);
    Rcpp::traits::input_parameter< const int >::type tmax(tmaxSEXP);
    Rcpp::traits::input_parameter< const int >::type genmax(genmaxSEXP);
    Rcpp::traits::input_parameter< const int >::type g_patho_init(g_patho_initSEXP);
    Rcpp::traits::input_parameter< const float >::type n_samples(n_samplesSEXP);
    Rcpp::traits::input_parameter< const float >::type range_food(range_foodSEXP);
    Rcpp::traits::input_parameter< const float >::type range_agents(range_agentsSEXP);
    Rcpp::traits::input_parameter< const float >::type range_move(range_moveSEXP);
    Rcpp::traits::input_parameter< const int >::type handling_time(handling_timeSEXP);
    Rcpp::traits::input_parameter< const int >::type regen_time(regen_timeSEXP);
    Rcpp::traits::input_parameter< float >::type pTransmit(pTransmitSEXP);
    Rcpp::traits::input_parameter< const float >::type p_vTransmit(p_vTransmitSEXP);
    Rcpp::traits::input_parameter< const int >::type initialInfections(initialInfectionsSEXP);
    Rcpp::traits::input_parameter< const float >::type costInfect(costInfectSEXP);
    Rcpp::traits::input_parameter< const bool >::type multithreaded(multithreadedSEXP);
    Rcpp::traits::input_parameter< const float >::type dispersal(dispersalSEXP);
    Rcpp::traits::input_parameter< const bool >::type infect_percent(infect_percentSEXP);
    Rcpp::traits::input_parameter< const bool >::type vertical(verticalSEXP);
    Rcpp::traits::input_parameter< const bool >::type evolve_sI(evolve_sISEXP);
    Rcpp::traits::input_parameter< const bool >::type reprod_threshold(reprod_thresholdSEXP);
    Rcpp::traits::input_parameter< const float >::type mProb(mProbSEXP);
    Rcpp::traits::input_parameter< const float >::type mSize(mSizeSEXP);
    Rcpp::traits::input_parameter< const float >::type spillover_rate(spillover_rateSEXP);
    rcpp_result_gen = Rcpp::wrap(run_pathomove(scenario, popsize, nItems, landsize, nClusters, clusterSpread, tmax, genmax, g_patho_init, n_samples, range_food, range_agents, range_move, handling_time, regen_time, pTransmit, p_vTransmit, initialInfections, costInfect, multithreaded, dispersal, infect_percent, vertical, evolve_sI, reprod_threshold, mProb, mSize, spillover_rate));
    return rcpp_result_gen;
END_RCPP
}

RcppExport SEXP run_testthat_tests(SEXP);

static const R_CallMethodDef CallEntries[] = {
    {"_pathomove_get_test_landscape", (DL_FUNC) &_pathomove_get_test_landscape, 5},
    {"_pathomove_run_pathomove", (DL_FUNC) &_pathomove_run_pathomove, 28},
    {"run_testthat_tests", (DL_FUNC) &run_testthat_tests, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_pathomove(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
