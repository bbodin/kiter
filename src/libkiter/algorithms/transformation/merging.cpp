/*
 * merging.cpp
 *
 *  Created on: Nov 13, 2019
 *      Author: toky
 */


#include <models/Dataflow.h>
#include <models/Scheduling.h>
#include <algorithms/transformation/merging.h>
#include <algorithms/schedulings.h>
#include <string>
#include <algorithm>

//Function to remove the edges and obtain phase vector values
//v1 -> cfg -> v2
static void getEdgesPhaseVector(models::Dataflow* d, Vertex& v1, Vertex cfg, Vertex& v2, std::vector<TOKEN_UNIT>& in, std::vector<TOKEN_UNIT>& out)
{
	//std::cout << "inside fn: v1=" << d->getVertexName(v1) << ",cfg=" << d->getVertexName(cfg) << ",v2=" << d->getVertexName(v2) << "\n";
	{ForInputEdges(d, cfg, E)	{
		in = d->getEdgeInVector(E);
		v1 = d->getEdgeSource(E);
		d->removeEdge(E);
		break;
	}}
	{ForOutputEdges(d, cfg, E)	{
		out = d->getEdgeOutVector(E);
		v2 = d->getEdgeTarget(E);
		d->removeEdge(E);
		break;
	}}
}


bool algorithms::transformation::mergeCSDFFromKperiodicSchedule(models::Dataflow* to, std::string name , std::vector< ARRAY_INDEX >& mergeNodes) {
	to->reset_computation();
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
	return algorithms::transformation::mergeCSDFFromSchedule(to,name,mergeNodes,&scheduling_res);
}

bool algorithms::transformation::basic_mergeCSDFFromSchedule(models::Dataflow* to, std::string name , const std::vector< ARRAY_INDEX >& mergeNodes, const models::Scheduling* scheduling_res) {

	VERBOSE_INFO("Start basic_mergeCSDFFromSchedule");
	// Bad idea cannot check easily the sched is for dataflow
	TIME_UNIT omega = scheduling_res->getGraphPeriod();
	VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule, thus I cannot merge anymore.");

	auto persched = scheduling_res->getTaskSchedule();

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
		EXEC_COUNT ni = to->getNi(vi);
		gcd_value = std::gcd (gcd_value , ni) ;
	}


	//step two , we produce the init phases for each task
	std::vector < std::pair<TIME_UNIT , ARRAY_INDEX> > init_start_times ;
	std::vector < std::pair<TIME_UNIT , ARRAY_INDEX> > periodic_start_times ;
	for (auto vid : mergeNodes) {
		Vertex vi     = to->getVertexById(vid);
		std::string vname = to->getVertexName(vi);
		EXEC_COUNT ni = to->getNi(vi);
		auto period = persched[vid].periodic_starts.first;
		auto starts = persched[vid].periodic_starts.second;
		VERBOSE_INFO("For task " << vid << " (" <<  vname << ") period is " << std::fixed  << std::setprecision(2) << period << " and starts " << commons::toString(starts));
		ARRAY_INDEX current_start_index = 0;
		TIME_UNIT current_start_time = starts[0];

		while (current_start_time < max_start) {
			init_start_times.push_back(std::make_pair(current_start_time , vid));
			current_start_index++;
			current_start_time = starts[current_start_index % starts.size()]  + (current_start_index / starts.size() ) * period;
		}

		for (EXEC_COUNT i = 0 ; i <  to->getPhasesQuantity(vi) * (ni/gcd_value) ; i++) {
			periodic_start_times.push_back(std::make_pair(current_start_time , vid));
			current_start_index++;
			current_start_time = starts[current_start_index % starts.size()]  + (current_start_index / starts.size() ) * period;
		}
	}

	std::sort(init_start_times.begin(), init_start_times.end());
	std::sort(periodic_start_times.begin(), periodic_start_times.end());

	VERBOSE_INFO("init_start_times = " << commons::toString(init_start_times));
	VERBOSE_INFO("periodic_start_times = " << commons::toString(periodic_start_times));

	std::vector<TIME_UNIT> phaseDurVec(periodic_start_times.size(), 0); //Create the phase duration
	std::vector<TIME_UNIT> InitPhaseDurVec(init_start_times.size(), 0); //Create the phase duration


	// TODO : set durations!!!!

	to->reset_computation();

	//3. Create the BIG router
	auto middle = to->addVertex();
	to->setVertexName(middle, name);
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
		periodic_token_vec [periodic_start_times[i].second][i] = 1; // TODO : Put real timing
	}

	for (unsigned int i = 0 ; i < init_start_times.size() ; i++) {
		init_token_vec [init_start_times[i].second][i] = 1;
	}

	//4. Remove the edges before/after the config node. Setup the flow. Remove the router nodes.

	for (ARRAY_INDEX vid : mergeNodes)
	{
		Vertex cfgVtx = to->getVertexById(vid);
		Vertex v1;
		Vertex v2;
		std::vector<TOKEN_UNIT> myin, myout;
		getEdgesPhaseVector(to, v1, cfgVtx, v2, myin, myout);

		auto new_edge = to->addEdge(v1, middle);
		to->setEdgeName(new_edge, "merged_" + commons::toString(to->getEdgeId(new_edge)));
		to->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		to->setPreload(new_edge, 0);

		to->setEdgeInPhases(new_edge, myin);
		to->setEdgeOutPhases(new_edge, periodic_token_vec[vid]);
		to->setEdgeOutInitPhases(new_edge, init_token_vec[vid]);

		new_edge = to->addEdge(middle, v2);
		to->setEdgeName(new_edge, "merged_" + commons::toString(to->getEdgeId(new_edge)));
		to->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		to->setPreload(new_edge, 0);
		to->setEdgeInPhases(new_edge, periodic_token_vec[vid]);
		to->setEdgeInInitPhases(new_edge, init_token_vec[vid]);
		to->setEdgeOutPhases(new_edge, myout);
	}

	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex cfgVtx = to->getVertexById(vid);
		to->removeVertex(cfgVtx);
	}

	return true;
}

struct vertex_infos {
	ARRAY_INDEX id;
	EXEC_COUNT init_phases;
	EXEC_COUNT periodic_phases;
	std::vector<TIME_UNIT> durations;
	TIME_UNIT period;
	std::vector<TIME_UNIT> starts;
	EXEC_COUNT ni;

	vertex_infos (models::Dataflow* to, periodic_task_schedule_t sched, Vertex v) :
		id (to->getVertexId(v)) ,
		init_phases (to->getInitPhasesQuantity(v)) ,
		periodic_phases (to->getPhasesQuantity(v)) {
		durations = to->getVertexInitPhaseDuration(v);

		const std::vector<TIME_UNIT>& original_periodic = to->getVertexPhaseDuration(v);
		durations.insert( durations.end(), original_periodic.begin(), original_periodic.end() );
		period = sched.first;
		starts = sched.second;
		ni = to->getNi(v);
	}
};

struct edge_infos {

	ARRAY_INDEX id;
	std::string name;
	ARRAY_INDEX src_id;
	ARRAY_INDEX dst_id;

	std::vector<TOKEN_UNIT> in_per_vector;
	std::vector<TOKEN_UNIT> in_ini_vector;

	std::vector<TOKEN_UNIT> out_per_vector;
	std::vector<TOKEN_UNIT> out_ini_vector;

	TOKEN_UNIT preload;

	EDGE_TYPE type;

	edge_infos (models::Dataflow* to, Edge e) :
		id (to->getEdgeId(e)),
		name (to->getEdgeName(e)),
		src_id (to->getVertexId(to->getEdgeSource(e))),
		dst_id (to->getVertexId(to->getEdgeTarget(e))),
		in_per_vector(to->getEdgeInVector(e)),
		in_ini_vector(to->getEdgeInitInVector(e)),
		out_per_vector(to->getEdgeOutVector(e)),
		out_ini_vector(to->getEdgeInitOutVector(e)),
		preload(to->getPreload(e)),
		type(to->getEdgeType(e)) {

	}
};


/**
 * given an init and a periodic pattern, return a sequence of the n first values
 * @param init      init pattern
 * @param periodic  periodic pattern
 * @param n         size of output vector
 * @return          sequence of the n first values
 */
std::vector<TOKEN_UNIT> produce_sequence (const std::vector<TOKEN_UNIT>& init, const std::vector<TOKEN_UNIT>& periodic, size_t n) {

	// Check input
	VERBOSE_ASSERT((init.size() + periodic.size()) <= n, "Invalid input");

	std::vector<TOKEN_UNIT> res;

	for (size_t i = 0 ; i < init.size(); i++) {
		res.push_back(init[i]);
	}

	for (size_t i = 0 ; (init.size() + i) < n; i++) {
		res.push_back(periodic[i % periodic.size()]);
	}

	VERBOSE_ASSERT(res.size() == n, "Something unexpected happen, result is not correct");
	return res;
}

TIME_UNIT get_next_execution_time (vertex_infos infos, EXEC_COUNT index) {
	VERBOSE_DEBUG("get_next_execution_time: Scheduling of task " << infos.id);
	VERBOSE_DEBUG("get_next_execution_time: period: " << commons::toString(infos.period));
	VERBOSE_DEBUG("get_next_execution_time: Init starts: " << commons::toString(std::vector<TIME_UNIT>(infos.starts.begin(), infos.starts.begin() + infos.init_phases)));
	VERBOSE_DEBUG("get_next_execution_time: periodic starts: " << commons::toString(std::vector<TIME_UNIT>(infos.starts.begin() + infos.init_phases, infos.starts.end())));

	if (index < infos.starts.size()) {
		VERBOSE_DEBUG("get_next_execution_time: result: " << infos.starts[index]);
		return infos.starts[index];
	}

	EXEC_COUNT periodic_index = index -  infos.init_phases;
	EXEC_COUNT iteration = periodic_index / (infos.starts.size() -  infos.init_phases);
	EXEC_COUNT phase     = periodic_index % (infos.starts.size() -  infos.init_phases);
	TIME_UNIT res = infos.starts[ infos.init_phases + phase ] + iteration * infos.period;
	{
		VERBOSE_DEBUG("get_next_execution_time: result: " << res);
		return res;
	}
}

struct task_execution_t {
	ARRAY_INDEX task_id;
	EXEC_COUNT  task_iteration;
	TIME_UNIT   starting_time;
	task_execution_t(ARRAY_INDEX task_id,
	EXEC_COUNT  task_iteration,
	TIME_UNIT   starting_time) : task_id (task_id) , task_iteration (task_iteration), starting_time (starting_time) {}
};


inline bool operator==(const task_execution_t& lhs, const task_execution_t& rhs){ return lhs.task_id == rhs.task_id ; }


inline std::ostream& operator<< (std::ostream &out,const task_execution_t& e)		{out << "(id:" << e.task_id << ", it:" << e.task_iteration << ", start:" << e.starting_time << ")"; return out;}

bool algorithms::transformation::mergeCSDFFromSchedule (models::Dataflow* to, std::string name, const std::vector< ARRAY_INDEX >& mergeNodes, const models::Scheduling* scheduling_res) {

	VERBOSE_INFO("Start mergeCSDFFromSchedule");
	VERBOSE_ASSERT(computeRepetitionVector(to),"Merge operator requires graph to be consistent.");

	// Check tasks
	// we assume there is no init phases to merge
	for (auto vid : mergeNodes) {
		Vertex v = to->getVertexById(vid);
		VERBOSE_ASSERT(to->getInitPhasesQuantity(v) == 0, "Unsupported case");
	}

	// Check the scheduling is correct
	VERBOSE_INFO("Check Scheduling");
	TIME_UNIT omega = scheduling_res->getGraphPeriod();
	VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule, thus I cannot merge anymore.");
	VERBOSE_INFO("omega = " << omega);
	auto persched = scheduling_res->getTaskSchedule();

	// For each edge we need to store , input vector, output vector, initial marking, gcd
	VERBOSE_INFO("Store what to merge");

	EXEC_COUNT gcd_value = 0;
	std::map<ARRAY_INDEX, vertex_infos> original_tasks;
	std::map<ARRAY_INDEX, std::vector<edge_infos>> input_edges;
	std::map<ARRAY_INDEX,  std::vector<edge_infos>> output_edges;


	for (auto vid : mergeNodes) {
		Vertex v = to->getVertexById(vid);

		EXEC_COUNT ni = to->getNi(v);
		gcd_value = std::gcd (gcd_value , ni) ;

		for (auto it : to->in_edges(v)) {
			edge_infos ne(to, *it);
			input_edges[vid].push_back(ne);
		}

		for (auto it : to->out_edges(v)) {
			edge_infos ne(to, *it);
			output_edges[vid].push_back(ne);
		}

		original_tasks.insert({vid,vertex_infos(to,persched[vid].periodic_starts,v)});
	}


	VERBOSE_DEBUG("Check what is stored");
	VERBOSE_DEBUG("gcd_value = " << gcd_value);
	for (auto it : original_tasks) {
		ARRAY_INDEX vid = it.first;
		vertex_infos infos = it.second;
		VERBOSE_DEBUG("  - Scheduling of task " << vid);
		VERBOSE_DEBUG("  - Ni:" << infos.ni);
		VERBOSE_DEBUG("  - period: " << commons::toString(infos.period));
		VERBOSE_DEBUG("  - Init starts: " << commons::toString(std::vector<TIME_UNIT>(infos.starts.begin(), infos.starts.begin() + infos.init_phases)));
		VERBOSE_DEBUG("  - periodic starts: " << commons::toString(std::vector<TIME_UNIT>(infos.starts.begin() + infos.init_phases, infos.starts.end())));
	}

	VERBOSE_INFO("Delete original tasks");

	to->reset_computation();
	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex cfgVtx = to->getVertexById(vid);
		to->removeVertex(cfgVtx);
	}

	VERBOSE_INFO("Schedule original tasks");

	std::map<ARRAY_INDEX,EXEC_COUNT> execution_count;
	for (auto it : original_tasks) {
		execution_count[it.first] = 0;
	}


	std::vector < task_execution_t > execution_sequence;

	bool dontstop = true;
	while(dontstop) {
		dontstop = false;

		// Pick the next candidate
		ARRAY_INDEX next_id   = -1;
		TIME_UNIT   next_time = std::numeric_limits<TIME_UNIT>::infinity() ;
		for (auto it : original_tasks) {
			ARRAY_INDEX vid = it.first;
			vertex_infos infos = it.second;
			TIME_UNIT task_execution_time = get_next_execution_time (infos, execution_count[vid] );
			VERBOSE_DEBUG("  - Scheduling of task " << vid << " execution count = " << execution_count[vid] << " next execution is " << task_execution_time);
			if (task_execution_time <= next_time) {
					next_time = task_execution_time;
					next_id = vid;
					VERBOSE_DEBUG("  - New next task!");
			}
		}

		// Execute candidate
		execution_sequence.push_back(  task_execution_t(next_id, execution_count[next_id], next_time) );
		execution_count[next_id] ++ ;

		// Stopping condition
		// NOTE: In theory a valid one should be similar to Symbolic Execution (reach previously seen state)
		//       I did not implement that yet, it will have to be done until a better solution emerge.
		//       Currently the solution used is not bullet proof!

		for (auto it : original_tasks) {
			ARRAY_INDEX vid = it.first;
			vertex_infos infos = it.second;
			EXEC_COUNT threshold = infos.init_phases + (infos.periodic_phases * infos.ni);
			if (execution_count[vid] < threshold) {
				dontstop = true;
				VERBOSE_DEBUG("  - We will continue because " << vid << " needs " << threshold << " and only has " << execution_count[vid]);
				break;
			}
		}
	}

	VERBOSE_DEBUG("End of scheduling with " << commons::toString(execution_sequence));

	for (auto it : original_tasks) {
		ARRAY_INDEX vid = it.first;
		vertex_infos infos = it.second;
		VERBOSE_DEBUG("  - Scheduling of task " << vid);
		VERBOSE_DEBUG("  - Ni:" << infos.ni);
		VERBOSE_DEBUG("  - Init phases:" << infos.init_phases);
		VERBOSE_DEBUG("  - periodic phases:" << infos.periodic_phases);
		VERBOSE_DEBUG("  - period: " << commons::toString(infos.period));
		VERBOSE_DEBUG("  - Init starts: " << commons::toString(std::vector<TIME_UNIT>(infos.starts.begin(), infos.starts.begin() + infos.init_phases)));
		VERBOSE_DEBUG("  - periodic starts: " << commons::toString(std::vector<TIME_UNIT>(infos.starts.begin() + infos.init_phases, infos.starts.end())));
	}


	VERBOSE_INFO("Check there is a valid init phase count");

	VERBOSE_ASSERT(execution_sequence.size()  > 0, "This should never happen");

	ARRAY_INDEX init_phase_count = execution_sequence.size() - 1 ;
	bool need_to_continue = true;
	while (init_phase_count >= 0 and need_to_continue) {
		VERBOSE_DEBUG("Check init phase count = " << init_phase_count);

		need_to_continue = false;

		std::map<ARRAY_INDEX,EXEC_COUNT> task_periodic_execution_count;
		for (auto exec_pair : std::vector< task_execution_t > (execution_sequence.begin() + init_phase_count , execution_sequence.end() ) ) {
			task_periodic_execution_count[exec_pair.task_id] ++ ;
		}

		for (auto it : original_tasks) {
			ARRAY_INDEX tid = it.first;
			vertex_infos infos = it.second;
			EXEC_COUNT ratio =   (infos.ni)/gcd_value;
			VERBOSE_DEBUG("    In the current periodic pattern the task " << tid << " is executed " << task_periodic_execution_count[tid] << " with expected ratio " << ratio );

		}

		EXEC_COUNT factor = 0;
		for (auto it : original_tasks) {
			ARRAY_INDEX tid = it.first;
			vertex_infos infos = it.second;
			EXEC_COUNT ratio =   (infos.ni)/gcd_value;
			if (task_periodic_execution_count[tid] == 0) {
				need_to_continue = true;
				VERBOSE_DEBUG("  Every task need to be executed, task " << tid  << " is not.");

			}
			if (task_periodic_execution_count[tid] %  ratio  != 0) {
				need_to_continue = true;
				VERBOSE_DEBUG("  In the current periodic pattern the task " << tid << " is executed " << task_periodic_execution_count[tid] << " not divisible by ratio " << ratio );

			}

			if (factor == 0) {
				factor = task_periodic_execution_count[tid] / ratio;
			}
			if (task_periodic_execution_count[tid] !=  ratio * factor) {
							need_to_continue = true;
							VERBOSE_DEBUG("  In the current periodic pattern the task " << tid << " is executed " << task_periodic_execution_count[tid] << " not equal to expected " << factor * ratio );

			}

		}
		if (need_to_continue) {
			init_phase_count--;
		}
	}

	VERBOSE_ASSERT(init_phase_count >= 0 , "Problem detected.");

	// Optimising when we can to reduce the init phase
	VERBOSE_INFO("Check if we can reduce the inint phase");

	std::vector<task_execution_t> periodic_sequence (execution_sequence.begin() + init_phase_count , execution_sequence.end() );
	std::vector<task_execution_t> init_sequence (execution_sequence.begin() , execution_sequence.begin() + init_phase_count  );

	EXEC_COUNT new_init_end;
	for (ARRAY_INDEX init_end = init_phase_count ;init_end >= 0; init_end -= periodic_sequence.size()) {
		VERBOSE_DEBUG("test new init_end = " << init_end);
		std::vector<task_execution_t> maybe_periodic_sequence (execution_sequence.begin() + init_end,  execution_sequence.begin() + init_end + periodic_sequence.size()  );

		VERBOSE_DEBUG("maybe_periodic_sequence = " << commons::toString(maybe_periodic_sequence));
		VERBOSE_DEBUG("periodic_sequence = " << commons::toString(periodic_sequence));
		VERBOSE_DEBUG("Test result  = " << (maybe_periodic_sequence == periodic_sequence));

		if (maybe_periodic_sequence == periodic_sequence) {
			VERBOSE_DEBUG("WORKED");
			new_init_end = init_end;

		} else {
			VERBOSE_DEBUG("FAILED");
		}
	}

	init_phase_count = new_init_end;
	execution_sequence = std::vector<task_execution_t>(execution_sequence.begin() , execution_sequence.begin() + init_phase_count + periodic_sequence.size()) ;

	ARRAY_INDEX periodic_phase_count = execution_sequence.size() - init_phase_count;


	VERBOSE_INFO("Optimised scheduling with " << commons::toString(execution_sequence));



	VERBOSE_INFO("periodic_phase_count = " << periodic_phase_count);
	VERBOSE_INFO("init_phase_count = " << init_phase_count);

	VERBOSE_INFO("Prepare rates");
	std::map<ARRAY_INDEX, std::vector<TOKEN_UNIT>> previous_input_edge_rates;
	std::map<ARRAY_INDEX, std::vector<TOKEN_UNIT>> previous_output_edge_rates;

	std::map<ARRAY_INDEX, std::vector<TOKEN_UNIT>> new_input_edge_rates;
	std::map<ARRAY_INDEX, std::vector<TOKEN_UNIT>> new_output_edge_rates;

	VERBOSE_INFO("Produce the input edge rates");
	for (auto it : input_edges) {
		for (edge_infos infos : it.second) {
			std::vector<TOKEN_UNIT> previous_rates = produce_sequence (infos.out_ini_vector, infos.out_per_vector, execution_sequence.size()) ;
			std::vector<TOKEN_UNIT> new_rates (execution_sequence.size(), 0);
			ARRAY_INDEX task_to_match = infos.dst_id;
			VERBOSE_DEBUG("  - Previous Edge " << infos.id);
			VERBOSE_DEBUG("  - Task to consider " << task_to_match);
			VERBOSE_DEBUG("  - previous_input_edge_rates " << commons::toString(previous_rates));
			VERBOSE_DEBUG("  - new_input_edge_rates (empty) " << commons::toString(new_rates));
			VERBOSE_DEBUG("  - execution_sequence " << commons::toString(execution_sequence));


			for (size_t idx = 0 ; idx < execution_sequence.size() ; idx++ ) {
				if (execution_sequence[idx].task_id == task_to_match) {
					new_rates[idx] = previous_rates[execution_sequence[idx].task_iteration];
				}
			}

			VERBOSE_DEBUG("  - new_input_edge_rates " << commons::toString(new_rates));
			new_input_edge_rates[infos.id] = new_rates;

		}
	}

	VERBOSE_INFO("Produce the output edge rates");
	for (auto it : output_edges) {
		for (edge_infos infos : it.second) {
			std::vector<TOKEN_UNIT> previous_rates = produce_sequence (infos.in_ini_vector, infos.in_per_vector, execution_sequence.size()) ;
			std::vector<TOKEN_UNIT> new_rates (execution_sequence.size(), 0);
			ARRAY_INDEX task_to_match = infos.src_id;
			VERBOSE_DEBUG("  - Previous Edge " << infos.id);
			VERBOSE_DEBUG("  - Task to consider " << task_to_match);
			VERBOSE_DEBUG("  - previous_input_edge_rates " << commons::toString(previous_rates));
			VERBOSE_DEBUG("  - new_input_edge_rates (empty) " << commons::toString(new_rates));
			VERBOSE_DEBUG("  - execution_sequence " << commons::toString(execution_sequence));


			for (size_t idx = 0 ; idx < execution_sequence.size() ; idx++ ) {
				if (execution_sequence[idx].task_id == task_to_match) {
					new_rates[idx] = previous_rates[execution_sequence[idx].task_iteration];
				}
			}

			VERBOSE_DEBUG("  - new_input_edge_rates " << commons::toString(new_rates));
			new_output_edge_rates[infos.id] = new_rates;

		}
	}

	std::vector<TIME_UNIT> init_duration_vec;
	std::vector<TIME_UNIT> periodic_duration_vec;

	VERBOSE_INFO("  - Gather init timings and phases ");

	for (auto exec_pair : execution_sequence) {

		ARRAY_INDEX tid  = exec_pair.task_id;
		EXEC_COUNT  phase = exec_pair.task_iteration;
		vertex_infos infos = original_tasks.at(tid);

		TIME_UNIT execution_time = (phase < infos.durations.size()) ? infos.durations[phase]: infos.durations[infos.init_phases +  ((phase - infos.init_phases) % infos.periodic_phases)];

		VERBOSE_INFO("     - Execution of task "  << tid << ", phase=" << phase << " duration=" << execution_time << " taken from " << commons::toString(infos.durations));

		if (init_duration_vec.size() < init_phase_count) {
			init_duration_vec.push_back(execution_time);
		} else {
			periodic_duration_vec.push_back(execution_time);
		}
	}

	VERBOSE_INFO("Add the merge task");

	auto middle = to->addVertex();
	to->setVertexName(middle, name);
	to->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.
	to->setInitPhasesQuantity(middle, init_phase_count);
	to->setPhasesQuantity(middle, periodic_phase_count);
	to->setVertexDuration(middle,periodic_duration_vec);
	to->setVertexInitDuration(middle,  init_duration_vec );


	VERBOSE_INFO("Add the input edges");

	for (auto task_it : input_edges ) {

		ARRAY_INDEX old_tid = task_it.first;

		VERBOSE_DEBUG("  - old_tid " << old_tid);

		for (auto infos : task_it.second ) {

			VERBOSE_DEBUG("   - input edge from " << infos.src_id
					<< " with  infos.out_ini_vector = " << commons::toString(infos.out_ini_vector)
			<< " and  infos.out_per_vector = " << commons::toString(infos.out_per_vector) );

			if ( original_tasks.count(infos.src_id)  and original_tasks.count(infos.dst_id) ) {
				VERBOSE_DEBUG("  skip internal edge");
				continue;
			}

			auto new_edge = to->addEdge(to->getVertexById(infos.src_id), middle, infos.id , infos.name);
			//to->setEdgeName(new_edge, "merged_" + commons::toString(to->getEdgeId(new_edge)));
			to->setEdgeType(new_edge,infos.type);
			to->setPreload(new_edge, infos.preload);
			to->setEdgeInPhases(new_edge, infos.in_per_vector);
			to->setEdgeInInitPhases(new_edge, infos.in_ini_vector);

			VERBOSE_ASSERT(new_input_edge_rates.count(infos.id), "The output rates for input edge " << infos.id << " not defined.");
			std::vector<TOKEN_UNIT> computed_rates = new_input_edge_rates[infos.id];
			std::vector<TOKEN_UNIT> init_out (computed_rates.begin(), computed_rates.begin() + init_phase_count) ;
			std::vector<TOKEN_UNIT> per_out (computed_rates.begin() + init_phase_count, computed_rates.end()) ;


			VERBOSE_DEBUG("    - computed_rates = " << commons::toString(computed_rates));
			VERBOSE_DEBUG("    - init_out = " << commons::toString(init_out));
			VERBOSE_DEBUG("    - per_out = " << commons::toString(per_out));

			to->setEdgeOutPhases(new_edge, per_out);
			to->setEdgeOutInitPhases(new_edge, init_out);
		}

	}

	VERBOSE_INFO("Add the output edges");

	for (auto task_it : output_edges ) {

		ARRAY_INDEX old_tid = task_it.first;

		VERBOSE_DEBUG("  - old_tid " << old_tid);

		for (auto infos : task_it.second ) {

			VERBOSE_DEBUG("   - output edge to " << infos.dst_id
					<< " with  infos.in_ini_vector = " << commons::toString( infos.in_ini_vector)
			<< " and  infos.in_per_vector = " << commons::toString( infos.in_per_vector ));

			if ( original_tasks.count(infos.src_id)  and original_tasks.count(infos.dst_id) ) {
					VERBOSE_DEBUG("  skip internal edge");
					continue;
				}

			auto new_edge = to->addEdge( middle, to->getVertexById(infos.dst_id), infos.id , infos.name);
			//to->setEdgeName(new_edge, "merged_" + commons::toString(to->getEdgeId(new_edge)));
			to->setEdgeType(new_edge,infos.type);
			to->setPreload(new_edge, infos.preload);

			to->setEdgeOutPhases(new_edge, infos.out_per_vector);
			to->setEdgeOutInitPhases(new_edge, infos.out_ini_vector);


			VERBOSE_ASSERT(new_output_edge_rates.count(infos.id), "The input rates for output edge " << infos.id << " not defined.");
			std::vector<TOKEN_UNIT> computed_rates = new_output_edge_rates[infos.id];
			std::vector<TOKEN_UNIT> init_in (computed_rates.begin(), computed_rates.begin() + init_phase_count) ;
			std::vector<TOKEN_UNIT> per_in (computed_rates.begin() + init_phase_count, computed_rates.end()) ;

			VERBOSE_DEBUG("    - computed_rates = " << commons::toString(computed_rates));
			VERBOSE_DEBUG("    - init_in = " << commons::toString(init_in));
			VERBOSE_DEBUG("    - per_in = " << commons::toString(per_in));

			to->setEdgeInPhases(new_edge, per_in);
			to->setEdgeInInitPhases(new_edge, init_in);
		}

	}
	to->reset_computation();
	return true;
}


void algorithms::transformation::merge_tasks    (models::Dataflow* const dataflow, parameters_list_t  parameters  ) {

	// Check arguments

	if (parameters.find("tasks") == parameters.end() ) {
		VERBOSE_ERROR("Please specify the 'tasks' argument.");
		return;
	}

	if (parameters.find("name") == parameters.end() ) {
		VERBOSE_ERROR("New vertex 'name' argument has not been specified.");
		return;
	}

	// Prepare parameters
	std::string new_name = parameters["name"];
	std::vector<ARRAY_INDEX> task_ids = commons::split<ARRAY_INDEX>(parameters["tasks"], ',');


	VERBOSE_ASSERT(algorithms::transformation::mergeCSDFFromKperiodicSchedule(dataflow,new_name, task_ids), "mergeCSDFFromKperiodicSchedule return error.");
}
