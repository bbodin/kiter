/*
 * ExpansionGraph.cpp
 *
 *  Created on: Feb 7, 2021
 *      Author: toky
 */




/*
 * ExpansionGraph.h
 *
 *  Created on: Sep 26, 2019
 *      Author: toky
 */

#include <algorithms/experiments/ExpansionGraph.h>

#include <models/EventGraph.h>
#include <models/Dataflow.h>
#include <algorithms/normalization.h>

#include <numeric>




std::string ExpansionGraph::toDOT () {return this->eg->printDOT();}

void ExpansionGraph::generate_E1_Edges(models::Dataflow * const dataflow , EXEC_COUNT ki,  models::EventGraph* g , Vertex t  ) {

	const ARRAY_INDEX task_id = dataflow->getVertexId(t);

	VERBOSE_DEBUG("generate E1 edges for task " <<  dataflow->getVertexId(t) << " with ki=" << ki);


	for (EXEC_COUNT source_ki = 1, target_ki = 2; target_ki <= ki; source_ki++, target_ki++) {

		models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,source_ki);
		models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,target_ki);


		TIME_UNIT d =  0;
		TIME_UNIT w =  0;

		g->addEventConstraint(source_event ,target_event,w,d,0);

	}

}

void ExpansionGraph::generate_E2_Edge(models::Dataflow * const dataflow , EXEC_COUNT Ki,  models::EventGraph* g , Vertex t  ) {

	const ARRAY_INDEX task_id = dataflow->getVertexId(t);
	const TOKEN_UNIT Zi       = dataflow->getZi(t);

	VERBOSE_DEBUG("generate E1 edges for task " <<  dataflow->getVertexId(t) << " with ki=" << Ki);


	models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,Ki);
	models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,1);


	TIME_UNIT d =  0;
	TIME_UNIT w =  Zi * Ki;

	g->addEventConstraint(source_event ,target_event,w,d,0);


}


void algorithm1(EXEC_COUNT x, double f0, double g0, TOKEN_UNIT Tx,TOKEN_UNIT Ty,TOKEN_UNIT gcdk, EXEC_COUNT maxX, EXEC_COUNT maxY) {

	VERBOSE_INFO("algorithm1");

	TOKEN_UNIT g = std::gcd(gcdk,Ty);

	VERBOSE_INFO("g=" << g);

	double yg0 = (Tx * x - g0 * gcdk) / Ty;
	double yf0 = (Tx * x - f0 * gcdk) / Ty;
	double Tyyg0 = Ty * yg0;
	double Tyyf0 = Ty * yf0;

	EXEC_COUNT start = std::ceil ( Tyyg0 );
	EXEC_COUNT stop  = std::floor ( Tyyf0 );


	VERBOSE_INFO("yg0=" << yg0);
	VERBOSE_INFO("yf0=" << yf0);
	VERBOSE_INFO("Tyyg0=" << Tyyg0);
	VERBOSE_INFO("Tyyf0=" << Tyyf0);

	VERBOSE_INFO("start=" << start);
	VERBOSE_INFO("stop=" << stop);

	for (EXEC_COUNT theta = start ; theta <= stop; theta ++ ) {
		VERBOSE_INFO(" try theta=" << theta);

		if (theta % g) continue;

		// theta divisible by g

		// Find (u0v0) solutions of uTy -vgcdk = tehta

		VERBOSE_INFO("interesting theta=" << theta);

		std::pair<TOKEN_UNIT,TOKEN_UNIT> res = commons::extended_euclide ( Ty,  gcdk, theta);
		EXEC_COUNT u0 = res.first;
		EXEC_COUNT v0 = res.second;


		VERBOSE_INFO("u0=" << u0);
		VERBOSE_INFO("v0=" << v0);


		for (EXEC_COUNT k = 0, counter = u0 ; counter <= maxY ; counter = u0 + (++k * gcdk)/g) {
			VERBOSE_INFO("k=" << k << " with counter =" << counter);
		}


	}

}

void algorithm2(double f0, double g0, TOKEN_UNIT Tx,TOKEN_UNIT Ty,TOKEN_UNIT gcdk, EXEC_COUNT maxX, EXEC_COUNT maxY) {
	VERBOSE_INFO("Algorithm 2 Starts");
	// Algorithm 2
		if (g0 >= 1 + f0) {
			// Take them all
			VERBOSE_INFO ("Case 1 : Take them all");
		} else if (Ty == gcdk) {

			VERBOSE_INFO ("Case 2 : Ty == gcdk");

			for (EXEC_COUNT x = 1; x <= maxX ; x++ ) {
				VERBOSE_INFO ("Test x =" << x);
				double shift = ((double) ( Tx * x ) /  (double) gcdk);

				double gx0 =  (double) g0 +  (double) shift;
				double fx0 =  (double) f0 +  (double) shift;

				TOKEN_UNIT floorgx0 = (TOKEN_UNIT) std::floor(gx0);
				TOKEN_UNIT ceilfx0 = (TOKEN_UNIT) std::ceil(fx0);

				if (floorgx0 == ceilfx0) {
					// Take (x,y) for every y
					VERBOSE_INFO ("Take (x,y) for every y");
				} else {
					VERBOSE_INFO ("Skip it");

				}
			}
		} else {

			VERBOSE_INFO ("Case 3 : algorithm 1");

			for (EXEC_COUNT x = 1; x <= maxX ; x++ ) {

				VERBOSE_INFO ("Run algorithm 1 :  x =" << x);

				// Algorithm 1
				algorithm1( x, f0,  g0,  Tx, Ty, gcdk,  maxX,  maxY);

			}


		}

}


void ExpansionGraph::generate_E3_Edges(models::Dataflow * const dataflow , const periodicity_vector_t& kValues,  models::EventGraph* g , Edge c) {


	VERBOSE_DEBUG("Constraint for " << dataflow->getEdgeName(c) );

	Vertex source = dataflow->getEdgeSource(c);
	Vertex target = dataflow->getEdgeTarget(c);

	const ARRAY_INDEX source_id = dataflow->getVertexId(source);
	const ARRAY_INDEX target_id = dataflow->getVertexId(target);

	const EXEC_COUNT  maxki        = kValues.at(source) ;
	const EXEC_COUNT  maxkj        = kValues.at(target) ;


	const TOKEN_UNIT Zi       = dataflow->getZi(source);
	const TOKEN_UNIT Zj       = dataflow->getZi(target);

	TOKEN_UNIT mop     =  commons::floor(dataflow->getNormMop(c), 1);

	const TOKEN_UNIT  gcdz       = std::gcd(Zi,Zj);
	const TOKEN_UNIT  gcdk       = std::gcd(maxki*Zi,maxkj*Zj);



	TIME_UNIT d =  dataflow->getVertexDuration(source);

	std::cout << "# Channel \"" <<  dataflow->getEdgeName(c) << "\" between \"" <<  dataflow->getVertexName(source) << "\" and \"" <<  dataflow->getVertexName(target) << "\"" << std::endl;
	std::cout << "ai,aj,shift,fxy,gxy,pibmin,pibmax,w" << std::endl;

	// By definition f00gcdz and g00gcdz are devisible by gcdz
	TOKEN_UNIT f0gcdk = std::max(Zi - Zj, (TOKEN_UNIT) 0) - mop;
	TOKEN_UNIT g0gcdk = Zi - gcdz - mop ;

	TOKEN_UNIT Tx = Zi;
	TOKEN_UNIT Ty = Zj;

	double f0 =  (double) f0gcdk / (double) gcdk;
	double g0 =  (double) g0gcdk / (double) gcdk;

	//VERBOSE_ASSERT ((f0gcdk % gcdk) == 0, "f0 Must be integer and here f0gcdk=" << f0gcdk << " with gcdk=" << gcdk << " that is (f0gcdk % gcdk)=" << f0gcdk % gcdk);
	//VERBOSE_ASSERT ((g0gcdk % gcdk) == 0, "g0 Must be integer and here g0gcdk=" << g0gcdk << " with gcdk=" << gcdk << " that is (g0gcdk % gcdk)=" << g0gcdk % gcdk);
	VERBOSE_ASSERT (f0 <= g0,   "f0 Must be less or equal to g0");


	VERBOSE_INFO ("Alternative version starts");
	algorithm2( f0,  g0,  Tx, Ty, gcdk,  maxki,  maxkj) ;


	for (EXEC_COUNT ai = 1; ai <= maxki ; ai++ ) {
		for (EXEC_COUNT aj = 1; aj <= maxkj ; aj++ ) {

			TOKEN_UNIT x = ai;
			TOKEN_UNIT y = aj;
			double shift = ((double) ( Tx * x - Ty * y ) /  (double) gcdk);

			double fxy =  (double) f0 +  (double) shift;
			double gxy =  (double) g0 +  (double) shift;


			const TOKEN_UNIT pibmin =  (TOKEN_UNIT)   std::ceil(fxy);
			const TOKEN_UNIT pibmax =  (TOKEN_UNIT)  std::floor(gxy);

			std::cout << ai << ", "
					<< aj << ", "
					<< shift << ", "
					<< fxy << ", "
					<< gxy << ", "
					<< pibmin << ", "
					<< pibmax  << ", " ;

			if (pibmin <= pibmax) {

				TIME_UNIT w =  - pibmax * gcdk;
				std::cout << w << std::endl;

				models::EventGraphVertex source_event = g->getEventGraphVertex(source_id,ai);
				models::EventGraphVertex target_event = g->getEventGraphVertex(target_id,aj);
				g->addEventConstraint(source_event ,target_event,w,d,0);


			} else {

				std::cout  << "NaN" << std::endl;
			}

		}
	}



}


models::EventGraph* ExpansionGraph::generate(models::Dataflow* dataflow,   const periodicity_vector_t& kvector) {


	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

	VERBOSE_ASSERT(algorithms::normalize(dataflow),"normalize failed");

	models::EventGraph * g = new models::EventGraph();

	VERBOSE_DEBUG_ASSERT(dataflow->is_normalized(),"The grah needs to be normalized");

	/* generate nodes */
	{ForEachVertex(dataflow,t) {
		EXEC_COUNT phase_count = dataflow->getPhasesQuantity(t);
		VERBOSE_ASSERT (phase_count == 1, "This is not a SDF");
		for (EXEC_COUNT ki = 1; ki <= kvector.at(t) ; ki++ ) {
			g->addEvent(models::SchedulingEvent(dataflow->getVertexId(t),ki));
		}
	}}


	// DEFINITION DES REENTRANCES
	//******************************************************************

	{ForEachVertex(dataflow,pTask) {
		EXEC_COUNT ki = kvector.at(pTask);
		this->generate_E1_Edges(dataflow,ki,g,pTask);
		this->generate_E2_Edge(dataflow,ki,g,pTask);
	}}


	// DEFINITION DES CONTRAINTES DE PRECEDENCES
	//******************************************************************
	{ForEachChannel(dataflow,c) {
		this->generate_E3_Edges(dataflow , kvector,   g ,  c);
	}}



	return g;



}

