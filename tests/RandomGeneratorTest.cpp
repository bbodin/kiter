/*
 * RandomgeneratorTest.cpp
 *
 *  Created on: Apr 30, 2020
 *      Author: toky
 */


#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include <algorithms/symbolic_execution.h>
#include <algorithms/schedulings.h>
#include "helpers/random_generator.h" //
#include <chrono>

#define MAX_ITER 30
#define MAX_PHASE_COUNT 5

static void verify_dataflow (models::Dataflow * dataflow, RandomGeneratorConfiguration config) {
	BOOST_REQUIRE_LE(dataflow->getVerticesCount() , config.max_vertices_count );
	BOOST_REQUIRE_LE(dataflow->getEdgesCount() , config.max_edges_count );
	BOOST_REQUIRE(computeRepetitionVector(dataflow));
	BOOST_REQUIRE_GT(algorithms::scheduling::CSDF_KPeriodicScheduling(dataflow).getGraphThroughput() , 0);
}

long performance_random_dataflow (int num_actor, int num_buffers, int max_phases, int max_weight, int max_duration) {
	
	RandomGeneratorConfiguration config;
	config.min_vertices_count      = num_actor;
	config.max_vertices_count      = num_actor;
	config.min_edges_count         = num_buffers;
	config.max_edges_count         = num_buffers;
	config.max_weight              = max_weight;
	config.max_duration            = max_duration;
	config.max_phase_quantity      = max_phases;

	auto start = std::chrono::high_resolution_clock::now();
	RandomGenerator generator (config);
	models::Dataflow * dataflow = generator.generate();
	auto elapse = std::chrono::high_resolution_clock::now() - start;

	verify_dataflow(dataflow, config);

	delete dataflow;
	return elapse.count() / 1000000;
}


BOOST_FIXTURE_TEST_SUITE( generator_test , WITH_VERBOSE)

BOOST_AUTO_TEST_CASE( generator_test_two_HSDF_tasks ) {
	performance_random_dataflow (2, 1, 1, 1, 1);
}

// BOOST_AUTO_TEST_CASE( dataflow_generator_test_HSDF ) {

// 	std::cout << "" << "actor number"
// 			<< ", " << "buffer number"
// 			<< ", " << "generation time";

// 	for (int i = MAX_ITER/10; i <= MAX_ITER; i += MAX_ITER/10) {
// 		int buf_num = std::rand() % (i/2) + i; 

// 		std::cout << "" << i
// 			<< ", " << buf_num
// 			<< ", " << performance_random_dataflow(i, buf_num, 1, 1, 1);
// 	}
// }

// BOOST_AUTO_TEST_CASE( dataflow_generator_test_SDF ) {

// 	std::cout << "" << "actor number"
// 			<< ", " << "buffer number"
// 			<< ", " << "generation time";

// 	for (int i = MAX_ITER/10; i <= MAX_ITER; i += MAX_ITER/10) {
// 		int buf_num = std::rand() % (i/2) + i; 

// 		std::cout << "" << i
// 			<< ", " << buf_num
// 			<< ", " << performance_random_dataflow(i, buf_num, 1, i, i);
// 	}
// }


// BOOST_AUTO_TEST_CASE( dataflow_generator_test_CSDF ) {

// 	std::cout << "" << "actor number"
// 			<< ", " << "buffer number"
// 			<< ", " << "generation time";

// 	for (int i = MAX_ITER/10; i <= MAX_ITER; i += MAX_ITER/10) {
// 		int buf_num = std::rand() % (i/2) + i; 

// 		std::cout << "" << i
// 			<< ", " << buf_num
// 			<< ", " << performance_random_dataflow(i, buf_num, MAX_PHASE_COUNT, i, i);
// 	}
// }

BOOST_AUTO_TEST_SUITE_END()








