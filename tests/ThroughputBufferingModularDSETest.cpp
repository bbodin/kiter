//
// Created by toky on 24/4/23.
//

#define BOOST_TEST_MODULE ThroughputBufferingModularDSETest

#include "helpers/test_classes.h"
#include "helpers/random_generator.h"
#include <algorithms/dse/ThroughputBufferingDSE.h>

//
models::Dataflow* generateGraph () {
    models::Dataflow* new_graph = new models::Dataflow();

    {
        auto new_vertex = new_graph->addVertex(1);
        new_graph->setVertexName(new_vertex,"A");
        new_graph->setVertexType(new_vertex,"");
        new_graph->setInitPhasesQuantity(new_vertex,0);
        new_graph->setPhasesQuantity(new_vertex,2);
        new_graph->setReentrancyFactor(new_vertex,1);
        new_graph->setVertexDuration(new_vertex,{3,1});
    }
    {
        auto new_vertex = new_graph->addVertex(2);
        new_graph->setVertexName(new_vertex,"B");
        new_graph->setVertexType(new_vertex,"");
        new_graph->setInitPhasesQuantity(new_vertex,0);
        new_graph->setPhasesQuantity(new_vertex,3);
        new_graph->setReentrancyFactor(new_vertex,1);
        new_graph->setVertexDuration(new_vertex,{2,1,2});
    }
    {
        auto new_vertex = new_graph->addVertex(3);
        new_graph->setVertexName(new_vertex,"C");
        new_graph->setVertexType(new_vertex,"");
        new_graph->setInitPhasesQuantity(new_vertex,0);
        new_graph->setPhasesQuantity(new_vertex,1);
        new_graph->setReentrancyFactor(new_vertex,1);
        new_graph->setVertexDuration(new_vertex,{1});
    }
    {
        auto new_edge = new_graph->addEdge(new_graph->getVertexById(1), new_graph->getVertexById(2));
        new_graph->setEdgeInPhases(new_edge,{3,5});
        new_graph->setEdgeOutPhases(new_edge,{1,1,4});
        new_graph->setPreload(new_edge,0);
        new_graph->setEdgeName(new_edge,"channel_1");
        new_graph->setEdgeType(new_edge, EDGE_TYPE::NORMAL_EDGE);
    }
    {
        auto new_edge = new_graph->addEdge(new_graph->getVertexById(2), new_graph->getVertexById(3));
        new_graph->setEdgeInPhases(new_edge,{6,2,1});
        new_graph->setEdgeOutPhases(new_edge,{6});
        new_graph->setPreload(new_edge,0);
        new_graph->setEdgeName(new_edge,"channel_2");
        new_graph->setEdgeType(new_edge, EDGE_TYPE::NORMAL_EDGE);
    }
    {
        auto new_edge = new_graph->addEdge(new_graph->getVertexById(2), new_graph->getVertexById(1));
        new_graph->setEdgeInPhases(new_edge,{1,1,4});
        new_graph->setEdgeOutPhases(new_edge,{3,5});
        new_graph->setPreload(new_edge,0);
        new_graph->setEdgeName(new_edge,"channel_1_prime");
        new_graph->setEdgeType(new_edge, EDGE_TYPE::FEEDBACK_EDGE);
    }
    {
        auto new_edge = new_graph->addEdge(new_graph->getVertexById(3), new_graph->getVertexById(2));
        new_graph->setEdgeInPhases(new_edge,{6});
        new_graph->setEdgeOutPhases(new_edge,{6,2,1});
        new_graph->setPreload(new_edge,0);
        new_graph->setEdgeName(new_edge,"channel_2_prime");
        new_graph->setEdgeType(new_edge, EDGE_TYPE::FEEDBACK_EDGE);
    }
    return new_graph;
}


BOOST_FIXTURE_TEST_SUITE(ThroughputBuffering_modular_dse_test, WITH_SAMPLE)

    BOOST_AUTO_TEST_CASE(solve_ThroughputBuffering_test_with_reentrancy) {

         commons::set_verbose_mode(commons::DEBUG_LEVEL);
        // create the input graph
        models::Dataflow g;
        Vertex v1 = g.addVertex(4,"v1");
        Vertex v2 = g.addVertex(6,"v2");
        Edge e1 = g.addEdge(v1, v2, 1,"e");

        g.setPreload(e1, 2);


        g.setEdgeInPhases(e1, {1, 2});
        g.setEdgeOutPhases(e1, {1});


        g.setVertexDuration(v1,{1,1});
        g.setVertexDuration(v2,{1});

        g.setReentrancyFactor(v1,1);
        g.setReentrancyFactor(v2,1);

        g.setEdgeType(e1,EDGE_TYPE::NORMAL_EDGE);
        auto throughput_res = algorithms::compute_Kperiodic_throughput_and_cycles(&g);
        TIME_UNIT target_throughput = throughput_res.throughput;
        std::cout << "Throughput is " << target_throughput << std::endl;

        algorithms::dse::ExplorationParameters params;
        algorithms::dse::TokenConfigurationSet res = algorithms::dse::solve_throughputbuffering( &g, algorithms::dse::KDSE_MODE, params) ;

    }

    BOOST_AUTO_TEST_CASE(ThroughputBuffering_dse_test) {

         commons::set_verbose_mode(commons::DEBUG_LEVEL);
        // create the input graph
        models::Dataflow g;
        Vertex v1 = g.addVertex(4,"v1");
        Vertex v2 = g.addVertex(6,"v2");
        Edge e1 = g.addEdge(v1, v2, 1,"e");

        g.setPreload(e1, 2);


        g.setEdgeInPhases(e1, {1, 2});
        g.setEdgeOutPhases(e1, {1});


        g.setVertexDuration(v1,{1,1});
        g.setVertexDuration(v2,{1});

        g.setReentrancyFactor(v1,1);
        g.setReentrancyFactor(v2,1);

        g.setEdgeType(e1,EDGE_TYPE::NORMAL_EDGE);

        parameters_list_t params;
        params["realtime"] = "1";
        algorithms::dse::throughput_buffering_dse(&g, params);
    }


    BOOST_AUTO_TEST_CASE(k2dse_subtask_test) {

        commons::set_verbose_mode(commons::DEBUG_LEVEL);
        VERBOSE_DEBUG("Start the test in debug mode");

        // 1. Crete sub graph and init config
        models::Dataflow* cc_g = generateGraph();
        computeRepetitionVector(cc_g);
        VERBOSE_ASSERT(cc_g, "Graph has not been generated");
        std::map<ARRAY_INDEX, TOKEN_UNIT> new_configuration_cc;
        new_configuration_cc[cc_g->getEdgeId(cc_g->getEdgeByName("channel_1_prime"))] = 8;
        new_configuration_cc[cc_g->getEdgeId(cc_g->getEdgeByName("channel_2_prime"))] = 9;
        algorithms::dse::TokenConfiguration tc(cc_g, new_configuration_cc);
        TIME_UNIT target_th = 0.04;
        VERBOSE_DEBUG("Init: " <<  tc.to_csv_line());

        // 2. run a dse with target throughout to beat.
        algorithms::dse::ModularDSE dse(cc_g,
                                        algorithms::dse::kperiodic_performance_func,
                                        algorithms::dse::ThroughputBufferingNext(algorithms::dse::KDSE_MODE, false),
                                        algorithms::dse::ThroughputBufferingStopCondition(target_th, true),
                                        1);
        dse.add_initial_job(tc);
        algorithms::dse::ExplorationParameters params;
        dse.explore(params);
        auto cc_sds = dse.getResults();


        VERBOSE_DEBUG("End of local search:");

        // 3. any solutions that beat the target are considered.
        for (auto solution : cc_sds) {
            if (solution.getPerformance().throughput > target_th) {
                VERBOSE_DEBUG("    - KEEP " << solution.to_csv_line());
            } else {
                VERBOSE_DEBUG("    - SKIP " << solution.to_csv_line());
            }
        }

        delete cc_g;
    }


    BOOST_AUTO_TEST_CASE(solve_ThroughputBuffering_test_with_timeout) {

        commons::set_verbose_mode(commons::DEBUG_LEVEL);
        // create the input graph
        models::Dataflow g;
        Vertex v1 = g.addVertex(4,"v1");
        Vertex v2 = g.addVertex(6,"v2");
        Edge e1 = g.addEdge(v1, v2, 1,"e");

        g.setPreload(e1, 2);


        g.setEdgeInPhases(e1, {1, 2});
        g.setEdgeOutPhases(e1, {11});


        g.setVertexDuration(v1,{1,1});
        g.setVertexDuration(v2,{4});

        g.setReentrancyFactor(v1,1);
        g.setReentrancyFactor(v2,1);

        g.setEdgeType(e1,EDGE_TYPE::NORMAL_EDGE);
        auto throughput_res = algorithms::compute_Kperiodic_throughput_and_cycles(&g);
        TIME_UNIT target_throughput = throughput_res.throughput;
        std::cout << "Throughput is " << target_throughput << std::endl;

        algorithms::dse::ExplorationParameters params;
        params.timeout_sec = 1;
        params.realtime_output = true;
        algorithms::dse::TokenConfigurationSet res = algorithms::dse::solve_throughputbuffering( &g, algorithms::dse::KDSE_MODE, params) ;


    }

BOOST_AUTO_TEST_SUITE_END()
