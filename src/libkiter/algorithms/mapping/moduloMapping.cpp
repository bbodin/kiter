/*
 * moduloMapping.cpp
 *
 *  Created on: Aug 21, 2019
 *      Author: toky
 */




#include <models/Dataflow.h>
#include <algorithms/mappings.h>

void algorithms::mapping::moduloMapping (models::Dataflow* const  dataflow, parameters_list_t params) {

	VERBOSE_INFO("moduloMapping");
	for (auto t : dataflow->vertices()) {
		if (params.find(dataflow->getVertexName(t)) != params.end()) {
			dataflow->setMapping(t, commons::fromString<node_id_t> (params[dataflow->getVertexName(t)]));
		} else {
			node_id_t modulo_id = (dataflow->getVertexId(t) - 1) % dataflow->getNoC().size();
			dataflow->setMapping(t, modulo_id);
		}
	}

	VERBOSE_INFO("XYRouting");
	// TODO: This needs to be done
	for (auto b : dataflow->edges()) {

	}

}

