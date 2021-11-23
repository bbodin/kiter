/*
 *
 *  Created on: Jul 23, 2021
 *      Author: SamKouteili
 */

#define BOOST_TEST_MODULE ValidScheduleTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include <models/Scheduling.h>
#include "helpers/random_generator.h"
#include <algorithms/throughput/csdf_strictly_periodic.h>
#include <algorithms/schedulings.h>
#include <printers/stdout.h>
#include <printers/SDF3Wrapper.h>


BOOST_FIXTURE_TEST_SUITE( valid_schedule_test , WITH_VERBOSE)

BOOST_AUTO_TEST_CASE( bugfix1_test_case)
{
	models::Dataflow* new_graph = new models::Dataflow();

	{
	auto new_vertex = new_graph->addVertex(1);
	 new_graph->setVertexName(new_vertex,"Node_1");
	 new_graph->setInitPhasesQuantity(new_vertex,0);
	 new_graph->setPhasesQuantity(new_vertex,2);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{2,3});
	}
	{
	auto new_vertex = new_graph->addVertex(2);
	 new_graph->setVertexName(new_vertex,"Node_2");
	 new_graph->setInitPhasesQuantity(new_vertex,0);
	 new_graph->setPhasesQuantity(new_vertex,2);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{3,1});
	}
	{
	auto new_vertex = new_graph->addVertex(3);
	 new_graph->setVertexName(new_vertex,"Node_3");
	 new_graph->setInitPhasesQuantity(new_vertex,0);
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{2});
	}
	{
	auto new_vertex = new_graph->addVertex(4);
	 new_graph->setVertexName(new_vertex,"Node_4");
	 new_graph->setInitPhasesQuantity(new_vertex,0);
	 new_graph->setPhasesQuantity(new_vertex,3);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1,1,2});
	}
	{
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(2), new_graph->getVertexById(1));
	 new_graph->setEdgeInPhases(new_edge,{0,3});
	 new_graph->setEdgeOutPhases(new_edge,{0,3});
	 new_graph->setPreload(new_edge,3);
	 new_graph->setEdgeName(new_edge,"buffer_1");
	}
	{
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(2));
	 new_graph->setEdgeInPhases(new_edge,{0,2,0});
	 new_graph->setEdgeOutPhases(new_edge,{3,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"buffer_2");
	}
	{
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(2));
	 new_graph->setEdgeInPhases(new_edge,{0,2,0});
	 new_graph->setEdgeOutPhases(new_edge,{2,1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"buffer_3");
	}
	{
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(3));
	 new_graph->setEdgeInPhases(new_edge,{0,2,0});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"buffer_4");
	}
	{
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(1), new_graph->getVertexById(2));
	 new_graph->setEdgeInPhases(new_edge,{1,2});
	 new_graph->setEdgeOutPhases(new_edge,{1,2});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"buffer_5");
	}

	VERBOSE_INFO("Print the graph");
    std::cout << printers::generateSDF3XML(new_graph) << std::endl;


	VERBOSE_INFO("schedule the graph");

    VERBOSE_ASSERT(computeRepetitionVector(new_graph),"inconsistent graph");
    models::Scheduling sched = algorithms::scheduling::CSDF_SPeriodicScheduling(new_graph);

	VERBOSE_INFO("Print the schedule");
    sched.verbose_print();

	VERBOSE_INFO("Check the schedule");
    bool res = sched.is_valid_schedule();
    VERBOSE_ASSERT(res, "Invalid Schedule");

    delete new_graph;




}

BOOST_AUTO_TEST_CASE( small_graph_test_valid_schedule)
{
	VERBOSE_INFO("generate the graph");
    const models::Dataflow* gg = generate_random_graph(4, 5, 3, 3, 3);

	VERBOSE_INFO("Print the graph");
    std::cout << printers::generateSDF3XML(gg) << std::endl;
    std::cout << printers::generate_kiter(gg, false) << std::endl;

	VERBOSE_INFO("schedule the graph");
    models::Scheduling sched = algorithms::scheduling::CSDF_SPeriodicScheduling(gg);

	VERBOSE_INFO("Print the schedule");
    sched.verbose_print();

	VERBOSE_INFO("Check the schedule");
    bool res = sched.is_valid_schedule();
    VERBOSE_ASSERT(res, "Invalid Schedule");

    delete gg;

}


BOOST_AUTO_TEST_SUITE_END()




