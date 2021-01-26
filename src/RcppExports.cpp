// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// export_test_landscapes
void export_test_landscapes(int foodClusters, double clusterDispersal, double landsize, int replicates);
RcppExport SEXP _socialitymodel_export_test_landscapes(SEXP foodClustersSEXP, SEXP clusterDispersalSEXP, SEXP landsizeSEXP, SEXP replicatesSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type foodClusters(foodClustersSEXP);
    Rcpp::traits::input_parameter< double >::type clusterDispersal(clusterDispersalSEXP);
    Rcpp::traits::input_parameter< double >::type landsize(landsizeSEXP);
    Rcpp::traits::input_parameter< int >::type replicates(replicatesSEXP);
    export_test_landscapes(foodClusters, clusterDispersal, landsize, replicates);
    return R_NilValue;
END_RCPP
}
// do_simulation
void do_simulation(int genmax, int tmax, int foodClusters, double clusterDispersal, double landsize);
RcppExport SEXP _socialitymodel_do_simulation(SEXP genmaxSEXP, SEXP tmaxSEXP, SEXP foodClustersSEXP, SEXP clusterDispersalSEXP, SEXP landsizeSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type genmax(genmaxSEXP);
    Rcpp::traits::input_parameter< int >::type tmax(tmaxSEXP);
    Rcpp::traits::input_parameter< int >::type foodClusters(foodClustersSEXP);
    Rcpp::traits::input_parameter< double >::type clusterDispersal(clusterDispersalSEXP);
    Rcpp::traits::input_parameter< double >::type landsize(landsizeSEXP);
    do_simulation(genmax, tmax, foodClusters, clusterDispersal, landsize);
    return R_NilValue;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_socialitymodel_export_test_landscapes", (DL_FUNC) &_socialitymodel_export_test_landscapes, 4},
    {"_socialitymodel_do_simulation", (DL_FUNC) &_socialitymodel_do_simulation, 5},
    {NULL, NULL, 0}
};

RcppExport void R_init_socialitymodel(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
