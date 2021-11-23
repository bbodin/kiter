/*
 * throughput.cpp
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#include <cmath>
#include <vector>
#include <commons/verbose.h>
#include <lp/glpsol.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/buffersizing.h>
#include <algorithms/normalization.h>
#include <algorithms/buffersizing/periodic.h>
#include <algorithms/throughput/kperiodic.h>



void algorithms::compute_csdf_1periodic_memory   (models::Dataflow* const  dataflow, parameters_list_t params) {
    VERBOSE_INFO("Please note you can specify the ILP and GENONLY parameter flags.");

    TIME_UNIT period    = commons::get_parameter<TIME_UNIT>(params, "PERIOD", 0.0) ;
    bool      solve_ilp = commons::get_parameter<bool>(params, "ILP", false) ;
    bool      gen_only  = commons::get_parameter<bool>(params, "GENONLY", false) ;


	  /// BEGIN $$$$$$$$$$$$$$$ This part compute a period if none provided
	  std::map<Vertex,EXEC_COUNT> kvector = algorithms::scheduling::generate1PeriodicVector(dataflow);

	  	kperiodic_result_t result = algorithms::KSchedule(dataflow,&kvector);

	  	TIME_UNIT MIN_PERIOD = 1 / result.throughput;

	  	if (MIN_PERIOD) {
	  	   	VERBOSE_ASSERT (MIN_PERIOD <= period, "The period must be possible");
	  	   	if (period == 0) {
	  	   	period = MIN_PERIOD;
	  	   		VERBOSE_WARNING("PERIOD set to " << period);
	  	   	}
	  	} else {
	  		VERBOSE_ERROR ("Cannot compute the maximum period...");
	  	}

	  /// END $$$$$$$$$$$$$$$ This part compute a period if none provided

	BufferSizingResult sizing_res = periodic_memory_sizing_csdf (dataflow, period, solve_ilp, gen_only);
	if (sizing_res.is_valid()) {
		std::cout << "Total buffer size : " << sizing_res.total_size()
				  << " + 2 * " << dataflow->getVerticesCount() << " = "
				  <<  sizing_res.total_size() + 2 * dataflow->getVerticesCount() << std::endl ;
		 std::cout << "1Periodic size is " <<  sizing_res.total_size() << std::endl;
	} else {
		std::cout << "No solution" << std::endl;
	}
}

BufferSizingResult algorithms::periodic_memory_sizing_csdf   (models::Dataflow* const  dataflow, TIME_UNIT PERIOD, bool ilp_solving, bool gen_only) {

	commons::ValueKind CONTINUE_OR_INTEGER = commons::KIND_CONTINUE;
	if (ilp_solving) CONTINUE_OR_INTEGER = commons::KIND_INTEGER;

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT (PERIOD > 0, "The period must be defined");
	VERBOSE_ASSERT (PERIOD != std::numeric_limits<TIME_UNIT>::infinity(), "The period must be defined");
	VERBOSE_ASSERT(dataflow->is_consistent(),"inconsistent graph");

	TIME_UNIT FREQUENCY = 1.0 / PERIOD;

   	// STEP 0 - CSDF Graph should be normalized
   	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");


	//##################################################################
	// Linear program generation
	//##################################################################
	const std::string problemName =  "PeriodicSizing_" + dataflow->getGraphName() + "_" + commons::toString(FREQUENCY) + ((CONTINUE_OR_INTEGER == commons::KIND_INTEGER) ? "_INT" : "");
	commons::GLPSol g = commons::GLPSol(problemName,commons::MIN_OBJ);


	// Starting times
	//******************************************************************
	{ForEachVertex(dataflow,pVertex) {
		std::string name = dataflow->getVertexName(pVertex);
		for(EXEC_COUNT a = 1; a <= dataflow->getPhasesQuantity(pVertex) ; a++) {
			g.addColumn("s_" + commons::toString<EXEC_COUNT>(a) + "_" + name,commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),0);
		}
	}}



	// Constraints
	//******************************************************************

	{ForEachEdge(dataflow,c) {
		const Vertex source   = dataflow->getEdgeSource(c);
		const Vertex target   = dataflow->getEdgeTarget(c);

		const std::string  buffername= dataflow->getEdgeName(c);
		const std::string  feedbackbuffername= "feedback_" + dataflow->getEdgeName(c);
		const std::string  feedback_mo_name   = "Mop_" + feedbackbuffername;
		const std::string  mo_name   = "Mop_" + buffername;
		const std::string  sourceStr = dataflow->getVertexName(source);
		const std::string  targetStr = dataflow->getVertexName(target);

		const TIME_UNIT  mu_i        = (TIME_UNIT) (PERIOD * dataflow->getPhasesQuantity(source)) / (TIME_UNIT) ( dataflow->getNi(source));
		const TIME_UNIT  mu_j        = (TIME_UNIT) (PERIOD * dataflow->getPhasesQuantity(target)) / (TIME_UNIT) ( dataflow->getNi(target));

		const TOKEN_UNIT  Zi        = dataflow->getEdgeIn(c);
		const TOKEN_UNIT  Zj        = dataflow->getEdgeOut(c);

		const TOKEN_UNIT  mop      =  commons::floor(dataflow->getPreload(c),dataflow->getFineGCD(c));

		const TOKEN_UNIT  gcdz      = std::gcd((Zi),(Zj));

		VERBOSE_DEBUG("Mu_i = " << mu_i);
		VERBOSE_DEBUG("Mu_j = " << mu_j);

		TOKEN_UNIT wai    = 0;  /* wai data write at start ai  */
		TOKEN_UNIT cwai   = 0;  /* cwai cumul data write at start ai   */
		TOKEN_UNIT cwaim1 = 0;  /* cwai cumul data write at start ai-1 */

		TOKEN_UNIT raj    = 0;   /* raj data read at start aj  */
		TOKEN_UNIT craj   = 0;   /* craj cumul data read at start aj   */
		TOKEN_UNIT crajm1 = 0;   /* craj cumul data read at start aj-1 */

		// Feedback Buffer marking (in objectif, its Theta pondered)
		g.addColumn("loopbackfix",CONTINUE_OR_INTEGER,commons::bound_s(commons::FIX_BOUND,dataflow->getVerticesCount()),2);
		g.addColumn(mo_name,CONTINUE_OR_INTEGER,commons::bound_s(commons::FIX_BOUND,dataflow->getPreload(c)),(double) dataflow->getTokenSize(c));
		g.addColumn(feedback_mo_name,CONTINUE_OR_INTEGER,commons::bound_s(commons::LOW_BOUND,0),(double) dataflow->getTokenSize(c));

		// init value for ai = 0
		wai    = 0;
		cwai   = 0;
		cwaim1 = 0;

		for(EXEC_COUNT ai = 1; ai <= dataflow->getPhasesQuantity(source) ; ai++) {

			const TIME_UNIT       ltai    = dataflow->getVertexDuration(source,ai);

			// update wai and cwai (new execution of ti)
			wai = dataflow->getEdgeInPhase(c,ai);
			cwai += wai;

			// init value for aj = 0
			raj    = 0;
			craj   = 0;
			crajm1 = 0;

			for(EXEC_COUNT  aj = 1; aj <= dataflow->getPhasesQuantity(target) ; aj++) {

				const TIME_UNIT       ltaj    = dataflow->getVertexDuration(target,aj);

				// update raj and craj (new execution of tj)
				raj = dataflow->getEdgeOutPhase(c,aj);
				craj += raj;

				// *** Normal Buffer constraint computation
				const TOKEN_UNIT  Ha        =   std::max((TOKEN_UNIT)0, wai - raj);
				const TOKEN_UNIT  alphamin  =   commons::ceil(Ha + craj - cwai - mop,gcdz);
				const TOKEN_UNIT  alphamax  =   commons::floor(  craj - cwaim1 - mop - 1 ,gcdz);


				if (alphamin <= alphamax) { // check if contraint exist
					const std::string pred_row_name = "precedence_" + buffername + "_" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(aj);
					TIME_UNIT coef = ltai + (TIME_UNIT) alphamax * (TIME_UNIT) ( mu_i /  (TIME_UNIT) Zi);
					VERBOSE_DEBUG("LP : s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " >= " << ltai << " + " << (TIME_UNIT) alphamax << " * (" << mu_i << "/" << Zi <<  ")");
					VERBOSE_DEBUG("     s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " >= " << coef);

					g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND, (double) coef));
					if ( (ai != aj) || (source != target)) {
						g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(aj) + "_"+ targetStr    ,  1        );
						g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(ai) + "_"+ sourceStr    , -1        );
					}
				}

				// *** Feedback Buffer constraint computation
				if ( std::min (wai,raj) > 0) { // best know constraint test for the moment (should be improve)
					const std::string pred_row_name = "precedence_" + feedbackbuffername + "_" +
							commons::toString<EXEC_COUNT>(aj) + "_" + commons::toString<EXEC_COUNT>(ai);
					const std::string local_mo_name = feedback_mo_name  + "_" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(aj) ;

					//TOKEN_UNIT      quot = commons::floor(- crajm1 + cwai  - gcdz,gcdz);
					//TOKEN_UNIT      rem =  TOKEN_UNIT - quot;
					//TIME_UNIT       coef     =  ltaj  + (TIME_UNIT) ( mu_j /  (TIME_UNIT) Zj) * quot;
					TIME_UNIT       coef     =  ltaj ;
					//VERBOSE_DEBUG(" add feedback coef, " << ltaj << " + " << (TIME_UNIT)  (quot - gcdz) << " * " << mu_j << "=" << coef);

					g.addColumn(local_mo_name,CONTINUE_OR_INTEGER,commons::bound_s(),0);
					// main constraint
					g.addRow(pred_row_name  ,commons::bound_s(commons::LOW_BOUND,coef));
					g.addCoef(pred_row_name ,local_mo_name                     , -   (double) ( (double) mu_j /  (double) Zj) * (double) gcdz );

					if ( (ai != aj) || (source != target)) {
						g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(ai) + "_"+ sourceStr    ,  1        );
						g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(aj) + "_"+ targetStr    , -1        );
					}
					VERBOSE_DEBUG("LP : s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " >= " << ltaj << " + f * " << gcdz <<  " *(" << mu_j << "/" << Zj <<  ")");
					VERBOSE_DEBUG("     s_"  <<  commons::toString<EXEC_COUNT>(ai) <<  "_" <<  sourceStr  << " - " << "s_"  <<  commons::toString<EXEC_COUNT>(aj) <<  "_" <<  targetStr  << " - f * " << (TIME_UNIT) ( mu_j /  (TIME_UNIT) Zj) * gcdz  << " >= " << coef);


					//local mop bound
					// -Mo -WaPred + Ra - 1 - gcdz + 1 \leq local_mo * gcdz  \leq -Mo -WaPred + Ra - 1
					// Low bound :
					//   -Mo -WaPred + Ra - 1 - gcdz + 1 \leq local_mo
					//   -Mo -gcdz * local_mo  \leq WaPred - Ra + gcdz
					//   Ra - WaPred - gcdz - gcdz * local_mo  \leq + Mo
					//   Mo \geq  Ra - WaPred - gcdz - gcdz * local_mo
					g.addRow (local_mo_name + "_L",commons::bound_s(commons::LOW_BOUND, - crajm1 + cwai  - gcdz ));
					g.addCoef(local_mo_name + "_L",local_mo_name        , (double) gcdz   );
					g.addCoef(local_mo_name + "_L",feedback_mo_name     , 1       );


					// resume
					//
					// -local_mo * gcdz - Mo \leq  WaPred - Ra + gcdz
					//  local_mo * gcdz + Mo \leq -WaPred + Ra - 1

				} // end of  if ( std::min (wai,raj) > 0)

				crajm1 = craj;
			}


			cwaim1 = cwai;
		}

	}}




	// LoopBack
	//******************************************************************

	{ForEachVertex(dataflow,t) {
		const TIME_UNIT timefactor    = (dataflow->getReentrancyFactor(t) <= 0)?0:1;


		const std::string  name   = dataflow->getVertexName(t);
		const EXEC_COUNT max_k    = dataflow->getPhasesQuantity(t) ;
		const TIME_UNIT  mu_i     = (TIME_UNIT)  (PERIOD * dataflow->getPhasesQuantity(t)) / (TIME_UNIT) (dataflow->getNi(t));
		//VERBOSE_DEBUG( "Mu_" << name << " = " << mu_i  << "   lti = " << dataflow->getVertexPhaseDuration(t) << "  Ni = " << dataflow->getNi(t) << "   Phi=" << dataflow->getPhasesQuantity(t));
		// constraintes k --> k + 1

		for(EXEC_COUNT k = 1; k < max_k ; k++) {
			EXEC_COUNT kp = k + 1;
			const std::string pred_row_name = "reentrancy_" + name + "_" + commons::toString<EXEC_COUNT>(k) + "_" + commons::toString<EXEC_COUNT>(kp);
			const TIME_UNIT       ltk    = dataflow->getVertexDuration(t,k) * timefactor;

			g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND,(double)ltk));
			g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(kp) + "_"+ name    ,  1      );
			g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(k) + "_"+ name    , -1       );
		}

		// constraintes last_k --> 1
		const std::string pred_row_name = "reentrancy_" + name + "_phi_1";
		const TIME_UNIT       ltk    = dataflow->getVertexDuration(t,max_k) * timefactor;
		const TIME_UNIT coef = ltk -  mu_i;

		VERBOSE_DEBUG("LoopBack for "<< name << " is " << ltk  <<  "-" << mu_i << " = " << coef);

		g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND,(double)coef));

		if (max_k > 1){
			g.addCoef(pred_row_name ,"s_1_"+ name    ,  1      );
			g.addCoef(pred_row_name ,"s_" + commons::toString<EXEC_COUNT>(max_k) + "_"+ name    , -1       );
		}


	}}

	//##################################################################
	// SOLVE LP
	//##################################################################

	// commons::GLPParameters ilp_params = commons::getDefaultParams();

	// ilp_params.general_doScale = true;
	// ilp_params.linear_doAdvBasis = true;
	// ilp_params.linear_method = commons::DUAL_LINEAR_METHOD;
	//
	// bool sol = g.solve(ilp_params);
	if (gen_only)  {
		g.writeMPSProblem();
		return BufferSizingResult(false);
	}
	bool sol = g.solveWith();

	VERBOSE_INFO("[CSDF K-periodic]  Solved");

	//##################################################################
	// GATHERING RESULTS
	//##################################################################

	// BUFFER SIZES
	//******************************************************************
	if (sol) {
		DATA_UNIT total_buffer_size = 0;
		// ** Good value retreiving method **
		ARRAY_INDEX  edge_indice = 0; // FIXME
		{ForEachEdge(dataflow,c) {
			edge_indice++;
			const Vertex source   = dataflow->getEdgeSource(c);
			const Vertex target   = dataflow->getEdgeTarget(c);

			const std::string  buffername= dataflow->getEdgeName(c);
			const std::string  feedbackbuffername= "feedback_" + dataflow->getEdgeName(c);
			const std::string  feedback_mo_name   = "Mop_" + feedbackbuffername;
			const std::string  sourceStr = dataflow->getVertexName(source);
			const std::string  targetStr = dataflow->getVertexName(target);

			const TOKEN_UNIT  Zi        = dataflow->getEdgeIn(c);
			const TOKEN_UNIT  Zj        = dataflow->getEdgeOut(c);

			const TOKEN_UNIT  gcdz      = std::gcd((Zi),(Zj));


			TOKEN_UNIT wai    = 0;  /* wai data write at start ai  */
			TOKEN_UNIT cwai   = 0;  /* cwai cumul data write at start ai   */

			TOKEN_UNIT raj    = 0;   /* raj data read at start aj  */
			TOKEN_UNIT craj   = 0;   /* craj cumul data read at start aj   */
			TOKEN_UNIT crajm1 = 0;   /* craj cumul data read at start aj-1 */

			// init value for ai = 0
			wai    = 0;
			cwai   = 0;
			TOKEN_UNIT feedbackmopmax =  commons::ceil(g.getValue(feedback_mo_name),1);
			if (CONTINUE_OR_INTEGER == commons::KIND_INTEGER) feedbackmopmax = (TOKEN_UNIT) round(g.getIntegerValue(feedback_mo_name));
			VERBOSE_DEBUG(dataflow->getEdgeName(c) << " : Starting with " << g.getValue(feedback_mo_name) << " => " << feedbackmopmax );
			for(EXEC_COUNT ai = 1; ai <= dataflow->getPhasesQuantity(source) ; ai++) {

				// update wai and cwai (new execution of ti)
				wai = dataflow->getEdgeInPhase(c,ai);
				cwai += wai;

				// init value for aj = 0
				raj    = 0;
				craj   = 0;
				crajm1 = 0;

				for(EXEC_COUNT  aj = 1; aj <= dataflow->getPhasesQuantity(target) ; aj++) {

					// update raj and craj (new execution of tj)
					raj = dataflow->getEdgeOutPhase(c,aj);
					craj += raj;

					//   -Mo -WaPred + Ra - 1 - gcdz + 1 \leq local_mo
					//   -Mo -gcdz * local_mo  \leq WaPred - Ra + gcdz
					//   -Mo \leq WaPred - Ra + gcdz  + gcdz * local_mo
					//   Mo   >= -  WaPred + Ra - gcdz - gcdz * local_mo


					const std::string pred_row_name = "precedence_" + feedbackbuffername + "_" + commons::toString<EXEC_COUNT>(aj) + "_" + commons::toString<EXEC_COUNT>(ai);
					const std::string local_coef = feedback_mo_name  + "_" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(aj) ;

					if (!g.haveValue(local_coef)) {
						if (std::min(wai,raj) > 0 ) VERBOSE_FAILURE();
						continue; // skip null constraint
					}

					TOKEN_UNIT from_lp = commons::floor(g.getValue(local_coef),1);
					if (CONTINUE_OR_INTEGER == commons::KIND_INTEGER)
						from_lp =  (TOKEN_UNIT) round(g.getIntegerValue(local_coef));

					// *** Feedback Buffer constraint computation
					TOKEN_UNIT bound =  - from_lp * gcdz - crajm1 + cwai - gcdz;
					if (bound > feedbackmopmax) {
						VERBOSE_DEBUG("   -> new fmax => " << g.getValue(local_coef) << "=> " << from_lp);
						VERBOSE_DEBUG("   -> new bound = " <<  - from_lp <<  "*"  << gcdz << "-" << crajm1 << "+" <<cwai << "-" << gcdz<< "=" << bound );
						feedbackmopmax = bound;
					}
					crajm1 = craj;
				}
			}

			TOKEN_UNIT buffersize =  feedbackmopmax  + dataflow->getPreload(c);
			VERBOSE_INFO(dataflow->getEdgeName(c) << " :" << g.getValue(feedback_mo_name) << " + " << dataflow->getPreload(c) << " -> " << feedbackmopmax << " + " << dataflow->getPreload(c)<< " = " << buffersize  );
			//TOKEN_UNIT from_integer_mop          = commons::floor(g.getIntegerValue(feedback_mo_name),1);

			total_buffer_size += buffersize * dataflow->getTokenSize(c);
		}}

		VERBOSE_INFO("Loopback buffers : " << dataflow->getVerticesCount());

		return total_buffer_size;
	} else {
		VERBOSE_ERROR("No feasible solution");
		return 0;
	}


}


