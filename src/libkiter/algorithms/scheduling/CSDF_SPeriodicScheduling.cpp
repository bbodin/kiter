/*
 * CSDF_SPeriodicScheduling.cpp
 *
 *  Created on: Mar 2, 2021
 *      Author: toky
 */


#include <algorithms/schedulings.h>

#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <map>
#include <printers/stdout.h>
#include <algorithms/throughput/csdf_strictly_periodic.h>




models::Scheduling algorithms::scheduling::CSDF_SPeriodicScheduling (const models::Dataflow* const dataflow) {

	VERBOSE_ASSERT(dataflow->has_repetition_vector(), "Repetition vector failed.");

	models::EventGraph* eg = algorithms::generate_csdf_strictly_periodic_event_graph(dataflow);

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
	for (auto v : dataflow->vertices()) {
		ARRAY_INDEX tid = dataflow->getVertexId(v);
		TIME_UNIT period = omega / dataflow->getNi(v);

		static_task_schedule_t is = {};
		periodic_task_schedule_t ps;
		ps.first = period;
		ps.second.push_back( eg->getStartingTime(eg->getEventGraphVertex(tid)) );

		task_schedule_t ts (is,ps);
		scheduling_result.set(tid, ts);
		VERBOSE_ASSERT(scheduling_result[tid].periodic_starts.second.size() == 1, "Scheduling data structure incorrect.");
	}

	return models::Scheduling(dataflow, omega, scheduling_result, critical_edges);

}

void algorithms::scheduling::SPeriodicScheduling (models::Dataflow*  dataflow, parameters_list_t params)  {

	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	models::Scheduling res = CSDF_SPeriodicScheduling    (dataflow);

   TIME_UNIT omega = res.getGraphPeriod();

   int linesize = params.count("LINE")? commons::fromString<int>(params["LINE"]) : 80;

   std::cout << res.asASCII(linesize);
   std::cout << res.asText();

   std::cout << "SPeriodic throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  1.0 / omega << std::endl;
   std::cout << "SPeriodic period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << omega   << std::endl;

}

