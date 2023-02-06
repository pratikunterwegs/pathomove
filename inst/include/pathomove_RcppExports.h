// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#ifndef RCPP_pathomove_RCPPEXPORTS_H_GEN_
#define RCPP_pathomove_RCPPEXPORTS_H_GEN_

#include <Rcpp.h>

namespace pathomove {

    using namespace Rcpp;

    namespace {
        void validateSignature(const char* sig) {
            Rcpp::Function require = Rcpp::Environment::base_env()["require"];
            require("pathomove", Rcpp::Named("quietly") = true);
            typedef int(*Ptr_validate)(const char*);
            static Ptr_validate p_validate = (Ptr_validate)
                R_GetCCallable("pathomove", "_pathomove_RcppExport_validate");
            if (!p_validate(sig)) {
                throw Rcpp::function_not_exported(
                    "C++ function with signature '" + std::string(sig) + "' not found in pathomove");
            }
        }
    }

    inline Rcpp::DataFrame get_test_landscape(const int nItems, const float landsize, const int nClusters, const float clusterSpread, const int regen_time) {
        typedef SEXP(*Ptr_get_test_landscape)(SEXP,SEXP,SEXP,SEXP,SEXP);
        static Ptr_get_test_landscape p_get_test_landscape = NULL;
        if (p_get_test_landscape == NULL) {
            validateSignature("Rcpp::DataFrame(*get_test_landscape)(const int,const float,const int,const float,const int)");
            p_get_test_landscape = (Ptr_get_test_landscape)R_GetCCallable("pathomove", "_pathomove_get_test_landscape");
        }
        RObject rcpp_result_gen;
        {
            RNGScope RCPP_rngScope_gen;
            rcpp_result_gen = p_get_test_landscape(Shield<SEXP>(Rcpp::wrap(nItems)), Shield<SEXP>(Rcpp::wrap(landsize)), Shield<SEXP>(Rcpp::wrap(nClusters)), Shield<SEXP>(Rcpp::wrap(clusterSpread)), Shield<SEXP>(Rcpp::wrap(regen_time)));
        }
        if (rcpp_result_gen.inherits("interrupted-error"))
            throw Rcpp::internal::InterruptedException();
        if (Rcpp::internal::isLongjumpSentinel(rcpp_result_gen))
            throw Rcpp::LongjumpException(rcpp_result_gen);
        if (rcpp_result_gen.inherits("try-error"))
            throw Rcpp::exception(Rcpp::as<std::string>(rcpp_result_gen).c_str());
        return Rcpp::as<Rcpp::DataFrame >(rcpp_result_gen);
    }

    inline Rcpp::S4 run_pathomove(const int scenario = 1, const int popsize = 100, const int nItems = 1800, const float landsize = 60.0, const int nClusters = 60, const float clusterSpread = 1.0, const int tmax = 100, const int genmax = 100, const int g_patho_init = 70, const int n_samples = 5, const float range_food = 1.0, const float range_agents = 1.0, const float range_move = 1.0, const int handling_time = 5, const int regen_time = 50, float pTransmit = 0.05, const float p_v_transmit = 0.05, const int initialInfections = 20, const float costInfect = 0.25, const bool multithreaded = true, const float dispersal = 2.0, const bool infect_percent = false, const bool vertical = false, const bool reprod_threshold = false, const float mProb = 0.01, const float mSize = 0.01, const float spillover_rate = 1.0, const int seed = 0) {
        typedef SEXP(*Ptr_run_pathomove)(SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP,SEXP);
        static Ptr_run_pathomove p_run_pathomove = NULL;
        if (p_run_pathomove == NULL) {
            validateSignature("Rcpp::S4(*run_pathomove)(const int,const int,const int,const float,const int,const float,const int,const int,const int,const int,const float,const float,const float,const int,const int,float,const float,const int,const float,const bool,const float,const bool,const bool,const bool,const float,const float,const float,const int)");
            p_run_pathomove = (Ptr_run_pathomove)R_GetCCallable("pathomove", "_pathomove_run_pathomove");
        }
        RObject rcpp_result_gen;
        {
            RNGScope RCPP_rngScope_gen;
            rcpp_result_gen = p_run_pathomove(Shield<SEXP>(Rcpp::wrap(scenario)), Shield<SEXP>(Rcpp::wrap(popsize)), Shield<SEXP>(Rcpp::wrap(nItems)), Shield<SEXP>(Rcpp::wrap(landsize)), Shield<SEXP>(Rcpp::wrap(nClusters)), Shield<SEXP>(Rcpp::wrap(clusterSpread)), Shield<SEXP>(Rcpp::wrap(tmax)), Shield<SEXP>(Rcpp::wrap(genmax)), Shield<SEXP>(Rcpp::wrap(g_patho_init)), Shield<SEXP>(Rcpp::wrap(n_samples)), Shield<SEXP>(Rcpp::wrap(range_food)), Shield<SEXP>(Rcpp::wrap(range_agents)), Shield<SEXP>(Rcpp::wrap(range_move)), Shield<SEXP>(Rcpp::wrap(handling_time)), Shield<SEXP>(Rcpp::wrap(regen_time)), Shield<SEXP>(Rcpp::wrap(pTransmit)), Shield<SEXP>(Rcpp::wrap(p_v_transmit)), Shield<SEXP>(Rcpp::wrap(initialInfections)), Shield<SEXP>(Rcpp::wrap(costInfect)), Shield<SEXP>(Rcpp::wrap(multithreaded)), Shield<SEXP>(Rcpp::wrap(dispersal)), Shield<SEXP>(Rcpp::wrap(infect_percent)), Shield<SEXP>(Rcpp::wrap(vertical)), Shield<SEXP>(Rcpp::wrap(reprod_threshold)), Shield<SEXP>(Rcpp::wrap(mProb)), Shield<SEXP>(Rcpp::wrap(mSize)), Shield<SEXP>(Rcpp::wrap(spillover_rate)), Shield<SEXP>(Rcpp::wrap(seed)));
        }
        if (rcpp_result_gen.inherits("interrupted-error"))
            throw Rcpp::internal::InterruptedException();
        if (Rcpp::internal::isLongjumpSentinel(rcpp_result_gen))
            throw Rcpp::LongjumpException(rcpp_result_gen);
        if (rcpp_result_gen.inherits("try-error"))
            throw Rcpp::exception(Rcpp::as<std::string>(rcpp_result_gen).c_str());
        return Rcpp::as<Rcpp::S4 >(rcpp_result_gen);
    }

}

#endif // RCPP_pathomove_RCPPEXPORTS_H_GEN_
