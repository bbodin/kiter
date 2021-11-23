/*
 * actor.cpp
 *
 *  Created on: 22 December 2020
 *      Author: jkmingwen
 *  Modified: June 2021 by katwinkl3
 */

#include "actor.h"
#include <models/Dataflow.h>

Actor::Actor()
  :actor{},
   numExecs{0}, // NOTE this might not be necessary as we only need to track numExecs for actor with lowest repetition factor
   phaseCount{0},
   repFactor{0},
   id{},
   isExecuting{false},
   consPhaseCount(),
   prodPhaseCount(),
   prodExecRate(),
   consExecRate() {}

Actor::Actor(models::Dataflow* const dataflow, Vertex a) {
  actor = a;
  numExecs = 0;
  phaseCount = dataflow->getPhasesQuantity(actor);
  repFactor = dataflow->getNi(actor);
  id = dataflow->getVertexId(actor);
  isExecuting = false;
  VERBOSE_INFO("initialising actor " << dataflow->getVertexName(actor));
  {ForInputEdges(dataflow, actor, e) {
      VERBOSE_INFO("input port execution rates (phases = "
                   << dataflow->getEdgeOutPhasesCount(e) <<"): ");
      consPhaseCount[e] = dataflow->getEdgeOutPhasesCount(e);
      {ForEachPhase(dataflow, actor, p) {
          VERBOSE_INFO(dataflow->getEdgeOutPhase(e, p) << " ");
          consExecRate[e][p] = dataflow->getEdgeOutPhase(e, p);
        }}
      VERBOSE_INFO("\n");
    }}
  {ForOutputEdges(dataflow, actor, e) {
      VERBOSE_INFO("output port execution rates (phases = "
                   << dataflow->getEdgeInPhasesCount(e) <<"): ");
      prodPhaseCount[e] = dataflow->getEdgeInPhasesCount(e);
      {ForEachPhase(dataflow, actor, p) {
          VERBOSE_INFO(dataflow->getEdgeInPhase(e, p) << " ");
          prodExecRate[e][p] = dataflow->getEdgeInPhase(e, p);
        }}
      VERBOSE_INFO("\n");
    }}
}

EXEC_COUNT Actor::getPhaseCount(Edge e) {
  assert(this->consPhaseCount.find(e) != this->consPhaseCount.end() ||
         this->prodPhaseCount.find(e) != this->prodPhaseCount.end()); // given edge must be either input/output edge
  if (this->consPhaseCount.find(e) != this->consPhaseCount.end()) {
    return this->consPhaseCount[e];
  } else if (this->prodPhaseCount.find(e) != this->prodPhaseCount.end()) {
    return this->prodPhaseCount[e];
  } else {
    std::cout << "Specified edge not attached to given actor!" << std::endl;
    return 0;
  }
}

// Return phase of execution for given edge (allows for different number of phases for each port)
PHASE_INDEX Actor::getPhase(Edge e) {
  return (this->getNumExecutions() % getPhaseCount(e)) + 1;
}

// Return phase of execution for actor (assumes that all ports have equal number of phases)
PHASE_INDEX Actor::getPhase() {
  return (this->getNumExecutions() % this->phaseCount) + 1;
}

EXEC_COUNT Actor::getRepFactor() {
  return this->repFactor;
}

ARRAY_INDEX Actor::getId() {
  return this->id;
}

void Actor::setId(ARRAY_INDEX newId) {
  this->id = newId;
}

TOKEN_UNIT Actor::getExecRate(Edge e) {
  assert(this->prodExecRate.find(e) != this->prodExecRate.end() ||
         this->consExecRate.find(e) != this->consExecRate.end()); // given edge must be either input/output edge
  PHASE_INDEX curPhase = (this->getNumExecutions() % getPhaseCount(e)) + 1; // phase indexing starts from 1 but stored in Actor class as starting from 0
  if (this->prodExecRate.find(e) != this->prodExecRate.end()) {
    return this->prodExecRate[e][curPhase];
  } else if (this->consExecRate.find(e) != this->consExecRate.end()) {
    return this->consExecRate[e][curPhase];
  } else {
    std::cout << "Specified edge not attached to given actor!" << std::endl;
    return 0;
  }
}

TOKEN_UNIT Actor::getExecRate(Edge e, PHASE_INDEX p) {
  assert(this->prodExecRate.find(e) != this->prodExecRate.end() ||
         this->consExecRate.find(e) != this->consExecRate.end()); // given edge must be either input/output edge
  if (this->prodExecRate.find(e) != this->prodExecRate.end()) {
    return this->prodExecRate[e][p];
  } else if (this->consExecRate.find(e) != this->consExecRate.end()) {
    return this->consExecRate[e][p];
  } else {
    std::cout << "Specified edge not attached to given actor!" << std::endl;
    return 0;
  }
}

EXEC_COUNT Actor::getNumExecutions() {
  return this->numExecs;
}

// Given current state of graph, returns whether actor is ready to execute or not
bool Actor::isReadyForExec(State s) {
  // Execution conditions (for given phase):
  // (1) enough room in output channel, (2) enough tokens in input channel, (3) not currently executing
  VERBOSE_DEBUG("Is Actor " << this->getId() << " ready to execute?" << std::endl);
  bool isExecutable = true;
  for (auto const &e : this->consPhaseCount) {
    VERBOSE_DEBUG("\tTokens in channel (current/required): "
                  << s.getTokens(e.first) << "/" << this->getExecRate(e.first)
                  << std::endl);
    if (s.getTokens(e.first) < this->getExecRate(e.first) || this->isExecuting) {
      isExecutable = false;
    }
  }
  if (s.hasBoundedBuffers()) {
    for (auto const &e : this->prodPhaseCount) {
      VERBOSE_DEBUG("\tSpace in channel (current/required): "
                    << s.getBufferSpace(e.first) << "/" << this->getExecRate(e.first)
                    << std::endl);
      // if (s.getBufferSize(e.first) - s.getTokens(e.first) < this->getExecRate(e.first)) {
      if (s.getBufferSpace(e.first) < this->getExecRate(e.first) || this->isExecuting) {
        isExecutable = false;
      }
    }
  }
  if (isExecutable) {
    VERBOSE_DEBUG("Actor " << this->getId() << " is ready to execute" << std::endl);
  } else {
    VERBOSE_DEBUG("Actor " << this->getId() << " is not ready to execute" << std::endl);
  }
  return isExecutable;
}

bool Actor::isReadyForExecWithMod(models::Dataflow* const dataflow, State s, std::map<std::pair<ARRAY_INDEX, ARRAY_INDEX>, long> cond, long step) {
  // Execution conditions (for given phase): Original + current slot = assigned slot
  bool isExecutable = true;
  bool currSlot = false;
  for (auto const &e : this->consPhaseCount) {
    if (s.getTokens(e.first) < this->getExecRate(e.first) || this->isExecuting) {
      isExecutable = false;
    }
    if (cond[std::make_pair(dataflow->getVertexId(dataflow->getEdgeSource(e.first)), 
      dataflow->getVertexId(dataflow->getEdgeTarget(e.first)))] == step){
      currSlot = true;
    }
  }
  return isExecutable && currSlot;
}

// Given current state of graph, returns whether actor is ready to end execution or not
bool Actor::isReadyToEndExec(State s) {
  bool isEndable = true;
  if (s.getExecQueue()[this->actor].empty() ||
      s.getExecQueue()[this->actor].front().first != 0) {
    isEndable = false;
  }
  return isEndable;
}

bool Actor::isReadyToEndExecWithMod(State s, models::Dataflow* const dataflow, std::map<std::pair<ARRAY_INDEX, ARRAY_INDEX>, long> cond, long step) {
  if (s.getExecQueue()[this->actor].empty() ||
      s.getExecQueue()[this->actor].front().first != 0) {
    return false;
  }
  {ForOutputEdges(dataflow, this->actor, e) {
      if (cond[std::make_pair(dataflow->getVertexId(dataflow->getEdgeSource(e)), 
      dataflow->getVertexId(dataflow->getEdgeTarget(e)))] == step){
        return true;
      }
    }}
    return true;
}

// Begin actor's execution, consuming tokens from input channels
void Actor::execStart(models::Dataflow* const dataflow, State &s) {
  {ForInputEdges(dataflow, this->actor, e) {
      s.setTokens(e, s.getTokens(e) - this->getExecRate(e));
    }}
  if (s.hasBoundedBuffers()) { // for bounded buffers, need to 'reserve' space for tokens about to be produced
    {ForOutputEdges(dataflow, this->actor, e) {
        VERBOSE_DEBUG("Consuming space in Channel " << dataflow->getEdgeId(e)
                      << ": " << s.getBufferSpace(e) << "-" << this->getExecRate(e)
                      << "=" << s.getBufferSpace(e) - this->getExecRate(e));
        s.setBufferSpace(e, s.getBufferSpace(e) - this->getExecRate(e));
      }}
  }
  std::pair<TIME_UNIT, PHASE_INDEX> newExec(dataflow->getVertexDuration(this->actor,
                                                                        this->getPhase()),
                                            this->getPhase());
  s.addExecution(this->actor, newExec);
  this->numExecs++;
  this->isExecuting = true;
}

// Begin actor's execution, consuming tokens from input channels
void Actor::execStartWithMod(models::Dataflow* const dataflow, State &s, 
std::map<std::pair<ARRAY_INDEX, ARRAY_INDEX>, long> cond, long step) {
  dataflow->reset_computation();
  {ForInputEdges(dataflow, this->actor, e) {
    if (cond[std::make_pair(dataflow->getVertexId(dataflow->getEdgeSource(e)), 
      dataflow->getVertexId(dataflow->getEdgeTarget(e)))] == step){
        dataflow->setPreload(e, dataflow->getPreload(e) - this->getExecRate(e)); // TODO don't edit dataflow graph

      //   std::cout << "Checked for " << dataflow->getVertexId(dataflow->getEdgeSource(e)) 
      //   << "->" << dataflow->getVertexId(dataflow->getEdgeTarget(e)) << ": " 
      //   << "slot " << cond[std::make_pair(dataflow->getVertexId(dataflow->getEdgeSource(e)), 
      // dataflow->getVertexId(dataflow->getEdgeTarget(e)))] << "==" << step << std::endl;
    }
    }}
  std::pair<TIME_UNIT, PHASE_INDEX> newExec(dataflow->getVertexDuration(this->actor,
                                                                        this->getPhase()),
                                            this->getPhase());
  s.addExecution(this->actor, newExec);
  this->numExecs++;
  this->isExecuting = true;
}

// End actor's execution, producing tokens into output channels
void Actor::execEnd(models::Dataflow* const dataflow, State &s) {
  // NOTE it is important that the tokens/spaces produced correspond to the phase
  // of the execution in the execution queue in order to correctly end execution
  PHASE_INDEX currentPhase = s.getRemExecTime(this->actor).front().second;; // phase of actor's execution in queue, rather than phase actor is currently in
  {ForOutputEdges(dataflow, this->actor, e) {
      VERBOSE_INFO("ending execution for phase "
                << currentPhase << ", producing "
                << this->getExecRate(e, currentPhase) << " tokens");
      s.setTokens(e, s.getTokens(e) + this->getExecRate(e, currentPhase));
    }}
  if (s.hasBoundedBuffers()) { // free up spaces after tokens that have been consumed
    {ForInputEdges(dataflow, this->actor, e) {
        VERBOSE_DEBUG("Producing space in Channel " << dataflow->getEdgeId(e)
                      << ": " << s.getBufferSpace(e) << "+" << this->getExecRate(e, currentPhase)
                      << "=" << s.getBufferSpace(e) + this->getExecRate(e, currentPhase));
        s.setBufferSpace(e, s.getBufferSpace(e) + this->getExecRate(e, currentPhase));
      }}
  }
  s.removeFrontExec(this->actor);
  this->isExecuting = false;
}

void Actor::execEndWithMod(models::Dataflow* const dataflow, State &s, 
std::map<std::pair<ARRAY_INDEX, ARRAY_INDEX>, long> cond, long step) {
  PHASE_INDEX currentPhase;
  dataflow->reset_computation();
  {ForOutputEdges(dataflow, this->actor, e) {
    if (cond[std::make_pair(dataflow->getVertexId(dataflow->getEdgeSource(e)), 
      dataflow->getVertexId(dataflow->getEdgeTarget(e)))] == step){
        currentPhase = s.getRemExecTime(this->actor).front().second;
        VERBOSE_INFO("ending execution for phase "
                  << currentPhase << ", producing "
                  << this->getExecRate(e, currentPhase) << " tokens");
        dataflow->setPreload(e, dataflow->getPreload(e) + this->getExecRate(e, currentPhase)); // TODO don't edit dataflow graph
      }
      
    }}
  s.removeFrontExec(this->actor);
  this->isExecuting = false;
}

void Actor::computeCausalDeps(models::Dataflow* const dataflow, State &prevState,
                              abstractDepGraph &absDepGraph) {
  for (auto const &e : this->consPhaseCount) {
    Vertex source = dataflow->getEdgeSource(e.first);
    Vertex target = dataflow->getEdgeTarget(e.first);
    ARRAY_INDEX sourceId = dataflow->getVertexId(source);
    ARRAY_INDEX targetId = dataflow->getVertexId(target);
    VERBOSE_DEBUG("\t\tChannel " << dataflow->getEdgeId(e.first) << " ("
                  << sourceId << "->" << targetId << "):" << std::endl);
    VERBOSE_DEBUG("\t\t " << prevState.getTokens(e.first) << " tokens available previously, "
                  << this->getExecRate(e.first) << " required (p "
                  << this->getPhase(e.first) << ")" << std::endl);
    if (prevState.getTokens(e.first) < this->getExecRate(e.first)) {
      VERBOSE_DEBUG("\t\t\tCausal dep between " << targetId << " and "
                    << sourceId << std::endl);
      absDepGraph.addCausalDep(targetId, sourceId);
    }
  }
  for (auto const &e : this->prodPhaseCount) {
    Vertex source = dataflow->getEdgeSource(e.first);
    Vertex target = dataflow->getEdgeTarget(e.first);
    ARRAY_INDEX sourceId = dataflow->getVertexId(source);
    ARRAY_INDEX targetId = dataflow->getVertexId(target);
    VERBOSE_DEBUG("\t\tChannel " << dataflow->getEdgeId(e.first) << " ("
                  << sourceId << "->" << targetId << "):" << std::endl);
    VERBOSE_DEBUG("\t\t " << prevState.getBufferSpace(e.first)
                  << " spaces available previously, " << this->getExecRate(e.first)
                  << " required (p " << this->getPhase(e.first) << ")" << std::endl);
    if (prevState.getBufferSpace(e.first) < this->getExecRate(e.first)) {
      VERBOSE_DEBUG("\t\t\tCausal dep between " << sourceId << " and "
                    << targetId << std::endl);
      absDepGraph.addCausalDep(sourceId, targetId);
    }
  }
}

std::string Actor::printStatus(models::Dataflow* const dataflow) {
  std::stringstream outputStream;

  outputStream << "\nActor " << dataflow->getVertexName(this->actor)
               << " (ID: " << this->getId() << ")" << std::endl;
  outputStream << "\tNumber of executions: " << this->getNumExecutions()
               << std::endl;
  {ForInputEdges(dataflow, this->actor, e) {
      outputStream << "\tTokens consumed from Channel "
                   << dataflow->getEdgeName(e) << ": " << this->getExecRate(e)
                   << std::endl;
    }}
  {ForOutputEdges(dataflow, this->actor, e) {
      outputStream << "\tTokens produced into Channel "
                   << dataflow->getEdgeName(e) << ": " << this->getExecRate(e)
                   << std::endl;
    }}
  return outputStream.str();
}
