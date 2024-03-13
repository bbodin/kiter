//
// Created by toky on 26/4/23.
//

#ifndef KITER_THROUGHPUT_BUFFERING_DSE_H
#define KITER_THROUGHPUT_BUFFERING_DSE_H



#include <algorithms/dse/kperiodic.h>
#include <algorithms/dse/ModularDSE.h>

namespace algorithms {
    namespace dse {
        enum ThroughputBufferingMode {
            ASAP_DSE_MODE, // Stuijk2006
            KDSE_MODE,     //  Koh2022
            K2DSE_MODE,
            K2DSEA_MODE
        };


        class ThroughputBufferingStopCondition {
        public:
            ThroughputBufferingStopCondition(TIME_UNIT target_th, bool surface_only = false) : surface_only(surface_only), target_th(target_th) {}
            bool operator()(const algorithms::dse::TokenConfiguration& new_config, const algorithms::dse::TokenConfigurationSet& searched_configs) const ;
        private:
            bool      surface_only;
            TIME_UNIT target_th;
        };

        class ThroughputBufferingNext {
        public:
            ThroughputBufferingNext(ThroughputBufferingMode mode, bool use_cache) :  mode(mode) , use_cache(use_cache) {}
            algorithms::dse::ModularDSE::NextFuncRes operator()(const algorithms::dse::TokenConfiguration& current) const;
        private:
            ThroughputBufferingMode mode;
            bool use_cache;
        };


        TokenConfiguration::PerformanceResult asap_performance_func(models::Dataflow* dataflow, const TokenConfiguration& config);
        TokenConfiguration::PerformanceResult kperiodic_performance_func(models::Dataflow* dataflow, const TokenConfiguration& config);
        std::vector<TokenConfiguration> throughputbuffering_initial_func(const models::Dataflow* dataflow);




        TokenConfigurationSet solve_throughputbuffering   (models::Dataflow* const  dataflow,
                                                           ThroughputBufferingMode mode,
                                                           const ExplorationParameters& params,
                                                           algorithms::dse::TokenConfiguration* tc = nullptr) ;

        void throughput_buffering_dse   (models::Dataflow* const  dataflow, parameters_list_t params);
    }
}


ADD_TRANSFORMATION(throughputbufferingDSE,
                   transformation_t({
                                            "throughputbufferingDSE",
                                            "DSE that solve the throughputbuffering problem for a graph with \"feedback\" buffers",
                                            algorithms::dse::throughput_buffering_dse}
                   ));



#endif //KITER_THROUGHPUT_BUFFERING_DSE_H
