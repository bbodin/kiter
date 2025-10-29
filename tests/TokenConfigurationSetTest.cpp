//
// Created by toky on 25/4/23.
//
#define BOOST_TEST_MODULE TokenConfigurationSetTest
#include "helpers/test_classes.h"
#include <algorithms/dse/TokenConfigurationSet.h>

#include "algorithms/transformation/AddFeedbackBuffers.h"

BOOST_FIXTURE_TEST_SUITE(test_suite_TokenConfigurationSet, WITH_SAMPLE)

    BOOST_AUTO_TEST_CASE(add_non_dominated_configurations) {
        algorithms::dse::TokenConfigurationSet config_set;
        cycle_sample->setEdgeType(cycle_sample->getFirstEdge(), EDGE_TYPE::FEEDBACK_EDGE);
        algorithms::dse::TokenConfiguration config1(cycle_sample, {1});
        algorithms::dse::TokenConfiguration config2(cycle_sample, {2});
        algorithms::dse::TokenConfiguration config3(cycle_sample, {3});

        config_set.add(config1);
        config_set.add(config2);
        config_set.add(config3);

        BOOST_CHECK_EQUAL(config_set.size(), 3);
    }

    BOOST_AUTO_TEST_CASE(add_dominated_configurations) {
        algorithms::dse::TokenConfigurationSet config_set;

        cycle_sample->setEdgeType(cycle_sample->getFirstEdge(), EDGE_TYPE::FEEDBACK_EDGE);
        algorithms::dse::TokenConfiguration config1(cycle_sample, {1});
        algorithms::dse::TokenConfiguration config2(cycle_sample, {1});
        algorithms::dse::TokenConfiguration config3(cycle_sample, {1});

        config_set.add(config1);
        config_set.add(config2);
        config_set.add(config3);

        BOOST_CHECK_EQUAL(config_set.size(), 1);
    }

    BOOST_AUTO_TEST_CASE(remove_configurations) {
        algorithms::dse::TokenConfigurationSet config_set;

        cycle_sample->setEdgeType(cycle_sample->getFirstEdge(), EDGE_TYPE::FEEDBACK_EDGE);
        algorithms::dse::TokenConfiguration config1(cycle_sample, {1});
        algorithms::dse::TokenConfiguration config2(cycle_sample, {2});
        algorithms::dse::TokenConfiguration config3(cycle_sample, {3});

        config_set.add(config3);
        config_set.add(config1);
        config_set.add(config2);

        BOOST_CHECK_EQUAL(config_set.size(), 3);
        config_set.remove(config3);
        BOOST_CHECK_EQUAL(config_set.size(), 2);
        config_set.remove(config2);
        BOOST_CHECK_EQUAL(config_set.size(), 1);
        config_set.remove(config1);
        BOOST_CHECK_EQUAL(config_set.size(), 0);

    }

    BOOST_AUTO_TEST_CASE(tokenconfig_to_storagedist_set_conversion) {
        // Original graph with one normal edge "e"
        std::cout << "Start test" << std::endl;

        commons::set_verbose_mode(commons::DEBUG_LEVEL);
        std::cout << "generate sample" << std::endl;

        models::Dataflow* new_graph = new models::Dataflow();
        {
                auto new_vertex = new_graph->addVertex(1);
                new_graph->setVertexName(new_vertex,"A");
                new_graph->setInitPhasesQuantity(new_vertex,0);
                new_graph->setPhasesQuantity(new_vertex,2);
                new_graph->setReentrancyFactor(new_vertex,1);
                new_graph->setVertexDuration(new_vertex,{3,1});
        }
        {
                auto new_vertex = new_graph->addVertex(2);
                new_graph->setVertexName(new_vertex,"B");
                new_graph->setInitPhasesQuantity(new_vertex,0);
                new_graph->setPhasesQuantity(new_vertex,3);
                new_graph->setReentrancyFactor(new_vertex,1);
                new_graph->setVertexDuration(new_vertex,{2,1,2});
        }

        {
                auto new_edge = new_graph->addEdge(new_graph->getVertexById(1), new_graph->getVertexById(2));
                new_graph->setEdgeInPhases(new_edge,{3,5});
                new_graph->setEdgeOutPhases(new_edge,{1,1,4});
                new_graph->setPreload(new_edge,0);
                new_graph->setEdgeName(new_edge,"channel_1");
        }
		auto g_orig =new_graph;

        std::cout << "copy sample" << std::endl;
        auto *g_fb = new models::Dataflow(*g_orig);

        std::cout << "generateInplaceFeedbackBuffers" << std::endl;
        algorithms::transformation::generateInplaceFeedbackBuffers(g_fb);

        std::cout << "generate config" << std::endl;
        std::map<ARRAY_INDEX, TOKEN_UNIT> conf;
        conf[g_fb->getEdgeId(g_fb->getEdgeByName("channel_1_prime"))] = 7;
        algorithms::dse::TokenConfiguration tc(g_fb, conf);
        algorithms::dse::TokenConfigurationSet set;
        set.add(tc);

        std::cout << "generation done" << std::endl;

        // Convert to storage distribution set for original graph
        StorageDistributionSet sd_set = set.toStorageDistributionSet(g_orig);
        BOOST_CHECK_EQUAL(sd_set.getSize(), 1);
        auto all = sd_set.getSet();
        BOOST_CHECK_EQUAL(all.size(), 1); // one distribution size bucket
        // Extract the only StorageDistribution and verify channel quantity: floor(7, gcd 2)=6 plus 5 => 11
        StorageDistribution sd = all.begin()->second.front();
        BOOST_CHECK_EQUAL(7, sd.getDistributionSize()); // one distribution size bucket



    }

BOOST_AUTO_TEST_SUITE_END()
