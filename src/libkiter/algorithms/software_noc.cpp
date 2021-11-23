/*
 * software_noc.cpp
 *
 *  Created on: 9 mai 2013
 *      Author: toky
 */

#include <models/NoC.h>
#include <models/NoCGraph.h>
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
#include <cstdlib>
#include <stack>
#include <climits>
#include <algorithms/schedulings.h>
#include <set>
#include <queue>
#include <tuple>
#include <printers/SDF3Wrapper.h>
#include <unordered_map>

static TIME_UNIT NULL_DURATION = 0;

//vector stores the cycle length in each of the vertices
static std::vector< std::vector<unsigned int> >     cycles;
static std::vector< std::set<unsigned int> >        cycid_per_vtxid;
static std::vector< std::vector<unsigned int> >     cyclen_per_vtxid;
typedef std::pair<ARRAY_INDEX, ARRAY_INDEX>  mypair;
typedef std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> mytuple;

struct node{
	ARRAY_INDEX index;
};

//typedef std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > conflictEtype;
//typedef std::map< unsigned int, std::vector< mypair > > conflictEtype;
typedef std::map< unsigned int, std::vector<ARRAY_INDEX> > conflictEtype;
typedef std::map< std::string, std::vector< ARRAY_INDEX > > conflictConfigs;
//typedef std::unordered_map< std::string, std::vector< mytuple > > conflictConfigs;
typedef std::map< ARRAY_INDEX, unsigned int> vid_to_nocEid;

static void print_graph (models::Dataflow * to, std::string suffix = "none") {

	if (not VERBOSE_IS_DEBUG()) return;

	static int counter = 0;
	counter ++ ;
	VERBOSE_INFO("=========== Write file " << counter << "\n");

	std::string sfilename = "software_noc_"+ commons::toString(counter) + "_" + suffix + "";

	{

		std::string filename = sfilename + "_graph";
		std::ofstream myfile;
		myfile.open (filename  + ".dot");
		myfile << printers::GenerateGraphDOT(to);
		myfile.close();

		std::string cmd = "dot " + filename + ".dot -Kneato -T pdf -o " + filename + ".pdf";
		auto out_err = system(cmd.c_str());
		if(out_err)
			VERBOSE_INFO ("System call returns error\n");

	}
	if (false)
	{

		std::string filename = sfilename + "_noc";
		std::ofstream myfile;
		myfile.open (filename  + ".dot");
		myfile << printers::GenerateNoCDOT(to);
		myfile.close();

		std::string cmd = "dot " + filename + ".dot -Kneato -T pdf -o " + filename + ".pdf";
		auto out_err = system(cmd.c_str());
		if(out_err) {
			VERBOSE_INFO ("System call returns error\n");
		}

	}

	printers::writeSDF3File(sfilename+ ".xml", to);
	to->reset_computation();

}


bool operator<(const node& a, const node& b) {

	auto alen = cyclen_per_vtxid[a.index].size();
	auto blen = cyclen_per_vtxid[b.index].size();
	auto lenmin = std::min(alen, blen);

	for(unsigned int i = 0; i < lenmin; i++)
	{
		if(cyclen_per_vtxid[a.index][i] == cyclen_per_vtxid[b.index][i])
			continue;
		else
			return cyclen_per_vtxid[a.index][i] < cyclen_per_vtxid[b.index][i];
	}

	if (alen == 0)
		return true;
	if (blen == 0)
		return false;
	return false;
}


std::vector<ARRAY_INDEX> symbolic_execution (models::Dataflow* const from) {

	std::vector<ARRAY_INDEX> prog_order;
	// Need RV.
	VERBOSE_ASSERT(computeRepetitionVector(from),"inconsistent graph");
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


static std::map<int, Edge> generateEdgesMap(models::Dataflow* dataflow, NoCGraph* noc) {
	std::map<int, Edge> edge_list;
	{ForEachEdge(dataflow,e) {
		auto v1 = dataflow->getEdgeSource(e);
		auto v2 = dataflow->getEdgeTarget(e);

		int v1_i = (int)dataflow->getVertexId(v1);
		int v2_i = (int)dataflow->getVertexId(v2);

		int index = noc->getMapIndex(v1_i, v2_i);
		edge_list[index] = e;
	}}
	return edge_list;
}


LARGE_INT gcdExtended(LARGE_INT x, LARGE_INT y, LARGE_INT *a, LARGE_INT *b)
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


void DFSUtil(Vertex v, std::vector<bool>& visited, models::Dataflow* to, std::vector<Vertex>& traversal)
{
	// Mark the current node as visited and print it
	auto myid = to->getVertexId(v);
	visited[myid] = true;
	traversal.push_back(v);

	// Recur for all the vertices adjacent to this vertex
	{ForOutputEdges(to,v,e){
		Vertex end = to->getEdgeTarget(e);
		ARRAY_INDEX endid = to->getVertexId(end);
		if(!visited[endid])
			DFSUtil(end, visited, to, traversal);
	}}
}


void DFSUtil_PgmOrder(Vertex v, std::vector<bool>& visited, models::Dataflow* to, std::vector<ARRAY_INDEX>& removeEdgeId)
{
	// Mark the current node as visited and print it
	auto myid = to->getVertexId(v);
	visited[myid] = true;
	// Recur for all the vertices adjacent to this vertex
	{ForOutputEdges(to,v,e){
		Vertex end = to->getEdgeTarget(e);
		ARRAY_INDEX endid = to->getVertexId(end);
		if(!visited[endid])
			DFSUtil_PgmOrder(end, visited, to, removeEdgeId);
		else if (cyclen_per_vtxid[endid].size() > 0)
			removeEdgeId.push_back(to->getEdgeId(e));
		//else
		//	std::cout << "Error: wrong removal of edge " << myid << "->" << endid << "\n";
	}}
}



std::string vertexVector2str(std::vector<Vertex>& traversal, models::Dataflow* to)
{
	std::stringstream s;
	for(unsigned int trav_i = 0; trav_i < traversal.size(); trav_i++)
		s << to->getVertexId(traversal[trav_i]) << " ";
	return s.str();
}


//find all parents and set their value with size
void updateAncestorsHelper(Vertex vtx, models::Dataflow* to, std::set<unsigned int>& myset, std::vector<bool>& visited)
{
	auto index = to->getVertexId(vtx);
	for(auto it = myset.begin(); it != myset.end(); it++)
		cycid_per_vtxid[index].insert(*it);
	visited[index] = true;

	{ForInputEdges(to,vtx,e){
		Vertex source = to->getEdgeSource(e);
		auto sourceid = to->getVertexId(source);
		if(!visited[sourceid])
			updateAncestorsHelper(source, to, myset, visited);
	}}
}


void updateAncestorsCycle(Vertex vtx, std::set<unsigned int>& myset, models::Dataflow* to)
{
	ARRAY_INDEX V = to->getVerticesCount();
	std::vector<bool> visited(V+1, false);
	updateAncestorsHelper(vtx, to, myset, visited);
}


// DFS traversal of the vertices reachable from v. It uses recursive DFSUtil()
void DFS(models::Dataflow* to, Vertex v)
{
	ARRAY_INDEX V = to->getVerticesCount();
	// Mark all the vertices as not visited
	std::vector<bool> visited(V+1, false);

	// Call the recursive helper function to print DFS traversal
	std::vector<Vertex> traversal;
	DFSUtil(v, visited, to, traversal);

	std::string traverseStr = vertexVector2str(traversal, to);
	VERBOSE_INFO ( "DFS Order: "  << traverseStr) ;
}


void my_dfs (models::Dataflow* adj, Vertex node, std::vector<bool> visited, Vertex start, std::vector<unsigned int>& path, std::set<unsigned int>& cycleids)
{
	auto node_id = adj->getVertexId(node);
	if (visited[node_id])
	{
		if (node == start)
		{
			std::vector<unsigned int> temp = path;
			sort(temp.begin(), temp.end());
			bool found = false;
			for(unsigned int i = 0; i < cycles.size(); i++)
			{
				if(cycles[i] == temp)
				{
					found = true;
					cycleids.insert((unsigned int)i);
					break;
				}
			}
			if(!found)
			{
				cycles.push_back(temp);
				cycleids.insert((unsigned int)cycles.size()-1);
				//std::cout << "adding a cycle\n";
			}
		}
		return;
	}

	visited[node_id]=true;
	path.push_back((unsigned int)node_id);

	{ForOutputEdges(adj,node,e) {
		auto child = adj->getEdgeTarget(e);
		my_dfs(adj, child, visited, start, path, cycleids);
	}}

	visited[node_id]=false;
	while(path.back() != node_id)
		path.pop_back();
	//popping the node_id element as well
	path.pop_back();
}


std::set<unsigned int> my_dfs_wrapper(models::Dataflow* adj, Vertex start)
{
	ARRAY_INDEX V = adj->getVerticesCount();
	std::vector<bool> visited(V+1, false);
	std::vector<unsigned int> path;

	//Call the above function with the start node:
	std::set<unsigned int> mycycleids;
	my_dfs(adj, start, visited, start, path, mycycleids);

	return mycycleids;
}


models::Dataflow getTranspose(models::Dataflow* to)
{
	models::Dataflow out;
	std::map<ARRAY_INDEX, Vertex> out_vtx_map;

	{ForEachVertex(to,source) {
		auto s_id = to->getVertexId(source);
		out_vtx_map[s_id] = out.addVertex();
	}}
	// Note: getEdgeOut and getEdgeIn are Output and input Rates of a buffer
	{ForEachVertex(to,source) {
		{ForOutputEdges(to,source,e) {
			auto target = to->getEdgeTarget(e);
			auto s_id = to->getVertexId(source);
			auto t_id = to->getVertexId(target);
			out.addEdge(out_vtx_map[t_id], out_vtx_map[s_id]);
		}}
	}}

	return out;
}


void fillOrder(Vertex vtx, std::vector<bool>& visited, std::stack<Vertex> &Stack, models::Dataflow* to)
{
	auto v = to->getVertexId(vtx);
	// Mark the current node as visited and print it
	visited[v] = true;
	// Recur for all the vertices adjacent to this vertex
	{ForOutputEdges(to,vtx,e){
		auto end = to->getEdgeTarget(e);
		auto endid = to->getVertexId(end);
		if(!visited[endid])
			fillOrder(end, visited, Stack, to);
	}}
	// All vertices reachable from v are processed by now, push v
	Stack.push(vtx);
}


// The main function that finds and prints all strongly connected components
void printSCCs(models::Dataflow* to, Vertex start)
{
	ARRAY_INDEX V = to->getVerticesCount();
	std::stack<Vertex> Stack;

	// Mark all the vertices as not visited (For first DFS)
	std::vector<bool> visited(V+1, false);

	//initiate with start node first
	fillOrder(start, visited, Stack, to);
	// Fill vertices in stack according to their finishing times
	{ForEachVertex(to,t) {
		auto v = to->getVertexId(t);
		if(!visited[v])
			fillOrder(t, visited, Stack, to);
	}}

	// Create a reversed graph
	auto transp = getTranspose(to);
	// Mark all the vertices as not visited (For second DFS)
	for(ARRAY_INDEX i = 0; i <= V; i++)
		visited[i] = false;

	// Now process all vertices in order defined by Stack
	while (Stack.size() != 0)
	{
		// Pop a vertex from stack
		Vertex vtx = Stack.top();
		auto v = to->getVertexId(vtx);
		Stack.pop();

		// Print Strongly connected component of the popped vertex
		if (visited[v] == false)
		{
			std::vector<Vertex> traversal;
			DFSUtil(vtx, visited, &transp, traversal);
			//printVector(traversal, to);

			if(traversal.size() > 1)
			{
				for(unsigned int i = 0; i < traversal.size(); i++)
				{
					auto temp_vtx = traversal[i];
					std::set<unsigned int> mycycles = my_dfs_wrapper(to, temp_vtx);
					updateAncestorsCycle(temp_vtx, mycycles, to);
				}
			}
		}
	}

	for(ARRAY_INDEX i = 0; i <= V; i++)
	{
		std::vector<unsigned int> temp;
		cyclen_per_vtxid.push_back(temp);

		for(auto it = cycid_per_vtxid[i].begin(); it != cycid_per_vtxid[i].end(); it++)
			cyclen_per_vtxid[i].push_back( (unsigned int)cycles[*it].size() );

		if(cycid_per_vtxid[i].size() > 0)
			sort(cyclen_per_vtxid[i].begin(), cyclen_per_vtxid[i].end());
	}
}


void setmap(std::vector<int> paths, std::vector<int>& curr_util, NoCGraph* noc)
{
	for(unsigned int p_j = 1; p_j < paths.size()-2; p_j++)
	{
		int mapindex = noc->getMapIndex(paths[p_j], paths[p_j+1]);
		curr_util[mapindex] += 1;
	}
}


int new_findPaths(int src, NoCGraph* noc, int core_considered, std::vector<int>& curr_util, std::map<int, route_t>& store_path, int storepath_id)
{
	if(src != -1 && core_considered != -1)
	{
		std::vector<int> paths = noc->getPath(src, core_considered);
		int max_contention = noc->findPathCost(paths);
		std::cout << "new_findPaths ***  s=" << src << ",d=" << core_considered << ",#paths=" << commons::toString(paths) << ",cont=" << max_contention << "\n";
		route_t path_str;
		for(unsigned int p_j = 0; p_j < paths.size()-1; p_j++) {
			path_str.push_back(  (edge_id_t)noc->getMapIndex(paths[p_j], paths[p_j+1]) );
		}
		store_path[storepath_id] = path_str;
		setmap(paths, curr_util, noc);
		//std::cout << "end\n";
		return max_contention;
	}
	return 0;
}



int dijkstra_findPaths(int src, NoCGraph* noc, int core_considered, std::vector<int>& curr_util, std::map<int, route_t>& store_path, int storepath_id)
{
	if(src != -1 && core_considered != -1)
	{
		std::vector<int> paths = noc->findPathDijkstra(src, core_considered);
		std::cout << "PATH FOUND BETW SRJKVR " << src << " and " << core_considered << " is " << commons::toString(paths) << "\n";

		int max_contention = noc->findPathCost(paths);
		std::cout << "dijkstra_findPaths ***   s=" << src << ",d=" << core_considered << ",#paths=" << commons::toString(paths) << ",cont=" << max_contention << "\n";
		route_t path_str;
		for(unsigned int p_j = 0; p_j < paths.size()-1; p_j++)
			path_str.push_back(  (edge_id_t)noc->getMapIndex(paths[p_j], paths[p_j+1]) );
		store_path[storepath_id] = path_str;
		setmap(paths, curr_util, noc);
		//std::cout << "end\n";
		return max_contention;
	}
	return 0;
}


void dijkstra_mapping(Vertex vtx, std::vector<int>& core_mapping, NoCGraph* noc, const models::Dataflow* d, std::vector<int>& avail_cores, std::map<int, route_t>& routes)
{
	const int start_core = avail_cores[0];
	auto index = d->getVertexId(vtx);
	if((int)avail_cores.size() == noc->size())
	{
		core_mapping[index] = start_core;
		std::remove(avail_cores.begin(), avail_cores.end(), start_core);
		avail_cores.resize( avail_cores.size() - 1);

		std::cout << "allocating " << index << " to " << start_core << "\n";
		return;
	}

	float best_contention_l1 = -1;
	std::map<int, route_t> best_store_path;
	std::vector<int> best_util;
	int core_allocated = -1;

	//in the list of avaiable cores
	for (auto core_considered : avail_cores)
	{
		VERBOSE_DEBUG("dijk core_considered=" << core_considered);//std::cout <<  << "\n";
		//std::cout << "dijk core_considered=" << core_considered << "\n";
		int cont_l1 = -1;
		std::vector<int> curr_util = noc->getLinkUtil(); //get the utilization of every link in the NoC
		std::map<int, route_t> store_path; //variable to store the route to be utilized by the (src,dest) core
		unsigned int counter = 0;
		unsigned int pathlen = 0;


		{ForInputEdges(d, vtx, e){	//Find the core index
			Vertex source_vtx = d->getEdgeSource(e);
			auto source = d->getVertexId(source_vtx);
			int src_core = core_mapping[source];
			//int storepath_id = noc->getMapIndex((int)source, (int)index);
			if(src_core != -1)
				pathlen += noc->getPathLength(src_core, core_considered);
		}}

		{ForOutputEdges(d, vtx, e){
			Vertex target_vtx = d->getEdgeTarget(e);
			auto target = d->getVertexId(target_vtx);
			int tgt_core = core_mapping[target];
			//int storepath_id = noc->getMapIndex((int)index, (int)target);
			if(tgt_core != -1)
				pathlen += noc->getPathLength(core_considered, tgt_core);
		}}


		if(best_contention_l1 != -1 && (float)pathlen > best_contention_l1) //reducing search space
			continue;

		{ForInputEdges(d, vtx, e){	//Find the core index
			Vertex source_vtx = d->getEdgeSource(e);
			auto source = d->getVertexId(source_vtx);
			int src_core = core_mapping[source];
			int storepath_id = noc->getMapIndex((int)source, (int)index);
			if(src_core != -1)
			{
				counter++;
				auto temp_cont_l1 = dijkstra_findPaths(src_core, noc, core_considered, curr_util, store_path, storepath_id);
				cont_l1 = std::max( temp_cont_l1, cont_l1 );
			}
		}}

		{ForOutputEdges(d, vtx, e){
			Vertex target_vtx = d->getEdgeTarget(e);
			auto target = d->getVertexId(target_vtx);
			int tgt_core = core_mapping[target];
			int storepath_id = noc->getMapIndex((int)index, (int)target);
			if(tgt_core != -1)
			{
				counter++;
				auto temp_cont_l1 = dijkstra_findPaths(core_considered, noc, tgt_core, curr_util, store_path, storepath_id);
				cont_l1 = std::max( temp_cont_l1, cont_l1 );
			}
		}}

		float cost = (float)cont_l1;
		if(counter > 0 )
			cost += (float)pathlen/(float)counter;


		VERBOSE_DEBUG("cost=" << cost);
		//std::cout << "cost=" << cost << "\n";

		if(best_contention_l1 == -1 || cost < best_contention_l1)
		{
			best_contention_l1 = cost;
			best_store_path = store_path;
			core_allocated = core_considered;
			best_util = curr_util;
		}
	}

	core_mapping[index] = core_allocated;
	std::remove(avail_cores.begin(), avail_cores.end(), core_allocated);
	avail_cores.resize(avail_cores.size()-1);
	noc->setLinkUtil(best_util);

	for(auto it: best_store_path)
	{
		if(routes.find(it.first) != routes.end())
			VERBOSE_INFO ( "already one route is stored for this");
		routes[it.first] = it.second;
	}
	std::cout << "allocating " << index << " to " << core_allocated << "\n";
}


int hwconstrainted_findPaths(int src, NoCGraph* noc, int core_considered, std::vector<int>& curr_util, std::map<int, route_t>& store_path, int storepath_id)
{
	if(src != -1 && core_considered != -1)
	{
		std::vector<int> paths = noc->findLowContentionPath(src, core_considered);

		if(paths.size() == 0) //if path not found, then that means that we encountered a hardware limit in router configs
		{
			return -1;
		}

		std::cout << "PATH FOUND BETW SRJKVR " << src << " and " << core_considered << " is " << commons::toString(paths) << "\n";

		int max_contention = noc->findPathCost(paths);
		std::cout << "hwconstrainted_findPaths **** s=" << src << ",d=" << core_considered << ",#paths=" << commons::toString(paths) << ",cont=" << max_contention << "\n";
		route_t path_str;
		for(unsigned int p_j = 0; p_j < paths.size()-1; p_j++)
			path_str.push_back(  (edge_id_t)noc->getMapIndex(paths[p_j], paths[p_j+1]) );
		store_path[storepath_id] = path_str;
		setmap(paths, curr_util, noc);
		//std::cout << "end\n";
		return max_contention;
	}
	return 0;
}


void hwconstrainted_mapping(Vertex vtx, std::vector<int>& core_mapping, NoCGraph* noc, const models::Dataflow* d, std::vector<int>& avail_cores, std::map<int, route_t>& routes)
{
	const int start_core = avail_cores[0];
	auto index = d->getVertexId(vtx);
	if((int)avail_cores.size() == noc->size())
	{
		core_mapping[index] = start_core;
		std::remove(avail_cores.begin(), avail_cores.end(), start_core);
		avail_cores.resize( avail_cores.size() - 1);

		std::cout << "allocating " << index << " to " << start_core << "\n";
		return;
	}

	float best_contention_l1 = -1;
	std::map<int, route_t> best_store_path;
	std::vector<int> best_util;
	int core_allocated = -1;
	bool myflag = false;

	//in the list of avaiable cores
	for (auto core_considered : avail_cores)
	{
		//std::cout << "dijk core_considered=" << core_considered << "\n";
		int cont_l1 = -1;
		std::vector<int> curr_util = noc->getLinkUtil(); //get the utilization of every link in the NoC
		std::map<int, route_t> store_path; //variable to store the route to be utilized by the (src,dest) core
		unsigned int counter = 0;
		unsigned int pathlen = 0;


		{ForInputEdges(d, vtx, e){	//Find the core index
			Vertex source_vtx = d->getEdgeSource(e);
			auto source = d->getVertexId(source_vtx);
			int src_core = core_mapping[source];
			//int storepath_id = noc->getMapIndex((int)source, (int)index);
			if(src_core != -1)
				pathlen += noc->getPathLength(src_core, core_considered);
		}}

		{ForOutputEdges(d, vtx, e){
			Vertex target_vtx = d->getEdgeTarget(e);
			auto target = d->getVertexId(target_vtx);
			int tgt_core = core_mapping[target];
			//int storepath_id = noc->getMapIndex((int)index, (int)target);
			if(tgt_core != -1)
				pathlen += noc->getPathLength(core_considered, tgt_core);
		}}


		if(best_contention_l1 != -1 && (float)pathlen > best_contention_l1) //reducing search space
			continue;

		{ForInputEdges(d, vtx, e){	//Find the core index
			Vertex source_vtx = d->getEdgeSource(e);
			auto source = d->getVertexId(source_vtx);
			int src_core = core_mapping[source];
			int storepath_id = noc->getMapIndex((int)source, (int)index);
			if(src_core != -1)
			{
				counter++;
				int temp_cont_l1 = hwconstrainted_findPaths(src_core, noc, core_considered, curr_util, store_path, storepath_id);
				if(temp_cont_l1 == -1)
				{
					myflag = true;
					break;
				}
				cont_l1 = std::max( temp_cont_l1, cont_l1 );
			}
		}}
		if(myflag)
		{
			myflag = false;
			continue;
		}

		{ForOutputEdges(d, vtx, e){
			Vertex target_vtx = d->getEdgeTarget(e);
			auto target = d->getVertexId(target_vtx);
			int tgt_core = core_mapping[target];
			int storepath_id = noc->getMapIndex((int)index, (int)target);
			if(tgt_core != -1)
			{
				counter++;
				int temp_cont_l1 = hwconstrainted_findPaths(core_considered, noc, tgt_core, curr_util, store_path, storepath_id);
				if(temp_cont_l1 == -1)
				{
					myflag = true;
					break;
				}
				cont_l1 = std::max( temp_cont_l1, cont_l1 );
			}
		}}
		if(myflag)
		{
			myflag = false;
			continue;
		}
		float cost = (float)cont_l1;
		if(counter > 0 )
			cost += (float)pathlen/(float)counter;


		//std::cout << "cost=" << cost << "\n";

		if(best_contention_l1 == -1 || cost < best_contention_l1)
		{
			best_contention_l1 = cost;
			best_store_path = store_path;
			core_allocated = core_considered;
			best_util = curr_util;
		}
	}

	core_mapping[index] = core_allocated;
	std::remove(avail_cores.begin(), avail_cores.end(), core_allocated);
	avail_cores.resize(avail_cores.size()-1);
	noc->setLinkUtil(best_util);

	for(auto it: best_store_path)
	{
		if(routes.find(it.first) != routes.end())
			VERBOSE_INFO ( "already one route is stored for this");
		routes[it.first] = it.second;
	}
	std::cout << "allocating " << index << " to " << core_allocated << "\n";
}

int findPaths(int src, NoCGraph* noc, int core_considered, std::vector<int>& curr_util, std::map<int, route_t>& store_path, int storepath_id)
{
	if(src != -1 && core_considered != -1)
	{
		int max_contention_l2 = INT_MAX;
		int path_idx = -1;

		std::vector< std::vector<int> > paths = noc->getShortestPaths(src, core_considered); //estimate the cost
		//std::cout << "s=" << src << ",d=" << core_considered << ",#paths=" << paths.size() << "\n";
		for(unsigned int p_i = 0; p_i < paths.size(); p_i++)
		{
			int cont_l2 = 0;
			for(unsigned int p_j = 1; p_j < paths[p_i].size()-2; p_j++)
			{
				//std::cout << paths[p_i][p_j] << "->" << paths[p_i][p_j+1] << " ";
				int mapindex = noc->getMapIndex(paths[p_i][p_j], paths[p_i][p_j+1]);
				//if(curr_util.find(mapindex) != curr_util.end())
				cont_l2 = std::max(cont_l2, curr_util[mapindex]);
			}
			if(cont_l2 < max_contention_l2)
			{
				max_contention_l2 = cont_l2;
				path_idx = p_i;
				//std::cout << "\nabove path is chosen";
			}
			//std::cout << "\n";
		}
		std::cout << "findPaths **** s=" << src << ",d=" << core_considered << ",#paths=" << commons::toString(paths) << "\n";

		route_t path_str;
		for(unsigned int p_j = 0; p_j < paths[path_idx].size()-1; p_j++) {
			path_str.push_back(  (edge_id_t)noc->getMapIndex(paths[path_idx][p_j], paths[path_idx][p_j+1]) );
		}
		store_path[storepath_id] = path_str;

		setmap(paths[path_idx], curr_util, noc);
		return max_contention_l2;
	}
	return 0;
}


void mapping(Vertex vtx, std::vector<int>& core_mapping, NoCGraph* noc, models::Dataflow* d, std::vector<int>& avail_cores, std::map<int, route_t>& routes)
{
	const int start_core = avail_cores[0];
	auto index = d->getVertexId(vtx);
	if((int)avail_cores.size() == noc->size())
	{
		core_mapping[index] = start_core;
		std::remove(avail_cores.begin(), avail_cores.end(), start_core);
		//std::cout << "mapping node " << index << " to core " << start_core << ",size=" << avail_cores.size() << "\n";
		avail_cores.resize( avail_cores.size() - 1);
		return;
	}

	float best_contention_l1 = -1;
	std::map<int, route_t> best_store_path;
	//std::map<int, int> best_util;
	std::vector<int> best_util;

	int core_allocated = -1;

	for (auto core_considered : avail_cores)
	{
		//std::cout << "considering core " << core_considered << "\n";
		int cont_l1 = -1;
		//std::map<int, int> curr_util = noc->getLinkUtil();
		std::vector<int> curr_util = noc->getLinkUtil();

		std::map<int, route_t> store_path; //variable to store the route to be utilized by the (src,dest) core
		unsigned int counter = 0;
		unsigned int pathlen = 0;

		{ForInputEdges(d, vtx, e){
			//Find the core index
			Vertex source_vtx = d->getEdgeSource(e);
			auto source = d->getVertexId(source_vtx);
			int src_core = core_mapping[source];
			int storepath_id = noc->getMapIndex((int)source, (int)index);
			auto temp_cont_l1 = findPaths(src_core, noc, core_considered, curr_util, store_path, storepath_id);
			cont_l1 = std::max( temp_cont_l1, cont_l1 );
			if(src_core != -1)
			{
				counter++;
				pathlen += noc->getPathLength(src_core, core_considered);
			}
			//std::cout << "cont_l1=" << cont_l1 << "\n";
		}}
		{ForOutputEdges(d, vtx, e){
			Vertex target_vtx = d->getEdgeTarget(e);
			auto target = d->getVertexId(target_vtx);
			int tgt_core = core_mapping[target];
			int storepath_id = noc->getMapIndex((int)index, (int)target);
			auto temp_cont_l1 = findPaths(core_considered, noc, tgt_core, curr_util, store_path, storepath_id);
			cont_l1 = std::max( temp_cont_l1, cont_l1 );
			if(tgt_core != -1)
			{
				counter++;
				pathlen += noc->getPathLength(core_considered, tgt_core);
			}
			//std::cout << "2-cont_l1=" << cont_l1 << "\n";
		}}
		float cost = (float)cont_l1;
		if(counter > 0 )
			cost += (float)pathlen/(float)counter;
		if(best_contention_l1 == -1 || cost < best_contention_l1)
		{
			best_contention_l1 = cost;
			best_store_path = store_path;
			core_allocated = core_considered;
			best_util = curr_util;
		}
		//std::cout << "trying core " << core_considered << ",cont=" << cont_l1 << ",lenfac=" << (float)pathlen/(float)counter << ",cost=" << cost << "\n";
	}
	//std::cout << "after edges\n";
	core_mapping[index] = core_allocated;
	//std::cout << "mapping node " << index << " to core " << core_allocated << ",cont=" << best_contention_l1 << "\n";
	std::remove(avail_cores.begin(), avail_cores.end(), core_allocated);
	avail_cores.resize(avail_cores.size()-1);
	noc->setLinkUtil(best_util);

	for(auto it: best_store_path)
	{
		if(routes.find(it.first) != routes.end()) {
			VERBOSE_INFO ( "already one route is stored fro this");
		}
		routes[it.first] = it.second;
	}
}


//remove the current edge between nodes add intermediate nodes based on the path between them
void taskAndNoCMapping(const models::Dataflow* input, models::Dataflow* to, Vertex start, NoCGraph* noc, std::map<int, route_t>& routes/*, std::vector<ARRAY_INDEX>& prog_order*/)
{
	ARRAY_INDEX V = to->getVerticesCount();
	std::priority_queue<node> pq;
	std::vector<bool> visited(V+1, false);
	std::vector<int> core_mapping(V+1, -1);

	//list of cores that are available
	std::vector<int> available_cores;//{5, 6, 10, 9, 8, 4, 0, 1, 2, 3, 7, 11, 15, 14, 13, 12};
	ARRAY_INDEX origV = input->getVerticesCount();

	if(origV <= 16)
	{
		std::vector<int> temp_vec{5, 6, 10, 9, 8, 4, 0, 1, 2, 3, 7, 11, 15, 14, 13, 12};
		available_cores = temp_vec;
	}
	else
	{
		for(int i = 0; i < noc->getMeshSize(); i++)
			available_cores.push_back(i);
	}


	VERBOSE_INFO ( "available_cores " << commons::toString(available_cores) ) ;

	noc->clear();

	auto top = start;
	ARRAY_INDEX endid = to->getVertexId(top);

	node temp2;
	temp2.index = endid;
	visited[endid] = true;
	pq.push(temp2);

	while(!pq.empty())
	{
		top = to->getVertexById(pq.top().index);
		pq.pop();
		if(top != start)
		{
			//std::cout << "mapping " << to->getVertexId(top) << ",mesh=" << noc->getMeshSize() << "\n";
			//if( noc->getMeshSize() <= 9*9)
			//	mapping(top, core_mapping, noc, input, available_cores, routes);
			//else
			//hwconstrainted_mapping(top, core_mapping, noc, input, available_cores, routes);
			dijkstra_mapping(top, core_mapping, noc, input, available_cores, routes);
			//new_mapping(top, core_mapping, noc, input, available_cores, routes);

		}
		visited[to->getVertexId(top)] = true;

		{ForOutputEdges(to, top, e){
			Vertex end = to->getEdgeTarget(e);
			ARRAY_INDEX endid = to->getVertexId(end);

			//std::cout << "trying node " << endid << "\n";
			if(!visited[endid])
			{
				bool flag = true;
				{ForInputEdges(to, end, e2){
					Vertex e2end = to->getEdgeSource(e2);
					ARRAY_INDEX e2endid = to->getVertexId(e2end);
					if(!visited[e2endid])//if parent is not executed, cannot map this node
						flag = false;
				}}
				if(flag)
				{
					//std::cout << "adding " << endid << " to queue\n";
					node temp;
					temp.index = endid;
					pq.push(temp);
				}
			}
		}}
	}

	/*for(unsigned int s = 0; s < prog_order.size(); s++)
	{
		auto top = to->getVertexById(prog_order[s]);
		mapping(top, core_mapping, noc, input, available_cores, routes);
	}*/

	/*
	for(ARRAY_INDEX s = 1; s <= (ARRAY_INDEX)(V-1); s++)
	{
		prog_order[s] = prog_order[s] + 1 - 1;
		auto top = to->getVertexById(s);
		mapping(top, core_mapping, noc, input, available_cores, routes);
	}
	 */
	VERBOSE_INFO ( "srjkvr-mapping " << commons::toString(core_mapping) ) ;
	std::cout << "SRJKVR";
	for(ARRAY_INDEX i = 1; i < core_mapping.size()-1; i++)
		std::cout << "," << core_mapping[i];
	std::cout << "\n";
}


//remove the current edge between nodes
//add intermediate nodes based on the path between them
std::vector<Vertex> addPathNode(models::Dataflow* d, Edge c, route_t list, conflictEtype& returnValue, conflictConfigs& configs, vid_to_nocEid& vid_to_conflict_map, bool addConfigNode)
{
	d->reset_computation();
	std::vector<Vertex> new_vertices;
	// We store infos about edge to be deleted
	auto source_vtx = d->getEdgeSource(c);
	auto target_vtx = d->getEdgeTarget(c);

	//Find the core index
	auto source = d->getVertexId(source_vtx);
	auto target = d->getVertexId(target_vtx);

	//use the inrate and route of the edges ans use it when creating the edges
	auto inrate = d->getEdgeIn(c);
	auto outrate = d->getEdgeOut(c);
	auto preload = d->getPreload(c);  // preload is M0

	if (source == target) //ignore this case
		return new_vertices;

	bool flag = true;
	d->removeEdge(c); //we delete the edge



	int list_idx = 0;
	//std::cout << "flow:" << source << "->" << target << ":";
	//for every link in the path, add a corresponding node

	VERBOSE_INFO("Model the NoC communication between " << source << " to " << target );
	VERBOSE_INFO("  Route : " << commons::toString(list));

	for (auto e : list) {
		// we create a new vertex "middle"
		auto middle = d->addVertex();
		new_vertices.push_back(middle);

		////PLLLEASE DONT CHANGE THE "mid-" value in the name"
		std::stringstream ss;
		ss << "mid-" << source << "-" << target << "_" << e;

		returnValue[(unsigned int)e].push_back(  d->getVertexId(middle)  );
		vid_to_conflict_map[ d->getVertexId(middle) ] = (unsigned int)e;

		d->setVertexName(middle,ss.str());

		d->setPhasesQuantity(middle,1); // number of state for the actor, only one in SDF
		d->setVertexDuration(middle,{1}); // is specify for every state , only one for SDF.
		d->setReentrancyFactor(middle,1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.




		// we create a new edge between source and middle,
		auto e1 = d->addEdge(source_vtx, middle);

		if(flag)
		{
			d->setEdgeInPhases(e1,{inrate});  // we specify the production rates for the buffer
			flag = false;
		}
		else
		{
			d->setEdgeInPhases(e1,{1});
			d->setEdgeType(e1,EDGE_TYPE::BUFFERLESS_EDGE);
		}

		d->setEdgeOutPhases(e1,{1}); // and the consumption rate (as many rates as states for the associated task)
		d->setPreload(e1,0);  // preload is M0

		source_vtx = middle;

		if(addConfigNode && list_idx <= (int)list.size() - 2)
		{
			std::stringstream config_key, config_name;
			config_key << list[list_idx] << "_" << list[list_idx+1];
			config_name << "cfg-" << source << "_" << target << "-" << list[list_idx] << "_" << list[list_idx+1];


			auto vtx = d->addVertex();
			mytuple tuple_temp;
			//get<0>(tuple_temp) = d->getVertexId(vtx);
			//get<1>(tuple_temp) = d->getVertexId(middle);
			//configs[config_key.str()].push_back(tuple_temp);
			configs[config_key.str()].push_back(d->getVertexId(vtx));




			d->setVertexName(vtx,config_name.str());
			d->setPhasesQuantity(vtx,1); // number of state for the actor, only one in SDF
			d->setVertexDuration(vtx,{NULL_DURATION}); // is specify for every state , only one for SDF.
			d->setReentrancyFactor(vtx,1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.

			// we create a new edge between source and middle,
			auto e1 = d->addEdge(middle, vtx);

			d->setEdgeInPhases(e1,{1});
			d->setEdgeType(e1,EDGE_TYPE::BUFFERLESS_EDGE);
			d->setEdgeOutPhases(e1,{1}); // and the consumption rate (as many rates as states for the associated task)
			d->setPreload(e1,0);  // preload is 0

			source_vtx = vtx;

		}
		list_idx++;
	}
	//find the final edge
	auto e2 = d->addEdge(source_vtx, target_vtx);
	d->setEdgeOutPhases(e2,{outrate});
	d->setEdgeInPhases(e2,{1});
	d->setPreload(e2,preload);  // preload is M0
	return new_vertices;
}


//Remove the cyclic edges
Vertex removeCycleEdges(models::Dataflow* to, std::vector<ARRAY_INDEX>& prog_order) {
	auto origV = to->getVerticesCount();
	std::vector<bool> visited(origV, false);
	std::vector<ARRAY_INDEX> removeEdgeId;
	std::vector<ARRAY_INDEX> vertexId;


	for(int i = 0; i < (int)prog_order.size(); i++)
	{
		auto vid = prog_order[i];
		Vertex v = to->getVertexById(vid);
		if(!visited[vid])
			DFSUtil_PgmOrder(v, visited, to, removeEdgeId);
	}
	for(int i = 0; i < (int)removeEdgeId.size(); i++)
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
	for(int i = 0; i < (int)vertexId.size(); i++)
	{
		auto t = to->getVertexById( vertexId[i]);
		to->addEdge(start, t);
	}
	return start;
}


//Process the graph for DFS, etc. in this function
std::map<int, route_t> graphProcessing(const models::Dataflow* dataflow, NoCGraph* noc, std::vector<ARRAY_INDEX>& prog_order) {
	models::Dataflow* to = new models::Dataflow(*dataflow);
	to->reset_computation();
	bool myflag = false;
	bool myflag2 = false;
	Vertex top;
	std::map<int, route_t> routes;

	auto start = to->addVertex();
	to->setVertexName(start, "start");
	{ForEachVertex(to,t) {
		if( (t != start) && (!myflag) && (!myflag2))
		{
			top = t;
			myflag2 = true;
		}

		if(t == start)
		{
			continue;
		}
		if(dataflow->getVertexInDegree( dataflow->getVertexById(to->getVertexId(t)) ) == 0)
		{
			myflag = true;
			to->addEdge(start, t);
			std::cout << "adding new edge between start " << to->getVertexId(t) << "\n";
		}
	}}

	if(!myflag)
		to->addEdge(start, top);

	ARRAY_INDEX V = to->getVerticesCount();
	for(ARRAY_INDEX i = 0; i < V+1; i++)
	{
		std::set<unsigned int> temp;
		cycid_per_vtxid.push_back(temp);
	}

	printSCCs(to, start);

	models::Dataflow* to2 = new models::Dataflow(*dataflow);
	to2->reset_computation();
	start = removeCycleEdges(to2, prog_order);

	taskAndNoCMapping(dataflow, to2, start, noc, routes/*, prog_order*/);

	return routes;
}



void addDependency(models::Dataflow* d, const Vertex vi, const Vertex vj, EXEC_COUNT ni, EXEC_COUNT nj)
{
	LARGE_INT a, b;
	auto gcd_value = gcdExtended((LARGE_INT) ni, (LARGE_INT) nj, &a, &b);

	ni /= gcd_value;
	nj /= gcd_value;
	if (d->is_read_only()) {
		d->reset_computation();
	}
	auto e1 = d->addEdge(vi, vj);
	auto e2 = d->addEdge(vj, vi);

	gcd_value = ni + nj - gcdExtended((LARGE_INT) ni, (LARGE_INT) nj, &a, &b);

	d->setEdgeOutPhases(e1,{(TOKEN_UNIT)ni});
	d->setEdgeInPhases(e1,{(TOKEN_UNIT)nj});
	d->setPreload(e1,gcd_value);  // preload is M0

	d->setEdgeOutPhases(e2,{(TOKEN_UNIT)nj});
	d->setEdgeInPhases(e2,{(TOKEN_UNIT)ni});
	d->setPreload(e2,0);  // preload is M0

	VERBOSE_INFO ( "Win=" << ni << " Wout=" << nj << " Mo=" << gcd_value);

}


//Function to remove all the edges that connect a particular vertex
void removeAllEdgesVertex(models::Dataflow* d, Vertex vtx)
{
	{ForInputEdges(d, vtx, E)	{
		d->removeEdge(E);
	}}
	{ForOutputEdges(d, vtx, E)	{
		d->removeEdge(E);
	}}
	// d->removeVertex(vtx);
}


//Function to remove the edges and obtain phase vector values
//v1 -> cfg -> v2
void getEdgesPhaseVector(models::Dataflow* d, Vertex& v1, Vertex cfg, Vertex& v2, std::vector<TOKEN_UNIT>& in, std::vector<TOKEN_UNIT>& out, TOKEN_UNIT& preload_v1, TOKEN_UNIT& preload_v2, EDGE_TYPE& e_v1, EDGE_TYPE& e_v2)
{
	//std::cout << "inside fn: v1=" << d->getVertexName(v1) << ",cfg=" << d->getVertexName(cfg) << ",v2=" << d->getVertexName(v2) << "\n";
	{ForInputEdges(d, cfg, E)	{
		in = d->getEdgeInVector(E);
		v1 = d->getEdgeSource(E);
		preload_v1 =  d->getPreload(E);
		e_v1 = d->getEdgeType(E);
		d->removeEdge(E);
		break;
	}}
	{ForOutputEdges(d, cfg, E)	{
		out = d->getEdgeOutVector(E);
		v2 = d->getEdgeTarget(E);
		preload_v2 =  d->getPreload(E);
		e_v2 = d->getEdgeType(E);
		d->removeEdge(E);
		break;
	}}
}


bool resolveDestConflicts(models::Dataflow* d, Vertex dst, std::vector<ARRAY_INDEX> & origV)
{
	LARGE_INT mygcd = 0, a, b;
	std::vector<Edge> dstedges, rtredges;
	TOKEN_UNIT flow = 0, preload = 0;

	//1. Find all the edges from the vertex to router (dstedges)
	// and router to next edge in the NoC (rtredges)
	{ForInputEdges(d,dst,inE)	{
		auto prev_node = d->getEdgeSource(inE);
		if(!commons::findInVector (origV, d->getVertexId(prev_node)) && prev_node!= dst)
		{
			dstedges.push_back(inE);
			auto edgeout = d->getEdgeOut(inE);

			if(mygcd == 0)
				mygcd = edgeout;
			else
				mygcd = gcdExtended(edgeout, mygcd, &a, &b);

			{ForInputEdges(d,prev_node,inE_p)     {
				if(prev_node != d->getEdgeSource(inE_p))
					rtredges.push_back(inE_p);
			}}
		}
	}}


	//1B. Do some sanity check here
	VERBOSE_ASSERT (dstedges.size() == rtredges.size() ,  "BIG ERROR in resolveDestConflicts" );

	if(dstedges.size() <= 1)
		return false;

	//1C. Initialize token vector
	std::vector<TOKEN_UNIT> temp;
	std::vector< std::vector<TOKEN_UNIT> > token_vec (dstedges.size(), temp);

	//2. calculate the preload and flow values of all incoming edges from destination vertices
	//Remove the edges between router and destionation. Remove the router nodes
	for(int i = 0; i < (int)dstedges.size(); i++)
	{
		for(int j = 0; j < (int)dstedges.size(); j++)
		{
			for(int vi = 0; vi < (int)d->getEdgeOut(dstedges[i])/mygcd; vi++)
			{
				if (i == j)
					token_vec[j].push_back(1);
				else
					token_vec[j].push_back(0);
			}
		}
		flow += d->getEdgeOut( dstedges[i] );
		preload += d->getPreload( dstedges[i] );
		VERBOSE_INFO ( "states=" << flow << ",preload=" << preload );
	}

	//2B. Divide flow by gcd to reduce the states
	flow = flow/mygcd;

	VERBOSE_ASSERT(flow > 0, "This case is not supported: flow between task has to be strictly positive");

	//2C. Create the phase duration and token per phase for the new router node
	std::vector<TOKEN_UNIT> dsttoken(flow, 1);
	std::vector<TIME_UNIT> phaseDurVec(flow, 1.0);

	//3. Create the BIG router, by combining all the router nodes that were used to reach the destination
	auto middle = d->addVertex();
	std::stringstream ss;
	ss << d->getVertexId(dst) << "-D";
	d->setVertexName(middle, ss.str());
	d->setPhasesQuantity(middle, flow); // number of state for the actor, only one in SDF
	d->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	d->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.
	VERBOSE_INFO ( "DST_CONF(ID="<< d->getVertexId(dst) << "):Done creating big router node  " << d->getVertexId(middle) << ",gcd=" << mygcd );

	//3B. Add edge between big router and destination vertex
	auto dstEdge = d->addEdge(middle,dst);
	d->setPreload(dstEdge, preload);
	d->setEdgeOutPhases(dstEdge, {flow*mygcd});
	d->setEdgeInPhases(dstEdge, dsttoken);


	//4. Setup the new edges. First remove the edge between router and the next NoC link. Setup the flow appropriately
	//Remove the router vertex and the edge between routet to the next NoC link
	for(int i = 0; i < (int)rtredges.size(); i++)
	{
		//(v1)  rtredges[i]  v2
		auto v1 = d->getEdgeSource(rtredges[i]);
		auto v2 = d->getEdgeTarget(rtredges[i]);
		auto loc_preload = d->getPreload(rtredges[i]);

		//std::cout << "dst=" << d->getVertexName(dst) << ",big_middle=" << d->getVertexName(middle) << ",v1=" << d->getVertexName(v1) << ",deleting=" << d->getVertexName(v2) << "\n";
		removeAllEdgesVertex(d, v2);

		auto new_edge = d->addEdge(v1, middle);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setEdgeOutPhases(new_edge, token_vec[i]);
		d->setEdgeInPhases(new_edge, {1});
		d->setPreload(new_edge,loc_preload);  // preload is M0

	}
	return true;
}


TOKEN_UNIT compute_local_flow (models::Dataflow* to, std::vector< ARRAY_INDEX >& mergeNodes)  {



	//Find GCD value
	EXEC_COUNT gcd_value = 0;
	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex vi     = to->getVertexById(vid);
		EXEC_COUNT ni = to->getNi(vi);
		gcd_value = std::gcd (gcd_value , ni) ;
	}

	//Find the total flow
	TOKEN_UNIT temp_flow = 0;
	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex vi     = to->getVertexById(vid);
		EXEC_COUNT ni = to->getNi(vi);
		temp_flow += ni/gcd_value;
	}
	return temp_flow;
}


static bool mergeConfigNodesInit(models::Dataflow* to, std::string name , std::vector< ARRAY_INDEX >& mergeNodes) {

	VERBOSE_INFO("name = " << name << " nodes: " << commons::toString( mergeNodes ) );
	// This function is not generic yet, cannot support preload
	if (mergeNodes.size() <= 1)
		return false;

	// step one we schedule

	to->reset_computation();
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	//std::cout << "calling csdf\n";
	models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
	TIME_UNIT omega = scheduling_res.getGraphPeriod();
	auto persched = scheduling_res.getTaskSchedule();
	VERBOSE_INFO("omega = " << omega );

	//std::cout << "done done\n";
	VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule, thus I cannot merge anymore.");

	TIME_UNIT mergeVtxDuration = NULL_DURATION;

	// step two, we find the maximum time to reach by init phases
	TIME_UNIT max_start = 0;
	for (auto vid : mergeNodes) {
		max_start = std::max(max_start, persched[vid].periodic_starts.second[0]);
	}

	//Find GCD value
	EXEC_COUNT gcd_value = 0;
	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex vi     = to->getVertexById(vid);
		mergeVtxDuration = std::max(mergeVtxDuration, to->getVertexDuration(vi));
		EXEC_COUNT ni = to->getNi(vi);
		gcd_value = std::gcd (gcd_value , ni) ;
	}




	//step two , we produce the init phases for each task
	std::vector < std::pair<TIME_UNIT , ARRAY_INDEX> > init_start_times ;
	std::vector < std::pair<TIME_UNIT , ARRAY_INDEX> > periodic_start_times ;
	for (auto vid : mergeNodes) {
		Vertex vi     = to->getVertexById(vid);
		auto period = persched[vid].periodic_starts.first;
		auto starts = persched[vid].periodic_starts.second;
		EXEC_COUNT ni = to->getNi(vi);
		VERBOSE_INFO("For task " << vid << " period is " <<period << " and starts " << commons::toString(starts));
		ARRAY_INDEX current_start_index = 0;
		VERBOSE_ASSERT(starts.size(), "Need a valid scheduling");
		TIME_UNIT current_start_time = starts[0];

		while (current_start_time < max_start) {
			init_start_times.push_back(std::make_pair(current_start_time , vid));
			current_start_index++;
			current_start_time = starts[current_start_index % starts.size()]  + (current_start_index / starts.size() ) * period;
		}

		for (EXEC_COUNT i = 0 ; i < (ni/gcd_value) ; i++) {
			periodic_start_times.push_back(std::make_pair(current_start_time , vid));
			current_start_index++;
			current_start_time = starts[current_start_index % starts.size()]  + (current_start_index / starts.size() ) * period;
		}

	}

	VERBOSE_INFO("Sorting" );
	std::sort(init_start_times.begin(), init_start_times.end());
	std::sort(periodic_start_times.begin(), periodic_start_times.end());


	std::vector<TIME_UNIT> phaseDurVec(periodic_start_times.size(), mergeVtxDuration); //Create the phase duration
	std::vector<TIME_UNIT> InitPhaseDurVec(init_start_times.size(), mergeVtxDuration); //Create the phase duration
	//std::vector<TIME_UNIT> phaseDurVec(periodic_start_times.size(), NULL_DURATION); //Create the phase duration
	//std::vector<TIME_UNIT> InitPhaseDurVec(init_start_times.size(), NULL_DURATION); //Create the phase duration



	to->reset_computation();


	VERBOSE_INFO("Create router" );
	//3. Create the BIG router
	auto middle = to->addVertex();
	to->setVertexName(middle,  name);
	to->setInitPhasesQuantity(middle, init_start_times.size()); // number of state for the actor, only one in SDF
	to->setPhasesQuantity(middle, periodic_start_times.size()); // number of state for the actor, only one in SDF
	to->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	to->setVertexInitDuration(middle, InitPhaseDurVec); // is specify for every state , only one for SDF.
	to->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.


	std::map <ARRAY_INDEX , std::vector <TOKEN_UNIT> > periodic_token_vec ;
	std::map <ARRAY_INDEX , std::vector <TOKEN_UNIT> > init_token_vec ;

	for(ARRAY_INDEX vid :mergeNodes) {
		periodic_token_vec[vid]  = std::vector <TOKEN_UNIT> (periodic_start_times.size(), 0);
		init_token_vec[vid]  = std::vector <TOKEN_UNIT> (init_start_times.size(), 0);
	}

	for (unsigned int i = 0 ; i < periodic_start_times.size() ; i++) {
		periodic_token_vec [periodic_start_times[i].second][i] = 1;
	}

	for (unsigned int i = 0 ; i < init_start_times.size() ; i++) {
		init_token_vec [init_start_times[i].second][i] = 1;
	}


	VERBOSE_INFO("Remove the edges" );

	//4. Remove the edges before/after the config node. Setup the flow. Remove the router nodes.

	for(ARRAY_INDEX vid :mergeNodes)
	{

		VERBOSE_INFO("Edge " << vid );
		Vertex cfgVtx = to->getVertexById(vid);
		Vertex v1;
		Vertex v2;
		std::vector<TOKEN_UNIT> myin, myout;
		std::vector<TOKEN_UNIT> myiin, myiout;
		TOKEN_UNIT preload_v1, preload_v2;
		EDGE_TYPE e_v1, e_v2;


		//getEdgesPhaseVector(to, v1, cfgVtx, v2, myin, myout, preload_v1, preload_v2, e_v1, e_v2);
		// void getEdgesPhaseVector(models::Dataflow* d, Vertex& v1, Vertex cfg, Vertex& v2, std::vector<TOKEN_UNIT>& in, std::vector<TOKEN_UNIT>& out, TOKEN_UNIT& preload_v1, TOKEN_UNIT& preload_v2, EDGE_TYPE& e_v1, EDGE_TYPE& e_v2)



		{ForInputEdges(to, cfgVtx, E)	{
			myin = to->getEdgeInVector(E);
			myiin = to->getEdgeInitInVector(E);
			v1 = to->getEdgeSource(E);
			preload_v1 =  to->getPreload(E);
			e_v1 = to->getEdgeType(E);
			to->removeEdge(E);
			break;
		}}
		{ForOutputEdges(to, cfgVtx, E)	{
			myout = to->getEdgeOutVector(E);
			myiout = to->getEdgeInitOutVector(E);
			v2 = to->getEdgeTarget(E);
			preload_v2 =  to->getPreload(E);
			e_v2 = to->getEdgeType(E);
			to->removeEdge(E);
			break;
		}}

		auto new_edge = to->addEdge(v1, middle);
		to->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		to->setPreload(new_edge, preload_v1);
		to->setEdgeType(new_edge, e_v1);

		to->setEdgeInInitPhases(new_edge, myiin);
		to->setEdgeInPhases(new_edge, myin);
		to->setEdgeOutPhases(new_edge, periodic_token_vec[vid]);
		to->setEdgeOutInitPhases(new_edge, init_token_vec[vid]);
		VERBOSE_ASSERT((EXEC_COUNT)init_token_vec[vid].size() == to->getInitPhasesQuantity(middle), "init_token_vec[vid] has the wrong size: " << init_token_vec[vid].size() << " !=" << to->getInitPhasesQuantity(middle));

		print_graph(to, "InsidemergeConfigNodesInit1");
		new_edge = to->addEdge(middle, v2);
		to->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		to->setPreload(new_edge, preload_v2);
		to->setEdgeInPhases(new_edge, periodic_token_vec[vid]);
		to->setEdgeInInitPhases(new_edge, init_token_vec[vid]);
		VERBOSE_ASSERT((EXEC_COUNT)init_token_vec[vid].size() == to->getInitPhasesQuantity(middle), "init_token_vec[vid] has the wrong size: " << init_token_vec[vid].size() << " !=" << to->getInitPhasesQuantity(middle));
		//VERBOSE_ASSERT(to->getInitPhasesQuantity(v2) == 0, "getInitPhasesQuantity(v2) has the wrong size: " <<  to->getInitPhasesQuantity(v2));

		to->setEdgeOutPhases(new_edge, myout);
		to->setEdgeOutInitPhases(new_edge, myiout);
		to->setEdgeType(new_edge, e_v2);
		print_graph(to, "InsidemergeConfigNodesInit2");
	}


	VERBOSE_INFO("End of Remove the edges" );
	return true;



}
bool mergeConfigNodes(models::Dataflow* d, std::string name , std::vector< ARRAY_INDEX >& mergeNodes, TIME_UNIT mergeDur) {

	VERBOSE_INFO ("STEP 1");
	int mn_size = (int)mergeNodes.size();
	if (mn_size <= 1)	return false;

	d->reset_computation();

	models::Dataflow* temp_d = new models::Dataflow(*d);
	VERBOSE_ASSERT(computeRepetitionVector(temp_d),"inconsistent graph");

	VERBOSE_INFO ("STEP 2");
	TOKEN_UNIT flow = (TOKEN_UNIT)mn_size, temp_flow = 0;
	LARGE_INT gcd_value = 0, a, b;
	//Find GCD value
	for(int i = 0; i < mn_size; i++)
	{
		Vertex vi = temp_d->getVertexById(mergeNodes[i]);
		auto ni = (LARGE_INT) temp_d->getNi(vi);
		if(gcd_value == 0)
			gcd_value = ni;
		gcd_value = gcdExtended(gcd_value, ni, &a, &b);
	}

	VERBOSE_INFO ("STEP 3");
	//Find the total flow
	for(int i = 0; i < mn_size; i++)
	{
		Vertex vi = temp_d->getVertexById(mergeNodes[i]);
		auto ni = (LARGE_INT) temp_d->getNi(vi);
		temp_flow += ni/gcd_value;
	}
	flow = temp_flow;

	VERBOSE_INFO ("STEP 4");
	//1. Initialize token vecot
	std::vector<TOKEN_UNIT> temp(flow, 0);
	std::vector< std::vector<TOKEN_UNIT> > token_vec (mn_size, temp);
	std::vector<TIME_UNIT> phaseDurVec(flow, mergeDur); //Create the phase duration
	LARGE_INT start = 0, end;
	for(int i = 0; i < mn_size; i++)
	{
		Vertex vi = temp_d->getVertexById(mergeNodes[i]);
		auto ni = (LARGE_INT) temp_d->getNi(vi);
		end = start + ni/gcd_value;
		for(LARGE_INT j = start; j < end; j++)
			token_vec[i][j] = 1;
		start = end;
	}
	VERBOSE_INFO ("STEP 5");

	//3. Create the BIG router
	auto middle = d->addVertex();
	d->setVertexName(middle, name);
	d->setPhasesQuantity(middle, flow); // number of state for the actor, only one in SDF
	d->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	d->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.

	//4. Remove the edges before/after the config node. Setup the flow. Remove the router nodes.
	for(int i = 0; i < mn_size; i++)
	{
		Vertex cfgVtx = d->getVertexById(mergeNodes[i]);
		Vertex v1;
		Vertex v2;
		std::vector<TOKEN_UNIT> myin, myout;
		TOKEN_UNIT preload_v1, preload_v2;
		EDGE_TYPE e_v1, e_v2;
		getEdgesPhaseVector(d, v1, cfgVtx, v2, myin, myout, preload_v1, preload_v2, e_v1, e_v2);

		auto new_edge = d->addEdge(v1, middle);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setPreload(new_edge, preload_v1);
		d->setEdgeInPhases(new_edge, myin);
		d->setEdgeOutPhases(new_edge, token_vec[i]);
		d->setEdgeType(new_edge, e_v1);

		new_edge = d->addEdge(middle, v2);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setPreload(new_edge, preload_v2);
		d->setEdgeInPhases(new_edge, token_vec[i]);
		d->setEdgeOutPhases(new_edge, myout);
		d->setEdgeType(new_edge, e_v2);
	}
	d->reset_computation();
	return true;
}



bool resolveSrcConflicts(models::Dataflow* d, Vertex src, std::vector<ARRAY_INDEX> & origV)
{

	VERBOSE_INFO ( "resolveSrcConflicts for "  << d->getVertexName(src) << " with original number of vertices " << commons::toString(origV.size()));

	std::vector<Edge> srcedges, rtredges;
	TOKEN_UNIT in_flow = 0;
	TOKEN_UNIT preload = 0;
	LARGE_INT mygcd = 0, a, b;


	//1. Find all the edges from the vertex to router
	// and router to next edge in the NoC
	{ForOutputEdges(d,src,outE)	{
		auto next_node = d->getEdgeTarget(outE);

		if( !commons::findInVector (origV, d->getVertexId(next_node)) && next_node!= src)
		{
			srcedges.push_back(outE);
			auto edgein = d->getEdgeIn(outE);
			in_flow += edgein;
			preload += d->getPreload(outE);
			VERBOSE_INFO ( "states=" << in_flow << ",preload=" << preload );
			if(mygcd == 0)
				mygcd = edgein;
			else
				mygcd = gcdExtended(edgein, mygcd, &a, &b);

			{ForOutputEdges(d,next_node,inE)     {
				if(next_node != d->getEdgeTarget(inE))
					rtredges.push_back(inE);
			}}
		}
		else if( commons::findInVector (origV, d->getVertexId(next_node)))
		{
			std::cout << "src=" << d->getVertexName(src) << " still has outgoing edge to " << d->getVertexName(next_node) << "\n";
			exit(0);
		}
	}}


	//1B. Do some sanity check here
	VERBOSE_ASSERT(srcedges.size() == rtredges.size(),  "BIG ERROR in resolveSrcConflicts\n");

	if(srcedges.size() <= 1) {
		//std::cout << "no resolveSrcConflicts for vertex " << d->getVertexId(src) << "\n";
		return false;
	}

	VERBOSE_ASSERT(mygcd > 0 , "Division by zero");
	in_flow = in_flow / mygcd;

	//1C. Initialize token vector
	std::vector<TOKEN_UNIT> temp;
	std::vector< std::vector<TOKEN_UNIT> > token_vec (srcedges.size(), temp);

	//2. calculate the preload and flow values of all outgoing edges from source vertices
	//Remove the edges between source and router.
	//Remove the router nodes
	for(int i = 0; i < (int)srcedges.size(); i++)
	{
		for(int j = 0; j < (int)srcedges.size(); j++)
		{
			for(int vi = 0; vi < (int)d->getEdgeIn(srcedges[i])/mygcd; vi++)
			{
				if (i == j)
					token_vec[j].push_back(1);
				else
					token_vec[j].push_back(0);
			}
		}
	}
	VERBOSE_INFO ( "done with removing edges");
	VERBOSE_ASSERT(in_flow > 0, "This case is not supported: flow between task has to be strictly positive");

	//2B. Create the phase duration and token per phase for the new router node
	std::vector<TOKEN_UNIT> srctoken(in_flow, 1);
	std::vector<TIME_UNIT> phaseDurVec(in_flow, 1.0);

	//3. Create the BIG router
	auto middle = d->addVertex();
	std::stringstream ss;
	ss << d->getVertexId(src) << "-S";
	d->setVertexName(middle, ss.str());
	d->setPhasesQuantity(middle, in_flow); // number of state for the actor, only one in SDF
	d->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	d->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.
	VERBOSE_INFO ( "SRC_CONF(ID="<< d->getVertexId(src) << "):Done creating big router node  " << d->getVertexId(middle) );

	//3B. Add edge between source vertex and big router node
	auto srcEdge = d->addEdge(src, middle);
	d->setPreload(srcEdge, preload);
	d->setEdgeInPhases(srcEdge, {in_flow*mygcd});
	d->setEdgeOutPhases(srcEdge, srctoken);


	//4. Setup the new edges. First remove the edge between router and the next NoC link
	//Setup the flow appropriately
	//Remove the router vertex and the edge between routet to the next NoC link
	for(int i = 0; i < (int)rtredges.size(); i++)
	{
		//(v1)  rtredges[i]  v2
		auto v1 = d->getEdgeSource(rtredges[i]);
		auto v2 = d->getEdgeTarget(rtredges[i]);
		auto loc_preload = d->getPreload(rtredges[i]);

		//std::cout << "src=" << d->getVertexName(src) << ",rtr_new=" << d->getVertexName(middle) << ",v2=" << d->getVertexName(v2) << ",deleting=" << d->getVertexName(v1) << "\n";
		removeAllEdgesVertex(d, v1);

		auto new_edge = d->addEdge(middle, v2);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setEdgeInPhases(new_edge, token_vec[i]);
		d->setEdgeOutPhases(new_edge, {1});
		d->setPreload(new_edge,loc_preload);  // preload is M0

	}
	return true;
}


void removeOrphanNodes(models::Dataflow* to, vid_to_nocEid& vid_to_conflict_map, conflictEtype& conflictEdgeMap)
{
	for (bool removeme = true; removeme; ) {
		{ForEachVertex(to,v) {
			if (to->getVertexDegree(v) == 0) {
				ARRAY_INDEX vid = to->getVertexId(v);
				std::string vname = to->getVertexName(v);
				VERBOSE_INFO ( " I remove one task (" << vid << ",name=" << vname << ") lah!");
				to->removeVertex(v);

				if(vid_to_conflict_map.find(vid) != vid_to_conflict_map.end())
				{
					unsigned int eid = vid_to_conflict_map[vid];
					std::vector<ARRAY_INDEX>::iterator it = std::find(conflictEdgeMap[eid].begin(), conflictEdgeMap[eid].end(), vid);
					if(it != conflictEdgeMap[eid].end())
					{
						conflictEdgeMap[eid].erase(it);
						//std::cout << "erasing " << vname << "\n";
					}
				}
				removeme=true;
				break;
			}
			removeme=false;
		}}
	}
}

void getPrevAndNextVertex(ARRAY_INDEX vtx_id, models::Dataflow* to, ARRAY_INDEX& prev, ARRAY_INDEX& next)
{
	auto vtx = to->getVertexById(vtx_id);
	int count = 0;
	std::cout << "vname=" << to->getVertexName(vtx) << "\n===========================\n";
	{ForInputEdges(to,vtx,inE)	{
		prev = to->getVertexId( to->getEdgeSource(inE) );
		std::cout << "source=" << to->getVertexName( to->getEdgeSource(inE) ) << "\n";
		std::cout << "target=" << to->getVertexName( to->getEdgeTarget(inE) ) << "\n";
		count ++;
	}}
	if(count > 1)
	{
		std::cout << "incoming edges more than 1\n"; exit(0);
	}
	std::cout << "===========================\n";
	count = 0;
	{ForOutputEdges(to,vtx,outE)	{
		next = to->getVertexId( to->getEdgeTarget(outE) );
		std::cout << "source=" << to->getVertexName( to->getEdgeSource(outE) ) << "\n";
		std::cout << "target=" << to->getVertexName( to->getEdgeTarget(outE) ) << "\n";
		count++;
	}}
	if(count > 1)
	{
		std::cout << "incoming edges more than 1\n"; exit(0);
	}
	std::cout << "===========================\n\n\n";
}

//std::vector<mytuple>
std::vector<ARRAY_INDEX> checkForConflicts(conflictEtype& conflictEdges, models::Dataflow* to, TIME_UNIT HP, scheduling_t& persched, models::Dataflow* original, std::string& conflictname)
{
	//std::vector< mytuple > mergeNodes;
	std::vector< ARRAY_INDEX > mergeNodes;
	mytuple tuple_temp;
	ARRAY_INDEX prev, next;

	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	VERBOSE_ASSERT(computeRepetitionVector(original),"inconsistent graph");
	int total_conflict = 0;
	for (auto  key : conflictEdges) {
		auto edge_id = key.first;
		auto mysize = conflictEdges[edge_id].size();
		if(mysize <= 1)
			continue;
		for(unsigned int i = 0; i < mysize; i++)
		{
			for(unsigned int j = 0; j < mysize; j++)
			{
				if (i <= j) //ignore the upper triangle
					continue;
				auto taski = conflictEdges[edge_id][i];
				auto taskj = conflictEdges[edge_id][j];

				std::cout << "problem in " << edge_id << ",vid=" << conflictEdges[edge_id][i] << ",vid2=" << conflictEdges[edge_id][j] << "\n";

				auto taski_vtx = to->getVertexById(taski);
				auto taskj_vtx = to->getVertexById(taskj);

				auto ni = to->getNi(taski_vtx);
				auto nj = to->getNi(taskj_vtx);

				//check if any of the starting times match
				std::vector<TIME_UNIT> si_vec, sj_vec;
				for (auto  skey : persched[taski].periodic_starts.second) {
					si_vec.push_back(skey);
				}
				for (auto  skey : persched[taskj].periodic_starts.second) {
					sj_vec.push_back(skey);
				}

				for(unsigned int si_i = 0; si_i < si_vec.size(); si_i++)
				{
					for(unsigned int sj_i = 0; sj_i < sj_vec.size(); sj_i++)
					{
						auto si =  persched[taski].periodic_starts.second[si_i];
						auto sj =  persched[taskj].periodic_starts.second[sj_i];

						if( algorithms::isConflictPresent((LARGE_INT) HP, si, (LARGE_INT) ni, sj, (LARGE_INT) nj) )
						{
							VERBOSE_INFO ("confl: " << to->getVertexName(taski_vtx) << " and " << to->getVertexName(taskj_vtx) << "\n");
							conflictname = to->getVertexName(taski_vtx) + "_" + to->getVertexName(taskj_vtx);
							get<0>(tuple_temp) = taski;
							getPrevAndNextVertex(taski, to, prev, next);
							get<1>(tuple_temp) = prev;
							get<2>(tuple_temp) = next;
							std::cout << prev << "-> " << taski << "->" << next << "\n";
							//mergeNodes.push_back(tuple_temp);
							mergeNodes.push_back(taski);


							get<0>(tuple_temp) = taskj;
							getPrevAndNextVertex(taskj, to, prev, next);

							if(prev == get<1>(tuple_temp))
							{
								std::cout << "avoiding conflicts\n";
								mergeNodes.clear();
								continue;
							}

							else
							{
								total_conflict += 1;
								get<1>(tuple_temp) = prev;
								get<2>(tuple_temp) = next;
								//mergeNodes.push_back(tuple_temp);
								mergeNodes.push_back(taskj);
								std::cout << prev << "-> " << taskj << "->" << next << "\n";
								return mergeNodes;
							}
						}
					}
				}
			}
		}
	}

	models::Dataflow* temp = new models::Dataflow(*original);
	VERBOSE_ASSERT(computeRepetitionVector(temp),"inconsistent graph");
	EXEC_COUNT orig_ni = 0, mod_ni = 0;
	{ForEachVertex(to,v) {
		if(to->getVertexId(v) == 1)
		{
			mod_ni = to->getNi(v);
			break;
		}
	}}
	{ForEachVertex(temp,v) {
		if(temp->getVertexId(v) == 1)
		{
			orig_ni = temp->getNi(v);
			break;
		}
	}}

	VERBOSE_INFO ( "total_conflict=" << total_conflict );
	VERBOSE_INFO ( "NEW HP=" << HP*orig_ni/mod_ni );
	return mergeNodes;
}


void printTasks(models::Dataflow* to)
{
	{ForEachVertex(to, to_v) {
		std::cout << "NODE " << to->getVertexName(to_v) << "\n";
	}}
}


void findHP(models::Dataflow* orig, models::Dataflow* to, scheduling_t& persched, TIME_UNIT* HP, EXEC_COUNT* LCM)
{
	//fins ratio between orig and new graph for the nodes
	float ratio = -1;
	std::map<ARRAY_INDEX, float> ratio_arr;
	std::map<ARRAY_INDEX, TIME_UNIT> delays;
	VERBOSE_ASSERT(computeRepetitionVector(orig),"inconsistent graph");
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	{ForEachVertex(orig,orig_v) {
		auto orig_vid = orig->getVertexId(orig_v);
		auto to_v = to->getVertexById(orig_vid);
		float new_ratio = (float) to->getNi(to_v) / (float)orig->getNi(orig_v);
		if(ratio != -1 && ratio != new_ratio)
			std::cout << "NODE " << to->getVertexName(to_v) << ",ni=" <<  to->getNi(to_v) << " failed\n";
		ratio = new_ratio;
		ratio_arr[orig_vid] = new_ratio;


		// Here I need to compute the Ni-th execution of a task
		auto Ni = orig->getNi(orig_v);
		auto periodic_start_count = persched[orig_vid].periodic_starts.second.size();
		VERBOSE_ASSERT(Ni % periodic_start_count == 0, "FIXME");
		VERBOSE_ASSERT(orig->getPhasesQuantity(orig_v) == 1, "FIXME");



		// I take Ni and I need to know which start correspond to this Ni-th
		// auto start_index = Ni % periodic_start_count
		auto start_index = periodic_start_count - 1;
		auto end_of_last_start  = persched[orig_vid].periodic_starts.second[start_index] * (Ni / periodic_start_count) + orig->getVertexDuration(orig_v);

		delays[orig_vid] = end_of_last_start;


	}}

	*HP = 0.0;
	*LCM = 1;
	for (auto  key : persched) {
		auto task = key.first;
		auto task_vtx = to->getVertexById(key.first);

		float new_ratio = ratio_arr[key.first];
		auto period = persched[task].periodic_starts.first;
		auto starts = persched[task].periodic_starts.second;
		std::stringstream ss;
		for(int i = 0; i < (int)starts.size(); i++)
			ss << std::setprecision(13) << starts[i] << " ";

		*HP =    ( period * to->getNi(task_vtx) ) / (new_ratio * ((float)starts.size())) ;
		auto delay = delays[key.first];
		VERBOSE_INFO ( "Task " <<  to->getVertexName(task_vtx) <<  " : "
				<< " duration=[ " << commons::toString(to->getVertexPhaseDuration(task_vtx)) <<  "]"
				<< " period=" <<  period << ""
				<< " HP=" << *HP << ""
				<< " Delay=" << delay << ""
				<< " Ni=" << to->getNi(task_vtx) << ""
				<< " starts=[ " << ss.str() << "]");
		*LCM = std::lcm(*LCM, to->getNi(task_vtx));

	}
	*LCM = std::lcm(*LCM, (EXEC_COUNT)*HP);

}



bool algorithms::isConflictPresent(LARGE_INT HP, TIME_UNIT si, LARGE_INT ni, TIME_UNIT sj, LARGE_INT nj)
{
	LARGE_INT temp1, temp2;
	LARGE_INT lhs = (LARGE_INT)(si - sj) * ni * nj;
	LARGE_INT rhs_gcd = HP * gcdExtended(myabs(ni), myabs(nj), &temp1, &temp2);

	if(si == sj) //if start time is same conflict
	{
		return true;
	}

	if (myabs(lhs) % rhs_gcd != 0)
	{
		return false; //No integral solutions exist as the division will yield floating point
	}
	else //check if any other solution exists
	{
		return true; //there exists a solution for sure
	}
}



static NoCGraph * createNoCGraph (const NoC * noc) {

	NoCGraph* g = new NoCGraph(noc->size()*2);
	for (auto edge : noc->getEdges()) {
		g->addEdge(edge.src, edge.dst);
	}
	return g;
}


static void updateNetworkOnChipModel (models::Dataflow* const  dataflow) {

	// #### Generate NoC
	int mesh_row = (int)ceil(sqrt((double) dataflow->getVerticesCount()));
	if(mesh_row <= 4)
		mesh_row = 4;
	NoC noc (mesh_row, mesh_row); //Init NoC
	dataflow->setNoC(noc);

}
void algorithms::software_noc_bufferless(models::Dataflow* const  dataflow, parameters_list_t   param_list) {
	// #### Parameters
	double yscale = 1;
	if (param_list.count("yscale") == 1) yscale = std::stod(param_list["yscale"]);



	updateNetworkOnChipModel(dataflow);
	NoCGraph *  noc = createNoCGraph (&(dataflow->getNoC()));

	// #### symbolic execution to find program execution order (prog_order)
	models::Dataflow* to2 = new models::Dataflow(*dataflow);
	std::vector<ARRAY_INDEX> prog_order = symbolic_execution(to2);
	delete to2;


	conflictEtype conflictEdges; //stores details of flows that share noc edges
	conflictConfigs configs; //stores the details of the router configs that are shared
	vid_to_nocEid vid_to_conflict_map; //used to index newly added nodes into the conflict table, so as to remove it easily


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

	/// Store a list of origina l actors id.
	std::vector <ARRAY_INDEX> original_vertex_ids;
	for (auto v : to->vertices()) {
		original_vertex_ids.push_back(to->getVertexId(v));
	}

	//#### do some processing to perfrom task mapping, NoC route determination
	//####  Task to Core mapping
	std::map<int, route_t> routes = graphProcessing(to, noc, prog_order);


	// #### Once the mapping is done, we apply the mapping to the SDFG
	// ## Run addPathNode, create intermediate nodes.
	std::map<int, Edge> edge_list = generateEdgesMap(to, noc);


	for (auto route_item : routes ) {
		VERBOSE_INFO ("Edge " << to->getEdgeName(edge_list[route_item.first])  << ", Route " << commons::toString(route_item.second));
		for (auto edge : route_item.second) {

			VERBOSE_INFO ("    - " << edge );
		}
	}
	std::cout << "done route size = " << routes.size() << "\n";
	//printTasks(to);




	for(auto it:routes) {
		Edge e = edge_list[it.first];
		VERBOSE_INFO("replace edge " << e << "by a sequence");
		addPathNode(to, e, it.second, conflictEdges, configs, vid_to_conflict_map, true);
	}

	VERBOSE_INFO ("configs = ");

	for(conflictConfigs::iterator it = configs.begin(); it != configs.end(); it++) {
		std::vector<std::string> parts = commons::split<std::string>((*it).first,'_');
		edge_id_t src = commons::fromString<edge_id_t>(parts[0]);
		edge_id_t dst = commons::fromString<edge_id_t>(parts[1]);
		std::vector<ARRAY_INDEX> vertex_ids = (*it).second ;
		VERBOSE_ASSERT(to->getNoC().getEdge(src).dst == to->getNoC().getEdge(dst).src, "NoC Graph doesnt match NoC");
		VERBOSE_INFO ("  - " << src << "::" << dst << " : "  << commons::toString((*it).second) << " = " );

		for (auto id : vertex_ids) {
			Vertex router_node = to->getVertexById(id);
			VERBOSE_INFO ("        x " << to->getVertexName(router_node) << "(" << id << ")");
		}

	}

	VERBOSE_INFO ("conflictEdges = ");
	for(auto item :  conflictEdges) {

		VERBOSE_INFO ("  - " << item.first << ":"  << commons::toString(item.second));
	}

	VERBOSE_INFO ("vid_to_conflict_map = ");
	for(auto item :  vid_to_conflict_map) {

		VERBOSE_INFO ("  - " << item.first << ":"  << item.second);
	}




	// ###### At this pint, we have a SDFG with artefacts for network.
	VERBOSE_INFO ("adding path nodes done");
	std::cout << "done addpath\n";

	print_graph(to,"after_addpath");

	//resolve cnflicts for all the  (a) sources that sent data to multiple nodes.
	//				(b) destinations that receive data from multiple nodes.
	//				(c) nodes that correspond to the same coniguration
	//Use [1, origV] as it denotes the list of nodes in the original SDF


	// ###### First source and destination merge
	// ############################################
	for (auto vid : original_vertex_ids) {
		auto src = to->getVertexById(vid);
		resolveSrcConflicts(to, src, original_vertex_ids);
		print_graph(to,"resolveSrcConflicts");
	}
	std::cout << "done sourceconflict\n";

	/*
	{
		to->reset_computation();
		VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
		models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
		TIME_UNIT omega = scheduling_res.getGraphPeriod();
		auto persched = scheduling_res.getTaskSchedule();

		VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule anymoe, resolveSrcConflicts failed.");
		to->reset_computation();
	}
	 */

	VERBOSE_INFO ( "done source conflict resolve" );

	//print_graph(to);
#ifdef WITH_DEST_CONFLICT
	for (auto vid : original_vertex_ids)
	{
		auto dest = to->getVertexById(vid);
		//resolveDestConflicts(to, dest, original_vertex_ids);

		//Change start
		std::vector<ARRAY_INDEX> mergeNodes;
		//Find all the edges from the vertex to router (dstedges) and router to next edge in the NoC (rtredges)
		{ForInputEdges(to,dest,inE)	{
			auto prev_node = to->getEdgeSource(inE);
			if(!commons::findInVector (original_vertex_ids, to->getVertexId(prev_node)) && prev_node!= dest)
				mergeNodes.push_back( to->getVertexId(prev_node));
		}}
		std::stringstream ss;
		ss << vid << "-D";
		auto retval = mergeConfigNodesInit(to, ss.str(), mergeNodes);
		//Change end

		if(retval)
		{
			removeOrphanNodes(to, vid_to_conflict_map, conflictEdges);
			//print_graph(to);
		}
	}
	std::cout << "done destconflict\n";
	VERBOSE_INFO ( "done dest conflict resolve" ) ;
#endif

	// ###### Systematically merge configs because we can
	// ############################################

	//print_graph(to);




	VERBOSE_INFO ("Call mergeConfigNodes");
	int idx = 0;
	for(conflictConfigs::iterator it = configs.begin(); it != configs.end(); it++) {
		std::cout << "Working on merge " << idx++ << " over " << configs.size() << "("  << it->first << ":" <<  commons::toString(it->second) << ")" <<  "\n";
		mergeConfigNodesInit(to, it->first, it->second);
		print_graph(to,"mergeConfigNodesInit");
	}
	VERBOSE_INFO ("mergeConfigNodes Done.");
	std::cout << "done merge conflict resolve\n";

	//Remove conflicts at source and destination router links as a big node has been created
	for(auto it:routes)
	{
		auto start_id = routes[it.first][0];
		auto cf_it = conflictEdges.find((unsigned int)start_id);
		if(cf_it != conflictEdges.end())
			conflictEdges.erase(cf_it);

#ifdef WITH_DEST_CONFLICT
		auto mysize = routes[it.first].size() - 1;
		auto end_id = routes[it.first][mysize];
		auto cf_it2 = conflictEdges.find((unsigned int)end_id);
		if(cf_it2 != conflictEdges.end())
			conflictEdges.erase(cf_it2);
#endif
	}

	VERBOSE_INFO("resolving conflicts done");
	//Original graph


	removeOrphanNodes(to, vid_to_conflict_map, conflictEdges);
	print_graph(to,"removeOrphanNodes");

	std::cout << "calling scheduling\n";


	while(true)
	{
		std::vector< ARRAY_INDEX > mergeNodes;
		//std::vector< mytuple > mergeNodes;
		std::string name;

		std::cout << "calling kperiodic\n";

		to->reset_computation();
		VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
		models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
		TIME_UNIT omega = scheduling_res.getGraphPeriod();
		auto persched = scheduling_res.getTaskSchedule();

		VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule.");

		mergeNodes = checkForConflicts(conflictEdges, to, omega, persched, dataflow, name);
		print_graph(to,"checkForConflicts");

		if(mergeNodes.size() == 0)
			break;

		std::cout << "conflict in " << name << "\n";
		//mergeConfigNodes(to, name, mergeNodes);
		mergeConfigNodesInit(to, name, mergeNodes);
		print_graph(to,"mergeConfigNodesInit");

		std::cout << "before removeme\n";
		removeOrphanNodes(to, vid_to_conflict_map, conflictEdges);
		print_graph(to,"removeOrphanNodes");
		std::cout << "before compp\n";


	}

	std::cout << "out of loop\n";

	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
	scheduling_t persched = scheduling_res.getTaskSchedule();
	VERBOSE_INFO("findHP");
	EXEC_COUNT LCM;
	TIME_UNIT HP;
	findHP(dataflow, to, persched, &HP, &LCM);
	print_graph(to,"findHP");


	auto utility = noc->getLinkUtil();
	VERBOSE_INFO ( "utility" <<  commons::toString(utility) );

	for (auto item : persched) {
		ARRAY_INDEX tid = item.first;
		Vertex v = to->getVertexById(item.first);
		std::string  tname = to->getVertexName(v);
		VERBOSE_ASSERT(item.second.initial_starts.size() == 0, "Does not support init starts");
		TIME_UNIT period = item.second.periodic_starts.first;
		std::vector<TIME_UNIT> &starts = item.second.periodic_starts.second;
		std::string line = "";

		for (TIME_UNIT time = 0 ; time < 50 ; time ++) {
			bool execute = false;
			for (ARRAY_INDEX sidx = 0 ; sidx < starts.size() ; sidx++) {
				TIME_UNIT s = starts[sidx];
				TIME_UNIT duration = to->getVertexDuration(v, 1 + (sidx % to->getPhasesQuantity(v))); // TODO : unsupported init phases
				TIME_UNIT normalize = (time > s) ? (time - s) - ((time - s) / period) : (time - s);
				bool execute_here = ((0  <= normalize ) and (normalize < duration ));
				execute = execute or execute_here;
			}
			line += execute ? "#" : " ";
		}
		VERBOSE_INFO("Task " << std::setw(5) << tid << " | " << line );
	}

}


