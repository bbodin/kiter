#define BOOST_TEST_MODULE ConstraintTest
#include "algorithms/dse/Constraint.h"
#include "algorithms/dse/TokenConfiguration.h"
#include "generators/NormalizedCycleGenerator.h"
#include "helpers/sample.h"
#include "helpers/test_classes.h"

#include <boost/test/unit_test_suite.hpp>
#include <map>
#include <variant>
#include <vector>

using namespace algorithms::dse;

bool areConfigsEqual(const std::map<ARRAY_INDEX, TOKEN_UNIT> &map1,
                     const std::map<ARRAY_INDEX, TOKEN_UNIT> &map2) {
  if (map1.size() != map2.size()) {
    return false;
  }

  for (const auto &pair : map1) {
    auto it = map2.find(pair.first);
    if (it == map2.end()) {
      return false;
    }

    if (pair.second != it->second) {
      return false;
    }
  }

  return true;
}

models::Dataflow *get_3node_df() {
  size_t node_count = 3;
  std::vector<TOKEN_UNIT> node_weights{2, 2, 2};
  std::vector<TIME_UNIT> node_durations{1, 1, 1};
  std::vector<TOKEN_UNIT> edge_preloads{0, 0, 0};
  models::Dataflow *df = generators::new_normalized_cycle(
      node_count, node_weights, node_durations, edge_preloads);

  {
    ForEachEdge(df, c) { df->setEdgeType(c, EDGE_TYPE::FEEDBACK_EDGE); }
  }

  return df;
}

models::Dataflow *get_2node_df() {
  size_t node_count = 2;
  std::vector<TOKEN_UNIT> node_weights{2, 2};
  std::vector<TIME_UNIT> node_durations{1, 1};
  std::vector<TOKEN_UNIT> edge_preloads{0, 0};
  models::Dataflow *df = generators::new_normalized_cycle(
      node_count, node_weights, node_durations, edge_preloads);

  {
    ForEachEdge(df, c) { df->setEdgeType(c, EDGE_TYPE::FEEDBACK_EDGE); }
  }

  return df;
}

TokenConfiguration get_config(const models::Dataflow *dataflow) {
  std::map<ARRAY_INDEX, TOKEN_UNIT> configuration;
  {
    ForEachEdge(dataflow, e) {
      ARRAY_INDEX tid = dataflow->getEdgeId(e);
      if (dataflow->getEdgeType(e) == EDGE_TYPE::FEEDBACK_EDGE) {
        configuration[tid] = dataflow->getPreload(e);
      }
    }
  }
  return {dataflow, configuration};
}

BOOST_FIXTURE_TEST_SUITE(constraint_test, WITH_VERBOSE)

// Empty constraints have no effect on the configuration
BOOST_AUTO_TEST_CASE(empty_constraint_test) {
  std::map<ARRAY_INDEX, TOKEN_UNIT> conf_map;
  conf_map[1] = 0;
  conf_map[2] = 0;
  Constraint empty_constraint;
  auto config = get_config(get_2node_df());

  // Apply constraint
  empty_constraint.apply(config);

  // See if configs match
  BOOST_TEST(areConfigsEqual(config.getConfiguration(), conf_map));
}

/**
 * Single feeback buffer case, we expect the new tokens to be equal to
 * the amount we pass into the constraint
 */
BOOST_AUTO_TEST_CASE(single_edge_apply_test) {
  auto config = get_config(get_2node_df());
  std::map<ARRAY_INDEX, TOKEN_UNIT> constraint_map;
  constraint_map[1] = 4;
  Constraint single_edge_constraint(constraint_map);

  BOOST_TEST(config.getConfiguration().at(1) == 0);

  VERBOSE_DEBUG(
      "Initial preloads: " << commons::toString(config.getConfiguration()));
  auto new_config = single_edge_constraint.apply(config)[0];
  VERBOSE_DEBUG("Modified preloads: "
                << commons::toString(new_config.getConfiguration()));

  BOOST_TEST(new_config.getConfiguration().at(1) == 4);
}

/**
 * We have a graph with two feedback edges, both are set to 0 at the start
 * We set the minimum tokens required equal to 2, then we get the following
 * token distributions:
 * ID -> TOKENS
 * 1.
 *   0 -> 2
 *   1 -> 0
 * 2.
 *   0 -> 0
 *   1 -> 2
 * 3.
 *   0 -> 1
 *   1 -> 1
 */
BOOST_AUTO_TEST_CASE(multiple_feedback_edges_test) {
  auto config = get_config(get_2node_df());
  bool passed = true;
  std::vector<ARRAY_INDEX> feedback_buffers{1, 2};
  TIME_UNIT tgt_throughput = 0;
  TOKEN_UNIT min_tokens = 2;
  Constraint constraint({{{feedback_buffers, tgt_throughput}, min_tokens}});

  VERBOSE_DEBUG("Constraint before: \n" << constraint.toString());

  // We expect to find these configs
  std::vector<std::map<ARRAY_INDEX, TOKEN_UNIT>> options = {
      {{1, 2}, {2, 0}}, {{1, 0}, {2, 2}}, {{1, 1}, {2, 1}}};
  // Apply the constraint
  std::vector<TokenConfiguration> next_configs = constraint.apply(config);

  // Loop over the generated configurations
  for (const auto &conf : next_configs) {
    auto map = conf.getConfiguration();
    bool found = false;

    // Loop over the configs we expect to find, we break if we find a match
    for (const auto &opt : options) {
      if (found) {
        break;
      }
      found = areConfigsEqual(map, opt);
    }

    if (!found) {
      VERBOSE_DEBUG(
          "Configuration not found in expected: " << conf.to_csv_line());
      passed = false;
    }
  }

  BOOST_TEST(passed);
}

/**
 * Same as the test above, but we start with non-zero preloads
 */
BOOST_AUTO_TEST_CASE(multiple_feedback_edges_non_zero_test) {
  models::Dataflow *df = get_2node_df();
  df->setPreload(df->getEdgeById(1), 1);
  df->setPreload(df->getEdgeById(2), 1);
  auto config = get_config(df);
  bool passed = true;
  std::vector<ARRAY_INDEX> feedback_buffers{1, 2};
  TIME_UNIT tgt_throughput = 0;
  TOKEN_UNIT min_tokens = 4;
  Constraint constraint({{{feedback_buffers, tgt_throughput}, min_tokens}});

  VERBOSE_DEBUG("Constraint before: \n" << constraint.toString());

  // We expect to find these configs
  std::vector<std::map<ARRAY_INDEX, TOKEN_UNIT>> options = {
      {{1, 3}, {2, 1}}, {{1, 1}, {2, 3}}, {{1, 2}, {2, 2}}};
  // Apply the constraint
  std::vector<TokenConfiguration> next_configs = constraint.apply(config);

  // Loop over the generated configurations
  for (const auto &conf : next_configs) {
    auto map = conf.getConfiguration();
    bool found = false;

    // Loop over the configs we expect to find, we break if we find a match
    for (const auto &opt : options) {
      if (found) {
        break;
      }
      found = areConfigsEqual(map, opt);
    }

    if (!found) {
      VERBOSE_DEBUG(
          "Configuration not found in expected: " << conf.to_csv_line());
      passed = false;
    }
  }

  BOOST_TEST(passed);
}

/**
 * Testing when we have multiple overlapping constraints,
 * the results should be all possible configurations form both
 * constraints. In this case, we use the 2 constraints from
 * the test above, the first applied to edges 1,2 and the second
 * to edges 2,3
 */
BOOST_AUTO_TEST_CASE(overlapping_constraints_test) {
  models::Dataflow *df = get_3node_df();
  auto config = get_config(df);
  bool passed = true;
  Constraint constraint1({{{{1, 2}, 0}, 2}});
  Constraint constraint2({{{{2, 3}, 0}, 4}});

  // Apply first
  std::vector<TokenConfiguration> next_configs1 = constraint1.apply(config);
  // Apply second
  std::vector<TokenConfiguration> next_configs2 = constraint2.apply(config);

  std::vector<TokenConfiguration> next_configs;
  next_configs.reserve(next_configs1.size() + next_configs2.size());
  next_configs.insert(next_configs.end(), next_configs1.begin(),
                      next_configs1.end());
  next_configs.insert(next_configs.end(), next_configs2.begin(),
                      next_configs2.end());

  std::vector<std::map<ARRAY_INDEX, TOKEN_UNIT>> options = {
      {{1, 0}, {2, 1}, {3, 3}},
      {{1, 0}, {2, 3}, {3, 1}},
      {{1, 0}, {2, 2}, {3, 2}},
      {{1, 0}, {2, 0}, {3, 4}},
      {{1, 0}, {2, 4}, {3, 0}},
      {{1, 0}, {2, 2}, {3, 0}},
      {{1, 2}, {2, 0}, {3, 0}},
      {{1, 1}, {2, 1}, {3, 0}},
  };

  for (const auto &conf : next_configs) {
    auto map = conf.getConfiguration();
    bool found = false;

    // Loop over the configs we expect to find, we break if we find a match
    for (const auto &opt : options) {
      if (found) {
        break;
      }
      found = areConfigsEqual(map, opt);
    }

    if (!found) {
      VERBOSE_DEBUG(
          "Configuration not found in expected: " << conf.to_csv_line());
      passed = false;
    }
  }

  BOOST_TEST(passed);
}
BOOST_AUTO_TEST_SUITE_END()
