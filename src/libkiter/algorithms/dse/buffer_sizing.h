/*
 * buffer_sizing.h
 *
 */

#ifndef BUFFER_SIZING_H_
#define BUFFER_SIZING_H_

#include <models/Dataflow.h>
#include <commons/commons.h>
#include <algorithms/throughput/kperiodic.h>

namespace models {
  class Dataflow;
}

class StorageDistribution {
public:
  StorageDistribution();
  StorageDistribution(ARRAY_INDEX edge_count,
                      TIME_UNIT thr,
                      std::map<Edge,
                      std::pair<TOKEN_UNIT, TOKEN_UNIT>> channel_quantities,
                      TOKEN_UNIT distribution_size);

  void setChannelQuantity(Edge e, TOKEN_UNIT quantity);
  void setInitialTokens(Edge e, TOKEN_UNIT token_count);
  void setDistributionSize(TOKEN_UNIT sz);
  void setThroughput(TIME_UNIT thr);
  TOKEN_UNIT getChannelQuantity(Edge e) const;
  TOKEN_UNIT getInitialTokens(Edge e) const;
  TOKEN_UNIT getDistributionSize() const;
  TIME_UNIT getThroughput() const;
  ARRAY_INDEX getEdgeCount() const;
  std::vector<Edge> getEdges() const;
  bool operator==(const StorageDistribution& distribution) const;
  bool operator!=(const StorageDistribution& distribution) const;
  void updateDistributionSize();
  std::string printInfo(models::Dataflow* const dataflow, bool isSymbExec);
  std::string print_quantities_csv(models::Dataflow* const dataflow, bool isSymbExec);
  std::string print_dependency_mask(models::Dataflow* const dataflow,
                                    kperiodic_result_t const result,
                                    bool isSymbExec);
  std::string printGraph(models::Dataflow* const dataflow);
  // BASE MONOTONIC OPTIMISATION FUNCTIONS
  bool inBackConeOf(StorageDistribution checkDist);
  bool inForConeOf(StorageDistribution checkDist);

private:
  ARRAY_INDEX edge_count;
  TIME_UNIT thr; // throughput of given storage distribution
  std::map<Edge, std::pair<TOKEN_UNIT, // initial tokens
                           TOKEN_UNIT>> channel_quantities; // amount of space (in tokens per channel)
  TOKEN_UNIT distribution_size; // should be equal to sum of channel quantities
};

class StorageDistributionSet {
public:
  StorageDistributionSet();
  StorageDistributionSet(TOKEN_UNIT dist_sz,
                         StorageDistribution distribution);
  void addStorageDistribution(StorageDistribution new_distribution); // add storage distribution to set
  void removeStorageDistribution(StorageDistribution dist_to_rm); // remove storage distribution from set
  void removeDistributionSize(TOKEN_UNIT dist_sz); /* removes all storage distributions of
                                                      distribution size dist_sz */
  StorageDistribution getNextDistribution() const;
  size_t getSize() const;
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> getSet();
  void minimizeStorageDistributions(StorageDistribution newDist);
  bool hasDistribution(TOKEN_UNIT dist_sz);
  bool hasStorageDistribution(StorageDistribution checkDist);
  bool isSearchComplete(StorageDistributionSet checklist, TIME_UNIT target_thr);
  bool isInBackCone(StorageDistribution checkDist,
                    std::map<Edge, TOKEN_UNIT> bufferLb);
  bool isInForeCone(StorageDistribution checkDist);
  void removeNonMaximum(StorageDistribution checkDist,
                        std::map<Edge, TOKEN_UNIT> bufferLb);
  void removeNonMinimum(StorageDistribution checkDist);
  void updateKneeSet(models::Dataflow* const dataflow,
                     StorageDistributionSet infeasibleSet,
                     StorageDistributionSet feasibleSet,
                     std::map<Edge, TOKEN_UNIT> bufferLb);
  void addEdgeKnees(models::Dataflow* const dataflow,
                    StorageDistributionSet infeasibleSet,
                    StorageDistributionSet feasibleSet,
                    std::map<Edge, TOKEN_UNIT> bufferLb);
  void updateInfeasibleSet(StorageDistribution new_sd,
                           std::map<Edge, TOKEN_UNIT> bufferLb); // add new SD to infeasible set of SDs for monotonic optimisation
  void updateFeasibleSet(StorageDistribution new_sd); // add new SD to infeasible set of SDs for monotonic optimisation
  std::string printDistributions(TOKEN_UNIT dist_sz,
                                 models::Dataflow* const dataflow,
                                 bool isSymbExec); /* prints info of all storage distributions
                                                      of given distribution size */
  std::string printDistributions(models::Dataflow* const dataflow,
                                 bool isSymbExec); // prints info of all storage distributions in set
  void writeCSV(std::string filename, models::Dataflow* const dataflow,
                bool isSymbExec); // writes to a CSV file for plots
  void printGraphs(models::Dataflow* const dataflow, std::string filename); // iterate through storage distribution set and print graphs
  // BASE MONOTONIC OPTIMISATION FUNCTIONS
  bool addToUnsat(StorageDistribution sd);
  void addToSat(StorageDistribution sd);
  void add(StorageDistribution sd, StorageDistributionSet &kneePoints);
  void addCut(models::Dataflow *dataflow, StorageDistribution sd,
              kperiodic_result_t result, StorageDistributionSet &kneePoints,
              bool modelBoundedBuffers);
  bool satContains(StorageDistribution sd);
  bool unsatContains(StorageDistribution sd);

private:
  std::map<TOKEN_UNIT, std::vector<StorageDistribution>> set; /* store storage distributions
                                                                 by distribution size*/
  std::pair<TOKEN_UNIT, TIME_UNIT> p_max; /* stores the current maximum throughput and corresponding
                                             distribution size of set */
};

// to search parameters
void findMinimumStepSz(models::Dataflow *dataflow,
                       std::map<Edge, TOKEN_UNIT> &minStepSizes);
void findMinimumChannelSz(models::Dataflow *dataflow,
                          std::map<Edge,
                          std::pair<TOKEN_UNIT, TOKEN_UNIT>> &minChannelSizes); // initial tokens in first element of pair and channel quantity in second element of pair
TOKEN_UNIT findMinimumDistributionSz(std::map<Edge,
                                     std::pair<TOKEN_UNIT, TOKEN_UNIT>> minChannelSizes);
void initSearchParameters(models::Dataflow *dataflow,
                          std::map<Edge, TOKEN_UNIT> &minStepSizes,
                          std::map<Edge,
                          std::pair<TOKEN_UNIT, TOKEN_UNIT>> &minChannelSizes);
std::string timeToString(TIME_UNIT t);
StorageDistribution makeMinimalSD(StorageDistribution sd1,
                                  StorageDistribution sd2);
void handleInfeasiblePoint(models::Dataflow* const dataflow,
                           StorageDistributionSet &infeasibleSet,
                           StorageDistributionSet feasibleSet,
                           StorageDistributionSet &kneeSet,
                           StorageDistribution newSD,
                           kperiodic_result_t deps,
                           std::map<Edge, TOKEN_UNIT> &bufferLb,
                           bool modelBoundedBuffers);
// BASE MONOTONIC OPTIMISATION FUNCTIONS
void addToExtensions(StorageDistributionSet &extensions,
                     StorageDistribution sd);
StorageDistribution createPoint(models::Dataflow *dataflow,
                                StorageDistribution sd,
                                StorageDistribution hp,
                                TOKEN_UNIT m,
                                bool modelBoundedBuffers);
#endif /* BUFFER_SIZING_H_ */
