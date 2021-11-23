/*
 * kperiodic.h
 *
 *  Created on: 9 mai 2013
 *      Author: toky
 */

#ifndef KPERIODIC_H_
#define KPERIODIC_H_

#include <algorithms/schedulings.h>
#include <commons/KiterRegistry.h>


#define VERBOSE_KPERIODIC_DEBUG(msg) {VERBOSE_CUSTOM_DEBUG("kperiodic",msg);}


namespace models {
	class Dataflow;
	class EventGraph;
}


namespace algorithms {

bool sameset(const models::Dataflow* const dataflow, std::set<Edge> *cc1 , std::set<Edge>* cc2) ;
std::string cc2string  (const models::Dataflow* const dataflow,std::set<Edge>* cc) ;
models::EventGraph* generateKPeriodicEventGraph(const models::Dataflow * const dataflow, const periodicity_vector_t * kValues );

std::string print_schedule (models::EventGraph* eg, models::Dataflow* const  dataflow,  const periodicity_vector_t & kvector , TIME_UNIT res ) ;

    void print_kperiodic_scheduling         (models::Dataflow* const  dataflow, parameters_list_t);
  void generateKperiodicSelfloop(const models::Dataflow * const dataflow , EXEC_COUNT ki, models::EventGraph* g  , Vertex t  );

  void generateKPeriodicConstraint(const models::Dataflow * const dataflow , const periodicity_vector_t * kValues,  models::EventGraph* g, Edge c) ;

  models::EventGraph*  updateEventGraph(const  models::Dataflow * const dataflow ,  periodicity_vector_t * kvector, std::set<Edge>* cc, models::EventGraph* g) ;
 models::EventGraph*  generateCycleOnly(models::Dataflow * const dataflow , const periodicity_vector_t * kValues,   std::set<Edge> * cc );

void                compute_Kperiodic_throughput              (models::Dataflow *  const , parameters_list_t);

    void                compute_1Kperiodic_throughput   		  (models::Dataflow* const  dataflow, parameters_list_t);
    void                compute_2Kperiodic_throughput             (models::Dataflow* const  dataflow, parameters_list_t);
    void                compute_NKperiodic_throughput             (models::Dataflow* const  dataflow, parameters_list_t);
    void                compute_GKperiodic_throughput  		      (models::Dataflow* const  dataflow, parameters_list_t);

    kperiodic_result_t KSchedule             (models::Dataflow *  const ,const periodicity_vector_t * kvector  , TIME_UNIT bound = 0) ;
    kperiodic_result_t KScheduleBufferLess   (models::Dataflow *  const ,const periodicity_vector_t * kvector  , TIME_UNIT bound = 0) ;

    bool                                     updateVectorWithLocalNi  (const models::Dataflow *  const  , periodicity_vector_t *  , std::set<Edge> * ) ;
    bool 									 updateVectorWithFineNi   (models::Dataflow * const   ,  periodicity_vector_t * , std::set<Edge>*  );

}
ADD_TRANSFORMATION(KPeriodicThroughput,
		transformation_t({ "KPeriodicThroughput" , "Optimal Throughput evaluation of CSDF by K-Periodic scheduling method 2.", algorithms::compute_Kperiodic_throughput}));

ADD_TRANSFORMATION(UnPeriodicThroughput,
		transformation_t({ "1PeriodicThroughput" , "Optimal 1-Periodic Throughput evaluation of CSDF by K-Periodic scheduling method.", algorithms::compute_1Kperiodic_throughput}));
ADD_TRANSFORMATION(DeuxPeriodicThroughput,
				transformation_t({ "2PeriodicThroughput" , "Optimal 1-Periodic Throughput evaluation of CSDF by K-Periodic scheduling method.", algorithms::compute_2Kperiodic_throughput}));
ADD_TRANSFORMATION(NKPeriodicThroughput,
		transformation_t({ "NKPeriodicThroughput" , "Optimal Throughput evaluation of CSDF by using N-periodic method.", algorithms::compute_NKperiodic_throughput}));


ADD_TRANSFORMATION(PrintKPeriodicScheduling,
		transformation_t({ "PrintKPeriodicScheduling" , "Print KPeriodicScheduling", algorithms::print_kperiodic_scheduling})
);

#endif /* KPERIODIC_H_ */
