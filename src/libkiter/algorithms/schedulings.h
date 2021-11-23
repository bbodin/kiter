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


struct kperiodic_result_t {
	TIME_UNIT      throughput;
	std::set<Edge> critical_edges;
};

scheduling_t period2scheduling    (const models::Dataflow* const  dataflow, const periodicity_vector_t & kvector , TIME_UNIT throughput) ;
models::Scheduling  period2Scheduling    (const models::Dataflow* const  dataflow, const  periodicity_vector_t & kvector , kperiodic_result_t &) ;

namespace algorithms {


void BufferlessNoCScheduling(models::Dataflow* const  dataflow, parameters_list_t   param_list);

	namespace scheduling {
		void KPeriodic_taskNoCbufferless(models::Dataflow*, parameters_list_t param_list);
		scheduling_t bufferless_scheduling(models::Dataflow* const  dataflow, periodicity_vector_t &  kvector);
		void sdf_bufferless_scheduling (models::Dataflow* const  dataflow, periodicity_vector_t &  kvector, std::vector<std::vector <Vertex> > task_sequences);
		scheduling_t bufferless_kperiodic_scheduling(models::Dataflow* const  dataflow, bool stop_at_first, bool get_previous);
		void bufferlessKPeriodicScheduling (models::Dataflow* const  dataflow, parameters_list_t params) ;



		models::Scheduling CSDF_KPeriodicScheduling       (const models::Dataflow* const dataflow) ;
		const periodicity_vector_t generate1PeriodicVector(const models::Dataflow* dataflow);
		const periodicity_vector_t generateKPeriodicVector(const models::Dataflow* dataflow, int k);
		const periodicity_vector_t generateNPeriodicVector(const models::Dataflow* dataflow);

		 models::Scheduling CSDF_KPeriodicScheduling_LP    (const models::Dataflow* const dataflow, const periodicity_vector_t& kvector);
		 void CSDF_1PeriodicScheduling_LP (models::Dataflow*  dataflow, parameters_list_t );
		 void CSDF_NPeriodicScheduling_LP (models::Dataflow*  dataflow, parameters_list_t );

		 void CSDF_1PeriodicThroughput    (models::Dataflow*  dataflow, parameters_list_t );
		 void CSDF_NPeriodicThroughput    (models::Dataflow*  dataflow, parameters_list_t );


		 models::Scheduling CSDF_1PeriodicScheduling    (const models::Dataflow* const dataflow);
		 void OnePeriodicScheduling (models::Dataflow*  dataflow, parameters_list_t ) ;


		 models::Scheduling CSDF_RealPeriodicScheduling_LP    (const models::Dataflow* const dataflow);
		 void CSDF_Real1PeriodicScheduling_LP (models::Dataflow*  dataflow, parameters_list_t );


		 models::Scheduling CSDF_SPeriodicScheduling       (const models::Dataflow* const dataflow) ;
		 void SPeriodicScheduling (models::Dataflow*  dataflow, parameters_list_t ) ;

		 models::Scheduling ASAPScheduling       (const models::Dataflow* const dataflow) ;
		 void ASAPScheduling (models::Dataflow*  dataflow, parameters_list_t ) ;

		 models::Scheduling So4Scheduling       (const models::Dataflow* const dataflow) ;
		 void So4Scheduling (models::Dataflow*  dataflow, parameters_list_t ) ;

	}
}


// Recent stuff

ADD_TRANSFORMATION(REALLP1,
transformation_t({ "REALLP1" , "Fully periodic scheduling for CSDF inefficient way", algorithms::scheduling::CSDF_Real1PeriodicScheduling_LP}));

ADD_TRANSFORMATION(LP1,
transformation_t({ "LP1" , "Rewriting Bodin2016 Threshold CSDF 1-Periodic Scheduling with Bufferless channel using Linear Programming", algorithms::scheduling::CSDF_1PeriodicScheduling_LP}));
ADD_TRANSFORMATION(LPN,
transformation_t({ "LPN" , "Rewriting Bodin2016 Threshold CSDF N-Periodic Scheduling with Bufferless channel using Linear Programming", algorithms::scheduling::CSDF_NPeriodicScheduling_LP}));

ADD_TRANSFORMATION(EG1,
transformation_t({ "EG1" , "Rewriting Bodin2013 CSDF 1-Periodic Scheduling", algorithms::scheduling::CSDF_1PeriodicThroughput}));
ADD_TRANSFORMATION(EGN,
transformation_t({ "EGN" , "Rewriting Bodin2013 CSDF N-Periodic Scheduling", algorithms::scheduling::CSDF_NPeriodicThroughput}));
ADD_TRANSFORMATION(BufferlessKPeriodicScheduling,
		transformation_t({ "BufferlessKPeriodicScheduling" , "Run Bufferless Kperiodic", algorithms::scheduling::bufferlessKPeriodicScheduling} )
	);



ADD_TRANSFORMATION(SPeriodicScheduling,
transformation_t({ "SPeriodicScheduling" , "Experimental", algorithms::scheduling::SPeriodicScheduling}));

ADD_TRANSFORMATION(OnePeriodicScheduling,
transformation_t({ "1PeriodicScheduling" , "CSDF 1-Periodic Scheduling [Bodin2013]", algorithms::scheduling::OnePeriodicScheduling}));

ADD_TRANSFORMATION(ASAPScheduling,
transformation_t({ "ASAPScheduling" , "Symbolic Execution", algorithms::scheduling::ASAPScheduling}));

ADD_TRANSFORMATION(So4Scheduling,
transformation_t({ "So4Scheduling" , "Symbolic Execution with TDMA", algorithms::scheduling::So4Scheduling}));

// Throughput techniques
ADD_TRANSFORMATION(BufferlessNoCScheduling,
		transformation_t({ "BufferlessNoCScheduling" , "BufferlessNoCScheduling, WIP", algorithms::BufferlessNoCScheduling}));


#endif /* SRC_LIBKITER_ALGORITHMS_SCHEDULINGS_H_ */
