/*
 * deGroote.c
 *
 *  Created on: 5 juil. 2013
 *      Author: toky
 */



#include <vector>
#include <commons/verbose.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>

#include "degroote.h"

models::EventGraph* algorithms::generate_LCG (models::Dataflow* const  dataflow) {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	VERBOSE_DEBUG_ASSERT(dataflow->is_normalized() == false,"Graph should not be normalized.");

	  {ForEachVertex(dataflow,t) {
	  	  VERBOSE_ASSERT(dataflow->getPhasesQuantity(t) == 1, "Support only SDF");
	  }}

	models::EventGraph * lcg = new models::EventGraph();

	/* generate nodes */
	{ForEachVertex(dataflow,pTask) {
		lcg->addEventGroup(dataflow->getVertexId(pTask),1,dataflow->getNi(pTask));
	}}

	// DEFINITION DES REENTRANCES
	//******************************************************************

	{ForEachVertex(dataflow,pTask) {
		EXEC_COUNT rfactor = dataflow->getReentrancyFactor(pTask);
		VERBOSE_ASSERT(rfactor <=1 , "unsupported");
		if (rfactor == 1) {
				Vertex source = pTask;
				Vertex target = pTask;

				ARRAY_INDEX source_id = dataflow->getVertexId(source);
				ARRAY_INDEX target_id = dataflow->getVertexId(target);
				ARRAY_INDEX edge_id   = 0;

				EXEC_COUNT Ni = dataflow->getNi(source);
				EXEC_COUNT Nj = dataflow->getNi(target);

				TOKEN_UNIT mc = 1;
				TOKEN_UNIT nc = 1;
				TOKEN_UNIT dc = 1;

				TIME_UNIT  tj = dataflow->getVertexDuration(target);


				VERBOSE_DEBUG("Buffer " << edge_id << " from " << dataflow->getVertexName(source) << "to " << dataflow->getVertexName(target) << " : " ) ;

				VERBOSE_DEBUG("  mc = " << mc );
				VERBOSE_DEBUG("  nc = " << nc );
				VERBOSE_DEBUG("  dc = " << dc );

				for (EXEC_COUNT j = 1 ; j <= Nj ; j++) {

					long int  pi_c_j       =  (long int) std ::ceil(commons::division(((double) j * (double) nc - (double) dc) , (double) mc));
					long int  tilde_pi_c_j = commons::modulo((pi_c_j - 1)  , (long int) Ni ) + 1 ;
					long int  delta_c_j    =  (long int )  std ::floor(commons::division( (double) pi_c_j -  (double) 1 , (double) Ni));

					EXEC_COUNT i           = (EXEC_COUNT) tilde_pi_c_j;

					VERBOSE_DEBUG("  j = " << j );
					VERBOSE_DEBUG("   pi_c_j = " << pi_c_j );
					VERBOSE_DEBUG("   tilde_pi_c_j = " << tilde_pi_c_j );
					VERBOSE_DEBUG("   delta_c_j = " << delta_c_j );
					VERBOSE_DEBUG("   i = " << i );

					models::EventGraphVertex source_event = lcg->getEventGraphVertex(source_id,1,i);
					models::EventGraphVertex target_event = lcg->getEventGraphVertex(target_id,1,j);

					TIME_UNIT  d = tj; // L(a) = l(ti)
					TIME_UNIT  w = - delta_c_j;

					lcg->addEventConstraint(source_event ,target_event,d,w,edge_id);
				}
			}
		}
	}


	// DEFINITION DES CONTRAINTES DE PRECEDENCES
	//******************************************************************
	{ForEachChannel(dataflow,c) {

		Vertex source = dataflow->getEdgeSource(c);
		Vertex target = dataflow->getEdgeTarget(c);

		ARRAY_INDEX source_id = dataflow->getVertexId(source);
		ARRAY_INDEX target_id = dataflow->getVertexId(target);
		ARRAY_INDEX edge_id   = dataflow->getEdgeId(c);

		EXEC_COUNT Ni = dataflow->getNi(source);
		EXEC_COUNT Nj = dataflow->getNi(target);

		TOKEN_UNIT mc = dataflow->getEdgeIn(c);
		TOKEN_UNIT nc = dataflow->getEdgeOut(c);
		TOKEN_UNIT dc = dataflow->getPreload(c);

		TIME_UNIT  tj = dataflow->getVertexDuration(target);

		VERBOSE_DEBUG("Buffer " << edge_id << " from " << dataflow->getVertexName(source) << "to " << dataflow->getVertexName(target) << " : " ) ;

		VERBOSE_DEBUG("  mc = " << mc );
		VERBOSE_DEBUG("  nc = " << nc );
		VERBOSE_DEBUG("  dc = " << dc );

		for (EXEC_COUNT j = 1 ; j <= Nj ; j++) {

			long int  pi_c_j       =  (long int) std ::ceil(commons::division(((double) j * (double) nc - (double) dc) , (double) mc));
			long int  tilde_pi_c_j = commons::modulo((pi_c_j - 1)  , (long int) Ni ) + 1 ;
			long int  delta_c_j    =  (long int )  std ::floor(commons::division( (double) pi_c_j -  (double) 1 , (double) Ni));

			EXEC_COUNT i          = (EXEC_COUNT) tilde_pi_c_j;


			VERBOSE_DEBUG("   j = " << j );
			VERBOSE_DEBUG("   pi_c_j = " << pi_c_j );
			VERBOSE_DEBUG("   tilde_pi_c_j = " << tilde_pi_c_j );
			VERBOSE_DEBUG("   delta_c_j = " << delta_c_j );
			VERBOSE_DEBUG("   i = " << i );


			models::EventGraphVertex source_event = lcg->getEventGraphVertex(source_id,1,i);
			models::EventGraphVertex target_event = lcg->getEventGraphVertex(target_id,1,j);

			TIME_UNIT  d = tj; // L(a) = l(ti)
			TIME_UNIT  w = - delta_c_j;

			lcg->addEventConstraint(source_event ,target_event,d,w,edge_id);
			}
		}
	}

	return lcg;

}


void algorithms::clean_LCG (models::EventGraph* lcg) {
	lcg->FullConnectionned();
}

void algorithms::compute_deGrooteClean_throughput (models::Dataflow* const  dataflow, parameters_list_t) {

	models::EventGraph* eg = generate_LCG(dataflow);
	VERBOSE_INFO("LCG size is " << eg->getEventCount() << " vertices and " << eg->getConstraintsCount() << " edges");
	clean_LCG(eg);
	VERBOSE_INFO(" LCG cleanned size is " << eg->getEventCount() << " vertices and " << eg->getConstraintsCount() << " edges");
	std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> >  result = eg->MaxCycleRatio();

	if (VERBOSE_IS_DEBUG()) {
		std::cout << (eg->printDOT());
		VERBOSE_INFO( "Maximum throughput is " << 1 / result.first  );
	} else {
		std::cout << "Maximum throughput is " << 1 / result.first   << std::endl;
	}
}


void algorithms::compute_deGroote_throughput (models::Dataflow* const dataflow, parameters_list_t) {

    	models::EventGraph* eg = generate_LCG(dataflow);
    	VERBOSE_INFO("LCG size is " << eg->getEventCount() << " vertices and " << eg->getConstraintsCount() << " edges");

    	std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> >  result = eg->MaxCycleRatio();

    	if (VERBOSE_IS_DEBUG()) {
    		std::cout << (eg->printDOT());
    		VERBOSE_INFO( "Maximum throughput is " << 1 / result.first  );
    	} else {
    		std::cout << "Maximum throughput is " << 1 / result.first   << std::endl;
    	}
    }
