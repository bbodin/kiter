/*
 * expansion_experiments.cpp
 *
 *  Created on: Feb 7, 2021
 *      Author: toky
 */

#include "expansion_experiment.h"
#include <algorithms/experiments/ExpansionGraph.h>

#include <models/Dataflow.h>
#include <algorithms/schedulings.h>
#include <commons/commons.h>


#include <string>

namespace algorithms {

	periodicity_vector_t generatePeriodicityVector (models::Dataflow* const  dataflow, const parameters_list_t& params) {

		periodicity_vector_t K = scheduling::generate1PeriodicVector (dataflow);

		for (Vertex t : dataflow->vertices()) {
			std::string task_name = dataflow->getVertexName(t);
			if (params.count(task_name)) {
				K[t] = commons::fromString<EXEC_COUNT>(params.at(task_name));
			}
		}

		return K;
	}

	void generate_expansion  (models::Dataflow* const  dataflow, parameters_list_t params) {

		periodicity_vector_t K = generatePeriodicityVector (dataflow, params);
		ExpansionGraph* eg = new ExpansionGraph (dataflow, K);

		if (params.count("DOT")) {
			std::cout << eg->toDOT();
		}

	}

}
