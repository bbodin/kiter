/*
 * PipelineNamingTest.cpp
 *
 *  Created on: Feb 8, 2021
 *      Author: toky
 */


#define BOOST_TEST_MODULE PerformanceTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include <generators/RandomGenerator.h>
#include <algorithms/normalization.h>
#include <chrono>

long generate_pipeline_with_names (int iter) {
	std::vector<std::string> names;

	for (auto i = 0 ; i <= iter ; i++) {
		 names.push_back(commons::toString(i));
	}

	models::Dataflow* g = new models::Dataflow(0);
	auto start = std::chrono::high_resolution_clock::now();
	Vertex last = g->addVertex(names[0]);
	
	for (auto i = 1 ; i <= iter ; i++) {
			 Vertex new_last = g->addVertex(names[i]);
			 g->addEdge(last, new_last,names[i]);
			 last = new_last;
	}
	
	auto elapse = std::chrono::high_resolution_clock::now() - start;
	BOOST_REQUIRE_EQUAL( g->getVerticesCount(), iter + 1);
 	BOOST_REQUIRE_EQUAL( g->getEdgesCount(), iter );

	delete g;
	return elapse.count() / 1000000;
}

long generate_pipeline_with_names_ids (int iter) {
	std::vector<std::string> names;

	for (auto i = 0 ; i <= iter ; i++) {
		names.push_back(commons::toString(i));
	}
	
	models::Dataflow* g = new models::Dataflow(0);
	auto start = std::chrono::high_resolution_clock::now();
	Vertex last = g->addVertex(0,names[0]);

	for (auto i = 1 ; i <= iter ; i++) {
			 Vertex new_last = g->addVertex(i,names[i]);
			 g->addEdge(last, new_last,i,names[i]);
			 last = new_last;

	}

	auto elapse = std::chrono::high_resolution_clock::now() - start;
	BOOST_REQUIRE_EQUAL( g->getVerticesCount(), iter + 1);
	BOOST_REQUIRE_EQUAL( g->getEdgesCount(), iter );

	delete g;
	return elapse.count() / 1000000;
}

long generate_pipeline_with_ids (int iter) {
	models::Dataflow* g = new models::Dataflow(0);
	auto start = std::chrono::high_resolution_clock::now();
	Vertex last = g->addVertex(0);

	for (auto i = 1 ; i <= iter ; i++) {
			 Vertex new_last = g->addVertex(i);
			 g->addEdge(last, new_last, i);
			 last = new_last;
	}

	auto elapse = std::chrono::high_resolution_clock::now() - start;
	BOOST_REQUIRE_EQUAL( g->getVerticesCount(), iter + 1);
	BOOST_REQUIRE_EQUAL( g->getEdgesCount(), iter );

	delete g;
	return elapse.count()/ 1000000;
}

long generate_pipeline (int iter) {
	models::Dataflow* g = new models::Dataflow(0);
	auto start = std::chrono::high_resolution_clock::now();
	Vertex last =g->addVertex();

	for (auto i = 0 ; i < iter ; i++) {
			 Vertex new_last = g->addVertex();
			 g->addEdge(last, new_last);
			 last = new_last;
	}

	auto elapse = std::chrono::high_resolution_clock::now() - start;
	BOOST_REQUIRE_EQUAL( g->getVerticesCount(), iter + 1);
	BOOST_REQUIRE_EQUAL( g->getEdgesCount(), iter );

	delete g;
	return elapse.count()/ 1000000;
}



BOOST_FIXTURE_TEST_SUITE( performance_test_suite, WITH_VERBOSE)

#define MAX_ITER 100
BOOST_AUTO_TEST_CASE( generate_pipeline_test )
{

	std::cout << "" << "iter"
			<<", " <<  "empty"
			<<", " << "with_ids"
			<<", " << "with_names"
			<<", " << "with_names_ids" << std::endl;

	for (int i = MAX_ITER / 10 ; i <= MAX_ITER ; i += MAX_ITER / 10) {
		std::cout << "" << i
				<<", " << generate_pipeline(i)
				<<", " << generate_pipeline_with_ids(i)
				<<", " << generate_pipeline_with_names(i)
				<<", " << generate_pipeline_with_names_ids(i) << std::endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()




