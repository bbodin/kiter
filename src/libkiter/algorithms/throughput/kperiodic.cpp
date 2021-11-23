/*
 * kperiodic.cpp
 *
 *  Created on: 9 mai 2013
 *      Author: toky
 */


#ifdef __RELEASE_MODE__
#define STRONGLY_OPTIMIZED
#endif

#include <chrono>
#include <map>
#include <vector>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
#include "kperiodic.h"
#include <algorithms/schedulings.h>
#include <algorithm>



bool algorithms::sameset(const models::Dataflow* const dataflow, critical_circuit_t *cc1 , critical_circuit_t* cc2) {

	VERBOSE_DEBUG_ASSERT(cc1,"cc1 is not valid");
	VERBOSE_DEBUG_ASSERT(cc2,"cc2 is not valid");

	if (cc1->size() != cc2->size()) return false;

	bool result = false;

	for (critical_circuit_t::iterator it1 = cc1->begin() ; it1 != cc1->end() ; it1++ ) {
		result = false;
		for (critical_circuit_t::iterator it2 = cc2->begin() ; it2 != cc2->end() ; it2++ ) {
			if (dataflow->getEdgeId(*it1) == dataflow->getEdgeId(*it2)) result = true;
		}
		if (result == false) return false;
	}
	return true;
}


std::string algorithms::cc2string  (const models::Dataflow* const dataflow,critical_circuit_t* cc) {
	std::ostringstream returnStream;
	for (critical_circuit_t::iterator it = cc->begin() ; it != cc->end() ; it++ ) {
		returnStream <<  dataflow->getEdgeName(*it) <<
				" (" << dataflow->getVertexName(dataflow->getEdgeSource(*it)) << "->"
				<< dataflow->getVertexName(dataflow->getEdgeTarget(*it)) << "), ";
	}
	return returnStream.str();

}

std::string algorithms::print_schedule (models::EventGraph* eg, models::Dataflow* const dataflow, const periodicity_vector_t & kvector, TIME_UNIT res) {
	std::ostringstream returnStream;

	TIME_UNIT omega = 1 / res ;
	eg->computeStartingTimeWithOmega (omega);

	EXEC_COUNT TOTAL_REPEAT = 2;

	TIME_UNIT maxtime = 0;

	std::vector<std::pair<EXEC_COUNT, TIME_UNIT>> ordered_edges;

	{ForEachEvent(eg,e) {
			models::SchedulingEvent se = eg->getEvent(e);
			EXEC_COUNT ti = se.getTaskId();
			EXEC_COUNT tp = se.getTaskPhase();
			TIME_UNIT start = eg->getStartingTime(e);
			Vertex v = dataflow->getVertexById(ti);
			TIME_UNIT duration = dataflow->getVertexDuration(v,tp);
			TIME_UNIT period = kvector.at(v) *  dataflow->getPhasesQuantity(v) * omega / dataflow->getNi(v);
			EXEC_COUNT repeat = dataflow->getNi(v) * TOTAL_REPEAT;
			maxtime = std::max ( maxtime, start + period * repeat);
			ordered_edges.push_back(std::pair<EXEC_COUNT, TIME_UNIT>(ti,start));
	}}

	std::sort(ordered_edges.begin(), ordered_edges.end(), [=](std::pair<EXEC_COUNT, TIME_UNIT>& a, std::pair<EXEC_COUNT, TIME_UNIT>& b){
		return (a.second < b.second) or (not (a.second > b.second) and (a.first < b.first));

	});

	std::map<EXEC_COUNT,EXEC_COUNT> new_task_ids;
	for (std::pair<EXEC_COUNT, TIME_UNIT> tpair : ordered_edges) {
		auto ti = tpair.first;
		if (new_task_ids.find(ti) == new_task_ids.end()){
			new_task_ids[ti] = new_task_ids.size();
		}
	}

	TIME_UNIT scale = maxtime / 7.0;
	returnStream << "\\tikzset{" << std::endl;
	returnStream << "    anyblock/.style={blank, rectangle,draw=black,  text centered}," << std::endl;
	returnStream << "    initblock/.style={anyblock,top color=blue!70, bottom color=blue!30, thick}," << std::endl;
	returnStream << "    repeatblock/.style={anyblock,top color=blue!50, bottom color=blue!00, thick}," << std::endl;
	returnStream << "} " << std::endl;

	returnStream << "\\begin{scheduling}{" << dataflow->getVerticesCount() <<  "}{" << 0 <<  "}{" << scale <<  "}{" << maxtime <<  "}" << std::endl;

	{ForEachVertex(dataflow,v) {
		EXEC_COUNT tid = dataflow->getVertexId(v);
		std::string latexName = dataflow->getVertexName(v);
		std::replace( latexName.begin(), latexName.end(), '_', '-');
		if (new_task_ids.count(tid)) {
			returnStream << "\\taskname{"  << new_task_ids[tid] <<  "}{"  <<latexName <<  "}"
				<< "% ki=" << kvector.at(v)   << " Ni=" << dataflow->getNi(v)  << std::endl;
		}
	}}

	returnStream << "% \\addexecution[initblock]{index}{Name}{duration }{start}" << std::endl;
	returnStream << "% \\addperiodictask[repeatblock]{index}{Name}{duration }{start}{period}{repeat}" << std::endl;

	{ForEachEvent(eg,e) {
		models::SchedulingEvent se = eg->getEvent(e);
		EXEC_COUNT tid = se.getTaskId();
		EXEC_COUNT tp = se.getTaskPhase();
		TIME_UNIT start = eg->getStartingTime(e);
		Vertex v = dataflow->getVertexById(tid);
		TIME_UNIT duration = dataflow->getVertexDuration(v,tp);
		TIME_UNIT period = kvector.at(v) *  dataflow->getPhasesQuantity(v) * omega / dataflow->getNi(v);
		EXEC_COUNT repeat = (EXEC_COUNT) std::floor ( (maxtime - start - period) / period ) ;

		if (new_task_ids.count(tid)) {

			returnStream << "\\addexecution[initblock]{"  << new_task_ids[tid] <<  "}"
					<< "{}"
					<< "{"  << duration <<  "}"
					<< "{"  << start    <<  "}"
					<< " % " << kvector.at(v)  << " *  " << dataflow->getPhasesQuantity(v) <<  " * " <<  omega <<  "/" <<  dataflow->getNi(v)  << std::endl;

			returnStream << "\\addperiodictask[repeatblock]{"  << new_task_ids[tid] <<  "}"
					<< "{}"
					<< "{"  << duration <<  "}"
					<< "{"  << start + period    <<  "}"
					<< "{"  << period   <<  "}"
					<< "{"  << repeat - 1  <<  "}"
					<< " % " << kvector.at(v)  << " *  " << dataflow->getPhasesQuantity(v) <<  " * " <<  omega <<  "/" <<  dataflow->getNi(v)  << std::endl;


		}
	}}

	returnStream << "\\end{scheduling}"  << std::endl;
	return returnStream.str();
}


void print_function (models::Dataflow* const dataflow, const periodicity_vector_t & kvector, TIME_UNIT res, bool printXML = false, bool printTikz = false, bool printSchedule = false){
	//STEP 1 - Generate Event Graph
	models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);

	if (printXML)      std::cout << eg->printXML();
	if (printTikz)     std::cout << eg->printTikz();
	if (printSchedule) std::cout << algorithms::print_schedule(eg,dataflow,kvector,res);

}


models::Scheduling period2Scheduling (const models::Dataflow* const  dataflow, const periodicity_vector_t & kvector , kperiodic_result_t & result){

	TIME_UNIT omega = 1.0 / result.throughput ;
	VERBOSE_INFO("models::Scheduling period2Scheduling   omega = "  << std::setprecision( 9 )  << omega  );
	scheduling_t scheduling_result = period2scheduling    (dataflow, kvector ,  result.throughput) ;
	return models::Scheduling(dataflow, omega, scheduling_result, result.critical_edges);
}

scheduling_t period2scheduling (const models::Dataflow* const dataflow,  const periodicity_vector_t & kvector, TIME_UNIT throughput){

	VERBOSE_INFO("scheduling_t period2scheduling throughput = " << std::setprecision( 9 ) << throughput  );

	scheduling_t scheduling_result;

	VERBOSE_INFO("Build EventGraph"  );
	models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);

	VERBOSE_INFO("Compute starts "  );
	TIME_UNIT omega = 1 / throughput ;

	if (omega <= 0) return scheduling_result;

	eg->computeStartingTimeWithOmega (omega);

	VERBOSE_INFO("Retrieve starts "  );
	for (auto v : dataflow->vertices()) {
		ARRAY_INDEX tid = dataflow->getVertexId(v);
		EXEC_COUNT pq = dataflow->getPhasesQuantity(v);
		EXEC_COUNT ipq = dataflow->getInitPhasesQuantity(v);
		auto maxki = kvector.at(v);

		auto first_pi = 1 - ipq;

		TIME_UNIT period = (kvector.at(v) *  dataflow->getPhasesQuantity(v) * omega) / dataflow->getNi(v);
		scheduling_result[dataflow->getVertexId(v)].periodic_starts.first = period;

		for (EXEC_COUNT ki = 1  ; ki <= maxki ; ki++) {
			for (EXEC_COUNT pi = first_pi ; pi <=pq ; pi++)  {
				auto se = eg->getEventGraphVertex(tid,pi,ki);

				TIME_UNIT start = eg->getStartingTime(se);
				scheduling_result[tid].periodic_starts.second.push_back( start );
				VERBOSE_KPERIODIC_DEBUG("  - Start ki=" << ki << " pi=" << pi << " se=" << se << ":" << start  );

			}
		}
		VERBOSE_DEBUG("Task " << dataflow->getVertexName(v) << " Starts=" <<  commons::toString(scheduling_result[tid].periodic_starts.second) << " Period = " << period  );


	}

	VERBOSE_INFO("period2scheduling done"  );

	return scheduling_result ;
}

void algorithms::print_kperiodic_scheduling (models::Dataflow* const  dataflow, parameters_list_t param_list){

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

	VERBOSE_INFO("Please note you can specify the values of K (like -pA=1 -pB=2).");
	VERBOSE_INFO("You can also output the XML with -pXML=1, the dependency graph wth -pDEP=1, and the scheduling with -pSCHED=1.");
	// STEP 1 - generate 1-periodic schedule
	periodicity_vector_t kvector;
	{ForEachVertex(dataflow,v) {
		kvector[v] = 1;
		if (param_list.count(dataflow->getVertexName(v)) == 1) {
			std::string str_value = param_list[dataflow->getVertexName(v)];
			kvector[v] =  commons::fromString<EXEC_COUNT> ( str_value );
		}
	}}
	kperiodic_result_t result = KSchedule(dataflow,&kvector);
	VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.critical_edges)) <<  "");


	bool do_xml = param_list.count("XML");
	bool do_dep = param_list.count("DEP");
	bool do_sched = param_list.count("SCHED");

	print_function (dataflow, kvector , result.throughput , do_xml, do_dep, do_sched);
}

kperiodic_result_t algorithms::KSchedule(models::Dataflow* const dataflow, const periodicity_vector_t * kvector, TIME_UNIT bound){

	kperiodic_result_t result;

	// STEP 0.1 - PRE
	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	//VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");

	VERBOSE_INFO("KPeriodic EventGraph generation");

	//STEP 1 - Generate Event Graph
	models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,kvector);

	VERBOSE_INFO("KPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

	//STEP 2 - resolve the MCRP on this Event Graph
	std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = (bound > 0) ? eg->MinCycleRatio(bound) : eg->MinCycleRatio();

	std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

	//STEP 3 - convert CC(eg) => CC(graph)
	VERBOSE_KPERIODIC_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
	for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
		VERBOSE_KPERIODIC_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
		ARRAY_INDEX channel_id = eg->getChannelId(*it);
		try {
			Edge        channel    = dataflow->getEdgeById(channel_id);
			result.critical_edges.insert(channel);
		} catch(...) {
			VERBOSE_KPERIODIC_DEBUG("      is loopback");
		}
	}

	TIME_UNIT frequency = howard_res.first;

	// /* return the best omega found in sdf3 way */
	VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
	VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );
	//VERBOSE_INFO("  -> considering task " << dataflow->getVertexName(first) << ", mu_t = " << mui );
	//VERBOSE_INFO("  -> then sdf3 normalize frequency is " << thg );

	//result.first = thg;
	result.throughput = frequency;
	delete eg;

	return result;
}

kperiodic_result_t algorithms::KScheduleBufferLess(models::Dataflow *  const dataflow ,const periodicity_vector_t * kvector , TIME_UNIT bound )  {

	kperiodic_result_t result;

	// STEP 0.1 - PRE
	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	//VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");

	VERBOSE_INFO("KPeriodic EventGraph generation");

	//STEP 1 - Generate Event Graph
	models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,kvector);

	VERBOSE_INFO("KPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

	//STEP 2 - resolve the MCRP on this Event Graph
	std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = (bound > 0) ? eg->MinCycleRatio(bound) : eg->MinCycleRatio();

	std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

	//STEP 3 - convert CC(eg) => CC(graph)
	VERBOSE_KPERIODIC_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
	for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
		VERBOSE_KPERIODIC_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
		ARRAY_INDEX channel_id = eg->getChannelId(*it);
		try {
			Edge        channel    = dataflow->getEdgeById(channel_id);
			result.critical_edges.insert(channel);
		} catch(...) {
			VERBOSE_KPERIODIC_DEBUG("      is loopback");
		}
	}

	TIME_UNIT frequency = howard_res.first;

	// /* return the best omega found in sdf3 way */
	VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
	VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );
	//VERBOSE_INFO("  -> considering task " << dataflow->getVertexName(first) << ", mu_t = " << mui );
	//VERBOSE_INFO("  -> then sdf3 normalize frequency is " << thg );

	//result.first = thg;
	result.throughput = frequency;
	delete eg;

	return result;
}


models::EventGraph* algorithms::generateKPeriodicEventGraph(const models::Dataflow * const dataflow , const periodicity_vector_t * kValues ) {

	VERBOSE_KPERIODIC_DEBUG("generateKPeriodicEventGraph Starts ");
	VERBOSE_ASSERT((dataflow->has_repetition_vector()),"inconsistent graph or repetition vector not computed");

	models::EventGraph * g = new models::EventGraph();

	/* generate nodes */
	{ForEachVertex(dataflow,t) {
		const ARRAY_INDEX tid = dataflow->getVertexId(t);

		EXEC_COUNT init_phase_count = dataflow->getInitPhasesQuantity(t);
		EXEC_COUNT periodic_phase_count = dataflow->getPhasesQuantity(t);

		for (EXEC_COUNT i = 1; i <= init_phase_count ; i++ ) {
			g->addEvent(models::SchedulingEvent(tid,1-i,1));
		}

		for (EXEC_COUNT j = 1; j <= kValues->at(t) ; j++ ) {
			for (EXEC_COUNT i = 1; i <= periodic_phase_count ; i++ ) {
				g->addEvent(models::SchedulingEvent(tid,i,j));
			}
		}
	}}

	// DEFINITION DES REENTRANCES
	//******************************************************************

	{ForEachVertex(dataflow,pTask) {
		EXEC_COUNT start_count = kValues->at(pTask);
		generateKperiodicSelfloop(dataflow,start_count,g,pTask);
	}}

	// DEFINITION DES CONTRAINTES DE PRECEDENCES
	//******************************************************************
	{ForEachChannel(dataflow,c) {
		generateKPeriodicConstraint(dataflow, kValues, g, c);
	}}

	return g;

}


models::EventGraph* algorithms::updateEventGraph(const models::Dataflow * const dataflow, periodicity_vector_t * oldkvector, critical_circuit_t* cc, models::EventGraph* g) {

	VERBOSE_ASSERT(dataflow,"error");
	VERBOSE_ASSERT(oldkvector,"error");
	VERBOSE_ASSERT(g,"error");

	EXEC_COUNT addconstraint = 0;

	// Compute old and new vector

	VERBOSE_INFO("Update event graph - Step 0 - Compute new K and check changed");
	//VERBOSE_INFO("Update event graph -  Critical path = " << commons::toString(*cc) );

	periodicity_vector_t  kvector ;
	for ( auto it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
		kvector[it->first] = it->second;
	}

	// STEP 1 = GENERATE OLD KI AND NEW KI VECTORS == GENERATE addconstraint

	bool changed = updateVectorWithLocalNi(dataflow,&kvector,cc);

	VERBOSE_INFO("Update event graph -  New KVector = " << commons::toString(kvector) );
	if (!changed) {
		VERBOSE_INFO("Unchanged ...");
		return NULL;
	}


	VERBOSE_INFO("Update event graph - Step 1 - Delete edges and add task");
	// STEP 1
	//remove all connected edges
	EXEC_COUNT current = 0;
	for (critical_circuit_t::iterator it = cc->begin() ; it != cc->end(); it++ ) {
		current ++ ;

		Vertex source = dataflow->getEdgeSource(*it);
		EXEC_COUNT ki = oldkvector->at(source);
		EXEC_COUNT newki =  kvector.at(source);
		if (ki < newki) {
			ARRAY_INDEX task_id= dataflow->getVertexId(source);
			for (PHASE_INDEX p = 1 - dataflow->getInitPhasesQuantity(source) ; p <= dataflow->getPhasesQuantity(source) ; p++) {
				for(EXEC_COUNT k = 1 ; k <= ki ; k++) {
					g->removeConnectedEdges(g->getEventGraphVertex(task_id,p,k));
				}
			}

			for (PHASE_INDEX p = 1 - dataflow->getInitPhasesQuantity(source)  ; p <= dataflow->getPhasesQuantity(source) ; p++) {
				g->addEventGroup(task_id,p,ki+1,newki);
			}
			if (newki > dataflow->getNi(source)) {
				VERBOSE_ERROR("newki=" << newki << " Ni=" << dataflow->getNi(source));
			}
			VERBOSE_ASSERT(newki <= dataflow->getNi(source),"Bad idea !");
		}
	}

	VERBOSE_INFO("Update event graph - Step 2 - Reentrancy");

	current = 0;

	for (critical_circuit_t::iterator it = cc->begin(); it != cc->end(); it++ ) {

		current ++ ;
		Vertex t = dataflow->getEdgeSource(*it);

		const EXEC_COUNT oldki =  oldkvector->at(t);
		const EXEC_COUNT newki =     kvector.at(t);

		// SKIP IF KI NOT CHANGED (no loopback to create, because loopback not deleted)
		if (oldki == newki) continue;

		VERBOSE_KPERIODIC_DEBUG("generate reentrancy loops for task " <<  t << " with newki=" << newki);
		EXEC_COUNT start_count = kvector.at(t);
		generateKperiodicSelfloop(dataflow,start_count,g,t);
	}

	VERBOSE_INFO("Update event graph - Step 3 - add " << addconstraint << " constraints.");

	// STEP 3
	// add all edges

	current = 0;
	for (critical_circuit_t::iterator it = cc->begin() ; it != cc->end() ; it++) {

		current ++ ;
		critical_circuit_t::iterator it_previous = it;
		if (it == cc->begin())  {
			it_previous = cc->end();
		}
		it_previous--;
		Edge current = *it;
		Edge previous = *it_previous;
		Vertex source = dataflow->getEdgeSource(current);
		Vertex target = dataflow->getEdgeTarget(current);
		const EXEC_COUNT oldki =  oldkvector->at(source);
		const EXEC_COUNT newki =     kvector.at(source);
		const EXEC_COUNT oldkj =  oldkvector->at(target);
		const EXEC_COUNT newkj =     kvector.at(target);

		// SKIP IF KI NOT CHANGED (no loopback to create, because loopback not deleted
				if (oldki == newki) continue;

				{ForOutputEdges(dataflow,source,c) {
					if ((c != current) || (oldkj == newkj)) {
						generateKPeriodicConstraint(dataflow , &kvector,   g ,  c);
					}

				}}

				{ForInputEdges(dataflow,source,c) {
					if (c != previous) {
						generateKPeriodicConstraint(dataflow , &kvector,   g ,  c);
					} else {
						generateKPeriodicConstraint(dataflow , &kvector,   g ,  c); //speed up here only
					}
				}}
	}

	for ( auto it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
		oldkvector->at(it->first) = kvector.at(it->first);
	}

	return g;
}


void algorithms::generateKPeriodicConstraint(const models::Dataflow * const dataflow, const periodicity_vector_t * kValues,  models::EventGraph* g, Edge c) {

	VERBOSE_KPERIODIC_DEBUG("Constraint for " << dataflow->getEdgeName(c) );

	ARRAY_INDEX id = dataflow->getEdgeId(c);

	Vertex source = dataflow->getEdgeSource(c);
	Vertex target = dataflow->getEdgeTarget(c);

	std::string sourceName = dataflow->getVertexName(source);
	std::string targetName = dataflow->getVertexName(target);

	const ARRAY_INDEX source_id = dataflow->getVertexId(source);
	const ARRAY_INDEX target_id = dataflow->getVertexId(target);

	EXEC_COUNT source_phase_count = dataflow->getEdgeInPhasesCount(c);
	EXEC_COUNT target_phase_count = dataflow->getEdgeOutPhasesCount(c);


	EXEC_COUNT source_init_phase_count = dataflow->getEdgeInInitPhasesCount(c);
	EXEC_COUNT target_init_phase_count = dataflow->getEdgeOutInitPhasesCount(c);

	const EXEC_COUNT  maxki        = kValues->at(source) ;
	const EXEC_COUNT  maxkj        = kValues->at(target) ;

	TOKEN_UNIT mop     =  commons::floor(dataflow->getPreload(c),dataflow->getFineGCD(c));

	const TOKEN_UNIT  Wc        =  dataflow->getEdgeIn(c) * maxki;
	const TOKEN_UNIT  Rc        =  dataflow->getEdgeOut(c) * maxkj;

	const TOKEN_UNIT  gcdz       = std::gcd<TOKEN_UNIT>((Wc),(Rc));
	const TOKEN_UNIT  stepa      = dataflow->getFineGCD(c);

	TOKEN_UNIT normdapkm1 = 0;
	TOKEN_UNIT normdapk   = 0;

	bool doBufferLessEdges = dataflow->getEdgeType(c) == EDGE_TYPE::BUFFERLESS_EDGE;

	VERBOSE_KPERIODIC_DEBUG(" - source_init_phase_count = " << source_init_phase_count );
	VERBOSE_KPERIODIC_DEBUG(" - target_init_phase_count = " << target_init_phase_count );
	VERBOSE_KPERIODIC_DEBUG(" - maxki = " << maxki );
	for (EXEC_COUNT ki = 1; ki <= maxki ; ki++ ) {
		VERBOSE_KPERIODIC_DEBUG("  - ki = " << ki );
		auto first_pi = (ki == 1) ?  1 - source_init_phase_count : 1 ;
		VERBOSE_KPERIODIC_DEBUG("  - first_pi = " << first_pi );
		for (EXEC_COUNT pi = first_pi; pi <= source_phase_count ; pi++ ) {
			VERBOSE_KPERIODIC_DEBUG("   - pi = " << pi );

			TIME_UNIT  d =  dataflow->getVertexDuration(source, pi);
			TOKEN_UNIT normdamkp = 0;
			const TOKEN_UNIT wak        = dataflow->getEdgeInPhase(c,pi)   ;
			normdapk += wak;
			models::EventGraphVertex source_event = g->getEventGraphVertex(source_id,pi,ki);

			VERBOSE_KPERIODIC_DEBUG("   - maxkj = " << maxkj );
			for (EXEC_COUNT kj = 1; kj <= maxkj ; kj++ ) {
				VERBOSE_KPERIODIC_DEBUG("    - kj = " << kj );
				auto first_pj = (kj == 1) ?  1 - target_init_phase_count : 1 ;
				VERBOSE_KPERIODIC_DEBUG("    - first_pj = " << first_pi );
				for (EXEC_COUNT pj = first_pj; pj <= target_phase_count ; pj++ ) {
					VERBOSE_KPERIODIC_DEBUG("     - pj = " << pj );

					const TOKEN_UNIT vakp       = dataflow->getEdgeOutPhase(c,pj) ;
					normdamkp += vakp;

#ifdef NOT_OPTIMIZED
					const TOKEN_UNIT  alphamin = (TOKEN_UNIT)  commons::ceil(std::max((TOKEN_UNIT)0, wak - vakp) - mop - normdapk + normdamkp,gcdz);
					const TOKEN_UNIT alphamax  =  commons::floor( - mop - normdapkm1 + normdamkp - stepa ,gcdz);
#else

					const TOKEN_UNIT lpimax =    ( - mop - normdapkm1 + normdamkp - stepa) ;
					const TOKEN_UNIT rpimax =     lpimax % gcdz;
					const TOKEN_UNIT alphamax =   (rpimax >= 0) ? ( lpimax - rpimax ) : ( lpimax - rpimax - gcdz );
					const TOKEN_UNIT lpimin =    (std::max((TOKEN_UNIT)0, wak - vakp) - mop - normdapk + normdamkp) ;

#ifdef STRONGLY_OPTIMIZED
					if (lpimin > alphamax ) continue; // ca ne fera qu'empirer
#endif
					const TOKEN_UNIT rpimin =     lpimin % gcdz;
					const TOKEN_UNIT alphamin =   (rpimin <= 0) ? ( lpimin - rpimin ) : ( lpimin - rpimin + gcdz );
#endif

					models::EventGraphVertex target_event = g->getEventGraphVertex(target_id,pj,kj);
					VERBOSE_KPERIODIC_DEBUG("  stepa=" << stepa);
					VERBOSE_KPERIODIC_DEBUG("  ki=" << ki<<" kj=" << kj << " pi=" << pi<<" pj=" << pj << " (" <<  source_event  << "," << target_event  << ")");
					VERBOSE_KPERIODIC_DEBUG("  alphamin=" << alphamin <<"   alphamax=" << alphamax );

					VERBOSE_KPERIODIC_DEBUG("$S\\langle " << targetName << "_{" << pj << "}," << kj
							  << "\\rangle - S\\langle " << sourceName << "_{" << pi << "}," << ki
								<< "\\rangle \\geq d(" << sourceName << "_{" << pi << "}) + \\Omega^S_G \\times \\frac{" << alphamax << "\\times" << source_phase_count * maxki << "}{" << (TIME_UNIT) Wc * (TIME_UNIT) dataflow->getNi(source) << "}$ % alphamin = " << alphamin);



					if (alphamin <= alphamax) {


						TIME_UNIT w = ((TIME_UNIT) alphamax * source_phase_count * maxki ) / ( (TIME_UNIT) Wc  * (TIME_UNIT) dataflow->getNi(source) );
						VERBOSE_KPERIODIC_DEBUG("   ** w = (" << alphamax << " * " << dataflow->getPhasesQuantity(source) * maxki << ") / (" << Wc << " * " << dataflow->getNi(source) / maxki << ")");
						VERBOSE_KPERIODIC_DEBUG("   ** d = (" << d << ")");


						if (doBufferLessEdges) {
							g->addEventConstraint(source_event ,target_event,-w,d,id);
						} else {
							g->addEventConstraint(source_event ,target_event,-w,d,id);
						}
					} else {
						VERBOSE_KPERIODIC_DEBUG("   ** Skip that one" );
					}
				}
			}
			normdapkm1 = normdapk;
		}
	}
}


void algorithms::generateKperiodicSelfloop (const models::Dataflow * const dataflow, EXEC_COUNT ki,  models::EventGraph* g, Vertex t){

	const ARRAY_INDEX task_id = dataflow->getVertexId(t);

	const TIME_UNIT timefactor = (dataflow->getReentrancyFactor(t) <= 0)?0:1;

	VERBOSE_KPERIODIC_DEBUG("generate reentrancy loops for task " <<  dataflow->getVertexId(t) << " with ki=" << ki);

	EXEC_COUNT pq = dataflow->getPhasesQuantity(t);
	TIME_UNIT lti = timefactor * dataflow->getVertexDuration(t,1);
	EXEC_COUNT i = 1;
	EXEC_COUNT p = 1 - dataflow->getInitPhasesQuantity(t);

	models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,p,i);
	models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,p,i);

	while (i <= ki) {
		p += 1;
		if (p > pq) {p = 1; i += 1;}
		if (i > ki) continue;

		target_event = g->getEventGraphVertex(task_id,p,i);

		TIME_UNIT d =  lti;
		TIME_UNIT w =  0;
		g->addEventConstraint(source_event ,target_event,w,d,0);

		source_event = target_event;
		lti = timefactor * dataflow->getVertexDuration(t,p);
	}

	if (dataflow->getReentrancyFactor(t) > 0) // consider only a factor 1
	{

		// constraintes last_k --> 1

		EXEC_COUNT p = dataflow->getPhasesQuantity(t);
		EXEC_COUNT i = ki;

		EXEC_COUNT np = 1;
		EXEC_COUNT ni = 1;

		const TIME_UNIT lti = timefactor * dataflow->getVertexDuration(t,p);

		models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,p,i);
		models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,np,ni);


		const TIME_UNIT       d     =  lti;
		const TIME_UNIT       w     =   -  (TIME_UNIT) (p * i) / (  (TIME_UNIT) dataflow->getNi(t)  );


		VERBOSE_KPERIODIC_DEBUG("  - add task constraint :" << dataflow->getVertexName(t) << " : " << target_event  << " --> " << source_event << " : " <<  " (- " << w << " / " << dataflow->getNi(t)  << ") ," << d);
		g->addEventConstraint(source_event ,target_event,-w,d,0);
	}
}


bool algorithms::updateVectorWithLocalNi(const models::Dataflow *  const dataflow , periodicity_vector_t * kvector , critical_circuit_t * cc) {

	bool changed = false;

	EXEC_COUNT gcdNi = 0;
	for (critical_circuit_t::iterator it = cc->begin() ; it != cc->end(); it++ ) {
		Vertex source = dataflow->getEdgeSource(*it);
		gcdNi = std::gcd<EXEC_COUNT>(gcdNi,dataflow->getNi(source)  / dataflow->getPhasesQuantity(source) );
	}

	VERBOSE_INFO("      updateVectorWithLocalNi -  gcdNi = " << commons::toString(gcdNi) );

	for (critical_circuit_t::iterator it = cc->begin() ; it != cc->end(); it++ ) {
		Vertex source = dataflow->getEdgeSource(*it);
		Vertex target = dataflow->getEdgeTarget(*it);

		EXEC_COUNT Ni =  dataflow->getNi(source) / dataflow->getPhasesQuantity(source);
		EXEC_COUNT Nj =  dataflow->getNi(target) / dataflow->getPhasesQuantity(target);

		EXEC_COUNT newki = std::lcm<EXEC_COUNT>( kvector->at(source),  Ni / gcdNi);

		if (newki != kvector->at(source) ) changed = true;

		VERBOSE_INFO("      updateVectorWithLocalNi - "
				<< " Source = " << dataflow->getVertexName(source)
				<< " Ni = " << Ni
				<< " ki = "  <<  kvector->at(source)
				<< " newki = " << newki );

		kvector->at(source) = newki;
		VERBOSE_ASSERT(kvector->at(source) <= Ni , "Bad idea");
		VERBOSE_ASSERT(kvector->at(target) <= Nj , "Bad idea");
	}

	return changed;
}


void algorithms::compute_NKperiodic_throughput (models::Dataflow* const  dataflow, parameters_list_t) {

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

	// STEP 0.2 - Assert SDF
	periodicity_vector_t kvector;
	{ForEachVertex(dataflow,t) {
		kvector[t] = dataflow->getNi(t);
	}}

	kperiodic_result_t result = KSchedule(dataflow,&kvector);

	TIME_UNIT res = result.throughput;
	std::cout << "NPeriodic throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
	std::cout << "NPeriodic period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;

}


void algorithms::compute_2Kperiodic_throughput (models::Dataflow* const  dataflow, parameters_list_t) {

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

	// STEP 0.2 - Assert SDF
	periodicity_vector_t kvector;
	{ForEachVertex(dataflow,t) {
		kvector[t] = 2;
	}}

	kperiodic_result_t result = KSchedule(dataflow,&kvector);
	TIME_UNIT res = result.throughput;
	std::cout << "2Periodic throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
	std::cout << "2Periodic period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;

}


void algorithms::compute_1Kperiodic_throughput (models::Dataflow* const  dataflow, parameters_list_t param_list) {

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

	bool printRequired = (param_list.count("print") == 1);

	VERBOSE_INFO("Please note you cna use the print parameter.");

	// STEP 0.2 - Assert SDF
	periodicity_vector_t kvector = algorithms::scheduling::generate1PeriodicVector(dataflow);

	kperiodic_result_t result = KSchedule(dataflow,&kvector);

	if (printRequired) {
		print_function    ( dataflow, kvector , result.throughput , false,false,true);
	} else {
		TIME_UNIT res = result.throughput;
		std::cout << "1Periodic throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
		std::cout << "1Periodic period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;
	}
}





EXEC_COUNT sum_Ni (models::Dataflow* const dataflow) {
	EXEC_COUNT sumNi = 0;

	{ForEachTask(dataflow,t) {
		sumNi += dataflow->getNi(t) ;
	}}
	return sumNi;
}

EXEC_COUNT sum_Ki (models::EventGraph* eg) {
	return eg->getEventCount();
}

EXEC_COUNT sum_KiKj (models::EventGraph* eg) {
	return eg->getConstraintsCount();
}

EXEC_COUNT sum_NiNj (models::Dataflow* const dataflow) {
	EXEC_COUNT sumNiNj = 0;
	{ForEachChannel(dataflow,c) {
		Vertex in  = dataflow->getEdgeSource(c);
		Vertex out = dataflow->getEdgeTarget(c);

		sumNiNj += std::max(dataflow->getNi(in),dataflow->getNi(out));
	}}
	return sumNiNj;
}


void print_kiter_throughput_header () {
	std::cout << "filename;task_count;buffer_count;iteration;"
			     "gduration;hduration;duration;"
			     "sum_Ki;sum_KiKj;"
			     "sum_Ni;sum_NiNj;"
			     "th" << std::endl;

}

void print_kiter_throughput_iteration (models::Dataflow* const dataflow, models::EventGraph* eg, kperiodic_result_t & result, int iteration_count, double gduration, double hduration) {

	EXEC_COUNT sumNi = sum_Ni (dataflow);
	EXEC_COUNT sumNiNj = sum_NiNj (dataflow);

	EXEC_COUNT total_ki = sum_Ki(eg);
	EXEC_COUNT total_kikj = sum_KiKj(eg);

std::cout << dataflow->getFilename()
				<< ";"<< dataflow->getVerticesCount()
				<< ";"<< dataflow->getEdgesCount()
				<< ";" <<  iteration_count
				<< ";" << gduration
				<< ";" << hduration
				<< ";" << hduration + gduration
				<< ";" << total_ki
				<< ";" << total_kikj
				<< ";" << sumNi
				<< ";" <<  sumNiNj
						<< ";"  << std::setw( 11 ) << std::setprecision( 9 ) <<  result.throughput   << std::endl;
}

/**
 * This function is the main algorithm to compute the throughput of a CSDF
 * using iter as described in Bodin16.
 */
void algorithms::compute_Kperiodic_throughput (models::Dataflow* const dataflow, parameters_list_t params ) {

	auto very_start = std::chrono::steady_clock::now();
	auto start = std::chrono::steady_clock::now();
	bool showdetails = params.count("DETAILS") > 0 ;

	if (showdetails) {
		print_kiter_throughput_header () ;
	}

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

	// STEP 0.1 - PRE
	EXEC_COUNT iteration_count = 0;

	// STEP 1 - generate initial vector
	periodicity_vector_t kvector = algorithms::scheduling::generate1PeriodicVector(dataflow);

	kperiodic_result_t result;

	VERBOSE_INFO("KPeriodic EventGraph generation");
	VERBOSE_INFO("KVector = " << commons::toString(kvector) );

	//STEP 1 - Generate Event Graph
	models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector);

	auto graph_done = std::chrono::steady_clock::now();
	VERBOSE_ASSERT(start <= graph_done,"Impossible");
	VERBOSE_INFO("KPeriodic EventGraph generation Done");

	//STEP 2 - resolve the MCRP on this Event Graph
	std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
	VERBOSE_INFO("Kperiodic throughput = " << howard_res.first);

	std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

	//STEP 3 - convert CC(eg) => CC(graph)
	VERBOSE_KPERIODIC_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");

	for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
		VERBOSE_KPERIODIC_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
		ARRAY_INDEX channel_id = eg->getChannelId(*it);
		try {
			Edge        channel    = dataflow->getEdgeById(channel_id);
			result.critical_edges.insert(channel);
		} catch(...) {
			VERBOSE_KPERIODIC_DEBUG("      is loopback");
		}
	}

	TIME_UNIT frequency = howard_res.first;

	VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
	VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

	result.throughput = frequency;

	////////////// SCHEDULE CALL // END

	auto end_phase = std::chrono::steady_clock::now();

	if (showdetails) {
			double gduration = std::chrono::duration<double> (graph_done-start).count();
			double hduration = std::chrono::duration<double> (end_phase-graph_done).count();
			double  duration = std::chrono::duration<double> (end_phase-start).count();
			VERBOSE_ASSERT(start <= end_phase,"Error in the system");
			VERBOSE_ASSERT(start <= graph_done,"Error in the system");
			VERBOSE_ASSERT(graph_done <= end_phase,"Error in the system");
			print_kiter_throughput_iteration (dataflow, eg, result , iteration_count, gduration,  hduration ) ;
	}

	if (result.critical_edges.size() != 0) {

		VERBOSE_INFO("1-periodic throughput (" << result.throughput <<  ") is not enough.");
		VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.critical_edges)) <<  "");

		while (true) {

			start = std::chrono::steady_clock::now();

			iteration_count++;
			////////////// SCHEDULE CALL // BEGIN : resultprime = KSchedule(dataflow,&kvector);

			kperiodic_result_t resultprime;

			//VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");
			VERBOSE_INFO("KPeriodic EventGraph generation");

			//STEP 1 - Generate Event Graph and update vector
			if (!updateEventGraph( dataflow ,  &kvector, &(result.critical_edges), eg)) break ;

			VERBOSE_INFO("KVector = " << commons::toString(kvector) );
			VERBOSE_INFO("KPeriodic EventGraph generation Done");

			graph_done = std::chrono::steady_clock::now();
			VERBOSE_ASSERT(start <= graph_done,"Impossible");
			//STEP 2 - resolve the MCRP on this Event Graph
			std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res_bis = eg->MinCycleRatio();
			VERBOSE_INFO("Kperiodic throughput = " << howard_res_bis.first);

			std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res_bis.second);

			//STEP 3 - convert CC(eg) => CC(graph)
			VERBOSE_KPERIODIC_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
			for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
				VERBOSE_KPERIODIC_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
				ARRAY_INDEX channel_id = eg->getChannelId(*it);
				try {
					Edge        channel    = dataflow->getEdgeById(channel_id);
					resultprime.critical_edges.insert(channel);
				} catch(...) {
					VERBOSE_KPERIODIC_DEBUG("      is loopback");
				}
			}

			TIME_UNIT frequency = howard_res_bis.first;

			VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
			VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

			resultprime.throughput = frequency;

			////////////// SCHEDULE LOG

			end_phase = std::chrono::steady_clock::now();

			if (showdetails) {
						double gduration = std::chrono::duration<double> (graph_done-start).count();
						double hduration = std::chrono::duration<double> (end_phase-graph_done).count();
						double  duration = std::chrono::duration<double> (end_phase-start).count();
						VERBOSE_ASSERT(start <= end_phase,"Error in the system");
						VERBOSE_ASSERT(start <= graph_done,"Error in the system");
						VERBOSE_ASSERT(graph_done <= end_phase,"Error in the system");
						print_kiter_throughput_iteration (dataflow, eg, resultprime , iteration_count, gduration,  hduration ) ;
					}


			////////////// SCHEDULE CALL // END

			if (sameset(dataflow,&(resultprime.critical_edges),&(result.critical_edges)))  {
				VERBOSE_INFO("Critical circuit is the same");
				result = resultprime;

				break;
			}
			result = resultprime;
			VERBOSE_INFO("Current K-periodic throughput (" << result.throughput << ") is not enough.");
			VERBOSE_KPERIODIC_DEBUG("   Critical circuit is " << cc2string(dataflow,&(result.critical_edges)) <<  "");

		}

	} else {

	}

	VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count << "  final size is " << eg->getEventCount() << " events and " << eg->getConstraintsCount() << " constraints.");
	delete eg;

	EXEC_COUNT total_kiphit = 0;
	{ForEachVertex(dataflow,t) {
		total_kiphit += kvector[t] * dataflow->getPhasesQuantity(t);
	}}


	//VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << sumNi );

	TIME_UNIT res = result.throughput;


	if (VERBOSE_IS_INFO()) {
		scheduling_t persched = period2scheduling    (dataflow,  kvector , res);

		for (auto  key : persched) {
			auto task = key.first;
			auto task_vtx = dataflow->getVertexById(key.first);
			VERBOSE_INFO(  "Task " <<  dataflow->getVertexName(task_vtx)
					<<  " : duration=" << commons::toString(dataflow->getVertexInitPhaseDuration(task_vtx))
			<< ";" << commons::toString(dataflow->getVertexPhaseDuration(task_vtx))
			<<  " period=" <<  persched[task].periodic_starts.first
			<<  " Ni=" << dataflow->getNi(task_vtx)
			<<  " starts=[ " << commons::toString(persched[task].periodic_starts.second) << "]");

		}
	}

	if (showdetails) {

	} else {

		auto very_end = std::chrono::steady_clock::now();
		double  duration = std::chrono::duration<double> (very_end-very_start).count() * 1000;

		std::cout << "KPeriodic Throughput is "  << std::setw( 20 ) << std::setprecision( 9 ) <<     res    << std::endl;
		std::cout << "KPeriodic Period is " << std::fixed      << std::setw( 20 ) << std::setprecision( 6 ) << 1.0/res    << std::endl;
		std::cout << "KPeriodic Execution Time is " << std::fixed      << std::setw( 20 ) << std::setprecision( 6 ) << duration   << std::endl;
		std::cout << "KPeriodic Scheduling Size is " << std::fixed      << std::setw( 20 ) << std::setprecision( 6 ) << total_kiphit   << std::endl;
		}
}




