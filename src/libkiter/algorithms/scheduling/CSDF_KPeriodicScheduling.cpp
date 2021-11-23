/*
 * CSDF_KPeriodicScheduling.cpp
 *
 *  Created on: Oct 4, 2019
 *      Author: toky
 */


#include <algorithms/schedulings.h>

#include <algorithms/throughput/kperiodic.h>
#include <models/EventGraph.h>
#include <printers/SDF3Wrapper.h>


 const periodicity_vector_t algorithms::scheduling::generateNPeriodicVector (const models::Dataflow* dataflow) {

	VERBOSE_ASSERT(dataflow->has_repetition_vector(),"Need repetition vector.");
	periodicity_vector_t kvector;
	for (Vertex v : dataflow->vertices()){
	        kvector[v] = dataflow->getNi(v);
	}
	return kvector;
}


 const periodicity_vector_t algorithms::scheduling::generate1PeriodicVector (const models::Dataflow* dataflow) {
	 return algorithms::scheduling::generateKPeriodicVector(dataflow, 1);
}


 const periodicity_vector_t algorithms::scheduling::generateKPeriodicVector (const models::Dataflow* dataflow, int k) {

	periodicity_vector_t kvector;
	for (Vertex v : dataflow->vertices()){
	        kvector[v] = k;
	}
	return kvector;
}


models::Scheduling algorithms::scheduling::CSDF_1PeriodicScheduling (const models::Dataflow* const dataflow) {

 	VERBOSE_ASSERT(dataflow->has_repetition_vector(), "Repetition vector failed.");

 	VERBOSE_INFO ("Start 1 periodic Scheduling");

    // STEP 1 - generate initial vector
    periodicity_vector_t kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;

    }}

 	models::EventGraph* eg =  generateKPeriodicEventGraph(dataflow,&kvector);

 	VERBOSE_INFO("EventGraph Latex\n" << eg->printTikz());

 	std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();

 	TIME_UNIT res = howard_res.first ;
 	std::vector<models::EventGraphEdge> critical_circuit = howard_res.second;



 	// Build critical_edges
 	std::set<Edge> critical_edges;
 	for (auto constraint: critical_circuit) {
 		ARRAY_INDEX channel_id = eg->getChannelId(constraint);
 		try {
 			Edge        channel    = dataflow->getEdgeById(channel_id);
 			critical_edges.insert(channel);
 		} catch(...) {
 			VERBOSE_DEBUG("      is loopback");
 		}
 	}

 	scheduling_t scheduling_result;
 	TIME_UNIT omega = 1.0 / res ;
 	eg->computeStartingTimeWithOmega (omega);

 	{ForEachEvent(eg,e) {
 	        models::SchedulingEvent se = eg->getEvent(e);
 	        ARRAY_INDEX ti = se.getTaskId();
 	        TIME_UNIT start = eg->getStartingTime(e);
 	        Vertex v = dataflow->getVertexById(ti);
 	        TIME_UNIT period = kvector[v] *  dataflow->getPhasesQuantity(v) * omega / dataflow->getNi(v);

 	        scheduling_result[ti].periodic_starts.first = period;
 	        scheduling_result[ti].periodic_starts.second.push_back(start);

 	        //sheduling_result[v].first = period;
 	        //sheduling_result[v].second.push_back(start);
 	    }}

 	return models::Scheduling(dataflow, omega, scheduling_result, critical_edges);


 }


models::Scheduling algorithms::scheduling::CSDF_KPeriodicScheduling    (const models::Dataflow* const dataflow) {

	if (VERBOSE_IS_DEBUG()) {
	    VERBOSE_DEBUG("Save SDF3 XML file.");
		static int count = 0;
		printers::writeSDF3File( "CSDF_KPeriodicScheduling_" + std::to_string(count++) + ".xml", dataflow);
	}

   VERBOSE_ASSERT((dataflow->has_repetition_vector()),"inconsistent graph or repetition vector not computed");

    VERBOSE_INFO("Please note you can use the PRINT parameter");

    EXEC_COUNT sumNi = 0;
    EXEC_COUNT sumKi = dataflow->getVerticesCount();

    {ForEachTask(dataflow,t) {
        sumNi += dataflow->getNi(t) / dataflow->getPhasesQuantity(t) ;
    }}

    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    EXEC_COUNT iteration_count = 0;

    // STEP 1 - generate initial vector
    periodicity_vector_t kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;

    }}


    kperiodic_result_t result;



    VERBOSE_INFO("KPeriodic EventGraph generation");

    VERBOSE_INFO("KVector = " << commons::toString(kvector) );
    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector);


    VERBOSE_INFO("KPeriodic EventGraph generation Done");

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
    VERBOSE_INFO("Kperiodic throughput = " << howard_res.first);

    std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

    //STEP 3 - convert CC(eg) => CC(graph)
    VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
    for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
        VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
        ARRAY_INDEX channel_id = eg->getChannelId(*it);
        try {
            Edge        channel    = dataflow->getEdgeById(channel_id);
            result.critical_edges.insert(channel);
        } catch(...) {
            VERBOSE_DEBUG("      is loopback");
        }
    }

    TIME_UNIT frequency = howard_res.first;

    VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
    VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

    result.throughput = frequency;

    ////////////// SCHEDULE CALL // END


    VERBOSE_INFO("Iteration "<< std::fixed << std::setw( 4 ) << iteration_count <<  "      period = "  << std::fixed << std::setw( 15 ) << std::setprecision( 2 ) << 1.0/result.throughput  <<  "      complexity = "  << std::setw( 4 )  << (sumNi ?  ((sumKi * 100) / sumNi) : 0));

    if (result.critical_edges.size() != 0) {


        VERBOSE_INFO("1-periodic throughput (" << result.throughput <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.critical_edges)) <<  "");

        while (true) {


            iteration_count++;
            ////////////// SCHEDULE CALL // BEGIN : resultprime = KSchedule(dataflow,&kvector);

            kperiodic_result_t resultprime;

            //VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");
            VERBOSE_INFO("KPeriodic EventGraph generation");

            //STEP 1 - Generate Event Graph and update vector
            if (!algorithms::updateEventGraph( dataflow ,  &kvector, &(result.critical_edges), eg)) break ;

            VERBOSE_INFO("KVector = " << commons::toString(kvector) );
            VERBOSE_INFO("KPeriodic EventGraph generation Done");

            //STEP 2 - resolve the MCRP on this Event Graph
            std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res_bis = eg->MinCycleRatio();
            VERBOSE_INFO("Kperiodic throughput = " << howard_res_bis.first);

            std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res_bis.second);

            //STEP 3 - convert CC(eg) => CC(graph)
            VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
            for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
                VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
                ARRAY_INDEX channel_id = eg->getChannelId(*it);
                try {
                    Edge        channel    = dataflow->getEdgeById(channel_id);
                    resultprime.critical_edges.insert(channel);
                } catch(...) {
                    VERBOSE_DEBUG("      is loopback");
                }
            }

            TIME_UNIT frequency = howard_res_bis.first;

            VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
            VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

            resultprime.throughput = frequency;

            ////////////// SCHEDULE CALL // END

            if (sameset(dataflow,&(resultprime.critical_edges),&(result.critical_edges)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;

                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.throughput <<  ") is not enough.");
            VERBOSE_DEBUG("   Critical circuit is " << cc2string(dataflow,&(result.critical_edges)) <<  "");


                sumKi = 0;
                {ForEachVertex(dataflow,t) {
                    sumKi += kvector[t];
                }}

                VERBOSE_INFO("Iteration "<< std::fixed << std::setw( 4 ) << iteration_count <<  "      period = "  << std::fixed << std::setw( 15 ) << std::setprecision( 2 ) << 1.0/result.throughput  <<  "      complexity = "  << std::setw( 4 )  << (sumKi * 100) / sumNi ) ;

        }

    }


    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count << "  final size is " << eg->getEventCount() << " events and " << eg->getConstraintsCount() << " constraints.");
    delete eg;

    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << sumKi << " total_ni=" << sumNi );

    TIME_UNIT res = result.throughput;
    std::cout << res << std::endl;
    VERBOSE_INFO( "Maximum throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  res );
    VERBOSE_INFO( "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   );

    VERBOSE_INFO("Run period2Scheduling");

    models::Scheduling persched = period2Scheduling(dataflow,kvector,result);

    VERBOSE_INFO("End of period2Scheduling");

    return persched;

}

void algorithms::scheduling::CSDF_1PeriodicThroughput (models::Dataflow*  dataflow, parameters_list_t )  {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
   //STEP 1 - Generate Event Graph
	auto kvector = generate1PeriodicVector(dataflow);
   models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector);


   VERBOSE_INFO("KPeriodic EventGraph generation Done");

   //STEP 2 - resolve the MCRP on this Event Graph
   std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
   TIME_UNIT omega = 1/howard_res.first;

   std::cout << "Maximum throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  1.0 / omega << std::endl;
   std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << omega   << std::endl;

}

void algorithms::scheduling::OnePeriodicScheduling (models::Dataflow*  dataflow, parameters_list_t params)  {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	models::Scheduling res = CSDF_1PeriodicScheduling (dataflow);

   TIME_UNIT omega = res.getGraphPeriod();

   int linesize = params.count("LINE")? commons::fromString<int>(params["LINE"]) : 80;

   std::cout << res.asASCII(linesize);
   std::cout << res.asText();

   std::cout << "1Periodic throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  1.0 / omega << std::endl;
   std::cout << "1Periodic period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << omega   << std::endl;

}



void algorithms::scheduling::CSDF_NPeriodicThroughput (models::Dataflow*  dataflow, parameters_list_t )  {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
   //STEP 1 - Generate Event Graph
	auto kvector = generateNPeriodicVector(dataflow);
   models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector);


   VERBOSE_INFO("KPeriodic EventGraph generation Done");

   //STEP 2 - resolve the MCRP on this Event Graph
   std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
   TIME_UNIT omega = 1/howard_res.first;

   std::cout << "Maximum throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  1.0 / omega << std::endl;
   std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << omega   << std::endl;

}
