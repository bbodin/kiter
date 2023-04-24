//
// Created by toky on 24/4/23.
//

#ifndef KITER_LIVENESSMODULARDSE_H
#define KITER_LIVENESSMODULARDSE_H


#include <algorithms/dse/kperiodic.h>
#include <algorithms/dse/ModularDSE.h>

void solve_liveness   (models::Dataflow* const  dataflow, parameters_list_t params) ;

ADD_TRANSFORMATION(LivenessDSE,
                   transformation_t({ "LivenessDSE" , "DSE that solve the liveness problem for a graph with \"feedback\" buffers", solve_liveness}));


#endif //KITER_LIVENESSMODULARDSE_H
