//
// Created by toky on 24/4/23.
//

#ifndef KITER_TOKENCONFIGURATIONSET_H
#define KITER_TOKENCONFIGURATIONSET_H

#include "TokenConfiguration.h"
#include <algorithms/dse/StorageDistributionSet.h>

namespace algorithms {
    namespace dse {

        class TokenConfigurationSet {
        public:
            void add(const TokenConfiguration& config);
            void remove(const TokenConfiguration& config);
            bool isDominated(const TokenConfiguration& config);
            size_t cleanByDominance(const TokenConfiguration& newConfig) ;
            bool contains(const TokenConfiguration& config);

            std::string toString() ;

            bool hasConfigWithCost(TokenConfiguration::CostUnit i);

            bool empty() {
                return this->configurations_by_cost.empty();
            }

            TIME_UNIT getTotalProcessTimeMs() const {
                if (this->last_added != nullptr) {
                    return this->last_added->getCumulExecutionTimeMs();
                } else {
                    return 0.0;
                }
            }
            const TokenConfiguration& top () const {
                return *(this->configurations_by_cost.begin()->second.begin());
            }
            const TokenConfiguration& last () const {
                return *(std::prev(std::prev(this->configurations_by_cost.end())->second.end()));
            }

            void pop_last () {
                // remove the last config
                std::prev(this->configurations_by_cost.end())->second.erase(std::prev(std::prev(this->configurations_by_cost.end())->second.end()));

                // remove empty set
                if (std::prev(this->configurations_by_cost.end())->second.empty()) {
                    this->configurations_by_cost.erase(std::prev(this->configurations_by_cost.end()));
                }
            }

            void pop () {
                // remove the first config
                this->configurations_by_cost.begin()->second.erase(this->configurations_by_cost.begin()->second.begin());

                // remove empty set
                if (this->configurations_by_cost.begin()->second.empty()) {
                    this->configurations_by_cost.erase(this->configurations_by_cost.begin());
                }
            }

            void push(const TokenConfiguration& config) {
                this->add(config);
            }
            // TODO: make this a cached value.
            size_t size() const {
                size_t sum = 0;
                for (auto cost_items : this->configurations_by_cost) {
                    sum += cost_items.second.size();
                }
                return sum;
            }

            const TokenConfiguration *getBestPerformancePoint() const {
                return this->best_point;
            }

            // Convert this set of TokenConfigurations (defined on a feedback-augmented graph)
            // into a set of StorageDistributions for the original graph without feedback edges.
            // For each normal edge e in original, storage = floor(preload(e), FineGCD(e)) + tokens on e_prime in the token configuration.
            StorageDistributionSet toStorageDistributionSet(const models::Dataflow* original) const;

        private:

            struct TokenConfigurationCostComparator {
                bool operator()(const TokenConfiguration& lhs, const TokenConfiguration& rhs) const {
                    return lhs.getCost() > rhs.getCost();
                }
            };

            struct TokenConfigurationValuesComparator {
                bool operator()(const TokenConfiguration& lhs, const TokenConfiguration& rhs) const {
                    auto lhs_config = lhs.getConfiguration();
                    auto rhs_config = rhs.getConfiguration();

                    for (auto lhs_it = lhs_config.begin(), rhs_it = rhs_config.begin(); lhs_it != lhs_config.end() && rhs_it != rhs_config.end(); ++lhs_it, ++rhs_it) {
                        if (lhs_it->first != rhs_it->first) {
                            return lhs_it->first < rhs_it->first;
                        }
                        if (lhs_it->second != rhs_it->second) {
                            return lhs_it->second < rhs_it->second;
                        }
                    }

                    return false;
                }
            };

            std::map<TokenConfiguration::CostUnit, std::set<TokenConfiguration, TokenConfigurationValuesComparator> > configurations_by_cost;

            const TokenConfiguration *best_point = nullptr;
            friend class ModularDSE;


            class iterator {
            public:
                using iterator_category = std::forward_iterator_tag;
                using value_type = TokenConfiguration;
                using difference_type = std::ptrdiff_t;
                using pointer = TokenConfiguration*;
                using reference = TokenConfiguration&;

                iterator() = default;

                explicit iterator(std::map<TokenConfiguration::CostUnit, std::set<TokenConfiguration, TokenConfigurationValuesComparator> >::iterator cost_it,
                                  std::map<TokenConfiguration::CostUnit, std::set<TokenConfiguration, TokenConfigurationValuesComparator> >::iterator cost_end,
                                  std::set<TokenConfiguration, TokenConfigurationValuesComparator>::iterator config_it = {}) :
                        cost_it_(cost_it), cost_end_(cost_end), config_it_(config_it) {
                    if (cost_it != cost_end && config_it == cost_it_->second.end()) {
                        ++(*this);
                    }
                }

                const TokenConfiguration& operator*() const { return *config_it_; }
                const TokenConfiguration* operator->() const { return &(*config_it_); }

                // Prefix increment
                iterator& operator++() {
                    ++config_it_;
                    if (config_it_ == cost_it_->second.end()) {
                        ++cost_it_;
                        if (cost_it_ != cost_end_) {
                            config_it_ = cost_it_->second.begin();
                        }
                    }
                    return *this;
                }

                // Postfix increment
                iterator operator++(int) {
                    iterator tmp = *this;
                    ++(*this);
                    return tmp;
                }

                friend bool operator== (const iterator& a, const iterator& b) {
                    return a.cost_it_ == b.cost_it_ && (a.cost_it_ == a.cost_end_ || a.config_it_ == b.config_it_);
                };

                friend bool operator!= (const iterator& a, const iterator& b) { return !(a == b); }

            private:
                std::map<TokenConfiguration::CostUnit, std::set<TokenConfiguration, TokenConfigurationValuesComparator> >::iterator cost_it_;
                std::map<TokenConfiguration::CostUnit, std::set<TokenConfiguration, TokenConfigurationValuesComparator> >::iterator cost_end_;
                std::set<TokenConfiguration, TokenConfigurationValuesComparator>::iterator config_it_;
            };

            const TokenConfiguration *last_added = nullptr;

        public:
            iterator begin() {
                if (configurations_by_cost.empty()) {
                    return end();
                } else {
                    auto cost_it = configurations_by_cost.begin();
                    return iterator(cost_it, configurations_by_cost.end(), cost_it->second.begin());
                }
            }

            iterator end() {
                return iterator(configurations_by_cost.end(), configurations_by_cost.end());
            }

            void clear();
        };


    } // algorithms
} // dse

#endif //KITER_TOKENCONFIGURATIONSET_H
