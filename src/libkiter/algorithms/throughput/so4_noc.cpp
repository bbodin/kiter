/*
 * so4_noc.cpp taken from "A Statically Scheduled Time-Division-Multiplexed
 *  Network-on-Chip for Real-Time Systems"
 *
 *  Created on: May 2021
 *      Author: katwinkl3
 */

#include <models/Dataflow.h>
#include <libxml/parser.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "actor.h"
#include "state.h"
#include "so4_noc.h"
#include <printers/stdout.h>
#include "../../models/Scheduling.h"
#include "../scc.h"
#include "../buffersizing/periodic_fixed.h"

std::tuple<std::map<ARRAY_INDEX, long>, long> generateConditions(models::Dataflow* const dataflow, std::string filename, std::set<ARRAY_INDEX> new_edges){
  std::map<ARRAY_INDEX, long> condition; // dataflow edge : [slot]
  long slots = 0;
  std::filesystem::path file(filename);
  if (file.extension() == ".xml"){
    std::map<edge_id_t, long> condition_param; //noc edge id : slot
    xmlDocPtr doc =  xmlParseFile(filename.c_str()); //TODO: error handling and shift into separate function
    xmlNodePtr tdma_node;
    for (xmlNodePtr cur_node = doc->children; cur_node; cur_node = cur_node->next) {
      if (cur_node->type == XML_ELEMENT_NODE) {
        if (std::string((const char*)cur_node->name) == std::string("tdma")) tdma_node = cur_node;
      }
    }
    const char* temp = (const char*) tdma_node->properties->children->content;
    slots = atoi(temp); //TODO: change to strtol 
    for (xmlNodePtr cur_node = tdma_node->children; cur_node; cur_node = cur_node->next) {
      if (cur_node->type == XML_ELEMENT_NODE) {
        if (std::string((const char*)cur_node->name) == std::string("rule")) {
          long eid;
          long s;
          char *ss1, *ss2; 
          for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next){
            if (strcmp((const char*)cur_attr->name,"slot") == 0){
              temp = (const char*) cur_attr->children->content;
              s = strtol(temp, &ss1, 10);
            }
            if (strcmp((const char*)cur_attr->name,"id") == 0){
              temp = (const char*) cur_attr->children->content;
              eid = strtol(temp, &ss2, 10);
            }
          }
          condition_param.insert({eid,s});
        }
      }
    }
    xmlFreeDoc(doc);
    {ForEachEdge(dataflow,e){
      if (dataflow->getEdgeType(e) == VIRTUAL_EDGE){
        condition.insert({dataflow->getEdgeId(e), -1});
      } else {
        condition.insert({dataflow->getEdgeId(e), condition_param[dataflow->getRoute(e)[0]]});
      }
      // if (new_edges.find(dataflow->getEdgeId(e)) != new_edges.end()){
      //   condition.insert({dataflow->getEdgeId(e), -1});
      // } else {
      //   condition.insert({dataflow->getEdgeId(e), condition_param[dataflow->getRoute(e)[0]]});
      // }
    }}
  }
  if (file.extension() == ".txt"){
    std::map<std::pair<long, long>, long> condition_param; //noc edge id : slot
    std::string line;
    std::ifstream readfile(filename);
    while (getline (readfile, line)) {
      std::istringstream ss(line);
      std::string subline;
      std::vector<long> vals = {}; 
      while(std::getline(ss, subline, ',')) {
        if (subline != "src" & subline != "dest" & subline != "slot"){
          vals.push_back(std::stol(subline));
        }
      }
      if (vals[2] > slots){
        slots = vals[2];
      }
      condition_param.insert({{vals[0]+dataflow->getNoC().size(), vals[1]+dataflow->getNoC().size()}, vals[2]});
    }
    readfile.close();
    {ForEachEdge(dataflow,e){
      if (dataflow->getEdgeType(e) == VIRTUAL_EDGE){
        condition.insert({dataflow->getEdgeId(e), -1}); //actually has no use
      } else {
        if (condition_param.find({ dataflow->getMapping(dataflow->getEdgeSource(e)), dataflow->getMapping(dataflow->getEdgeTarget(e)) }) != condition_param.end()){
          condition.insert({dataflow->getEdgeId(e), condition_param[{dataflow->getMapping(dataflow->getEdgeSource(e)), dataflow->getMapping(dataflow->getEdgeTarget(e))}] });
        } else {
          std::cout << "Missing TDMA rule for edge " << dataflow->getEdgeId(e) << "src=" << dataflow->getMapping(dataflow->getEdgeSource(e)) << " dest=" << dataflow->getMapping(dataflow->getEdgeTarget(e))<< std::endl;
          condition.insert({dataflow->getEdgeId(e), 0});
        }
      }
    }}
  }
  return {condition, slots};
}

std::pair<TIME_UNIT, scheduling_t> algorithms::computeComponentSo4Schedule(models::Dataflow* const dataflow,
                                       std::pair<ARRAY_INDEX, EXEC_COUNT> &minActorInfo, scheduling_t schedule, std::string filename, std::set<ARRAY_INDEX> new_edges) {
  VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
  VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
  StateList visitedStates;
  EXEC_COUNT minRepFactor = INT_MAX; // EXEC_COUNT is of type long int, and LONG_MAX has same max value
  ARRAY_INDEX minRepActorId;
  TOKEN_UNIT minRepActorExecCount = 0;
  TIME_UNIT timeStep; // total time passed
  int periodic_state_idx; // idx of first periodic state in statelist
  TIME_UNIT thr;
  bool end_check = false; // 
  int actors_left = 0;
  std::map<ARRAY_INDEX, TIME_UNIT> actors_check;
  {ForEachTask(dataflow, t){
    ++actors_left;
    actors_check[dataflow->getVertexId(t)] = -1;
  }}

  std::map<ARRAY_INDEX, long> condition;
  long slots;
  std::tie(condition, slots) = generateConditions(dataflow, filename, new_edges);
  
  std::map<TIME_UNIT, std::map<ARRAY_INDEX, std::pair<long, bool>>> *buffer = new std::map<TIME_UNIT, std::map<ARRAY_INDEX, std::pair<long, bool>>>();; //slot: [channel (actor's edge): {tokens released, if actor needs to execute}]
  std::deque<std::pair<TIME_UNIT, std::pair<ARRAY_INDEX, long>>> *n_buffer = new std::deque<std::pair<TIME_UNIT, std::pair<ARRAY_INDEX, long>>>();; //[timeslot, channel, token]
  scheduling_t task_schedule; // {vertex_id : {time_unit: [time_unit]}}
  ARRAY_INDEX skip_edge = 0;
  ARRAY_INDEX skip_vertex = 0;
  TIME_UNIT skip_time = 0;
  TIME_UNIT curr_step = 0;
  std::map<ARRAY_INDEX, std::vector<std::vector<TIME_UNIT>>> starts; //{task idx : [[state 0 starts], [state 1 ...]]}
  std::map<ARRAY_INDEX, std::vector<TIME_UNIT>> state_start = {};  //{task idx : [ starts]}, 
  bool periodic_state = false;

  // initialise actors
  std::map<ARRAY_INDEX, Actor> actorMap;
  {ForEachTask(dataflow, t) {
      actorMap[dataflow->getVertexId(t)] = Actor(dataflow, t);
    }}
  State prevState(dataflow, actorMap, new_edges);
  State currState(dataflow, actorMap, new_edges);
  {ForEachTask(dataflow, t) {
      // track actor with lowest repetition factor (determines when states are stored)
      if (actorMap[dataflow->getVertexId(t)].getRepFactor() < minRepFactor) {
        minRepFactor = actorMap[dataflow->getVertexId(t)].getRepFactor();
        minRepActorId = actorMap[dataflow->getVertexId(t)].getId();
      }
    }}
 
  minActorInfo = std::make_pair(minRepActorId, minRepFactor);
  // Start ASAP execution loop
  while (true) {
    {ForEachEdge(dataflow, e) {
      if (new_edges.find(dataflow->getEdgeId(e)) != new_edges.end()){
        continue;
      }
        prevState.setTokens(e, currState.getTokens(e));
    }}
    // end actor firing
    {ForEachTask(dataflow, t) {
        while (actorMap[dataflow->getVertexId(t)].isReadyToEndExec(currState)) {
          if (actorMap[dataflow->getVertexId(t)].getId() == minRepActorId) {
            minRepActorExecCount++;
            if (minRepActorExecCount == minRepFactor) {
              if (!end_check){
                if (!visitedStates.addState(currState)) {
                  VERBOSE_INFO("ending execution and computing throughput");
                  // compute throughput using recurrent state
                  thr = visitedStates.computeThroughput();
                  periodic_state_idx = visitedStates.computeIdx(currState); //idx for repeated state
                  end_check = true;
                }
              }
              if (!end_check){
                {ForEachTask(dataflow, task){ //start new state vector in starts
                  starts[dataflow->getVertexId(task)].push_back({});
                }}
              }
              currState.setTimeElapsed(0);
              minRepActorExecCount = 0;
            }
          }
          actorMap[dataflow->getVertexId(t)].execEndWithMod(dataflow, currState, n_buffer, curr_step, slots, condition);
          // NOTE updating tokens/phase in state done separately from execEnd function, might be a cause for bugs
          // std::cout <<"END FIRING\n"<< std::endl; 
          // std::cout << currState.print(dataflow) << std::endl;
        }
      }}
    
    // set preload from ended firings
    int cast_out_idx;
    for (cast_out_idx = 0; cast_out_idx < (*n_buffer).size(); ++cast_out_idx){
      if ((*n_buffer)[cast_out_idx].first <= curr_step){
        Edge e = dataflow->getEdgeById((*n_buffer)[cast_out_idx].second.first);
        dataflow->setPreload(e, 
        dataflow->getPreload(e) + (*n_buffer)[cast_out_idx].second.second);
      }
      else{
        break;
      }
    }
    while (cast_out_idx > 0){
      (*n_buffer).pop_front();
      cast_out_idx--;
    }
    currState.updateState(dataflow, actorMap, new_edges);

    {ForEachTask(dataflow, t) {

      // if (skip_time != 0 && dataflow->getVertexId(t) != skip_vertex){
      //   continue;
      // }
        if(actorMap[dataflow->getVertexId(t)].isReadyForExec(currState, new_edges)) {
            actorMap[dataflow->getVertexId(t)].execStart(dataflow, currState);
              currState.updateState(dataflow, actorMap, new_edges);
              // std::cout <<"FIRING\n"<< std::endl;
              // std::cout << currState.print(dataflow) << std::endl;
              if (end_check){
                if (actors_check[dataflow->getVertexId(t)] < 0){
                  actors_check[dataflow->getVertexId(t)] = curr_step;
                  starts[dataflow->getVertexId(t)].back().push_back(curr_step);
                  --actors_left;
                }
                if (actors_left == 0){
                  {ForEachTask(dataflow, task){ //Creates schedule after additional actor is fired after end of period
                    static_task_schedule_t initials; 
                    periodic_task_schedule_t periodics;
                    for(std::size_t i = 0; i < starts[dataflow->getVertexId(task)].size(); ++i){
                      if (i < periodic_state_idx){ //initials
                        initials.insert(initials.end(),starts[dataflow->getVertexId(task)][i].begin(),starts[dataflow->getVertexId(task)][i].end());
                        continue;
                      } 
                      if (i == starts[dataflow->getVertexId(task)].size() - 1){
                        periodics.second.insert(periodics.second.end(),starts[dataflow->getVertexId(task)][i].begin(),starts[dataflow->getVertexId(task)][i].end()-1);
                      } else { //periodic
                        periodics.second.insert(periodics.second.end(),starts[dataflow->getVertexId(task)][i].begin(),starts[dataflow->getVertexId(task)][i].end());
                      }
                    }
                    periodics.first = actors_check[dataflow->getVertexId(task)] - periodics.second[0]; 
                    task_schedule_t sched_struct = {initials,periodics};
                    schedule.set(dataflow->getVertexId(task), sched_struct);
                    // std::cout << dataflow->getVertexName(task) << ": initial starts=" << commons::toString(initials) << ", periodic starts=" << commons::toString(periodics) << std::endl;
                  }}
                  return std::make_pair(thr, schedule);
                }
              } else {
                if (starts[dataflow->getVertexId(t)].size() == 0){
                  starts[dataflow->getVertexId(t)].push_back({curr_step}); //push into current state's task start list
                } else {
                  starts[dataflow->getVertexId(t)].back().push_back(curr_step);
                }
              }
        }
      }}
    timeStep = currState.advanceTimeWithMod();
    curr_step+= timeStep;
    if (timeStep == LONG_MAX) { // NOTE should technically be LDBL_MAX cause TIME_UNIT is of type long double
      VERBOSE_INFO("Deadlock found!");
      return std::make_pair(0, schedule);
    }
  }
}

void algorithms::scheduling::So4Scheduling(models::Dataflow* const dataflow,
                                         parameters_list_t param_list) {
  VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
  VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
  std::map<int, std::vector<ARRAY_INDEX>> sccMap;
  std::vector<models::Dataflow*> sccDataflows;
  TIME_UNIT minThroughput = LONG_MAX; // NOTE should technically be LDBL_MAX cause TIME_UNIT is of type long double
  
  scheduling_t scheduling_result;
  int linesize = param_list.count("LINE")? commons::fromString<int>(param_list["LINE"]) : 80;

  // Temp fix to turn graph into 1 SCC
  std::set<ARRAY_INDEX> new_edges = {};//add_vbuffers(dataflow, param_list);

  // generate SCCs if any
  sccMap = computeSCCKosaraju(dataflow);
  if (sccMap.size() > 1) { // if the original graph isn't one SCC, need to break into SCC subgraphs
    sccDataflows = generateSCCs(dataflow, sccMap);
    for (auto g : sccDataflows) {
      if (g->getEdgesCount() > 0) {
        std::pair<ARRAY_INDEX, EXEC_COUNT> actorInfo;
        auto res_pair = computeComponentSo4Schedule(g, actorInfo, scheduling_result, param_list["TDMA"], new_edges);
        TIME_UNIT componentThroughput = res_pair.first;
        scheduling_result = res_pair.second;
        TIME_UNIT scaledThroughput = (componentThroughput * actorInfo.second) /
          dataflow->getNi(dataflow->getVertexById(actorInfo.first));
        if (scaledThroughput < minThroughput) {
          minThroughput = scaledThroughput;
        }
      } else if (g->getVerticesCount() == 1 && g->getEdgesCount() == 0) {
        /* NOTE this is a workaround from ignoring reentrancy edges --- if this
           condition is met, we assume that we have a single actor with re-entrancy */
        ARRAY_INDEX standaloneId = g->getVertexId(g->getFirstVertex());
        EXEC_COUNT standaloneRepFactor = g->getPhasesQuantity(g->getFirstVertex());
        Vertex vertexInDataflow = dataflow->getVertexById(standaloneId);
        /* repetition factor for standalone component will be equal to its phase count:
           this makes sense because, while it's producing and consuming 1 token in its
           re-entrant edge, it will need to execute its number of phases to arrive
           back at the same state */
        EXEC_COUNT repFactor = dataflow->getNi(vertexInDataflow);
        TIME_UNIT componentThroughput = (TIME_UNIT) (1 * standaloneRepFactor) /
          dataflow->getVertexTotalDuration(vertexInDataflow);
        TIME_UNIT scaledThroughput = (TIME_UNIT) componentThroughput / repFactor;
        if (scaledThroughput < minThroughput) {
          minThroughput = scaledThroughput;
        }
      }
    }
    
    TIME_UNIT omega = 1.0 / minThroughput ;
    models::Scheduling res = models::Scheduling(dataflow, omega, scheduling_result);

    std::cout << res.asASCII(linesize);
    std::cout << res.asText();
    // std::cout << printers::Scheduling2Tikz(res);

    std::cout << "So4 throughput is  " << minThroughput << std::endl;
    std::cout << "So4 period is  " << omega << std::endl;

    return;
  }
  // if graph is strongly connected, just need to use computeComponentThroughput
  std::pair<ARRAY_INDEX, EXEC_COUNT> actorInfo; // look at note for computeComponentThroughput
  auto res_pair = computeComponentSo4Schedule(dataflow, actorInfo, scheduling_result, param_list["TDMA"], new_edges);
  minThroughput = res_pair.first;
  scheduling_result = res_pair.second;
  std::cout << "Throughput of graph: " << minThroughput << std::endl;
  TIME_UNIT omega = 1.0 / minThroughput ;
  models::Scheduling res = models::Scheduling(dataflow, omega, scheduling_result);
  std::cout << res.asASCII(linesize);
  std::cout << res.asText();

  std::cout << "So4 throughput is  " << minThroughput << std::endl;
  std::cout << "So4 period is  " << omega << std::endl;

  return;
}
