//
// Created by Bruno on 26/4/23.
//

#include "ThroughputBufferingDSE.h"
#include "algorithms/transformation/subgraph.h"
#include "algorithms/transformation/AddFeedbackBuffers.h"
#include "algorithms/throughput/symbolic_execution.h"
#include "Constraint.h"
#include <algorithms/dse/kperiodic.h>
#include <algorithms/dse/ModularDSE.h>
#include <printers/stdout.h>


    namespace algorithms::dse {


        bool ThroughputBufferingStopCondition::operator()(const algorithms::dse::TokenConfiguration &new_config,
                                                          const algorithms::dse::TokenConfigurationSet &searched_configs) const {
            const algorithms::dse::TokenConfiguration *best = searched_configs.getBestPerformancePoint();

            bool decision_to_stop = false;

            if (best) {

                VERBOSE_DEBUG("new_config = " << new_config);
                VERBOSE_DEBUG("*best = " << *best);
                VERBOSE_DEBUG("new_config.dominates(*best) = " << new_config.dominates(*best));
                VERBOSE_DEBUG("best->getPerformance().throughput = " << best->getPerformance().throughput);
                VERBOSE_DEBUG("target_th = " << target_th);
                VERBOSE_DEBUG("surface_only = " << surface_only);
                VERBOSE_DEBUG("best->getCost() = " << best->getCost());
                VERBOSE_DEBUG("new_config.getCost() = " << new_config.getCost());
                VERBOSE_DEBUG("(best->getPerformance().throughput > target_th) = " << (best->getPerformance().throughput > target_th));

                if (surface_only) {
                    decision_to_stop =  (((best->getCost() < new_config.getCost()) && (new_config.dominates(*best)) &&
                            (best->getPerformance().throughput > std::numeric_limits<TIME_UNIT>::epsilon() + target_th))); // FIXME: I hate this epsilon hack!
                } else {
                    decision_to_stop =  ((best->getCost() < new_config.getCost()) &&
                            (best->getPerformance().throughput >= target_th));
                }
            }

            VERBOSE_DEBUG("decision_to_stop = " << decision_to_stop);
            return decision_to_stop;
        }

        std::vector<algorithms::dse::TokenConfiguration>
        critical_based_surface_dse_mode(const algorithms::dse::TokenConfiguration &current,
                                        bool pareto_only = false,
                                        bool use_db = false) {

#define USE_DATABASE
#ifdef USE_DATABASE
            // TODO FIXME This is a hack to test something
            static std::mutex database_mtx;
            static std::map<std::set<TokenConfiguration::EdgeIdentifier>,std::pair<TIME_UNIT,TokenConfigurationSet>> database;
#endif

            VERBOSE_DEBUG("     critical_based_surface_dse_mode: " << current << " with pareto_only=" << pareto_only);
            if (current.getPerformance().critical_edges.empty()) {
                // Special case, the exploration asked to continue while there is no critical cycle...
                VERBOSE_INFO("Empty set of critical edge. End of search.");
                return {};
            }

            // K2DSE mode, local search in the cycle to see what increments actually improve this cycle
            // K2DSEA mode, include the pareto_only flag
            const models::Dataflow *df = current.getDataflow();
            const auto &current_configuration = current.getConfiguration();
            std::vector<algorithms::dse::TokenConfiguration> next_configurations;

            const auto current_th = current.getPerformance().throughput;
            VERBOSE_DEBUG("ThroughputBufferingNext Mode: K2DSE Current throughput: " << current_th
            << " current:" << current.to_csv_line());

            // 1. build a minimal graph with the critical edges only.
            models::Dataflow *cc_g = algorithms::build_subgraph(df, current.getPerformance().critical_edges);
            computeRepetitionVector(cc_g);

            VERBOSE_DEBUG("cc_g created.");

            kperiodic_result_t cc_max_performance = algorithms::compute_Kperiodic_throughput_and_cycles(cc_g);

            std::map<ARRAY_INDEX, TOKEN_UNIT> new_configuration_cc;
            ForEachEdge(cc_g, e) {
                if (cc_g->getEdgeType(e) != FEEDBACK_EDGE) continue;
                auto edgeId = cc_g->getEdgeId(e);
                new_configuration_cc[edgeId] = current.getConfiguration().at(edgeId);
            }



            algorithms::dse::TokenConfiguration tc(cc_g, new_configuration_cc);

            VERBOSE_DEBUG("tc created.");

            // To avoid computing the throughput of the initial TC (required to stop the search)
            // We can use the original graph period. BUT if the repetition vector change, the graph period changes.
            // TO solve the problem we can rescale the cc period.
            // current_cc_th = current_th * Ni/Ni;
            TIME_UNIT current_cc_th = 0;
            ARRAY_INDEX reference_edge_id = *current.getPerformance().critical_edges.begin();
            reference_edge_id = cc_g->getVertexId(cc_g->getFirstVertex());
            const auto df_Ni = df->getNi(df->getVertexById(reference_edge_id));
            const auto cc_Ni = cc_g->getNi(cc_g->getVertexById(reference_edge_id));
            current_cc_th = (current_th * df_Ni) / cc_Ni;


            // // Instead we could have computed the throughput ...
            //models::Dataflow sandbox = *cc_g;
            //sandbox.reset_computation();
            //auto cc_current_performance = kperiodic_performance_func(&sandbox, tc);
            //const auto current_cc_th = cc_current_performance.throughput;

            VERBOSE_DEBUG("Init: " << tc.to_csv_line());
            VERBOSE_DEBUG("current_cc_th: " << current_cc_th << " Ni=" << cc_g->getNi(cc_g->getVertexById(reference_edge_id)));
            VERBOSE_DEBUG("max_cc_th: " << cc_max_performance.throughput);
            VERBOSE_DEBUG("current_th: " << current_th<< " Ni=" << df->getNi(df->getVertexById(reference_edge_id)));

            // 2. run a dse with target throughout to beat.
            algorithms::dse::ModularDSE dse(cc_g,
                                            kperiodic_performance_func,
                                            ThroughputBufferingNext(KDSE_MODE, false),
                                            ThroughputBufferingStopCondition(current_cc_th, true),
                                            false);

#ifdef USE_DATABASE

            TIME_UNIT database_coverage = -1.0; // Lower than the zero coverage
            if (use_db) {
                std::unique_lock<std::mutex> lock(database_mtx);
                if (database.contains(current.getPerformance().critical_edges)) {
                    VERBOSE_INFO("Time saved, we imported past results of size " << database[current.getPerformance().critical_edges].second.size());
                    dse.import_results(database[current.getPerformance().critical_edges].second);
                    database_coverage = database[current.getPerformance().critical_edges].first;
                }
            }
#endif
            dse.add_initial_job(tc);
            ExplorationParameters params = {
                    .limit = false,
                    .bottom_up = false,
                    .realtime_output = false,
                    .return_pareto_only = pareto_only // very important K2DSEA here requires pareto only
            };

#ifdef USE_DATABASE
            // We only need to run the DSE if the cache is not sufficient.
            if (database_coverage < current_cc_th) {
                dse.explore(params);
            } else {
                VERBOSE_INFO("More time saved, we skip the DSE database_coverage= " << database_coverage << " current_cc_th=" << current_cc_th);
            }
#else
            dse.explore(params);
#endif
            auto cc_sds = dse.getResults();

#ifdef USE_DATABASE
            if (use_db) {
                std::unique_lock<std::mutex> lock(database_mtx);
                if (database.contains(current.getPerformance().critical_edges)) {
                    VERBOSE_INFO("previous DSE for this point was "
                                         << database[current.getPerformance().critical_edges].first
                                         << "," << database[current.getPerformance().critical_edges].second.size()
                                         << " new one is " << current_cc_th << "," << cc_sds.size());
                }

                database[current.getPerformance().critical_edges] = std::pair<TIME_UNIT, TokenConfigurationSet>(
                        std::max(database_coverage, current_cc_th), cc_sds);
                VERBOSE_INFO("Database size is " << database.size());
            }
#endif


            VERBOSE_DEBUG("End of local search:");

            // 3. any solutions that beat the target are considered.
            for (const auto& solution: cc_sds) {
                if (solution.getPerformance().throughput > current_cc_th) {
                    VERBOSE_DEBUG("    - KEEP " << solution.to_csv_line());
                    auto new_configuration = current_configuration; // Make a copy of the current configuration
                    for (auto edgeId_tokenSize: solution.getConfiguration()) {
                        new_configuration[edgeId_tokenSize.first] = edgeId_tokenSize.second;
                    }

                    algorithms::dse::TokenConfiguration new_token_configuration(current.getDataflow(),
                                                                                new_configuration);
                    next_configurations.push_back(new_token_configuration);
                } else {
                    VERBOSE_DEBUG("    - SKIP " << solution.to_csv_line());
                }

            }
            VERBOSE_DEBUG("     critical_based_surface_dse_mode finished with " << next_configurations.size() << " new configurations");
            return next_configurations;
        }


        std::vector<algorithms::dse::TokenConfiguration>
        critical_based_incremental_dse_mode(const algorithms::dse::TokenConfiguration &current) {
            const models::Dataflow *df = current.getDataflow();
            const auto &current_configuration = current.getConfiguration();
            std::vector<algorithms::dse::TokenConfiguration> next_configurations;
            for (algorithms::dse::TokenConfiguration::EdgeIdentifier edgeId: current.getPerformance().critical_edges) {
                Edge c = df->getEdgeById(edgeId);
                if (df->getEdgeType(c) != EDGE_TYPE::FEEDBACK_EDGE) continue; // Skip non feedback buffers.
                auto new_configuration = current_configuration; // Make a copy of the current configuration
                new_configuration[edgeId] = new_configuration[edgeId] +
                                            df->getFineGCD(
                                                    c); // Increment the buffer size for the current edge by 1

                // Create a new TokenConfiguration with the updated buffer size for the current edge
                algorithms::dse::TokenConfiguration new_token_configuration(current.getDataflow(),
                                                                            new_configuration);
                next_configurations.push_back(new_token_configuration);
            }

            return next_configurations;
        }

        algorithms::dse::ModularDSE::NextFuncRes
        ThroughputBufferingNext::operator()(const algorithms::dse::TokenConfiguration &current) const {

            std::vector<algorithms::dse::TokenConfiguration> next_configurations;

            if (mode == KDSE_MODE) { // Normal mode, just increase each buffer.
                VERBOSE_DEBUG("ThroughputBufferingNext Mode: KDSE current:" << current.to_csv_line());
                next_configurations = critical_based_incremental_dse_mode(current);
            } else if (mode == K2DSE_MODE) {
                next_configurations = critical_based_surface_dse_mode(current, false, use_cache);
            } else if (mode == K2DSEA_MODE) {
                // This true flag reduce the space to explore to locally pareto optimum points.
                next_configurations = critical_based_surface_dse_mode(current, true, use_cache);
            } else {
                VERBOSE_ERROR("Unsupported mode");
                VERBOSE_FAILURE();
            }


            VERBOSE_DEBUG("next configs: " << next_configurations.size());
            for (const auto& solution: next_configurations) {
                VERBOSE_DEBUG("     " << solution.to_csv_line());
            }

            return {next_configurations, Constraint()};
        }


        TOKEN_UNIT computeMinimalBufferSize(const models::Dataflow *dataflow, const Edge c) {

            // initialise channel size to maximum int size
            TOKEN_UNIT buffer_size = INT_MAX; // NOTE (should use ULONG_MAX, but it's a really large value)
            auto ratePeriod = (TOKEN_UNIT) std::lcm(dataflow->getEdgeInPhasesCount(c),
                                                          dataflow->getEdgeOutPhasesCount(c));

            TOKEN_UNIT tokensInitial = dataflow->getPreload(c);
            for (TOKEN_UNIT i = 0; i < ratePeriod; i++) {
                // might want to change variables to p, c, and t for legibility
                TOKEN_UNIT tokensProduced = dataflow->getEdgeInVector(c)[i % dataflow->getEdgeInPhasesCount(c)];
                TOKEN_UNIT tokensConsumed = dataflow->getEdgeOutVector(c)[i % dataflow->getEdgeOutPhasesCount(c)];

                TOKEN_UNIT lowerBound = tokensProduced + tokensConsumed - std::gcd(tokensProduced, tokensConsumed);
                VERBOSE_DEBUG(
                        "First Lower bound with " << tokensProduced << "," << tokensConsumed << " is " << lowerBound
                                                  << " gcd is " << std::gcd(tokensProduced, tokensConsumed));

                // FIXME: What is this for ?
                if (std::gcd(tokensProduced, tokensConsumed)) {
                    lowerBound += tokensInitial % std::gcd(tokensProduced, tokensConsumed);
                }
                VERBOSE_DEBUG(
                        "Second Lower bound with " << tokensProduced << "," << tokensConsumed << " is "
                                                   << lowerBound
                                                   << " gcd is " << std::gcd(tokensProduced, tokensConsumed));

                lowerBound = std::max(lowerBound, tokensInitial);

                VERBOSE_DEBUG("Lower bound with " << tokensProduced << "," << tokensConsumed << " is " << lowerBound
                                                  << " gcd is " << std::gcd(tokensProduced, tokensConsumed));
                // take the lowest bound amongst phases of prod/cons
                if (lowerBound < buffer_size) {
                    buffer_size = lowerBound;
                }
            }

            VERBOSE_DEBUG("Final buffer_size is " << buffer_size);

            return buffer_size;
        }

        algorithms::dse::TokenConfiguration::PerformanceResult
        asap_performance_func(models::Dataflow *g, const algorithms::dse::TokenConfiguration &config) {

            {
                ForEachEdge(g, e) {
                    if (g->getEdgeType(e) == EDGE_TYPE::FEEDBACK_EDGE) {
                        TOKEN_UNIT v = config.getConfiguration().at(g->getEdgeId(e));
                        g->setPreload(e, v);
                    }
                }
            }

            //VERBOSE_INFO("compute_Kperiodic_throughput_and_cycles");
            std::pair<ARRAY_INDEX, EXEC_COUNT> actorInfo;
            bool useCorrectedStorDepDetection = false;

            std::map<Edge, BufferInfos> channelQuants;
            TOKEN_UNIT distSz = 0;
            {
                ForEachEdge(g, e) {
                    channelQuants[e].buffer_size = g->getPreload(
                            e); // assume initial tokesn specified by dataflow graph
                    channelQuants[e].preload = g->getPreload(e);
                    distSz += g->getPreload(e);
                }
            }
            StorageDistribution testSD(g,
                                       0,
                                       channelQuants);

            kperiodic_result_t result_max = computeComponentThroughputCycles(g, actorInfo, testSD,
                                                                             useCorrectedStorDepDetection);
            //VERBOSE_INFO("Done");
            algorithms::dse::TokenConfiguration::PerformanceResult res(0, {});
            res.throughput = result_max.throughput;
            for (Edge e: result_max.critical_edges) res.critical_edges.insert(g->getEdgeId(e));
            return res;
        }

        algorithms::dse::TokenConfiguration::PerformanceResult
        kperiodic_performance_func(models::Dataflow *g, const algorithms::dse::TokenConfiguration &config) {

            {
                ForEachEdge(g, e) {
                    if (g->getEdgeType(e) == EDGE_TYPE::FEEDBACK_EDGE) {
                        TOKEN_UNIT v = config.getConfiguration().at(g->getEdgeId(e));
                        g->setPreload(e, v);
                    }
                }
            }

            //VERBOSE_INFO("compute_Kperiodic_throughput_and_cycles");
            kperiodic_result_t result_max = algorithms::compute_Kperiodic_throughput_and_cycles(g);
            //VERBOSE_INFO("Done");
            algorithms::dse::TokenConfiguration::PerformanceResult res(0, {});
            res.throughput = result_max.throughput;
            for (Edge e: result_max.critical_edges) res.critical_edges.insert(g->getEdgeId(e));
            return res;
        }

        std::vector<algorithms::dse::TokenConfiguration>
        throughputbuffering_initial_func(const models::Dataflow *dataflow) {

            std::map<ARRAY_INDEX, TOKEN_UNIT> configuration;


            // FIXME: This matching section is ugly, but it's only once I'll fix it later .. (maybe)
            {
                ForEachEdge(dataflow, e1) {

                    if (dataflow->getEdgeType(e1) == EDGE_TYPE::NORMAL_EDGE) {

                        const std::string channel_name = dataflow->getEdgeName(e1);
                        Edge e2 = dataflow->getEdgeByName(
                                dataflow->getEdgeName(e1) + "_prime"); // Must be found ... if not better to crash !

                        VERBOSE_ASSERT(dataflow->getEdgeType(e2) == EDGE_TYPE::FEEDBACK_EDGE,
                                       "The prime edge must be a feedback edge.");

                        VERBOSE_ASSERT((dataflow->getEdgeSource(e1) == dataflow->getEdgeTarget(e2)) and
                                       (dataflow->getEdgeSource(e2) == dataflow->getEdgeTarget(e1)),
                                       "The prime edge must be return way.");

                        ARRAY_INDEX tid = dataflow->getEdgeId(e2);
                        TOKEN_UNIT minimal_buffer_size = computeMinimalBufferSize(dataflow, e1);
                        TOKEN_UNIT initial_feedback_size = minimal_buffer_size -
                                                           commons::floor(dataflow->getPreload(e1),
                                                                          dataflow->getFineGCD(e1));

                        configuration[tid] = initial_feedback_size;

                        VERBOSE_DEBUG("  - Channel " << channel_name
                                                     << ", input rates are "
                                                     << commons::toString(dataflow->getEdgeInVector(e1))
                                                     << ", output rates are "
                                                     << commons::toString(dataflow->getEdgeInVector(e1))
                                                     << ", minimal buffer size is " << minimal_buffer_size
                                                     << ", initial feedback size is " << initial_feedback_size);
                    }


                }
            }


            {
                ForEachEdge(dataflow, e) {
                    ARRAY_INDEX tid = dataflow->getEdgeId(e);
                    if (dataflow->getEdgeType(e) == EDGE_TYPE::FEEDBACK_EDGE) {
                        VERBOSE_ASSERT(configuration.count(tid) > 0, "All feedback edge must be set.");
                    }
                }
            }

            return {algorithms::dse::TokenConfiguration(dataflow, configuration)};
        }


        TokenConfigurationSet solve_throughputbuffering(models::Dataflow *const dataflow,
                                                        ThroughputBufferingMode mode,
                                                        const ExplorationParameters& params,
                                                        algorithms::dse::TokenConfiguration *tc
        ) {


            // Ensure dataflow has no feedbackbuffers.
            // Create feedbackbuffers with correct initial configuration (taking into account useful preload).

            // Compute the target throughput
            TIME_UNIT target_throughput = algorithms::compute_Kperiodic_throughput_and_cycles(dataflow).throughput;
            auto *dataflow_prime = new models::Dataflow(*dataflow);
            algorithms::transformation::generateInplaceFeedbackBuffers(dataflow_prime);
            dataflow_prime->precomputeFineGCD();
            computeRepetitionVector(dataflow_prime);
            ThroughputBufferingStopCondition throughput_buffering_stop_condition(target_throughput);
            ThroughputBufferingNext throughputbuffering_next_distributions(
                    (mode == ASAP_DSE_MODE) ? KDSE_MODE : mode,
                    params.use_cache
                    );

            algorithms::dse::ModularDSE dse(dataflow_prime,
                                            (mode == ASAP_DSE_MODE) ? asap_performance_func
                                                                    : kperiodic_performance_func,
                                            throughputbuffering_next_distributions,
                                            throughput_buffering_stop_condition,
                                            params.thread_count);

            if (!params.import_filename.empty()) {
                VERBOSE_INFO("Load previous search points from " << params.import_filename);
                dse.import_results(params.import_filename);
            }

            if (tc) {
                VERBOSE_INFO("Initial state of the search is forced to " << *tc);
                dse.add_initial_job(*tc);
            } else {
                dse.add_initial_jobs(throughputbuffering_initial_func(dataflow_prime));
            }


            // Run the DSE exploration for a short period of time (e.g., 100 milliseconds)
            std::future<void> exploration_future = std::async(std::launch::async, [&dse, params] {
                dse.explore(params);
                VERBOSE_INFO("Explore function returned.");
            });



            VERBOSE_INFO("Start waiting for exploration_future.");
            bool timeout_happened = false;
            if (params.timeout_sec > 0) {
                // Manage the end with a timeout
                if (std::future_status::timeout ==
                    exploration_future.wait_for(std::chrono::seconds(params.timeout_sec))) {
                    // The timeout happened, we need to force the end.
                    timeout_happened = true;
                    VERBOSE_INFO("Timeout, sending a stop signal.");
                    dse.stop();
                    VERBOSE_INFO("Wait for stop signal effect.");
                } else {
                    VERBOSE_INFO("exploration_future ended by itself, without stop signal.");
                }
            }

            exploration_future.wait();

            if (timeout_happened) {
                VERBOSE_INFO("Exploration_future ended, after stop signal.");
                VERBOSE_WARNING("Incomplete exploration.");
            }

            return dse.getResults();

        }


        void throughput_buffering_dse(models::Dataflow *const dataflow, parameters_list_t params) {
            size_t realtime_output = (params.count("realtime") > 0) && commons::fromString<bool>(
                    params.at("realtime"));
            size_t thread_count = (params.count("thread") > 0) ? commons::fromString<size_t>(params.at("thread"))
                                                               : 1;
            size_t timeout = (params.count("timeout") > 0) ? commons::fromString<size_t>(params.at("timeout")) : 0;
            size_t limit = (params.count("limit") > 0) ? commons::fromString<size_t>(params.at("limit")) : 0;
            std::string filename = (params.count("import") > 0) ? params.at("import") : "";
            algorithms::dse::TokenConfiguration *tc = (params.count("init") > 0)
                                                      ? new algorithms::dse::TokenConfiguration(dataflow,
                                                                                                commons::split<TOKEN_UNIT>(
                                                                                                        params.at(
                                                                                                                "init"),
                                                                                                        ','))
                                                      : nullptr;

            VERBOSE_ASSERT(params.count("init") + params.count("import") != 2,
                           "Please pass your init inside the import file");

            ThroughputBufferingMode mode = KDSE_MODE; // Default mode
            if (params.count("mode")) {
                if (params.at("mode") == "ASAP") mode = ASAP_DSE_MODE;
                if (params.at("mode") == "KDSE") mode = KDSE_MODE;
                if (params.at("mode") == "K2DSE") mode = K2DSE_MODE;
                if (params.at("mode") == "K2DSEA") mode = K2DSEA_MODE;
            }
            size_t use_cache = (params.count("usecache") > 0) && commons::fromString<bool>(
                    params.at("usecache"));

            ExplorationParameters exploration_parameters;
            exploration_parameters.timeout_sec = timeout;
            exploration_parameters.thread_count = thread_count;
            exploration_parameters.limit = limit;
            exploration_parameters.realtime_output = realtime_output;
            exploration_parameters.timeout_sec = timeout;
            exploration_parameters.timeout_sec = timeout;
            exploration_parameters.import_filename = filename;
            exploration_parameters.use_cache = use_cache;

            TokenConfigurationSet result = solve_throughputbuffering(dataflow, mode, exploration_parameters, tc);

            delete tc;
        }
    } // end of dse namespace
// end of algorithms namespace

