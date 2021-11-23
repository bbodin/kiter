/*
 * symbolicExecution.cpp
 *
 *  Created on: 28 March 2019
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/symbolic_execution.h>



using namespace models;



void print_packet_line (ARRAY_INDEX src, ARRAY_INDEX dst, TIME_UNIT duration, ARRAY_INDEX pid, std::vector <ARRAY_INDEX> deps , bool verb = false ) {

	VERBOSE_ASSERT(src > 0, "unsupported ids for mapping");
	VERBOSE_ASSERT(dst > 0, "unsupported ids for mapping");

	ARRAY_INDEX bid = 0 ;
	if (verb) std::cout << "src=" ;
	std::cout << (src - 1) % 16 << "," ;
	if (verb) std::cout << "dst=";
	std::cout << (dst - 1) % 16 << "," ;
	if (verb) std::cout << "dur=";
	std::cout << duration << "," ;
	if (verb) std::cout << "pid=";
	std::cout << pid << "," ;
	if (verb) std::cout << "bid=";
	std::cout << bid  ;
	if (verb) std::cout << "deps=" ;
	for (auto dep : deps) {
		std::cout << ","  << dep ;
	}
	std::cout << std::endl;
}



void algorithms::symbolic_execution_with_packets (models::Dataflow* const graph, parameters_list_t param_list) {

	// We need the repetition vector to proceed.
	VERBOSE_ASSERT(graph->is_consistent(),"inconsistent graph");

	  {ForEachVertex(graph,t) {
	  	  VERBOSE_ASSERT(graph->getPhasesQuantity(t) == 1, "Support only SDF");
	  }}

	// psize is the number of byte per packet to be send.
	bool do_ack = 0;
	if (param_list.count("ack") == 1) {
		std::string str_value = param_list["ack"];
		do_ack =  commons::fromString<EXEC_COUNT> ( str_value );

	} else {
		VERBOSE_WARNING("The 'ack' parameter has not been provided, packet has no acknowledgment.");
	}
	VERBOSE_INFO("acknowledgment = " << do_ack);

	// psize is the number of byte per packet to be send.
	EXEC_COUNT psize = 0;
	if (param_list.count("psize") == 1) {
		std::string str_value = param_list["psize"];
		psize =  commons::fromString<EXEC_COUNT> ( str_value );

	} else {
		VERBOSE_WARNING("The 'psize' parameter has not been provided, packet has unlimited size.");
	}
	VERBOSE_INFO("packets size = " << psize);

	// psize is the number of byte per packet to be send.
	EXEC_COUNT iteration_count = 1;
	if (param_list.count("iteration") == 1) {
		std::string str_value = param_list["iteration"];
		iteration_count =  commons::fromString<EXEC_COUNT> ( str_value );

	} else {
		VERBOSE_WARNING("The 'iteration' parameter has not been provided, only one iteration is generated.");
	}
	VERBOSE_INFO("packets iteration count = " << iteration_count);

	// We store the total number of execution, the number of execution per task id
	EXEC_COUNT total = 0 ;
	std::vector < EXEC_COUNT >  remained_execution (graph->getMaxVertexId());

	for (Vertex v : graph->vertices()) {
		EXEC_COUNT Ni =  graph->getNi(v) * iteration_count ;
		remained_execution[graph->getVertexId(v)] = Ni ;
		total += Ni ;
	}

	// This is the number of token per buffer
	std::vector < EXEC_COUNT >  buffer_content (graph->getMaxEdgeId());
	{ForEachChannel(graph,c) {
		buffer_content[graph->getEdgeId(c)] = graph->getPreload(c);
	}}

	std::cout << "#NUM_ROWS,NUM_COLS,NUM_BANKS" << std::endl;
	std::cout << "4,4,1" << std::endl;
	std::cout << "#SRC,DEST,DURATION,PACKET_ID,BANK_ID,DEP1,DEP2,..." << std::endl;

    VERBOSE_INFO("Start of symbolic execution.");
    VERBOSE_INFO("Packet size is " << psize);
	// Loop over to execute everyone. Unoptimized, could be much faster if tasks were stacked, and sum of dependences stored.
    // This does not model timings, useless for throughput

	std::map < Edge , ARRAY_INDEX > packet_ids;
	std::map < Vertex , ARRAY_INDEX > last_compute_time;
	ARRAY_INDEX packet_count = 0;

	while (total > 0) {

		for (Vertex t : graph->vertices()) {

			ARRAY_INDEX vId = graph->getVertexId(t);
			bool can_execute = false;
			if ( remained_execution [vId] > 0) {
				can_execute = true;
				//VERBOSE_DEBUG("Need to execute " << vId);
				{ForInputEdges(graph,t,inE)	{
					TOKEN_UNIT inCount  = buffer_content[graph->getEdgeId(inE)];
					TOKEN_UNIT reqCount = graph->getEdgeOut(inE);
					if (!(inCount >= reqCount)) {
						can_execute = false;
						//VERBOSE_DEBUG("Cannot execute " << vId);
						break;
					}
				}}

				if (can_execute) {
					VERBOSE_DEBUG("Execute " << vId);
					remained_execution[graph->getVertexId(t)] -= 1 ;
					total -= 1;

					std::vector <ARRAY_INDEX> deps ;


					{ForInputEdges(graph,t,inE)	{
						buffer_content[graph->getEdgeId(inE)] -= graph->getEdgeOut(inE);
						if (packet_ids.find(inE) != packet_ids.end()) deps.push_back(packet_ids[inE]);
					}}


					ARRAY_INDEX compute_pid  = packet_count++;
					std::vector <ARRAY_INDEX> subdeps = deps ;

					if (last_compute_time.find(t) != last_compute_time.end()) {
						subdeps.push_back(last_compute_time[t]);
					}

					print_packet_line ( graph->getVertexId(t),  graph->getVertexId(t),  graph->getVertexTotalDuration(t),  compute_pid, subdeps ) ;

					last_compute_time[t] = compute_pid;

					{ForOutputEdges(graph,t,outE)	{
						buffer_content[graph->getEdgeId(outE)] += graph->getEdgeIn(outE);


						ARRAY_INDEX src = graph->getVertexId(t) ;
						ARRAY_INDEX dst = graph->getVertexId(graph->getEdgeTarget(outE));


						TOKEN_UNIT datatotransfert = graph->getEdgeIn(outE);
						if (psize == 0) psize = datatotransfert;

						VERBOSE_DEBUG(" - Produce " << datatotransfert << " by packet of " << psize);


						ARRAY_INDEX previous = compute_pid;

						while (datatotransfert) {
							packet_ids[outE] = packet_count++;
							ARRAY_INDEX pid = packet_ids[outE] ;
							std::vector <ARRAY_INDEX> subdeps = deps ;
							subdeps.push_back(previous);
							print_packet_line ( src,  dst,  0,  pid, subdeps ) ;
							previous = pid;
							datatotransfert -= psize;
						}
					}}

				}
			}

		}

	}

    VERBOSE_INFO("End of symbolic execution.");

}

bool algorithms::symbolic_execution (models::Dataflow* const from) {

	// Need RV.
	VERBOSE_ASSERT(from->is_consistent(),"inconsistent graph");

	// Store task to execute by number of execution.
	EXEC_COUNT total = 0 ;
	std::vector < EXEC_COUNT >  remained_execution (from->getMaxVertexId());
	{ForEachVertex(from,t) {
		EXEC_COUNT Ni =  from->getNi(t) ;
		remained_execution[from->getVertexId(t)] = Ni ;
		total += Ni ;
	}}

	// Store buffer content.
	std::vector < EXEC_COUNT >  buffer_content (from->getMaxEdgeId());
	{ForEachChannel(from,c) {
		buffer_content[from->getEdgeId(c)] = from->getPreload(c);
	}}

    VERBOSE_INFO("Start of symbolic execution.");
	// Loop over to execute everyone. Unoptimized, could be much faster if tasks were stacked, and sum of dependences stored.
    // This does not model timings, useless for throughput
	while (total > 0) {
		{ForEachVertex(from,t) {
			ARRAY_INDEX vId = from->getVertexId(t);
			bool can_execute = false;
			if ( remained_execution [vId] > 0) {
				can_execute = true;
				//VERBOSE_DEBUG("Need to execute " << vId);
				{ForInputEdges(from,t,inE)	{
					TOKEN_UNIT inCount  = buffer_content[from->getEdgeId(inE)];
					TOKEN_UNIT reqCount = from->getEdgeOut(inE);
					if (!(inCount >= reqCount)) {
						can_execute = false;
						//VERBOSE_DEBUG("Cannot execute " << vId);
						break;
					}
				}}

				if (can_execute) {
					VERBOSE_DEBUG("Execute " << vId);
					remained_execution[from->getVertexId(t)] -= 1 ;
					total -= 1;

					{ForInputEdges(from,t,inE)	{
						buffer_content[from->getEdgeId(inE)] -= from->getEdgeOut(inE);
					}}

					{ForOutputEdges(from,t,outE)	{
						buffer_content[from->getEdgeId(outE)] += from->getEdgeIn(outE);
					}}

				}
			}

		}}
	}

    VERBOSE_INFO("End of symbolic execution.");

    return total == 0;
}

void algorithms::symbolic_execution (models::Dataflow* const from, parameters_list_t ) {

	bool res = algorithms::symbolic_execution(from);

	VERBOSE_ASSERT(res, "Symbolic execution failed.");
}


