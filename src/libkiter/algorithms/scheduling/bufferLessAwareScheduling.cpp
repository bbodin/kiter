/*
 * bufferLessAwareScheduling.cpp
 *
 *  Created on: Aug 22, 2019
 *      Author: toky
 */


#include <algorithms/schedulings.h>
#include <algorithms/mappings.h>
#include <lp/glpsol.h>


#include <algorithms/throughput/kperiodic.h>
#include <models/EventGraph.h>

static inline const std::string OMEGA_COL_STR ()  {
	return "OMEGA" ;
}


static inline const std::string START_COL_STR (const std::string name, const EXEC_COUNT a, const EXEC_COUNT k)  {
	return "s_" + name + "_" + commons::toString<EXEC_COUNT>(a)+ "_" + commons::toString<EXEC_COUNT>(k) + "_" ;
}

static inline const std::string PRED_ROW_STR (const std::string buffername, const EXEC_COUNT ai, const EXEC_COUNT ki, const EXEC_COUNT aj, const EXEC_COUNT kj)  {
	return "precedence_" + buffername + "___" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(ki)+ "___" + commons::toString<EXEC_COUNT>(aj) + "_" + commons::toString<EXEC_COUNT>(kj);

}


void algorithms::scheduling::bufferlessKPeriodicScheduling (models::Dataflow* const dataflow, parameters_list_t param_list) {

	 bool stop_at_first = (param_list.find("stop_at_first") != param_list.end());
	 bool get_previous = (param_list.find("get_previous") != param_list.end());;
	 bool do_linear = (param_list.find("do_linear") != param_list.end());;
	 bool do_old = (param_list.find("do_old") != param_list.end());;

	VERBOSE_INFO(  " stop_at_first =" << stop_at_first << " get_previous =" << get_previous);

	if (do_linear) return;
	if (do_old) {
		 algorithms::compute_Kperiodic_throughput (dataflow, param_list);
		 return;
	}

	scheduling_t sched = algorithms::scheduling::bufferless_kperiodic_scheduling (dataflow, stop_at_first, get_previous);

}


scheduling_t algorithms::scheduling::bufferless_kperiodic_scheduling (models::Dataflow* const  dataflow, bool stop_at_first, bool get_previous) {

    EXEC_COUNT iteration_count = 0;
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}

    VERBOSE_INFO("Start with a 1-periodic schedule");
    kperiodic_result_t result = algorithms::KScheduleBufferLess(dataflow,&kvector);

    if ((result.critical_edges.size() != 0) and ((!stop_at_first) or (result.throughput > 0))) {

        VERBOSE_INFO("1-periodic throughput is not enough");
        VERBOSE_INFO("   Th = " << result.throughput);
        VERBOSE_INFO("   Critical circuit is " << algorithms::cc2string(dataflow,&(result.critical_edges)) <<  "");

        while (true) {
            iteration_count++;
            std::map<Vertex,EXEC_COUNT> previous_kvector = kvector;
            updateVectorWithLocalNi(dataflow,&kvector,&(result.critical_edges));
            kperiodic_result_t resultprime = KScheduleBufferLess(dataflow,&kvector);
            if (algorithms::sameset(dataflow,&(resultprime.critical_edges),&(result.critical_edges)) or (stop_at_first and result.throughput > 0))  {
                VERBOSE_INFO("End criteria verified.");
                if ((result.throughput < resultprime.throughput) and !get_previous) {
                	result = resultprime;
                } else if (get_previous) {
                	kvector = previous_kvector;
                }
                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput is not enough");
            VERBOSE_INFO("   K  = " << commons::toString(kvector));
            VERBOSE_INFO("   Th = " << result.throughput);
            VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.critical_edges)) <<  "");
        }

    } else {
        VERBOSE_INFO("End criteria verified.");
        VERBOSE_INFO(" - Size of critical circuit was: " << result.critical_edges.size());
        VERBOSE_INFO(" - Throughput was: " << result.throughput);
        VERBOSE_INFO(" - stop_at_first: " << stop_at_first);
        VERBOSE_INFO("   K  = " << commons::toString(kvector));
        VERBOSE_INFO("   Th = " << result.throughput);
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.critical_edges)) <<  "");
        iteration_count++;
    }

    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count);


    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );
    TIME_UNIT res = result.throughput;
    std::cout << "Maximum throughput is "  << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;

    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);

    TIME_UNIT omega = 1 / result.throughput ;
    eg->computeStartingTimeWithOmega (omega);

    scheduling_t sheduling_result;

    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        TIME_UNIT start = eg->getStartingTime(e);
        Vertex v = dataflow->getVertexById(ti);
        TIME_UNIT period = kvector[v] *  dataflow->getPhasesQuantity(v) * omega / dataflow->getNi(v);

        sheduling_result[(ARRAY_INDEX)ti].periodic_starts.first = period;
        sheduling_result[(ARRAY_INDEX)ti].periodic_starts.second.push_back(start);

        //sheduling_result[v].first = period;
        //sheduling_result[v].second.push_back(start);
    }}

    return sheduling_result;

}

scheduling_t algorithms::scheduling::bufferless_scheduling (models::Dataflow* const dataflow, std::map<Vertex,EXEC_COUNT> &  kvector ) {

    commons::ValueKind CONTINUE_OR_INTEGER = commons::KIND_CONTINUE;

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    {ForEachVertex(dataflow,t) {
        std::string name = dataflow->getVertexName(t);
        VERBOSE_INFO(" - " << name << " k=" <<  kvector[t] << " self-reentrancy = " << dataflow->getReentrancyFactor(t));
        VERBOSE_ASSERT(kvector[t], "periodicity vector must be positive non zero.");
    }}

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    VERBOSE_INFO("Generate Constraints ...");

    //##################################################################
    // Linear program generation
    //##################################################################

    const std::string problemName =  "BUFFERLESS_KPERIODIC_THROUGHPUT_" + dataflow->getGraphName() + ((CONTINUE_OR_INTEGER == commons::KIND_INTEGER) ? "_INT" : "");
    commons::GLPSol g = commons::GLPSol(problemName,commons::MIN_OBJ);

    // Hyper Period
    //******************************************************************
    auto OMEGA_COL = g.addColumn(OMEGA_COL_STR(),commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),1);

    // Starting times
    //******************************************************************
    {ForEachVertex(dataflow,t) {
        std::string name = dataflow->getVertexName(t);
		for(EXEC_COUNT a = 1; a <= dataflow->getPhasesQuantity(t) ; a++) {
			for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {
				g.addColumn(START_COL_STR(name,a,k),commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),0);
			}
		}
    }}


    // Reetrancy Constraints
    //******************************************************************

    {ForEachVertex(dataflow,t) {
        std::string name = dataflow->getVertexName(t);
        std::string last_col_name = "";

		for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {
			for(EXEC_COUNT a = 1; a <= dataflow->getPhasesQuantity(t) ; a++) {
				const TIME_UNIT       ltai    = dataflow->getVertexDuration(t,a);
				std::string new_col_name = START_COL_STR(name,a,k);
				std::string row_name = "Task_order_"  + name + "_" + new_col_name;
				if (last_col_name != "") {
					g.addRow(row_name,commons::bound_s(commons::LOW_BOUND, ltai ));
					g.addCoef(row_name , last_col_name   , - 1     );
					g.addCoef(row_name , new_col_name    ,   1     );
				}
				last_col_name = new_col_name;
			}
		}

	    // Last one for reetrancy

		if (dataflow->getReentrancyFactor(t)) {
			auto coef = (TIME_UNIT) (dataflow->getPhasesQuantity(t) * kvector[t]) / (  (TIME_UNIT) dataflow->getNi(t)  );
			std::string last_row_name = "Task_reentracy_" + name;
			g.addRow(last_row_name,commons::bound_s(commons::LOW_BOUND, dataflow->getVertexDuration(t,dataflow->getPhasesQuantity(t)) ));
			std::string source_col =  START_COL_STR(name,dataflow->getPhasesQuantity(t), kvector[t]);
			std::string target_col =  START_COL_STR(name,1,1);
			if (target_col != source_col) {
				g.addCoef(last_row_name , source_col   , - 1     );
				g.addCoef(last_row_name , target_col    ,   1    );
			}
			g.addCoef(last_row_name , OMEGA_COL_STR(), (double)  coef     );
		}

    }}


    // Communication Constraints
    //******************************************************************

    {ForEachEdge(dataflow,c) {

		const std::string  buffername = dataflow->getEdgeName(c);

		const Vertex source   = dataflow->getEdgeSource(c);
		const Vertex target   = dataflow->getEdgeTarget(c);

		const std::string  sourceStr = dataflow->getVertexName(source);
		const std::string  targetStr = dataflow->getVertexName(target);

		const TOKEN_UNIT  Zi        = dataflow->getEdgeIn(c);
		const TOKEN_UNIT  Zj        = dataflow->getEdgeOut(c);

		const TOKEN_UNIT  mop      =  commons::floor(dataflow->getPreload(c),dataflow->getFineGCD(c));

		// TODO : I am not sure about this part,
        const TOKEN_UNIT  gcdk      = std::gcd( kvector[source]  * (Zi), kvector[target] * (Zj));

		TOKEN_UNIT wai    = 0;  /* wai data write at start ai  */
		TOKEN_UNIT cwai   = 0;  /* cwai cumul data write at start ai   */
		TOKEN_UNIT cwaim1 = 0;  /* cwai cumul data write at start ai-1 */

		TOKEN_UNIT raj    = 0;   /* raj data read at start aj  */
		TOKEN_UNIT craj   = 0;   /* craj cumul data read at start aj   */
		TOKEN_UNIT crajm1 = 0;   /* craj cumul data read at start aj-1 */


		// init value for ai = 0
		wai    = 0;
		cwai   = 0;
		cwaim1 = 0;

		for (EXEC_COUNT ai = 1; ai <= dataflow->getPhasesQuantity(source) ; ai++) {
			for(EXEC_COUNT ki = 1; ki <= kvector[source] ; ki++) {
	            int saicolid = g.getColumn(START_COL_STR(sourceStr,ai,ki)) ;

			const TIME_UNIT       ltai    = dataflow->getVertexDuration(source,ai);

			// update wai and cwai (new execution of ti)
			wai = dataflow->getEdgeInPhase(c,ai);
			cwai += wai;

			// init value for aj = 0
			raj    = 0;
			craj   = 0;
			crajm1 = 0;

			for (EXEC_COUNT  aj = 1; aj <= dataflow->getPhasesQuantity(target) ; aj++) {
				for(EXEC_COUNT kj = 1; kj <= kvector[target] ; kj++) {
		            int sajcolid = g.getColumn(START_COL_STR(targetStr,aj,kj)) ;

				// update raj and craj (new execution of tj)
				raj = dataflow->getEdgeOutPhase(c,aj);
				craj += raj;

				// *** Normal Buffer constraint computation
				const TOKEN_UNIT  Ha        =   std::max((TOKEN_UNIT)0, wai - raj);
				const TOKEN_UNIT  alphamin  =   commons::ceil(Ha + craj - cwai - mop,gcdk);
				const TOKEN_UNIT  alphamax  =   commons::floor(  craj - cwaim1 - mop - 1 ,gcdk);


				if (alphamin <= alphamax) { // check if contraint exist
					const std::string pred_row_name = PRED_ROW_STR(buffername,ai,ki,aj,kj);

					  TIME_UNIT coef =  ((TIME_UNIT) alphamax) / ( (TIME_UNIT) dataflow->getNi(source)  * (TIME_UNIT) Zi );

					  commons::BoundType bound =(dataflow->getEdgeType(c) == EDGE_TYPE::BUFFERLESS_EDGE)  ?  commons::FIX_BOUND :  commons::LOW_BOUND;


					  int rowid = g.addRow(pred_row_name,commons::bound_s(bound, (double) ltai ));
					  VERBOSE_INFO("Add " << pred_row_name);



					  g.fastAddCoef(rowid ,OMEGA_COL    , (double) - coef      );

					  if ( (ai != aj) || (source != target) || (ki != kj)) {
						  g.fastAddCoef(rowid ,sajcolid    ,  1        );
						  g.fastAddCoef(rowid ,saicolid    , -1        );
					  }
				}

				crajm1 = craj;
			}
			}

			cwaim1 = cwai;
		}
		}
    }
}




    //##################################################################
    // SOLVE LP
    //##################################################################

    // commons::GLPParameters ilp_params = commons::getDefaultParams();

    // ilp_params.general_doScale = true;
    // ilp_params.linear_doAdvBasis = true;
    // ilp_params.linear_method = commons::DUAL_LINEAR_METHOD;
    //
    // bool sol = g.solve(ilp_params);


    VERBOSE_INFO("Solving problem ...");

    bool sol = g.solve();

    VERBOSE_INFO("Solved, gathering results ...");

    //##################################################################
    // GATHERING RESULTS
    //##################################################################

    // SCHEDULING RESULT
    //******************************************************************
    scheduling_t res_schedule;

    if (sol) {

    	TIME_UNIT OMEGA = g.getValue("OMEGA");

        std::cout << "OMEGA : " << OMEGA << std::endl ;

        {ForEachVertex(dataflow,t) {
            std::string name = dataflow->getVertexName(t);

    		res_schedule[dataflow->getVertexId(t)].periodic_starts.first = OMEGA / (TIME_UNIT) ( (TIME_UNIT) dataflow->getNi(t) / (TIME_UNIT) kvector[t] );

    		for(EXEC_COUNT a = 1; a <= dataflow->getPhasesQuantity(t) ; a++) {
    			for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {
    				auto starting_time_col = START_COL_STR(name,a,k);
    				auto starting_time = g.getValue(starting_time_col);
    				std::cout << starting_time_col <<  "=" << starting_time
                		<< "  NI=" <<  dataflow->getNi(t)
        	         	<< "  period=" <<  OMEGA / (TIME_UNIT) ( (TIME_UNIT) dataflow->getNi(t) / (TIME_UNIT) kvector[t] )
						<< "  end_of_execution_of_last_instance_of_hyper_period=" <<
						starting_time + dataflow->getVertexDuration(t,a) + (dataflow->getNi(t)-1)  * ((OMEGA / (TIME_UNIT) ( (TIME_UNIT) dataflow->getNi(t) / (TIME_UNIT) kvector[t] ))) << std::endl ;
    				res_schedule[dataflow->getVertexId(t)].periodic_starts.second.push_back(starting_time);
    			}
    		}


        }}


    } else {
        VERBOSE_ERROR("No feasible solution");
    }

    VERBOSE_INFO("Done");
    return res_schedule;


}


void algorithms::scheduling::sdf_bufferless_scheduling (models::Dataflow* const  dataflow, std::map<Vertex,EXEC_COUNT> &  kvector, std::vector<std::vector <Vertex> > task_sequences) {


	  {ForEachVertex(dataflow,t) {
	  	  VERBOSE_ASSERT(dataflow->getPhasesQuantity(t) == 1, "Support only SDF");
	  }}

    commons::ValueKind CONTINUE_OR_INTEGER = commons::KIND_CONTINUE;
   // With gurobi might be needed, need to fix that.
    //CONTINUE_OR_INTEGER = commons::KIND_INTEGER;

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    {ForEachVertex(dataflow,t) {
        std::string name = dataflow->getVertexName(t);
        VERBOSE_INFO(" - " << name << " k=" <<  kvector[t]);
        VERBOSE_ASSERT(kvector[t], "periodicity vector must be positive non zero.");
    }}
    VERBOSE_INFO("Getting period ...");

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    VERBOSE_INFO("Generate Graph ...");

    //##################################################################
    // Linear program generation
    //##################################################################
    const std::string problemName =  "SDF_BUFFERLESS_KPERIODIC_THROUGHPUT_" + dataflow->getGraphName() + ((CONTINUE_OR_INTEGER == commons::KIND_INTEGER) ? "_INT" : "");
    commons::GLPSol g = commons::GLPSol(problemName,commons::MIN_OBJ);

    // Hyper Period
    //******************************************************************

    auto OMEGA_COL = g.addColumn("OMEGA",commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),1);

    // Starting times
    //******************************************************************
    {ForEachVertex(dataflow,t) {
        std::string name = dataflow->getVertexName(t);
        for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {
            g.addColumn("s_" + commons::toString<EXEC_COUNT>(k) + "_" + name,commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),0);
        }
    }}


    for (std::vector<Vertex> sequence : task_sequences) {
        VERBOSE_INFO("One sequence of " << sequence.size() << " task to force inline");
        if (sequence.size() == 0) continue;
        EXEC_COUNT force_k = kvector[sequence[0]];

        std::vector<std::string> previous;
        std::string name = dataflow->getVertexName(sequence[0]);

        for(EXEC_COUNT k = 1; k <= kvector[sequence[0]] ; k++) {
        	previous.push_back("s_" + commons::toString<EXEC_COUNT>(k) + "_" + name);
        }

        for (Vertex t : sequence) {
        	VERBOSE_ASSERT(force_k == kvector[t], "Edges from the same sequence must have the same k values.");

            for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {

            	// ADD sequence between between current task k and previous task k.
            	std::string previous_name = previous[k-1];
            	std::string current_name = "s_" + commons::toString<EXEC_COUNT>(k) + "_" + dataflow->getVertexName(t);

            	if (previous_name != current_name)  {
            		// add constraint
            		std::string row_name = "sequence_" + previous_name + "_to_" + current_name;
            		g.addRow(row_name,commons::bound_s(commons::FIX_BOUND, 1 ));
            		g.addCoef(row_name , previous_name   , - 1     );
            		g.addCoef(row_name , current_name    ,   1     );
                    VERBOSE_INFO("Add " << row_name);
            	}
            	previous[k-1] = current_name;
            }

        }


    }



    // Reetrancy Constraints
    //******************************************************************


    {ForEachVertex(dataflow,t) {
        std::string name = dataflow->getVertexName(t);
        std::string last_col_name = "";

		for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {
				const TIME_UNIT       lt    = dataflow->getVertexDuration(t);
				std::string new_col_name = "s_" + commons::toString<EXEC_COUNT>(k) + "_" + name;
				std::string row_name = "Task_order_"  + name + "_" + new_col_name;
				if (last_col_name != "") {
					g.addRow(row_name,commons::bound_s(commons::LOW_BOUND, lt ));
					g.addCoef(row_name , last_col_name   , - 1     );
					g.addCoef(row_name , new_col_name    ,   1     );
				}
				last_col_name = new_col_name;
		}

	    // Last one for reetrancy

		if (dataflow->getReentrancyFactor(t)) {
			auto coef = (TIME_UNIT) (dataflow->getPhasesQuantity(t) * kvector[t]) / (  (TIME_UNIT) dataflow->getNi(t)  );
			std::string last_row_name = "Task_reentracy_" + name;
			g.addRow(last_row_name,commons::bound_s(commons::LOW_BOUND, dataflow->getVertexDuration(t,dataflow->getPhasesQuantity(t)) ));
			std::string source_col =  "s_" + commons::toString<EXEC_COUNT>(kvector[t]) + "_" + name;
			std::string target_col =  "s_" + commons::toString<EXEC_COUNT>(1) + "_" + name;
			if (target_col != source_col) {
				g.addCoef(last_row_name , source_col   , - 1     );
				g.addCoef(last_row_name , target_col    ,   1    );
			}
			g.addCoef(last_row_name , OMEGA_COL_STR(), (double)  coef     );
		}

    }}


    // Constraints
    //******************************************************************

    {ForEachEdge(dataflow,c) {
        const Vertex source   = dataflow->getEdgeSource(c);
        const Vertex target   = dataflow->getEdgeTarget(c);

        const std::string  buffername= dataflow->getEdgeName(c);
        const std::string  sourceStr = dataflow->getVertexName(source);
        const std::string  targetStr = dataflow->getVertexName(target);
        const EXEC_COUNT  Ni        =  dataflow->getNi(source);

        const TOKEN_UNIT  in_b        = dataflow->getEdgeIn(c);
        const TOKEN_UNIT  ou_b        = dataflow->getEdgeOut(c);

        const TOKEN_UNIT  gcdb      = std::gcd((in_b),(ou_b));
        const TOKEN_UNIT  gcdk      = std::gcd( kvector[source]  * (in_b), kvector[target] * (ou_b));

        const TOKEN_UNIT  mop      =  commons::floor(dataflow->getPreload(c),gcdb);



        const TIME_UNIT       ltai    = dataflow->getVertexDuration(source,1);
        const TOKEN_UNIT  Ha        =   std::max((TOKEN_UNIT)0, in_b - ou_b);

        for(EXEC_COUNT ai = 1; ai <= kvector[source] ; ai++) {
            int saicolid = g.getColumn("s_" + commons::toString<EXEC_COUNT>(ai) + "_"+ sourceStr );
            for(EXEC_COUNT  aj = 1; aj <= kvector[target] ; aj++) {
                int sajcolid = g.getColumn("s_" + commons::toString<EXEC_COUNT>(aj) + "_"+ targetStr );


                // *** Normal Buffer constraint computation
                const TOKEN_UNIT  alphamin  =   commons::ceil(Ha + (TOKEN_UNIT) aj * ou_b - (TOKEN_UNIT) ai * in_b - mop,gcdk);
                const TOKEN_UNIT  alphamax  =   commons::floor(  in_b + (TOKEN_UNIT)aj * ou_b - (TOKEN_UNIT)ai * in_b  - mop - 1 ,gcdk);


                if (alphamin <= alphamax) { // check if contraint exist
                    const std::string pred_row_name = "precedence_" + buffername + "_" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(aj);
                    TIME_UNIT coef =  ((((TIME_UNIT) alphamax) / ( (TIME_UNIT) Ni  * (TIME_UNIT) in_b )));



                    int rowid = g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND, (double) ltai ));
                    VERBOSE_INFO("Add " << pred_row_name);



                    g.fastAddCoef(rowid ,OMEGA_COL    , (double) - coef      );

                    if ( (ai != aj) || (source != target)) {
                        g.fastAddCoef(rowid ,sajcolid    ,  1        );
                        g.fastAddCoef(rowid ,saicolid    , -1        );
                    }
                }
            }
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


    VERBOSE_INFO("Solving problem ...");

    bool sol = g.solve();

    VERBOSE_INFO("Solved, gathering results ...");

    //##################################################################
    // GATHERING RESULTS
    //##################################################################

    // SCHEDULING RESULT
    //******************************************************************
    if (sol) {

    	TIME_UNIT OMEGA = g.getValue("OMEGA");

        std::cout << "OMEGA : " << OMEGA << std::endl ;

        {ForEachVertex(dataflow,t) {
            std::string name = dataflow->getVertexName(t);
            for(EXEC_COUNT k = 1; k <= kvector[t] ; k++) {
                auto starting_time = g.getValue("s_" + commons::toString<EXEC_COUNT>(k) + "_" + name);

                std::cout << "s_" << k  << "_" << name <<  "=" << starting_time
                		<< "  NI=" <<  dataflow->getNi(t)
        	         	<< "  period=" <<  OMEGA / (TIME_UNIT) ( (TIME_UNIT) dataflow->getNi(t) / (TIME_UNIT) kvector[t] )
						<< "  end_of_execution_of_last_instance_of_hyper_period=" <<
						starting_time + dataflow->getVertexDuration(t) + (dataflow->getNi(t)-1)  * ((OMEGA / (TIME_UNIT) ( (TIME_UNIT) dataflow->getNi(t) / (TIME_UNIT) kvector[t] ))) << std::endl ;
            }
        }}


    } else {
        VERBOSE_ERROR("No feasible solution");
    }

    VERBOSE_INFO("Done");
    return;


}
