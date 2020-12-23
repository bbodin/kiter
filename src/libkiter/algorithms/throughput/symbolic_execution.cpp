/*
 * symbolic_execution.cpp
 *
 *  Created on: 15 December 2020
 *      Author: jkmingwen
 */

#include <map>
#include <vector>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <commons/commons.h>
#include <models/Dataflow.h>
#include "symbolic_execution.h"
#include "actor.h"

// TODO: Functions to add:
// (0) Actor class
// - Attributes: current phase, number of phases, execution rates (indexed by phase), status (currently firing)
// - Methods: advance phase, execute
// (1) Print status

void algorithms::compute_asap_throughput(models::Dataflow* const dataflow,
                                         parameters_list_t param_list) {
  VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
  VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

  // initialise actors
  std::map<ARRAY_INDEX, Actor> actorMap;
  {ForEachTask(dataflow, t) {
      actorMap[dataflow->getVertexId(t)] = Actor(dataflow, t);
      std::cout << std::endl;
    }}
  // testing functions
  printStatus(dataflow);
  {ForEachTask(dataflow, t) {
      if (actorMap[dataflow->getVertexId(t)].isReadyForExec(dataflow)) {
        std::cout << "Actor " << dataflow->getVertexName(t) << " ready for execution" << std::endl;
        std::cout << "executing..." << std::endl;
        actorMap[dataflow->getVertexId(t)].execute(dataflow);
        printStatus(dataflow);
        actorMap[dataflow->getVertexId(t)].printStatus(dataflow);
      } else {
        std::cout << "Actor " << dataflow->getVertexName(t) << " NOT ready for execution" << std::endl;
        printStatus(dataflow);
        actorMap[dataflow->getVertexId(t)].printStatus(dataflow);
      }
      std::cout << std::endl;
    }}
}

// prints current status of dataflow graph
void algorithms::printStatus(models::Dataflow* const dataflow) {
  std::cout << "Token counts:" << std::endl;
  {ForEachEdge(dataflow, e) {
      std::cout << "\tChannel " << dataflow->getEdgeName(e) << " ("
                << dataflow->getVertexName(dataflow->getEdgeSource(e))
                << "->"
                << dataflow->getVertexName(dataflow->getEdgeTarget(e))
                << "): " << dataflow->getPreload(e) << std::endl;
    }}
  // std::cout << "Actor " << dataflow->getVertexName(actor) << std::endl;
  // std::cout << "rep factor (" << dataflow->getVertexName(actor)
  //           << "): " << dataflow->getNi(actor) << std::endl;
  // {ForOutputEdges(dataflow, actor, e) {
  //     std::cout << "initial tokens in output channel: " << dataflow->getPreload(e) << std::endl;
  //     std::cout << "output port execution rates (phases = " << dataflow->getEdgeInPhasesCount(e) <<"): ";
  //     {ForEachPhase(dataflow, actor, p) {
  //           std::cout << dataflow->getEdgeInPhase(e, p) << " ";
  //         }}
  //     std::cout << std::endl;
  //     std::cout << "Current phase token prod: " << dataflow->getEdgeInPhase(e, 1) << std::endl;
  //   }}
  // {ForInputEdges(dataflow, actor, e) {
  //     std::cout << "initial tokens in input channel: " << dataflow->getPreload(e) << std::endl;
  //     std::cout << "input port execution rates: ";
  //     {ForEachPhase(dataflow, actor, p) {
  //           std::cout << dataflow->getEdgeOutPhase(e, p) << " ";
  //         }}
  //     std::cout << "Current phase token cons: " << dataflow->getEdgeOutPhase(e, 1) << std::endl;
  //   }}
}
