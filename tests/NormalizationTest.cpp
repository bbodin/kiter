/*
 * NormalizationTest.cpp
 *
 *  Created on: Jun 21, 2021
 *      Author: SamKouteili
 */

#define BOOST_TEST_MODULE NormalizationTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include "helpers/random_generator.h"
#include <algorithms/normalization.h>
#include <chrono>
#include <printers/stdout.h>


#define MAX_ITER        10
#define MAX_PHASE_COUNT 5

BOOST_FIXTURE_TEST_SUITE( normalization_test_suite , WITH_VERBOSE)

long generate_normalization_vector (models::Dataflow* dataflow){
	printers::printInfos(dataflow);
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	auto start = std::chrono::high_resolution_clock::now();
	//TODO : The normalization does not support non-connected graphs!
	//VERBOSE_ASSERT(algorithms::normalize(dataflow), "Could not normalize");
	auto elapse = std::chrono::high_resolution_clock::now() - start;
	return elapse.count()/ 1000000;
}

BOOST_AUTO_TEST_CASE( generate_normalization_vector__performance_test )
{

	std::cout << "" << "actor number"
			<< ", " << "buffer number"
			<<", " <<  "repetition vector" << std::endl;

	for (int i = 1; i <= MAX_ITER; i++) {
		int buf_num = std::rand() % std::max(1,(i)/2);
		models::Dataflow*  g = generate_random_graph(i, buf_num,  MAX_PHASE_COUNT, i, i);
		std::cout << "" << i
			<< ", " << buf_num
			<< ", " << generate_normalization_vector(g);
		delete g;
	}
}

BOOST_AUTO_TEST_SUITE_END()




