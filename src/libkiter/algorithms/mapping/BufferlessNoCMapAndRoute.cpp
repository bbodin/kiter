/*
 * BufferlessNoCMapAndRoute.cpp
 *
 *  Created on: Apr 21, 2020
 *      Author: toky
 */



#include <models/Dataflow.h>
#include <algorithms/mappings.h>
#include <algorithms/schedulings.h>

#include <models/NoC.h>
#include <vector>
#include <iostream>
#include <commons/verbose.h>
#include <commons/commons.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
#include <algorithms/software_noc.h>
#include <algorithms/throughput/kperiodic.h>
#include <cstdlib>
#include <stack>
#include <climits>
#include <algorithms/schedulings.h>
#include <set>
#include <queue>
#include <tuple>
#include <unordered_map>


#include <algorithms/mapping/bufferless_utils/SCCFinder.h>
#include <algorithms/mapping/bufferless_utils/GraphMapper.h>


static NoCGraph * createNoCGraph (const NoC * noc) {

		NoCGraph* g = new NoCGraph(noc->size()*2);
		for (auto edge : noc->getEdges()) {
			g->addEdge((int) edge.src, (int) edge.dst); // TODO : integer around here
		}
		return g;
	}


static std::vector<ARRAY_INDEX> generate_task_order (const models::Dataflow* const  from) {

	// TODO : this function only works for SDF Graph not CSDF

	std::vector<ARRAY_INDEX> prog_order;
	// Need RV.
	VERBOSE_ASSERT(from->has_repetition_vector(),"The graph needs to have a repetition vector.");
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

	// Loop over to execute everyone. Unoptimized, could be much faster if tasks were stacked, and sum of dependences stored.
	// This does not model timings, useless for throughput
	while (total > 0) {
		{ForEachVertex(from,t) {
			ARRAY_INDEX vId = from->getVertexId(t);
			bool can_execute = false;
			if ( remained_execution [vId] > 0) {
				can_execute = true;
				{ForInputEdges(from,t,inE)	{
					TOKEN_UNIT inCount  = buffer_content[from->getEdgeId(inE)];
					TOKEN_UNIT reqCount = from->getEdgeOut(inE);
					if (!(inCount >= reqCount)) {
						can_execute = false;
						break;
					}
				}}

				if (can_execute) {

					if ( std::find (prog_order.begin(), prog_order.end(), vId) == prog_order.end())
					{
						prog_order.push_back(vId);
					}

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

	return prog_order;
}


static void updateNetworkOnChipModel (models::Dataflow* const  dataflow, int meshsize) {

	// #### Generate NoC
	int mesh_row = (int)ceil(sqrt((double) dataflow->getVerticesCount()));
	if(mesh_row <= 2)
		mesh_row = 2;
	if (meshsize > 0) mesh_row = meshsize;
	VERBOSE_INFO("Set the NoC with mesh_row=" << mesh_row << " when Data vertices count is " << dataflow->getVerticesCount());
	NoC noc (mesh_row, mesh_row); //Init NoC
	dataflow->setNoC(noc);

}

void DFSUtil_PgmOrder (Vertex v, std::vector<bool>& visited, models::Dataflow* to, std::vector<ARRAY_INDEX>& removeEdgeId, const std::vector< std::vector<unsigned int> > &cyclen_per_vtxid)
{
	// Mark the current node as visited and print it
	auto myid = to->getVertexId(v);
	visited[myid] = true;
	// Recur for all the vertices adjacent to this vertex
	{ForOutputEdges(to,v,e){
		Vertex end = to->getEdgeTarget(e);
		ARRAY_INDEX endid = to->getVertexId(end);
		if(!visited[endid])
			DFSUtil_PgmOrder(end, visited, to, removeEdgeId,cyclen_per_vtxid);
		else if (cyclen_per_vtxid[endid].size() > 0)
			removeEdgeId.push_back(to->getEdgeId(e));
		//else
		//	std::cout << "Error: wrong removal of edge " << myid << "->" << endid << "\n";
	}}
}


//Remove the cyclic edges
static Vertex removeCycleEdges (models::Dataflow* to, const std::vector<ARRAY_INDEX>& prog_order, const std::vector< std::vector<unsigned int> > &cyclen_per_vtxid) {
	ARRAY_INDEX origV = to->getVerticesCount();
	std::vector<bool> visited(origV, false);
	std::vector<ARRAY_INDEX> removeEdgeId;
	std::vector<ARRAY_INDEX> vertexId;


	for(auto i = 0; i < prog_order.size(); i++)
	{
		auto vid = prog_order[i];
		Vertex v = to->getVertexById(vid);
		if(!visited[vid])
			DFSUtil_PgmOrder(v, visited, to, removeEdgeId,cyclen_per_vtxid);
	}
	for(auto i = 0; i < removeEdgeId.size(); i++)
	{
		auto e = to->getEdgeById( removeEdgeId[i] );
		auto src = to->getEdgeSource(e);
		auto dst = to->getEdgeTarget(e);

		VERBOSE_INFO ( "Removing edge " << to->getVertexId(src) << "->" << to->getVertexId(dst) );
		to->removeEdge(e);
	}

	//find isolated nodes
	{ForEachVertex(to,t) {
		if(to->getVertexInDegree(t) == 0)
		{
			vertexId.push_back( to->getVertexId(t) );
			//std::cout << "inside function::::: adding new edge between start " << to->getVertexId(t) << "\n";
		}
	}}


	//Now create the start node
	auto start = to->addVertex();
	to->setVertexName(start, "start");

	//add edges between start node and isolated edges
	for (int i = 0; i < (int)vertexId.size(); i++)
	{
		auto t = to->getVertexById( vertexId[i]);
		auto ne = to->addEdge(start, t);
		to->setEdgeName(ne,"start_to_isolated_" + commons::toString(to->getEdgeId(ne)));
	}
	return start;
}

//Process the graph for DFS, etc. in this function
static std::map<int, route_t> graphProcessing(const models::Dataflow* const dataflow,  NoCGraph * noc) {

	VERBOSE_INFO("Starting graphProcessing.");

	models::Dataflow* to = new models::Dataflow(*dataflow);

	to->reset_computation();


	// Step X - Add a task called start and a bunch of edges from start to sources tasks (tasks without input flow)
	//          TODO: This is assuming we don't have self-loop I think, some risk here

	auto start = to->addVertex();
	to->setVertexName(start, "start");

	bool input_task_found = false;
	bool top_is_set = false;
	Vertex top;

	{ForEachVertex(to,t) {
		if( (t != start) && (!input_task_found) && (!top_is_set)) {
			top = t;
			top_is_set = true;
		}

		if(t == start) {
			continue;
		}

		if(dataflow->getVertexInDegree( dataflow->getVertexById(to->getVertexId(t)) ) == 0) {
			input_task_found = true;
			auto ne = to->addEdge(start, t);
			to->setEdgeName(ne,"between_start_" + commons::toString(to->getEdgeId(ne)));
			//std::cout << "adding new edge between start " << to->getVertexId(t) << "\n";
		}
	}}

	if(!input_task_found) {
		auto ne = to->addEdge(start, top);
		to->setEdgeName(ne,"start_top_" + commons::toString(to->getEdgeId(ne)));
	}

	// Result: "start" in an artifical task, and every other task is reachable from start.


	// Step X - We run printSCCs, I enclosed it inside an object because it was too many globals.
	//          TODO: This looks much more complicated than it should be, to rewrite ASAP


	algorithms::SCCFinder finder = algorithms::SCCFinder(to);
	finder.printSCCs(to, start);

	// Result: Not sure to know yet, so many globals.... but  is used later


	// Step X - No idea what is happening here
	//          TODO: To read the code


	std::vector<ARRAY_INDEX> prog_order = generate_task_order(dataflow); // #### symbolic execution to find program execution order (prog_order)
	models::Dataflow* to2 = new models::Dataflow(*dataflow);
	to2->reset_computation();
	start = removeCycleEdges(to2, prog_order,finder.getcyclen_per_vtxid());

	// Result: Seems to remove some edges



	// Step X - Final part that produce the list of route this is the actual task Mapping and Routing
	//          TODO: To read the code


	algorithms::GraphMapper mapper = algorithms::GraphMapper(finder.getcyclen_per_vtxid());
	std::map<int, route_t> routes = mapper.taskAndNoCMapping(dataflow, to2, start, noc);



	// Result: routes is the route between every task of the graph.

	VERBOSE_DEBUG( "done route size = " << routes.size() );


	VERBOSE_INFO("End of graphProcessing.");

	return routes;


}

static std::map<int, std::vector<Edge>> generateEdgesMap(models::Dataflow* dataflow, NoCGraph* noc) {
	std::map<int, std::vector<Edge>> edge_list;
	{ForEachEdge(dataflow,e) {
		auto v1 = dataflow->getEdgeSource(e);
		auto v2 = dataflow->getEdgeTarget(e);

		int v1_i = (int)dataflow->getVertexId(v1);
		int v2_i = (int)dataflow->getVertexId(v2);

		int index = noc->getMapIndex(v1_i, v2_i);
		edge_list[index].push_back ( e );
	}}
	return edge_list;
}


void algorithms::mapping::BufferlessNoCMapAndRoute (models::Dataflow* const dataflow, parameters_list_t params) {

	VERBOSE_INFO("Starting BufferlessNoCMapAndRoute.");
	if (params.count("MESHSIZE")) {
		updateNetworkOnChipModel(dataflow, commons::fromString<int>(params["MESHSIZE"]) );
	} else {
		updateNetworkOnChipModel(dataflow, 0);
	}
	NoCGraph *  noc = createNoCGraph (&(dataflow->getNoC()));

	// STEP 0.2 - Assert SDF
	models::Dataflow* to = new models::Dataflow(*dataflow);


	{
		to->reset_computation();
		VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
		models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
		TIME_UNIT omega = scheduling_res.getGraphPeriod();
		auto persched = scheduling_res.getTaskSchedule();
		VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule, thus I cannot map it.");
	}




	VERBOSE_DEBUG("graphProcessing.");

	//#### do some processing to perfrom task mapping, NoC route determination
	//####  Task to Core mapping
	std::map<int, route_t> routes = graphProcessing(to,noc);
	std::map<int, std::vector<Edge>> edge_list = generateEdgesMap(to,noc);

	std::map<Vertex, node_id_t> mapping;
	std::map<Edge, std::vector <edge_id_t> > routing;

	VERBOSE_INFO("len of routes: " << routes.size());

	for (auto route_item : routes ) {
		std::vector<Edge> e_to_vec = edge_list[route_item.first];
		for (Edge e_to : e_to_vec) {
			VERBOSE_INFO ("Edge " << to->getEdgeName(e_to) << "(" << to->getVertexName(to->getEdgeSource(e_to)) << " to " << to->getVertexName(to->getEdgeTarget(e_to))<< ")"   << ", Route " << commons::toString(route_item.second));
			for (auto edge : route_item.second) {

				VERBOSE_INFO ("    - " << edge <<" links " << noc->getMapIndexPair(edge).first << " to " << noc->getMapIndexPair(edge).second  );
				dataflow->getNoC().getEdge( noc->getMapIndexPair(edge).first , noc->getMapIndexPair(edge).second);
			}

			ARRAY_INDEX edge_id = to->getEdgeId(e_to);
			Edge e_dataflow = dataflow->getEdgeById(edge_id);

			node_id_t source_node = noc->getMapIndexPair(route_item.second[0]).first;
			node_id_t dest_node   = noc->getMapIndexPair(route_item.second[route_item.second.size() - 1]).second;

			Vertex source_vtx   = dataflow->getEdgeSource(e_dataflow);
			Vertex target_vtx   = dataflow->getEdgeTarget(e_dataflow);

			if (mapping.count(source_vtx)) {
				VERBOSE_ASSERT (mapping.at(source_vtx) == source_node, "Invalid routes, mor than one mapping per task");
			} else {
				mapping[source_vtx] = source_node;
			}

			if (mapping.count(target_vtx)) {
				VERBOSE_ASSERT (mapping.at(target_vtx) == dest_node, "Invalid routes, mor than one mapping per task");
			} else {
				mapping[target_vtx] = dest_node;
			}

			for (auto edge : route_item.second) {

						const NetworkEdge& nedge = dataflow->getNoC().getEdge(noc->getMapIndexPair(edge).first, noc->getMapIndexPair(edge).second);
						VERBOSE_ASSERT(nedge.id == edge, "Inconsistent mapping between Original NoC and replicate.");


			}

			routing[e_dataflow] = route_item.second;
		}
	}


	auto utility = noc->getLinkUtil();
	VERBOSE_INFO ( "utility " <<  commons::toString(utility) );


	for (auto mapping_item : mapping) {
		dataflow->setMapping(mapping_item.first,mapping_item.second);
		VERBOSE_INFO("Mapping of task " << dataflow->getVertexName(mapping_item.first) << " to " << mapping_item.second);
	}

	for (auto routing_item : routing) {
		dataflow->setRoute(routing_item.first,routing_item.second);
		VERBOSE_DEBUG("Routing of buffer " << dataflow->getEdgeName(routing_item.first) << " to " << commons::toString(routing_item.second));
	}

	for (auto v : dataflow->vertices()) {

			ARRAY_INDEX tid = dataflow->getVertexId(v);
			auto current_mapping =  (dataflow->getMapping(v));

			VERBOSE_ASSERT(current_mapping >= 0, "UNSUPPORTED CASE, EVERY TASK NEED TO BE MAPPED AND THE TASK " << tid << " IS NOT!");

		}

	VERBOSE_DEBUG("End of BufferlessNoCMapAndRoute.");

}
