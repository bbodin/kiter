/*
 * sample.h
 *
 *  Created on: Jun 24, 2021
 *      Author: SamKouteili
 */

#ifndef TESTS_GENERATE_RANDOM_GRAPH_H_
#define TESTS_GENERATE_RANDOM_GRAPH_H_


#include <models/Dataflow.h>
#include <generators/RandomGenerator.h>


models::Dataflow* generate_random_graph (int num_actor,
					 int num_buffers,
					 int max_phases,
					 int max_weight,
					 int max_duration) {
	
	RandomGeneratorConfiguration config;
	config.min_vertices_count      = num_actor;
	config.max_vertices_count      = num_actor;
	config.min_edges_count         = num_buffers;
	config.max_edges_count         = num_buffers;
	config.max_weight              = max_weight;
	config.max_duration            = max_duration;
	config.max_phase_quantity      = max_phases;

	RandomGenerator generator (config);
	models::Dataflow * dataflow = generator.generate();

	return dataflow;
}


#endif /* TESTS_GENERATE_RANDOM_GRAPH_H_ */
