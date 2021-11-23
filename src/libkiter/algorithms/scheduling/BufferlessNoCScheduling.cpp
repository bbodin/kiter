/*
 * BufferlessNoCScheduling.cpp
 *
 *  Created on: Apr 21, 2020
 *      Author: toky
 */


#include <models/NoC.h>
#include <vector>
#include <iostream>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <commons/commons.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
#include <algorithms/software_noc.h>
#include <algorithms/throughput/kperiodic.h>
#include <algorithms/mappings.h>
#include <algorithms/transformation/merging.h>
#include <algorithms/schedulings.h>
#include <cstdlib>
#include <stack>
#include <climits>
#include <algorithms/schedulings.h>
#include <set>
#include <queue>
#include <tuple>
#include <printers/SDF3Wrapper.h>
#include <unordered_map>
#include <commons/GroupList.h>


typedef std::pair<ARRAY_INDEX, ARRAY_INDEX>  mypair;
typedef std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> mytuple;


static inline models::Scheduling  CSDF_SCHEDULING_FUNCTION    (const models::Dataflow* const dataflow) {
	//return algorithms::scheduling::CSDF_KPeriodicScheduling_LP (dataflow , algorithms::scheduling::generateNPeriodicVector(dataflow)) ;
		
	return algorithms::scheduling::CSDF_KPeriodicScheduling (dataflow ) ;
}


static void print_graph (models::Dataflow * to, std::string suffix = "none") {

	static int counter = 0;
	counter ++ ;
	VERBOSE_INFO("=========== Write file " << counter << "\n");

	std::string sfilename = "bufferless_noc_schedule_"+ to->getGraphName() + "_"+ commons::toString(counter) + "_" + suffix + "";

	printers::writeSDF3File(sfilename + ".xml", to);

	{

		std::string filename = sfilename + "_noc";
		std::ofstream myfile;
		myfile.open (filename  + ".dot");
		myfile << printers::GenerateNoCDOT(to);
		myfile.close();

		std::string cmd = "dot " + filename + ".dot -Gdpi=300  -Kneato -T png -o " + filename + ".png";
		auto out_err = system(cmd.c_str());
		if(out_err) {
			VERBOSE_INFO ("System call returns error\n");
		}

	}

	{
		std::string filename = sfilename + "_gaph";
		std::ofstream myfile;
		myfile.open (filename  + ".dot");
		myfile << printers::GenerateGraphDOT(to);
		myfile.close();

		std::string cmd = "dot " + filename + ".dot -Gdpi=200  -T png -o " + filename + ".png";
		auto out_err = system(cmd.c_str());
		if(out_err) {
			VERBOSE_INFO ("System call returns error\n");
		}

	}

	to->reset_computation();
}

static LARGE_INT gcdExtended(LARGE_INT x, LARGE_INT y, LARGE_INT *a, LARGE_INT *b)
{
	if (x == 0) //Base Case
	{
		*a = 0;
		*b = 1;
		return y;
	}

	LARGE_INT a1, b1; // To store results of recursive call
	LARGE_INT gcd = gcdExtended(y%x, x, &a1, &b1);
	// Update x and y using results of recursive  call
	*a = b1 - (y/x) * a1;
	*b = a1;

	return gcd;
}


static router_xbar_usage_t build_router_xbar_usage (const models::Dataflow* const  dataflow) {

	router_xbar_usage_t router_xbar_usage;

	for (auto v : dataflow->vertices()) {
				ARRAY_INDEX tid = dataflow->getVertexId(v);
				auto current_mapping =  (dataflow->getMapping(v));
				VERBOSE_ASSERT(current_mapping >= 0, "UNSUPPORTED CASE, EVERY TASK NEED TO BE MAPPED AND THE TASK " << tid << " IS NOT!");
				VERBOSE_ASSERT(dataflow->getNoC().hasEdge(current_mapping) xor dataflow->getNoC().hasNode(current_mapping), "UNSUPPORTED CASE, NOC NODE AND NOC EDGE WITH SIMILAR ID : " << current_mapping);

				VERBOSE_DEBUG("Process task " << tid << " - " << dataflow->getVertexName(v) << " mapped to " << current_mapping);

				if (dataflow->getNoC().hasEdge(current_mapping)) {
					VERBOSE_DEBUG("  - Is a NoC Edge: add to links_usage");
				} else if (dataflow->getNoC().hasNode(current_mapping)) {
					VERBOSE_DEBUG("  - Is a NoC Node: add to router_usage and router_xbar_usage");
					if (dataflow->getNoC().getNode(current_mapping).type == NetworkNodeType::Router) {

						VERBOSE_ASSERT(dataflow->getVertexInDegree(v) == 1, "The NoC has not been modelled has expected, every NoCRouter-task should have one input NoCEdge Task. The task " << dataflow->getVertexName(v) << " has " << dataflow->getVertexInDegree(v));
						VERBOSE_ASSERT(dataflow->getVertexOutDegree(v) == 1, "The NoC has not been modelled has expected, every NoCRouter-task should have one output NoCEdge Task. The task " << dataflow->getVertexName(v) << " has " << dataflow->getVertexOutDegree(v));

						Vertex inputEdgeTask = dataflow->getEdgeSource(*(dataflow->getInputEdges(v).first));
						Vertex outputEdgeTask = dataflow->getEdgeTarget(*(dataflow->getOutputEdges(v).first));

						edge_id_t input_edge = dataflow->getMapping(inputEdgeTask);
						edge_id_t output_edge = dataflow->getMapping(outputEdgeTask);

						VERBOSE_ASSERT(dataflow->getNoC().hasEdge(dataflow->getMapping(inputEdgeTask)), "The NoC has bot been modelled has expected, every  NoCEdge Task should be properly mapped");
						VERBOSE_ASSERT(dataflow->getNoC().hasEdge(dataflow->getMapping(outputEdgeTask)), "The NoC has bot been modelled has expected, every  NoCEdge Task should be properly mapped");

						router_xbar_usage[current_mapping].push_back( std::tuple<edge_id_t,edge_id_t,ARRAY_INDEX> (input_edge, output_edge, tid) );

					}else {
						VERBOSE_DEBUG("!! SKIP because node is not a ROUTER ");
					}
				} else {
					VERBOSE_DEBUG("!! SKIP because not found in the NoC ");
				}
			}

			return (router_xbar_usage);
}


static std::vector<std::vector<ARRAY_INDEX>> get_overlaps (models::Dataflow* const  dataflow) {

	router_xbar_usage_t router_xbar_usage = build_router_xbar_usage (dataflow);

	std::vector<std::vector<ARRAY_INDEX>> res ;

	for (auto item : router_xbar_usage) {

			const node_id_t router_id       =  item.first;

			GroupList<edge_id_t, ARRAY_INDEX> g;

			for (auto subitem : item.second) {
				const edge_id_t in_noc_edge_id  =  std::get<0>(subitem);
				const edge_id_t out_noc_edge_id =  std::get<1>(subitem);
				const ARRAY_INDEX vertex_id     =  std::get<2>(subitem);
				VERBOSE_ASSERT(dataflow->getNoC().getEdge(in_noc_edge_id).dst  == router_id, "The router_xbar_usage is incorrect.");
				VERBOSE_ASSERT(dataflow->getNoC().getEdge(out_noc_edge_id).src == router_id, "The router_xbar_usage is incorrect.");
				g.add(in_noc_edge_id, out_noc_edge_id, vertex_id);
			}

			for (auto bag : g.getall()) {
				res.push_back(std::vector<ARRAY_INDEX>());

				for (auto e : bag) {
					res.back().push_back(e);
				}
			}

		}

	return res;

}


//print_graph(to, original_df);
void algorithms::BufferlessNoCScheduling(models::Dataflow* const  _dataflow, parameters_list_t params) {

	models::Dataflow* to = new models::Dataflow(*_dataflow);

	print_graph(to, "begin");

	VERBOSE_INFO("Step 1 - ModelNoCConflictFreeCommunication");
	algorithms::ModelNoCConflictFreeCommunication(to) ;
	print_graph(to, "ModelNoCConflictFreeCommunication");
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");

	VERBOSE_INFO("Step 2 - Identify task that we care about");
	std::vector<std::vector<ARRAY_INDEX>> bags = get_overlaps(to);

	for (auto bag : bags) {
		if (bag.size() > 1) {
			VERBOSE_INFO("Possible conflicts with bag " << commons::toString(bag));
			std::string new_name = commons::join<std::string,std::vector<ARRAY_INDEX>::const_iterator>(bag.begin(), bag.end(), "_");

			edge_id_t old_mapping = to->getMapping( to->getVertexById( bag[0] ) );

			for (auto e : bag) {
				VERBOSE_ASSERT(old_mapping == to->getMapping( to->getVertexById( e ) ) , "Should not try to merge task from different mapping.");
			}

			models::Scheduling scheduling_res = CSDF_SCHEDULING_FUNCTION(to);

			algorithms::transformation::mergeCSDFFromSchedule(to,new_name,bag,&scheduling_res);
			Vertex new_task = to->getVertexByName(new_name);
			to->setMapping(new_task,old_mapping);
			print_graph(to, "after_mergeCSDFFromKperiodicSchedule_" + new_name);
			VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");

		}
	}


	print_graph(to, "end");



	VERBOSE_INFO("Step 3 - Check scheduling");
    VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	models::Scheduling scheduling_res = CSDF_SCHEDULING_FUNCTION(to);


	for (auto task_schedule : scheduling_res.getTaskSchedule()) {


		ARRAY_INDEX tid = task_schedule.first;
		Vertex v = to->getVertexById(task_schedule.first);
		std::string  tname = to->getVertexName(v);
		VERBOSE_ASSERT(task_schedule.second.initial_starts.size() == 0, "Does not support init starts in schedule");
		TIME_UNIT period = task_schedule.second.periodic_starts.first;
		std::vector<TIME_UNIT> &starts =  task_schedule.second.periodic_starts.second;

		VERBOSE_INFO("Task " << std::setw(4) << tid << " | "
				             << std::setw(15) << tname << " | "
							 << std::setprecision(2) << std::setw(7)  << commons::toString(to->getVertexPhaseDuration(v)) << " | "
							 << std::setprecision(2) << std::setw(4)  << period<< " | "
							 << std::setw(7) << commons::toString(starts) );
	}


	if (params.count("PRINT")) {
		std::cout << printers::PeriodicScheduling2DOT(to, scheduling_res ,   60, true ,  1 , 1);
	}
}




