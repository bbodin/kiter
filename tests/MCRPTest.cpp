/*
 * MCRPTest.cpp
 *
 *  Created on: Jun 24, 2021
 *      Author: SamKouteili
 */

#define BOOST_TEST_MODULE MCRPTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include "helpers/random_generator.h"
#include <algorithms/throughput/kperiodic.h>
#include <algorithms/schedulings.h>
#include <models/EventGraph.h>
#include <chrono>


#define MAX_ITER        10
#define MAX_PHASE_COUNT 5

/* how to define bound? */

BOOST_FIXTURE_TEST_SUITE( MCRP_test_suite , WITH_VERBOSE)

double event_graph_performance_tester (models::Dataflow* dataflow){


    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	const periodicity_vector_t kvector_one = algorithms::scheduling::generate1PeriodicVector(dataflow);

	auto start = std::chrono::high_resolution_clock::now();
	models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector_one);
    auto elapse = std::chrono::high_resolution_clock::now() - start;

    VERBOSE_INFO("KPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

	return elapse.count()/ 1000000;
}

double MCRP_performance_tester (models::Dataflow* dataflow){
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	const periodicity_vector_t kvector_one = algorithms::scheduling::generate1PeriodicVector(dataflow);

    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector_one);
	VERBOSE_INFO("KPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

    auto start = std::chrono::high_resolution_clock::now();
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
    auto elapse = std::chrono::high_resolution_clock::now() - start;

	return elapse.count()/ 1000000;
}


BOOST_AUTO_TEST_CASE( generate_event_graph__performance_test )
{
	std::cout << "Start generate_event_graph__performance_test" << std::endl;

	for (int i = 1; i <= MAX_ITER; i++) {
		int buf_num = std::rand() % std::max(1,(i)/2);
		std::cout << "Iteration " << i << " buf_num=" << buf_num << std::endl;
		models::Dataflow*  g = generate_random_graph(i, buf_num,  MAX_PHASE_COUNT, i, i);
		std::cout << "Graph done " << i << std::endl;
		std::cout << event_graph_performance_tester(g) << std::endl;
		std::cout << "event_graph_performance_tester done " << i << std::endl;
		std::cout << MCRP_performance_tester(g) << std::endl;
		std::cout << "MCRP_performance_tester done " << i << std::endl;
	    delete g;
	};
}

BOOST_AUTO_TEST_CASE( MCRP_performance_test )
{
// 	for (int i = MAX_ITER/10; i <= MAX_ITER; i += MAX_ITER/10) {
// 		int buf_num = std::rand() % (i/2) + i; 
// 		models::Dataflow*  g = generate_random_graph(i, buf_num,  MAX_PHASE_COUNT, i, i);
// 		std::cout << MCRP_performance_tester(g);
// 	};
}

BOOST_AUTO_TEST_SUITE_END()




