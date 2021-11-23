/*
 * periodic_fixed.cpp
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#include <cmath>
#include <vector>
#include <commons/verbose.h>
#include <lp/glpsol.h>
#include <models/Dataflow.h>
#include <algorithms/normalization.h>
#include <algorithms/schedulings.h>
#include <algorithms/throughput/kperiodic.h>

#include "periodic_fixed.h"


void algorithms::checkOffsets (models::Dataflow * const dataflow,TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & offsets) {

            VERBOSE_ASSERT(OMEGA > 0 , "unsuable period.");

    {ForEachVertex(dataflow,pTask) {
        EXEC_COUNT max_k = dataflow->getPhasesQuantity(pTask);

        VERBOSE_ASSERT (offsets.count(pTask)  == 1, "Task has no offset") ;
        VERBOSE_ASSERT_EQUALS (offsets[pTask].size()  , max_k) ;

        for (EXEC_COUNT k = 1 ; k <= max_k ; k++) {

            if (offsets[pTask][(int)(k-1)] < 0) {
                VERBOSE_ERROR("Bad offset, Task = " << dataflow->getVertexName(pTask) << " k=" << k << " Value is " << offsets[pTask][(int)(k-1)]  );
                }
            VERBOSE_ASSERT_GreaterEqualThan(offsets[pTask][(int)(k-1)] , 0);

            if (k>1) {
                if (offsets[pTask][(int)(k-1)] < (offsets[pTask][(int)(k-2)] + dataflow->getVertexDuration(pTask,k-1))) {
                    VERBOSE_ERROR("Bad offset, Task = " << dataflow->getVertexName(pTask) << " s(" << k << ")=" << offsets[pTask][(int)(k-1)]  << " s(" << k-1 << ")=" << offsets[pTask][(int)(k-2)]  << " l(" << k-1 << ")=" << dataflow->getVertexDuration(pTask,k-1) << " s(" << k-1 << ") + l(" << k-1  << ")="  << offsets[pTask][(int)(k-2)] + dataflow->getVertexDuration(pTask,k-1) );
                                }
            //   VERBOSE_ASSERT(offsets[pTask][(int)(k-1)] >= (offsets[pTask][(int)(k-2)] + dataflow->getVertexDuration(pTask,k-1)), "negative offset !");

            }
            TIME_UNIT mut = OMEGA  * dataflow->getPhasesQuantity(pTask) / dataflow->getNi(pTask);

            if ((offsets[pTask][(int)(k-1)] + dataflow->getVertexDuration(pTask,k)) > (mut)) {
                VERBOSE_ERROR("mut = " << mut << " end of pases is " << offsets[pTask][(int)(k-1)] + dataflow->getVertexDuration(pTask,k));
            }
            //VERBOSE_ASSERT((offsets[pTask][(int)(k-1)] + dataflow->getVertexDuration(pTask,k)) <= mut, "negative offset !");
        }
    }}


}

BufferSizingResult algorithms::speriodic_memory_sizing_csdf (models::Dataflow* const  dataflow, TIME_UNIT period, bool solve_ilp, bool gen_only) {
    std::map<Vertex,std::vector<TIME_UNIT> > offsets;

    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    VERBOSE_ASSERT (period > 0, "The period must be defined");
    VERBOSE_ASSERT (period != std::numeric_limits<TIME_UNIT>::infinity(), "The period must be defined");

    if (generateStrictlyPeriodicOffsets(dataflow,period,offsets)) {
        checkOffsets(dataflow,period,offsets);
        return compute_periodic_fixed_memory(dataflow, offsets,period, solve_ilp, gen_only);
    } else {
        return BufferSizingResult();
    }
}

void algorithms::compute_strictly_periodic_memory (models::Dataflow* const dataflow, parameters_list_t params) {
        std::map<Vertex,std::vector<TIME_UNIT> > offsets;

        VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");


        TIME_UNIT period    = commons::get_parameter<TIME_UNIT>(params, "PERIOD", 0.0) ;
        bool      solve_ilp = commons::get_parameter<bool>(params, "ILP", false) ;
        bool      gen_only  = commons::get_parameter<bool>(params, "GENONLY", false) ;

        /// BEGIN $$$$$$$$$$$$$$$ This part computes a period if none provided
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
        /// END $$$$$$$$$$$$$$$ This part computed a period if none was provided


        VERBOSE_ASSERT (period > 0, "The period must be defined");
        VERBOSE_ASSERT (period != std::numeric_limits<TIME_UNIT>::infinity(), "The period must be defined");

        generateStrictlyPeriodicOffsets(dataflow,period,offsets);
        checkOffsets(dataflow,period,offsets);
        auto total_buffer_size = compute_periodic_fixed_memory(dataflow, offsets,period, solve_ilp, gen_only).total_size();
        std::cout << "Total buffer size : " << total_buffer_size
                    << " + 2 * " << dataflow->getVerticesCount() << " = "
                    << total_buffer_size + 2 * dataflow->getVerticesCount() << std::endl ;
            std::cout << "SPeriodicSizing size is " << total_buffer_size << std::endl;
}

   void algorithms::add_vbuffers (models::Dataflow* const  dataflow, parameters_list_t params) {
        std::map<Vertex,std::vector<TIME_UNIT> > offsets;
        std::set<ARRAY_INDEX> vbuffers;

        VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
        TIME_UNIT period    = commons::get_parameter<TIME_UNIT>(params, "PERIOD", 0.0) ; // assumes period is available in param list
        VERBOSE_ASSERT (period > 0, "The period must be defined");
        VERBOSE_ASSERT (period != std::numeric_limits<TIME_UNIT>::infinity(), "The period must be defined");
        bool      solve_ilp = commons::get_parameter<bool>(params, "ILP", false) ;
        bool      gen_only  = commons::get_parameter<bool>(params, "GENONLY", false) ;
        generateStrictlyPeriodicOffsets(dataflow,period,offsets);
        checkOffsets(dataflow,period,offsets);
        BufferSizingResult buf_res = compute_periodic_fixed_memory(dataflow, offsets,period, solve_ilp, gen_only);
        dataflow->reset_computation();

        std::vector<ARRAY_INDEX> id_list; // no known way of getting a static edge list
        ForEachEdge(dataflow, e){ // for each virtual buffer, add max token, set (inversed) in and out edge phases, and record new edges in set
            id_list.push_back(dataflow->getEdgeId(e));
        }
        for (auto & id : id_list){
            Edge e = dataflow->getEdgeById(id);
            long buf_size = buf_res.get_edge_size(dataflow->getEdgeId(e));
            Edge v_e = dataflow->addEdge(dataflow->getEdgeTarget(e), dataflow->getEdgeSource(e), "vir_" + dataflow->getEdgeName(e) );
            dataflow->setPreload(v_e, buf_size-dataflow->getPreload(e)); // TODO: useful tokens from preload
            std::vector<TOKEN_UNIT> inphase;
            for (PHASE_INDEX ip = 1; ip <= dataflow->getEdgeOutPhasesCount(e); ip++){
                inphase.push_back(dataflow->getEdgeOutPhase(e, ip));
            }
            dataflow->setEdgeInPhases(v_e, inphase);
            std::vector<TOKEN_UNIT> outphase;
            for (PHASE_INDEX op = 1; op <= dataflow->getEdgeInPhasesCount(e); op++){
                outphase.push_back(dataflow->getEdgeInPhase(e, op));
            }
            dataflow->setEdgeOutPhases(v_e, outphase);
            dataflow->setTokenSize(v_e, dataflow->getTokenSize(e));
            // dataflow->setEdgeName(v_e, dataflow->getEdgeName(e));
            dataflow->setEdgeType(v_e, VIRTUAL_EDGE);
            vbuffers.insert(dataflow->getEdgeId(v_e));
        }

        // return vbuffers;
    }


void algorithms::compute_fixed_offset_buffer_sizing (models::Dataflow* const dataflow, parameters_list_t params){
    std::map<Vertex,std::vector<TIME_UNIT> > offsets;

    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    TIME_UNIT period    = commons::get_parameter<TIME_UNIT>(params, "PERIOD", 0.0) ;
    bool      solve_ilp = commons::get_parameter<bool>(params, "ILP", false) ;
    bool      gen_only  = commons::get_parameter<bool>(params, "GENONLY", false) ;

    std::string mem;
    if (params.count("OFFSET")) {
        std::string offset_type = params["OFFSET"];

        if (offset_type == "BURST"){
            mem = "Burst";
            generateBurstOffsets(dataflow, offsets);
        } else if (offset_type == "AVERAGE"){
            mem = "Average";
            generateAverageOffsets(dataflow, period, offsets);
        } else if (offset_type == "MINMAX"){
            mem = "MinMax";
            generateMinMaxOffsets(dataflow, period, offsets);
        } else if (offset_type == "WIGGERS"){
            mem = "Wiggers";
            generateWiggersOffsets(dataflow, period, offsets);
        } else {
            VERBOSE_ASSERT(false, "Invalid Schedule Paradigm Defined")
        };
    }

    checkOffsets(dataflow,period,offsets);
    auto total_buffer_size = compute_periodic_fixed_memory(dataflow, offsets,period, solve_ilp, gen_only).total_size();

    std::cout << "Total buffer size : " << total_buffer_size
                << " + 2 * " << dataflow->getVerticesCount() << " = "
                << total_buffer_size + 2 * dataflow->getVerticesCount() << std::endl ;
    std::cout << (std::string) mem << " Sizing size is " << total_buffer_size << std::endl;
}


bool algorithms::generateStrictlyPeriodicOffsets  (models::Dataflow * dataflow, TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res) {

    VERBOSE_DEBUG("generateStrictlyPeriodicOffsets");

	   if (OMEGA == 0 ) {
		   return false;
	   }

    res.clear();

    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    {ForEachVertex(dataflow,pTask) {
        EXEC_COUNT max_k = dataflow->getPhasesQuantity(pTask);
        TIME_UNIT shift = OMEGA / dataflow->getNi(pTask) ;

        res.insert(std::pair<Vertex ,  std::vector<TIME_UNIT> > (pTask, std::vector<TIME_UNIT>()));


        VERBOSE_DEBUG("Task " << dataflow->getVertexName(pTask) << " max_k=" << max_k << " shift=" << shift);

        if (max_k == 1) {
            res.at(pTask).push_back(0);
        } else {
            for (EXEC_COUNT k = 1 ; k <= max_k ; k++) {
                TIME_UNIT newtime = (k -1) * shift;
                VERBOSE_DEBUG("task =" << dataflow->getVertexName(pTask) <<  " k=" << k << " offset = " << newtime);

                if ( dataflow->getVertexDuration(pTask,k)  > shift ) {
                    VERBOSE_DEBUG(" END BECAUSE " << dataflow->getVertexDuration(pTask,k) << " <= " << shift);
                    return false;
                }

                res.at(pTask).push_back(newtime);
            }
        }
    }}
    return true;
}



bool algorithms::generateBurstOffsets(models::Dataflow * const dataflow,std::map<Vertex,std::vector<TIME_UNIT> > & res) {
    //recuperer les transition


    res.clear();

    {ForEachVertex(dataflow,pTask) {
        EXEC_COUNT max_k = dataflow->getPhasesQuantity(pTask);
        res.insert(std::pair<Vertex ,  std::vector<TIME_UNIT> > (pTask, std::vector<TIME_UNIT>()));
        TIME_UNIT val = 0;
        for (EXEC_COUNT k = 1 ; k <= max_k ; k++) {

            if (k > 1) val += dataflow->getVertexDuration(pTask,k-1);
            res[pTask].push_back(val);

        }
    }}
    return true;
}


bool algorithms::generateAverageOffsets(models::Dataflow * dataflow, TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res) {

    VERBOSE_ASSERT(OMEGA > 0 , "unsuable period.");
    res.clear();

    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    {ForEachVertex(dataflow,pTask) {
        EXEC_COUNT max_k = dataflow->getPhasesQuantity(pTask);
        TIME_UNIT muti = OMEGA  * dataflow->getPhasesQuantity(pTask) / dataflow->getNi(pTask);
        VERBOSE_ASSERT(muti > 0 , "unsuable period.");
        TIME_UNIT sum = 0;
        TIME_UNIT lti = 0;
        for (EXEC_COUNT k = 1 ; k <= max_k ; k++) {
                lti += dataflow->getVertexDuration(pTask,k)  ;
        }

        res.insert(std::pair<Vertex ,  std::vector<TIME_UNIT> > (pTask, std::vector<TIME_UNIT>()));

        if (max_k == 1) {
            res.at(pTask).push_back(0);
        } else {
            for (EXEC_COUNT k = 1 ; k <= max_k ; k++) {
                if (k > 1) sum += dataflow->getVertexDuration(pTask,k-1)  ;

                TIME_UNIT newtime = sum *  (muti / lti);
                if (lti == 0) newtime = 0; // SUPERFIX
                VERBOSE_DEBUG("task =" << dataflow->getVertexName(pTask) <<  " k=" << k << " offset = " << newtime);
                res.at(pTask).push_back(newtime);
            }
        }
    }}
    return true;
}


bool algorithms::generateWiggersOffsets(models::Dataflow * dataflow, TIME_UNIT PERIOD, std::map<Vertex,std::vector<TIME_UNIT> > & res){

    //recuperer les transition
    res.clear();


    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);



    // STEP 0 - Need the repetition vector
    VERBOSE_INFO("STEP 0 :  Need the repetition vector");
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");


    std::map<std::pair<Vertex,EXEC_COUNT>,TIME_UNIT> s;

    std::map<Vertex,TIME_UNIT> mu;
    {ForEachVertex(dataflow,vi){
        mu[vi] = (PERIOD / dataflow->getNi(vi)) * dataflow->getPhasesQuantity(vi);
        VERBOSE_DEBUG( "Mu_" << dataflow->getVertexName(vi) << " = " << mu[vi]);
    }}

    // STEP 1 - Generate BURST EXECUTION
    VERBOSE_INFO("STEP 1 :  Burst execution and alpha + lambda computation ");
    {ForEachVertex(dataflow,vi){
        std::string verbose_line = dataflow->getVertexName(vi) + std::string(" s = 0 ");
        const  EXEC_COUNT maxk = dataflow->getPhasesQuantity(vi);
        s[std::make_pair(vi,1)] = 0;
        for(EXEC_COUNT k = 2 ; k <= maxk;k++){
            s[std::make_pair(vi,k)] = s[std::make_pair(vi,k-1)] + dataflow->getVertexDuration(vi,k-1);
            verbose_line += std::string(",") + commons::toString(s[std::make_pair(vi,k)]);
        }
        VERBOSE_DEBUG(verbose_line);
    }}


    std::map<Edge,TOKEN_UNIT> lambda_prod;
    std::map<Edge,TOKEN_UNIT> lambda_cons;
    std::map<Edge,TIME_UNIT>  alpha;

    {ForEachEdge(dataflow,e) {

        TOKEN_UNIT lambda_p = dataflow->getEdgeIn(e);
        TOKEN_UNIT lambda_c = dataflow->getEdgeOut(e);

        const EXEC_COUNT max_ki = dataflow->getEdgeInPhasesCount(e);
        const EXEC_COUNT max_kj = dataflow->getEdgeOutPhasesCount(e);

        const Vertex vi = dataflow->getEdgeSource(e);
        const TIME_UNIT alpha_e = (TIME_UNIT) dataflow->getEdgeIn(e) / mu[vi] ; // token by sec


        for (EXEC_COUNT kj = 1 ; kj <= max_kj ; kj++) {
            lambda_p = std::gcd(lambda_p , dataflow->getEdgeOutPhase(e,kj));
        }
        for (EXEC_COUNT ki = 1 ; ki <= max_ki ; ki++) {
            lambda_c = std::gcd(lambda_c , dataflow->getEdgeInPhase(e,ki));
        }

        lambda_prod[e]      = lambda_p;
        lambda_cons[e]      = lambda_c;
        alpha[e]            = alpha_e;
        VERBOSE_DEBUG(dataflow->getEdgeName(e) << " : Alpha = " <<  alpha_e << " Lambda in/out = " << lambda_p << "/" << lambda_c);
    }}

    // Lambda and alpha  : Done

    // STEP 2 - compute p_l^e(t) = \alpha_e + \Beta_e^p (courbe production)
    VERBOSE_INFO("STEP 2 : production Beta_e^p computation.");
    std::map<Edge,TIME_UNIT> beta_prod;

    {ForEachEdge(dataflow,e) {
        const TIME_UNIT alpha_e   = alpha[e];
        const TOKEN_UNIT lambda_p = lambda_prod[e];
        const Vertex vi           = dataflow->getEdgeSource(e);
        const EXEC_COUNT max_ki   = dataflow->getEdgeInPhasesCount(e);

        TIME_UNIT  beta_e = std::numeric_limits<TIME_UNIT>::infinity();
        TOKEN_UNIT cumul_production = 0;

        for (EXEC_COUNT ki = 1 ; ki <= max_ki ; ki++) {
            VERBOSE_DEBUG("Check production at " << ki << " of " << cumul_production << "+" << dataflow->getEdgeInPhase(e,ki));
            cumul_production += dataflow->getEdgeInPhase(e,ki);
            TOKEN_UNIT level = commons::ceil(cumul_production - dataflow->getEdgeInPhase(e,ki),lambda_p) + 1;
            if (cumul_production >= lambda_p) {
                VERBOSE_DEBUG(" -> Good point is " << lambda_p << " ; " << s[std::make_pair(vi,ki)] + dataflow->getVertexDuration(vi,ki) );
                beta_e = std::min ( beta_e ,
                                    (TIME_UNIT)  level - alpha_e * (s[std::make_pair(vi,ki)]  + dataflow->getVertexDuration(vi,ki))
                                    );
                VERBOSE_DEBUG(" -> " << dataflow->getEdgeName(e) << " :  Beta prod = " <<  beta_e);
                break;
            } else {
                VERBOSE_DEBUG(" -> Skip it ");
            }
        }
        beta_prod[e]=beta_e;
        VERBOSE_ASSERT(beta_prod[e]==beta_e,TXT_NEVER_HAPPEND);
    }}



       //step 3 - retiming of starting times
    VERBOSE_INFO("STEP 3 : Retiming.");

    std::map<std::pair<Vertex,EXEC_COUNT>,TIME_UNIT> g; // new start time
    std::map<std::pair<Vertex,EXEC_COUNT>,TIME_UNIT> h; // new end time


    {ForEachVertex(dataflow,vi){

        VERBOSE_DEBUG("Retiming of " << dataflow->getVertexName(vi));
        const EXEC_COUNT maxk = dataflow->getPhasesQuantity(vi);



        h[std::make_pair(vi,maxk)] = mu[vi];
        //{ForOutputEdges(dataflow,vi,e) {
        //    h[std::make_pair(vi,maxk)] = std::min(h[std::make_pair(vi,maxk)], commons::division(commons::ceil(dataflow->getEdgeIn(e) - dataflow->getEdgeInPhase(e,maxk) + 1,lambda_prod[e]) - beta_prod[e],alpha[e]) );
        //}}
        g[std::make_pair(vi,maxk)] = h[std::make_pair(vi,maxk)] - dataflow->getVertexDuration(vi,maxk);
        VERBOSE_DEBUG("Task_duration("<< dataflow->getVertexName(vi) << ","<< maxk << ") = "<< dataflow->getVertexDuration(vi,maxk));
        VERBOSE_DEBUG("burst_start(" << dataflow->getVertexName(vi) << ","<< maxk << ") = " << s[std::make_pair(vi,maxk)]);
        VERBOSE_DEBUG("burst_end(" << dataflow->getVertexName(vi) << ","<< maxk << ") = " << s[std::make_pair(vi,maxk)] + dataflow->getVertexDuration(vi,maxk));
        VERBOSE_DEBUG("g(" << dataflow->getVertexName(vi) << ","<< maxk << ") = " << g[std::make_pair(vi,maxk)]);
        VERBOSE_DEBUG("h(" << dataflow->getVertexName(vi) << ","<< maxk << ") = " << h[std::make_pair(vi,maxk)]);

        for(EXEC_COUNT k = (maxk - 1); k >= 1;k--){
            // print ax + b
            // print h(v)
            h[std::make_pair(vi,k)] = g[std::make_pair(vi,k+1)];
            VERBOSE_DEBUG("Task_duration("<< dataflow->getVertexName(vi) << ","<< k << ") = "<< dataflow->getVertexDuration(vi,k));
            VERBOSE_DEBUG("burst_start(" << dataflow->getVertexName(vi) << ","<< k << ") = " << s[std::make_pair(vi,k)]);
            VERBOSE_DEBUG("burst_end(" << dataflow->getVertexName(vi) << ","<< k << ") = " << s[std::make_pair(vi,k)] + dataflow->getVertexDuration(vi,k));

            VERBOSE_DEBUG("     compute h(" << dataflow->getVertexName(vi) << ","<< k << ") = " << h[std::make_pair(vi,k)]);
        if ((h[std::make_pair(vi,k)] - dataflow->getVertexDuration(vi,k)) < s[std::make_pair(vi,k)]) {
        VERBOSE_ERROR("start is too early...");
        VERBOSE_FAILURE();
        }
            {ForOutputEdges(dataflow,vi,e) {
                TOKEN_UNIT cumul_production_km1 = 0;
                for (EXEC_COUNT subk = 1 ; subk < k ; subk++) cumul_production_km1 += dataflow->getEdgeInPhase(e,subk);

                const TOKEN_UNIT m_e_k = commons::ceil(cumul_production_km1 + 1,lambda_prod[e]); // relevant token size
                const TIME_UNIT bound = commons::division(m_e_k - beta_prod[e],alpha[e]);
                VERBOSE_DEBUG("   bound computation is : " << m_e_k <<"-"<< beta_prod[e] << "/" << alpha[e]);
                VERBOSE_DEBUG("     compute h(" << dataflow->getVertexName(vi) << ","<< k << ") = min (h=" << h[std::make_pair(vi,k)] << ",b=" << bound << ")");
                h[std::make_pair(vi,k)] = std::min (h[std::make_pair(vi,k)] , bound);
        if ((bound - dataflow->getVertexDuration(vi,k)) < s[std::make_pair(vi,k)]) {
        VERBOSE_ERROR("start with bound " <<bound << " is too early...");   
            h[std::make_pair(vi,k)] = std::max (h[std::make_pair(vi,k)] ,s[std::make_pair(vi,k)] + dataflow->getVertexDuration(vi,k) );
            
        }

            }}


            g[std::make_pair(vi,k)] = h[std::make_pair(vi,k)]  - dataflow->getVertexDuration(vi,k);
            VERBOSE_DEBUG("g(" << dataflow->getVertexName(vi) << ","<< k << ") = " << g[std::make_pair(vi,k)]);
            VERBOSE_DEBUG("h(" << dataflow->getVertexName(vi) << ","<< k << ") = " << g[std::make_pair(vi,k)]+ dataflow->getVertexDuration(vi,k));

        }
    }}

    {ForEachVertex(dataflow,vi){
        const EXEC_COUNT maxk = dataflow->getPhasesQuantity(vi);

        res.insert(std::pair<Vertex ,  std::vector<TIME_UNIT> > (vi, std::vector<TIME_UNIT>()));
        if (maxk == 1 ) {
            res.at(vi).push_back(0);
        } else {
            for (EXEC_COUNT k = 1; k <= maxk;k++) {
                VERBOSE_DEBUG("g(" << dataflow->getVertexName(vi) << ","<< k << ") = " << g[std::make_pair(vi,k)]);

                res.at(vi).push_back(g[std::make_pair(vi,k)]);
            }
        }
    }}

    return true;
}

bool algorithms::generateMinMaxOffsets(models::Dataflow * dataflow, TIME_UNIT OMEGA, std::map<Vertex,std::vector<TIME_UNIT> > & res){

    //recuperer les transition
    res.clear();

    bool minmaxwork = true;

    {ForEachVertex(dataflow,pTask) {
        res.insert(std::pair<Vertex ,  std::vector<TIME_UNIT> > (pTask, std::vector<TIME_UNIT>()));
        // Solve MinMax for this task
        minmaxwork = minmaxwork && ctnewMinMax(dataflow,OMEGA, pTask,res.at(pTask));

    }}
    return minmaxwork;
}



   bool algorithms::ctnewMinMax(models::Dataflow * dataflow, TIME_UNIT OMEGA, Vertex t, std::vector<TIME_UNIT>& res) {

       VERBOSE_INFO("generateMinMaxOffsets : Begin");
       /********************************************** PREPARE RES**************************************************/
       res.clear();
       /**************************************** DONT'COMPUTE SDF TASK **********************************************/

       if (dataflow->getPhasesQuantity(t) <= 1) { // Ct(1) = 0
           res.push_back(0);
           VERBOSE_INFO("generateMinMaxOffsets : Skip");
           return true;
       }


       std::map<Edge,TOKEN_UNIT> lambda_prod;
       std::map<Edge,TOKEN_UNIT> lambda_cons;

       {ForEachEdge(dataflow,e) {

           TOKEN_UNIT lambda_p = dataflow->getEdgeIn(e);
           TOKEN_UNIT lambda_c = dataflow->getEdgeOut(e);

           const EXEC_COUNT max_ki = dataflow->getEdgeInPhasesCount(e);
           const EXEC_COUNT max_kj = dataflow->getEdgeOutPhasesCount(e);

           for (EXEC_COUNT kj = 1 ; kj <= max_kj ; kj++) {
               lambda_p = std::gcd(lambda_p , dataflow->getEdgeOutPhase(e,kj));
           }
           for (EXEC_COUNT ki = 1 ; ki <= max_ki ; ki++) {
               lambda_c = std::gcd(lambda_c , dataflow->getEdgeInPhase(e,ki));
           }

           lambda_prod[e]      = lambda_p;
           lambda_cons[e]      = lambda_c;
       }}


       VERBOSE_INFO("generateMinMaxOffsets : Real Begin");

       /********************************************** PREPARE LP ***************************************************/


       // on considere une tache 't'.

       const std::string taskName    = dataflow->getVertexName(t);
       const std::string problemName = "CT_MinMax_" + taskName;
       const EXEC_COUNT  max_k       = dataflow->getPhasesQuantity(t);

       const TIME_UNIT   mut         = OMEGA  * ((TIME_UNIT) max_k / (TIME_UNIT) dataflow->getNi(t));

       VERBOSE_DEBUG("TASKNAME=" << taskName );
       VERBOSE_DEBUG(" OMEGA=" << OMEGA <<" Phi=" << max_k <<" Fi=" << dataflow->getNi(t)  <<" Ni=" << dataflow->getNi(t) / max_k );

       const TIME_UNIT   ltphi       = dataflow->getVertexDuration(t,max_k);

       //   1 . definition du programme lineaire
       commons::GLPSol g = commons::GLPSol(problemName,commons::MIN_OBJ);

       //     a - Objectif Min Sommes des Phi_a * delta_a + Phi_a_prime * delta_a_prime , ( Phi_a  is tokenSize)

       {ForConnectedEdges(dataflow,t,pChannel) {

           const std::string   channelName = dataflow->getEdgeName(pChannel);
           const DATA_UNIT     Phi         =  dataflow->getTokenSize(pChannel);

           g.addColumn("D_" + channelName,commons::KIND_CONTINUE,commons::bound_s(),(double)Phi);
           g.addColumn("E_" + channelName,commons::KIND_CONTINUE,commons::bound_s(),(double)Phi);

       }}

       // definition des Ct

       // F : ct_1 = 0
       g.addColumn("ct_1",commons::KIND_CONTINUE,commons::bound_s(commons::FIX_BOUND,0),0);


       // G : ct(k) - ct(k-1) >= lt(k-1)
       for (EXEC_COUNT k = 2 ; k <= max_k ; k++) {
           const std::string   columnName  = "ct_" + commons::toString<EXEC_COUNT>(k);
           g.addColumn(columnName,commons::KIND_CONTINUE,commons::bound_s(),0);
           const std::string   row_name             = "ct_" + commons::toString<EXEC_COUNT>(k);
           const std::string   ctkcolumnName       = "ct_" + commons::toString<EXEC_COUNT>(k);
           const std::string   ctkmoins1columnName = "ct_" + commons::toString<EXEC_COUNT>(k-1);
           const TIME_UNIT        ltikmoins1          = dataflow->getVertexDuration(t,k-1);

           g.addRow(row_name,commons::bound_s(commons::LOW_BOUND,ltikmoins1));
           g.addCoef(row_name,ctkcolumnName      ,   1  );
           g.addCoef(row_name,ctkmoins1columnName,  -1  );
       }


       // H : ct(phi) + lt(phi) <= mu
       {
           const std::string   row_name             = "ct_phi";
           const std::string   ctPhicolumnName     = "ct_" + commons::toString<EXEC_COUNT>(max_k);
           g.addRow(row_name,commons::bound_s(commons::UP_BOUND,mut - ltphi));
           g.addCoef(row_name,ctPhicolumnName      ,   1  );
       }


       // A : definition des alpha


       {ForOutputEdges(dataflow,t,pChannel) {
	   


           const std::string   channelName      = dataflow->getEdgeName(pChannel);
           const TOKEN_UNIT        wa           = dataflow->getEdgeIn(pChannel);

           const TOKEN_UNIT        gcdaout      = lambda_prod[pChannel];
           const TOKEN_UNIT        gcdpaout     = lambda_cons[pChannel];
	   VERBOSE_DEBUG("Output channel " << channelName << " gcda=" << gcdaout << " gcdpa=" << gcdpaout );
           const TIME_UNIT        mutwa         = commons::division(mut,wa);
	   VERBOSE_DEBUG("Output channel " << channelName << " mut=" << mut << " wa=" << wa << " and mutwa = " << mutwa  );
           // considération du buffer réél (ai)

           TOKEN_UNIT Daoutpluspred = 0;

           for (EXEC_COUNT k = 1 ; k <= max_k ; k++) {

               const TOKEN_UNIT Daoutpmoins = Daoutpluspred +  dataflow->getEdgeInPhase(pChannel,k);


               const std::string  deltaRowName       = "delta_" + channelName + "_"+ commons::toString<EXEC_COUNT>(k);
               const std::string  epsilonRowName     = "epsilon_" + channelName + "_"+ commons::toString<EXEC_COUNT>(k);
               const std::string  deltaColumnName    = "D_" + channelName;
               const std::string  epsilonColumnName  = "E_" + channelName;
               const std::string  ctkcolumnName      = "ct_" + commons::toString<EXEC_COUNT>(k);
               const TIME_UNIT       ltk                = dataflow->getVertexDuration(t,k);
               const TOKEN_UNIT quotaoutk    = commons::floor ( - Daoutpluspred  -1, gcdaout   );
               const TOKEN_UNIT quotpaoutpk  = commons::floor (    Daoutpmoins   -1,  gcdaout );

               // As real output
               // Ctk + ltk + mut/wa1 * quotaik <= DELTAai
               // Ctk - DELTAai <= - ltk - mut/wa1 * quotaik

	       TOKEN_UNIT delta_bound   = (gcdaout > gcdpaout) ? quotaoutk  : - Daoutpluspred ;
	       TOKEN_UNIT epsilon_bound = (gcdaout > gcdpaout) ? quotpaoutpk :   Daoutpmoins  ;
	       VERBOSE_DEBUG(" k = " << k);
	       VERBOSE_DEBUG("  - PredDa+=" << Daoutpluspred << " Da'-=" << Daoutpmoins);
	       VERBOSE_DEBUG("  - wa(k) = " <<  dataflow->getEdgeInPhase(pChannel,k));
	       VERBOSE_DEBUG("  - quot(a,k)=" << quotaoutk << "    quot'(a',k)=" << quotpaoutpk);	       
	       VERBOSE_DEBUG("  - Real buffer bound is :  Ct(k) + lt(k) + (mut/wa1) * " << delta_bound);
	       VERBOSE_DEBUG("  - Fake buffer bound is : -Ct(k)         + (mut/wa1) * " << epsilon_bound);

               g.addRow(deltaRowName,commons::bound_s(commons::UP_BOUND, -ltk - mutwa * (TIME_UNIT) delta_bound));
               g.addCoef(deltaRowName,deltaColumnName             ,  - 1  );
               g.addCoef(deltaRowName,ctkcolumnName               ,    1  );

               // As fake input
               // - Ctk + mut/wa1 * quotpaipkp <= EPSILONaip
               // - Ctk - EPSILONaip <= - mut/wa1 * quotpaipkp

               g.addRow(epsilonRowName,commons::bound_s(commons::UP_BOUND, - mutwa * (TIME_UNIT) epsilon_bound));
               g.addCoef(epsilonRowName,epsilonColumnName           ,  - 1  );
               g.addCoef(epsilonRowName,ctkcolumnName               ,  - 1  );

               Daoutpluspred += dataflow->getEdgeInPhase(pChannel,k);

           }




       }}

       {ForInputEdges(dataflow,t,pChannel) {

           const std::string   channelName = dataflow->getEdgeName(pChannel);
           const TOKEN_UNIT        wa          = dataflow->getEdgeOut(pChannel);
           const TOKEN_UNIT        gcdain         = lambda_cons[pChannel];
           const TOKEN_UNIT        gcdpain        = lambda_prod[pChannel];
           const TIME_UNIT       mutwa = commons::division(mut,wa);
           TOKEN_UNIT Dainppluspred = 0;

           for (EXEC_COUNT k = 1 ; k <= max_k ; k++) {

               const TOKEN_UNIT Dainmoins = Dainppluspred +  dataflow->getEdgeOutPhase(pChannel,k);

               const std::string  deltaRowName       = "delta_" + channelName + "_"+ commons::toString<EXEC_COUNT>(k);
               const std::string  epsilonRowName     = "epsilon_" + channelName + "_"+ commons::toString<EXEC_COUNT>(k);
               const std::string  deltaColumnName    = "D_" + channelName;
               const std::string  epsilonColumnName  = "E_" + channelName;
               const std::string  ctkcolumnName      = "ct_" + commons::toString<EXEC_COUNT>(k);
               const TIME_UNIT       ltk             = dataflow->getVertexDuration(t,k);


               const TOKEN_UNIT quotaink                     = commons::floor (- Dainppluspred  , gcdpain );

               const TOKEN_UNIT quotpainpk                    = commons::floor (Dainmoins , gcdpain );


	       TOKEN_UNIT delta_bound   = (gcdain > gcdpain) ? quotpainpk  : - Dainppluspred ;
	       TOKEN_UNIT epsilon_bound = (gcdain > gcdpain) ? quotaink  :    Dainmoins ;

	       VERBOSE_DEBUG(" k = " << k);
	       VERBOSE_DEBUG("  - PredDa+=" << Dainppluspred << " Da'-=" << Dainmoins);
	       VERBOSE_DEBUG("  - va(k) = " <<  dataflow->getEdgeOutPhase(pChannel,k));
	       VERBOSE_DEBUG("  - quot(a,k)=" << quotaink << "    quot'(a',k)=" << quotpainpk);	       
	       VERBOSE_DEBUG("  - Real buffer bound is :  Ct(k) + lt(k) + (mut/wa1) * " << delta_bound);
	       VERBOSE_DEBUG("  - Fake buffer bound is : -Ct(k)         + (mut/wa1) * " << epsilon_bound);


               // As fake input
               // Ctk + ltk + mut/va1 * quotainpk <= DELTAainp
               // Ctk - DELTAainp <= - ltk - mut/va1 * quotainpk
               g.addRow(deltaRowName,commons::bound_s(commons::UP_BOUND, -ltk - mutwa * (TIME_UNIT)delta_bound));
               g.addCoef(deltaRowName,deltaColumnName           ,  - 1  );
               g.addCoef(deltaRowName,ctkcolumnName             ,    1  );


               // As real input
               // - Ctk + mut/va1 * quotpaink <= EPSILONpain
               // - Ctk - EPSILONpain          <=  - mut/va1 * quotpaink
               g.addRow(epsilonRowName,commons::bound_s(commons::UP_BOUND,  - mutwa * (TIME_UNIT) epsilon_bound));
               g.addCoef(epsilonRowName,epsilonColumnName             ,  - 1  );
               g.addCoef(epsilonRowName,ctkcolumnName               ,   - 1  );





               Dainppluspred += dataflow->getEdgeOutPhase(pChannel,k);


           }

       }}


       VERBOSE_INFO("generateMinMaxOffsets : Run ILP");
       /********************************************** SOLVE LP ***************************************************/

       bool sol = g.solve();


       if (sol) {

           /********************************************** GET RESULTS ***************************************************/

           for (EXEC_COUNT k = 1 ; k <= max_k ; k++) {
               const std::string   columnName  = "ct_" + commons::toString<EXEC_COUNT>(k);
               res.push_back(g.getValue(columnName));
           }

       } else {
           return false;
       }
       return true;
   }






    BufferSizingResult algorithms::compute_periodic_fixed_memory (models::Dataflow* const  dataflow, std::map<Vertex,std::vector<TIME_UNIT> > & offsets,  TIME_UNIT PERIOD, bool ilp_solving, bool gen_only){

    BufferSizingResult res;
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
        const std::string problemName =  "PeriodicSizingFixed_" + dataflow->getGraphName() + "_" + commons::toString(FREQUENCY) + ((CONTINUE_OR_INTEGER == commons::KIND_INTEGER) ? "_INT" : "");
        commons::GLPSol g = commons::GLPSol(problemName,commons::MIN_OBJ);


        // Starting times
        //******************************************************************
        {ForEachVertex(dataflow,pVertex) {
            std::string name = dataflow->getVertexName(pVertex);
            g.addColumn("s_" + name,commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),0);
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
             g.addColumn(mo_name,CONTINUE_OR_INTEGER,commons::bound_s(commons::FIX_BOUND,dataflow->getPreload(c)),(double)dataflow->getTokenSize(c));
             g.addColumn(feedback_mo_name,CONTINUE_OR_INTEGER,commons::bound_s(commons::LOW_BOUND,0),(double)dataflow->getTokenSize(c));

             // init value for ai = 0
             wai    = 0;
             cwai   = 0;
             cwaim1 = 0;

             for(EXEC_COUNT ai = 1; ai <= dataflow->getPhasesQuantity(source) ; ai++) {

                 const TIME_UNIT       ltai    = dataflow->getVertexDuration(source,ai);
                 const TIME_UNIT       offsetai= offsets[source][(int)(ai-1)];

                 // update wai and cwai (new execution of ti)
                 wai = dataflow->getEdgeInPhase(c,ai);
                 cwai += wai;

                 // init value for aj = 0
                 raj    = 0;
                 craj   = 0;
                 crajm1 = 0;

                 for(EXEC_COUNT  aj = 1; aj <= dataflow->getPhasesQuantity(target) ; aj++) {

                     const TIME_UNIT       ltaj    = dataflow->getVertexDuration(target,aj);
                     const TIME_UNIT       offsetaj= offsets[target][(int)(aj-1)];

                     // update raj and craj (new execution of tj)
                     raj = dataflow->getEdgeOutPhase(c,aj);
                     craj += raj;

                     // *** Normal Buffer constraint computation
                     const TOKEN_UNIT  Ha        =   std::max((TOKEN_UNIT)0, wai - raj);
                     const TOKEN_UNIT  alphamin  =   commons::ceil(Ha + craj - cwai - mop,gcdz);
                     const TOKEN_UNIT  alphamax  =   commons::floor(  craj - cwaim1 - mop - 1 ,gcdz);


                     if (alphamin <= alphamax) { // check if contraint exist
                          const std::string pred_row_name = "precedence_" + buffername + "_" + commons::toString<EXEC_COUNT>(ai) + "_" + commons::toString<EXEC_COUNT>(aj);
                          TIME_UNIT coef = offsetai - offsetaj + ltai + (TIME_UNIT) alphamax * (TIME_UNIT) ( mu_i /  (TIME_UNIT) Zi);
                          VERBOSE_DEBUG("LP : s_" << targetStr << " - " << "s_" << sourceStr << " >= " << offsetai << " - " << offsetaj << " + " << ltai << " + " << (TIME_UNIT) alphamax << " * (" << mu_i << "/" << Zi <<  ")");
                          VERBOSE_DEBUG("     s_" << targetStr << " - " << "s_" << sourceStr << " >= " << coef);

                          g.addRow(pred_row_name,commons::bound_s(commons::LOW_BOUND, (double) coef));
                          if ((source != target)) {
                                   g.addCoef(pred_row_name ,"s_" + targetStr    ,  1        );
                                   g.addCoef(pred_row_name ,"s_" + sourceStr    , -1        );
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
                         TIME_UNIT       coef     =  ltaj - offsetai + offsetaj  ;
                         //VERBOSE_DEBUG(" add feedback coef, " << ltaj << " + " << (TIME_UNIT)  (quot - gcdz) << " * " << mu_j << "=" << coef);

                           g.addColumn(local_mo_name,CONTINUE_OR_INTEGER,commons::bound_s(),0);
                           // main constraint
                           g.addRow(pred_row_name  ,commons::bound_s(commons::LOW_BOUND,coef));
                           g.addCoef(pred_row_name ,local_mo_name                     , -   (double) ((double) mu_j /  (double) Zj) * (double) gcdz );

                           if ((source != target)) {
                                  g.addCoef(pred_row_name ,"s_" +  sourceStr    ,  1        );
                                  g.addCoef(pred_row_name ,"s_" +  targetStr    , -1        );
                           }
                           VERBOSE_DEBUG("LP : s_"  <<  sourceStr  << " - " << "s_"  << targetStr  << " >= " << ltaj<< "-"<< offsetai << "+"<< offsetaj  << " + f * " << gcdz <<  " *(" << mu_j << "/" << Zj <<  ")");
                           VERBOSE_DEBUG("     s_"  <<  sourceStr  << " - " << "s_"  << targetStr  << " - f * " << (TIME_UNIT) ( mu_j /  (TIME_UNIT) Zj) * gcdz  << " >= " << coef);


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
       	return 0;
       }


       VERBOSE_INFO("[CSDF 1-periodic with fixed start times]  Start LP");


       bool sol = g.solveWith();


       VERBOSE_INFO("[CSDF 1-periodic with fixed start times]  LP Finished");

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

                //TOKEN_UNIT from_integer_mop          = commons::floor(g.getIntegerValue(feedback_mo_name),1);

                 TOKEN_UNIT buffersize =  feedbackmopmax  + dataflow->getPreload(c);
                 res.add_edge_size(dataflow->getEdgeId(c), buffersize);
                 total_buffer_size += buffersize * dataflow->getTokenSize(c);
                 VERBOSE_INFO(dataflow->getEdgeName(c) << " :" << g.getValue(feedback_mo_name) << " + " << dataflow->getPreload(c) << " -> " << feedbackmopmax << " + " << dataflow->getPreload(c)<< " = " << buffersize);

             }}

             VERBOSE_INFO("Loopback buffers : " << dataflow->getVerticesCount());

             res.set_validity(true);
             res.set_total_size(total_buffer_size);
        	 return res;
       } else {
           VERBOSE_ERROR("No feasible solution");
       }
       return BufferSizingResult();


   }
