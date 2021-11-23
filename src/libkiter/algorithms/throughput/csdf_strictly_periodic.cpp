/*
 * csdf_strictly_periodic.cpp
 *
 *  Created on: Feb 22, 2021
 *      Author: toky
 */


#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <map>
#include "csdf_strictly_periodic.h"
#include <numeric>

models::EventGraph* algorithms::generate_csdf_strictly_periodic_event_graph (const models::Dataflow * const dataflow, bool use_max_duration) {
	
	models::EventGraph* eg = new models::EventGraph();

	std::map<ARRAY_INDEX,models::SchedulingEvent> tid2event;

	// we define only one start per task
	for (auto t : dataflow->vertices()){
		auto tid = dataflow->getVertexId(t);
		tid2event.insert({tid, models::SchedulingEvent(dataflow->getVertexId(t))});
		eg->addEvent(tid2event.at(tid));

		TIME_UNIT w = 1.0 /  dataflow->getNi(t) ; // We assume GetNi returns phit*qt
		auto exec_times = dataflow->getVertexPhaseDuration(t);
		TIME_UNIT d = *std::max_element(exec_times.begin(), exec_times.end());
		models::SchedulingEventConstraint sec (tid2event.at(tid), tid2event.at(tid),  w, d, tid);
		eg->addEventConstraint(sec);
	}

	for (auto t : dataflow->vertices()) {

		// `max_d` is only used if max_duration for approximation of exact Strictly Periodic
		auto exec_times = dataflow->getVertexPhaseDuration(t);
		TIME_UNIT max_d = *std::max_element(exec_times.begin(), exec_times.end());


		// for each task we look at different timings bags
		auto phit = dataflow->getPhasesQuantity(t);
		std::map <TIME_UNIT, std::vector<EXEC_COUNT>> delta_l_t;

		for (EXEC_COUNT k = 1 ; k <= phit ; k ++) {
			TIME_UNIT duration = dataflow->getVertexDuration(t, k);
			if (use_max_duration) { // approx
				delta_l_t[max_d].push_back(k);
			} else { // exact case
				delta_l_t[duration].push_back(k);
			}
		}

		EXEC_COUNT ni = dataflow->getNi(t) / phit;

		// for each timings bag, and for each output buffers we add a constraint
		for (auto item : delta_l_t) { // only one constraint required here

			std::vector<EXEC_COUNT> ks = item.second;

			for (auto _c : dataflow->out_edges(t)) {
				auto c = *_c;
				auto tp = dataflow->getEdgeTarget(c);
				auto phitp = dataflow->getPhasesQuantity(tp);
				auto tid = dataflow->getVertexId(t);
				auto tpid = dataflow->getVertexId(tp);

				VERBOSE_DEBUG ("Buffer from tid=" << tid << " tpid=" << tpid );

				auto d = item.first;
				TOKEN_UNIT ia = dataflow->getEdgeIn(c);
				TOKEN_UNIT oa = dataflow->getEdgeOut(c);

				const TOKEN_UNIT  stepa      = dataflow->getFineGCD(c);
				TOKEN_UNIT mop               =  commons::floor(dataflow->getPreload(c),stepa);
				const TOKEN_UNIT  gcdz       = std::gcd(ia,oa);

				TIME_UNIT w = std::numeric_limits<TIME_UNIT>::lowest();

				VERBOSE_ASSERT(ks.size(), "No ks");
				VERBOSE_ASSERT(phitp > 0, "No ks");

				EXEC_COUNT nj = dataflow->getNi(tp) / phitp;

				VERBOSE_DEBUG("*** NI=" << ni << " phit=" << phit << " ia=" << ia << " ia*ni=" << ia * ni);
				VERBOSE_DEBUG("*** NJ=" << nj << " phitp=" << phitp << " oa=" << oa << " oa*nj=" << oa * nj);

				VERBOSE_ASSERT (ia * ni == oa * nj, "Repetition vector doesnt match expected definition");


				for (auto k : ks) {
					for (auto kp = 1 ; kp <= phitp; kp++) {

						// alphaMin <= alphaMax test
						TOKEN_UNIT iapm1 = 0 ; for (int p = 1; p  < k ; p++ ) {iapm1 += dataflow->getEdgeInPhase(c, p);}
						TOKEN_UNIT oapp  = 0 ; for (int pp = 1; pp <= kp ; pp++ ) {oapp += dataflow->getEdgeOutPhase(c, pp);}
						TOKEN_UNIT inak =   dataflow->getEdgeInPhase(c, k);
						TOKEN_UNIT outakp =   dataflow->getEdgeOutPhase(c, kp);
						TOKEN_UNIT iap   = iapm1 +  inak;
						TOKEN_UNIT Ha_k_kp = std::max ( (TOKEN_UNIT) 0, inak - outakp);
						TOKEN_UNIT alphamin_k_kp = commons::ceil( Ha_k_kp + oapp - iap - mop  ,gcdz);
						TOKEN_UNIT alphamax_k_kp = commons::floor( oapp - iapm1 - mop - 1 ,gcdz);

						auto tw = alphamax_k_kp - ((TIME_UNIT)((kp - 1) * oa) / (TIME_UNIT) phitp) + ((TIME_UNIT) ((k - 1) * ia) / (TIME_UNIT) phit) ;
						auto nw =   (TIME_UNIT) tw / ( (TIME_UNIT) ia  * (TIME_UNIT) ni);

						VERBOSE_DEBUG ("k=" << k << " kp=" << kp );
						VERBOSE_DEBUG (" ia=" << ia << " oa=" << oa<< " phit=" << phit<< " phitp=" << phitp);
						VERBOSE_DEBUG (" alphamax_k_kp=" << alphamin_k_kp  << " alphamax_k_kp=" << alphamax_k_kp );
						VERBOSE_DEBUG (" tw = " << tw);
						VERBOSE_DEBUG (" nw = " << nw);


						if (alphamin_k_kp <= alphamax_k_kp) {

							VERBOSE_DEBUG (" **** PICK IT " << nw);

							w = std::max ( w , nw )  ;
						}
					}
				}

				if ((w < std::numeric_limits<TIME_UNIT>::max()) and (w > std::numeric_limits<TIME_UNIT>::lowest())) {
					VERBOSE_DEBUG (" w" << w);
					auto bid = dataflow->getEdgeId(c);
					models::SchedulingEventConstraint sec (tid2event.at(tid), tid2event.at(tpid), - w, d, bid);
					eg->addEventConstraint(sec);
				}

			}


		}
	}

	return eg;
}

void algorithms::compute_SPeriodic_throughput    (models::Dataflow*  dataflow, parameters_list_t params) {
	VERBOSE_ASSERT(computeRepetitionVector(dataflow), "Repetition vector failed.");
	models::EventGraph* eg = algorithms::generate_csdf_strictly_periodic_event_graph(dataflow, false);
	std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();

	TIME_UNIT res = howard_res.first ;

	if (params.count("PRINTEG")) {
		std::cout << eg->printXML() << std::endl;
	}

	if (params.count("PRINTCC")) {
		std::cout << "MCRP: " << howard_res.first << std::endl;
		std::cout << "Critical cycle: " <<commons::toString(howard_res.second) << std::endl;
	}


	std::cout << "SPeriodic throughput is "  << std::setw( 20 ) << std::setprecision( 9 ) <<     res    << std::endl;
	std::cout << "SPeriodic period     is " << std::fixed      << std::setw( 20 ) << std::setprecision( 6 ) << 1.0/res    << std::endl;

}


void algorithms::compute_ASPeriodic_throughput    (models::Dataflow*  dataflow, parameters_list_t params) {
	VERBOSE_ASSERT(computeRepetitionVector(dataflow), "Repetition vector failed.");
	models::EventGraph* eg = algorithms::generate_csdf_strictly_periodic_event_graph(dataflow, true);
	std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();

	TIME_UNIT res = howard_res.first ;

	if (params.count("PRINTEG")) {
		std::cout << eg->printXML() << std::endl;
	}

	if (params.count("PRINTCC")) {
		std::cout << "MCRP: " << howard_res.first << std::endl;
		std::cout << "Critical cycle: " <<commons::toString(howard_res.second) << std::endl;
	}


	std::cout << "SPeriodic throughput is "  << std::setw( 20 ) << std::setprecision( 9 ) <<     res    << std::endl;
	std::cout << "SPeriodic period     is " << std::fixed      << std::setw( 20 ) << std::setprecision( 6 ) << 1.0/res    << std::endl;
}

