/*
 * schedulings.h
 *
 *  Created on: Aug 23, 2019
 *      Author: toky
 */

#ifndef SRC_LIBKITER_ALGORITHMS_SCHEDULINGS_H_
#define SRC_LIBKITER_ALGORITHMS_SCHEDULINGS_H_

#include <map>
#include <commons/KiterRegistry.h>
#include <models/Dataflow.h>
#include <models/Scheduling.h>


typedef std::map<Vertex,EXEC_COUNT> periodicity_vector_t;

void sdf_bufferless_scheduling (models::Dataflow* const  dataflow, periodicity_vector_t &  kvector, std::vector<std::vector <Vertex> > task_sequences);
void bufferless_scheduling (models::Dataflow* const  dataflow, periodicity_vector_t &  kvector);



scheduling_t period2scheduling    (models::Dataflow* const  dataflow,  periodicity_vector_t & kvector , TIME_UNIT throughput) ;
models::Scheduling  period2Scheduling    (models::Dataflow* const  dataflow,  periodicity_vector_t & kvector , TIME_UNIT throughput) ;

namespace algorithms {


void BufferlessNoCScheduling(models::Dataflow* const  dataflow, parameters_list_t   param_list);

	namespace scheduling {
		void KPeriodic_taskNoCbufferless(models::Dataflow*, parameters_list_t param_list);
		scheduling_t bufferless_scheduling(models::Dataflow* const  dataflow, periodicity_vector_t &  kvector);
		scheduling_t bufferless_kperiodic_scheduling(models::Dataflow* const  dataflow, bool stop_at_first, bool get_previous);
		void SDFKPeriodicScheduling (models::Dataflow* const  dataflow, parameters_list_t params) ;
		void bufferlessKPeriodicScheduling (models::Dataflow* const  dataflow, parameters_list_t params) ;

		models::Scheduling CSDF_KPeriodicScheduling       (models::Dataflow* const dataflow) ;
		models::Scheduling CSDF_KPeriodicScheduling_LP    (const models::Dataflow* const dataflow, const periodicity_vector_t& kvector);
		const periodicity_vector_t generate1PeriodicVector(const models::Dataflow* dataflow);
		const periodicity_vector_t generateKPeriodicVector(const models::Dataflow* dataflow, int k);
		const periodicity_vector_t generateNPeriodicVector(const models::Dataflow* dataflow);

		 void CSDF_1PeriodicScheduling_LP (models::Dataflow*  dataflow, parameters_list_t );
		 void CSDF_NPeriodicScheduling_LP (models::Dataflow*  dataflow, parameters_list_t );
		 void CSDF_NPeriodicScheduling    (models::Dataflow*  dataflow, parameters_list_t );


	}
}


// Recent stuff
ADD_TRANSFORMATION(LP1,
transformation_t({ "LP1" , "Rewriting Bodin2016 Threshold CSDF 1-Periodic Scheduling", algorithms::scheduling::CSDF_1PeriodicScheduling_LP}));
ADD_TRANSFORMATION(LPN,
transformation_t({ "LPN" , "Rewriting Bodin2016 Threshold CSDF N-Periodic Scheduling", algorithms::scheduling::CSDF_NPeriodicScheduling_LP}));
ADD_TRANSFORMATION(EGN,
transformation_t({ "EGN" , "Rewriting Bodin2013 Threshold CSDF Periodic Scheduling", algorithms::scheduling::CSDF_NPeriodicScheduling}));
ADD_TRANSFORMATION(BufferlessKPeriodicScheduling,
		transformation_t({ "BufferlessKPeriodicScheduling" , "Run Bufferless Kperiodic", algorithms::scheduling::bufferlessKPeriodicScheduling} )
	);

// Throughput techniques
ADD_TRANSFORMATION(SDFKPeriodic,
		transformation_t({ "SDFKPeriodic" , "Build an Exansion graph given a set of value K (1 by default), and compute its MCRP.", algorithms::scheduling::SDFKPeriodicScheduling}));






#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULINGS_H_ */
