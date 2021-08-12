/*
 * UserCaseTest.cpp
 *
 *  Created on: Dec 3, 2019
 *      Author: toky
 */


#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"
#include <printers/stdout.h>
#include "helpers/mergetester.h"
#include <models/Dataflow.h>
#include <algorithms/schedulings.h>
#include <printers/SDF3Wrapper.h>
#include <algorithms/transformation/merging.h>

BOOST_FIXTURE_TEST_SUITE( use_case_test , WITH_VERBOSE)

BOOST_AUTO_TEST_CASE( merge_test_LTE )
{

	//models::Dataflow* graph = commons::readSDF3File("software_noc_246.xml");
	models::Dataflow* graph = generateMergeTester () ;

	std::string name = "mid-10-14_34_mid-9-13_34";
	auto v1 = graph->getVertexByName("mid-10-14_34");
	auto v2 = graph->getVertexByName("mid-9-13_34");
	std::vector<ARRAY_INDEX> tasks = {graph->getVertexId(v1), graph->getVertexId(v2) };
	BOOST_REQUIRE(algorithms::transformation::mergeCSDFFromKperiodicSchedule(graph,name, tasks));

	printers::GenerateGraphDOT(graph);
}

BOOST_AUTO_TEST_SUITE_END()






