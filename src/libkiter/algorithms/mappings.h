/*
 * mappings.h
 *
 *  Created on: Aug 21, 2019
 *      Author: toky
 */

#ifndef _MAPPINGS_H_
#define _MAPPINGS_H_

#include <commons/commons.h>
#include <commons/KiterRegistry.h>

namespace models {
	class Dataflow;
}

typedef std::map <node_id_t, std::vector < std::tuple<edge_id_t,edge_id_t,ARRAY_INDEX> > > router_xbar_usage_t;

namespace algorithms {



	namespace mapping {
		void createNoC (models::Dataflow* const  dataflow, parameters_list_t params);
		void randomMapping (models::Dataflow* const  dataflow, parameters_list_t params);
		void moduloMapping (models::Dataflow* const  dataflow, parameters_list_t params);
		void BufferlessNoCMapAndRoute (models::Dataflow* const  dataflow, parameters_list_t params);
		void xyRouting (models::Dataflow* const  dataflow, parameters_list_t params);
		void randomRouting (models::Dataflow* const  dataflow, parameters_list_t params);
		void generateTDMA (models::Dataflow* const  dataflow, parameters_list_t params);
	}

	void ModelNoCConflictFreeCommunication(models::Dataflow* const  dataflow, parameters_list_t   param_list = parameters_list_t());

	void FindConflicts(models::Dataflow* const  dataflow, parameters_list_t   param_list = parameters_list_t());

}
// Helpers to map vertex to cores, need a way to also consider routers
ADD_TRANSFORMATION(createNoC,
	transformation_t({ "createNoC" , "Create NoC based on file inserted", algorithms::mapping::createNoC} )
);
ADD_TRANSFORMATION(randomMapping,
		transformation_t({ "randomMapping" , "This command will associate a mapping to each task of the graph. Task unspecified as parameters will be randomly allocated to a core.", algorithms::mapping::randomMapping} )
);
ADD_TRANSFORMATION(generateTDMA,
		transformation_t({ "generateTDMA" , "Change seed.", algorithms::mapping::generateTDMA} )
);
ADD_TRANSFORMATION(moduloMapping,
		transformation_t({ "moduloMapping" , "This command will associate a mapping to each task of the graph. Task unspecified as parameters will be randomly allocated to a core.", algorithms::mapping::moduloMapping} )
);
ADD_TRANSFORMATION(BufferlessNoCMapAndRoute,
		transformation_t({ "BufferlessNoCMapAndRoute" , "Mapping and Routing combined.", algorithms::mapping::BufferlessNoCMapAndRoute} )
);
ADD_TRANSFORMATION(xyRouting,
		transformation_t({ "xyRouting" , "Create X-Y routing", algorithms::mapping::xyRouting} )
);

ADD_TRANSFORMATION(randomRouting,
		transformation_t({ "randomRouting" , "Create random X-Y routing", algorithms::mapping::randomRouting} )
);
ADD_TRANSFORMATION(ModelNoCConflictFreeCommunication,
		transformation_t({ "ModelNoCConflictFreeCommunication" , "Given a graph with Mapping, can add fictive task to model network", algorithms::ModelNoCConflictFreeCommunication} )
);


#endif
