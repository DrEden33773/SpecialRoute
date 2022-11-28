/**
 * @file Floyd.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief Floyd Algorithm for Graph
 * @version 0.1
 * @date 2022-11-18
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * @warning @b Discarded!
 *
 */

#pragma once

#include "Graph.hpp"

#include <algorithm>
#include <iostream>
#include <list>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Utility {

template <typename T>
requires std::equality_comparable<T>
class Floyd {
    template <typename U>
    using Matrix = std::vector<std::vector<U>>;
    template <typename U>
    using SubMatrix = std::vector<U>;

    using IntPair = std::pair<int, int>;
    using VexList = std::list<T>;

    struct IntPairHash {
        std::size_t operator()(const IntPair& s) const {
            std::size_t h1 = std::hash<int> {}(s.first);
            std::size_t h2 = std::hash<int> {}(s.second);
            return h1 ^ (h2 << 1);
        }
    };

    Graph<T>* Data = nullptr;
    int       size = 0;

    Matrix<int> Dist;
    Matrix<int> Adj;

    std::unordered_map<IntPair, VexList, IntPairHash> AllMinRoute;
    std::unordered_set<IntPair, IntPairHash>          NoRouteIdxPair;

    std::vector<std::pair<T, T>> SpRouteList;

private:
    bool if_closer_judger(
        int source_to_passed,
        int passed_to_end,
        int source_to_end
    ) {
        bool res = false;
        // source_to_passed + passed_to_end < source_to_end
        // a + b < c
        const int& a             = source_to_passed;
        const int& b             = passed_to_end;
        const int& c             = source_to_end;
        bool       if_a_lim      = a == Data->LIM;
        bool       if_b_lim      = b == Data->LIM;
        bool       if_c_lim      = c == Data->LIM;
        bool       if_ab_no_lim  = !if_a_lim && !if_b_lim;
        bool       if_abc_no_lim = !if_a_lim && !if_b_lim && !if_c_lim;
        if (if_abc_no_lim) {
            res = ((a + b) < (c));
        } else {
            if (if_ab_no_lim && if_c_lim) {
                res = true;
            }
            // all cases else should be `false`
        }
        return res;
    }

    bool forward_trace(
        const int&    source,
        const int&    end,
        std::list<T>& list
    ) {
        bool if_success = true;

        auto key_pair   = std::make_pair(source, end);
        T&   end_vex    = Data->Index_V_Map[end];
        T&   source_vex = Data->Index_V_Map[source];

        int  trace_idx             = source;
        bool if_no_route_to_source = false;

        // source_vex will be missing
        while (trace_idx != end) {
            int adj_idx = Adj[trace_idx][end];
            if (adj_idx == -1) {
                list.clear();
                if_no_route_to_source = true;
                break;
            }
            if (adj_idx == trace_idx) {
                if_success = false;
                break;
            }
            T& adj_vex = Data->Index_V_Map[adj_idx];
            list.push_back(adj_vex);
            trace_idx = adj_idx;
        } // source_vex will be missing

        if (if_no_route_to_source) {
            NoRouteIdxPair.insert(key_pair);
        }

        return if_success;
    }

    bool reversed_trace(
        const int&    source,
        const int&    end,
        std::list<T>& list
    ) {
        bool if_success = true;

        auto key_pair   = std::make_pair(source, end);
        T&   end_vex    = Data->Index_V_Map[end];
        T&   source_vex = Data->Index_V_Map[source];

        int  trace_back_idx        = end;
        bool if_no_route_to_source = false;

        // end_vex will be missing
        while (trace_back_idx != source) {
            int adj_idx = Adj[source][trace_back_idx];
            if (adj_idx == -1) {
                list.clear();
                if_no_route_to_source = true;
                break;
            }
            if (adj_idx == trace_back_idx) {
                if_success = false;
                break;
            }
            T& adj_vex = Data->Index_V_Map[adj_idx];
            list.push_front(adj_vex);
            trace_back_idx = adj_idx;
        } // end_vex will be missing

        if (if_no_route_to_source) {
            NoRouteIdxPair.insert(key_pair);
        }

        return if_success;
    }

    void build_all_min_route() {
        for (int source = 0; source < size; ++source) {
            int dbg_source = source;
            for (int end = 0; end < size; ++end) {
                auto key_pair = std::make_pair(source, end);

                std::list<T> reversed_traced;
                std::list<T> forward_traced;

                bool if_rev_success = reversed_trace(source, end, reversed_traced);
                bool if_fwd_success = forward_trace(source, end, forward_traced);

                bool fwd_eq_rev = forward_traced.size() == reversed_traced.size();
                bool fwd_gt_rev = forward_traced.size() > reversed_traced.size();
                bool fwd_lt_rev = forward_traced.size() < reversed_traced.size();

                bool if_used_forward_one  = false;
                bool if_used_reversed_one = false;

                if (!if_rev_success) {
                    AllMinRoute[key_pair] = forward_traced;
                    if_used_forward_one   = true;
                } else if (!if_fwd_success) {
                    AllMinRoute[key_pair] = reversed_traced;
                    if_used_reversed_one  = true;
                } else {
                    if (fwd_eq_rev || fwd_gt_rev) {
                        AllMinRoute[key_pair] = forward_traced;
                        if_used_forward_one   = true;
                    } else {
                        AllMinRoute[key_pair] = reversed_traced;
                        if_used_reversed_one  = true;
                    }
                }

                // add the missing vex
                if (NoRouteIdxPair.contains(key_pair)) {
                    continue;
                }
                std::list<T>& curr_route = AllMinRoute.at(key_pair);
                if (if_used_reversed_one) { // end_vex missing
                    T& end_vex = Data->Index_V_Map[end];
                    curr_route.push_back(end_vex);
                } else { // source_vex missing
                    T& source_vex = Data->Index_V_Map[source];
                    curr_route.push_front(source_vex);
                }
            }
        }
    }
    void build_sp_route_list() {
        for (int source = 0; source < size; ++source) {
            int end_init = 0;
            if (!Data->if_directed) {
                end_init = source;
            }
            for (int end = end_init; end < size; ++end) {
                auto key_pair = std::make_pair(source, end);
                T&   src      = Data->Index_V_Map[source];
                T&   ed       = Data->Index_V_Map[end];

                if (src == ed) {
                    continue;
                }
                bool src_gt_ed = src > ed;
                bool src_lt_ed = src < ed;

                std::list<T>& route         = AllMinRoute.at(key_pair);
                bool          if_route_suit = true;

                for (auto&& vex : route) {
                    bool if_in_range = false;
                    if (src_gt_ed) {
                        if_in_range = src >= vex && vex >= ed;
                    } else {
                        if_in_range = src <= vex && vex <= ed;
                    }
                    if (!if_in_range) {
                        if_route_suit = false;
                        break;
                    }
                }

                if (if_route_suit) {
                    SpRouteList.push_back(
                        std::make_pair(src, ed)
                    );
                }
            }
        }
    }

    Floyd() = default;

public:
    explicit Floyd(Utility::Graph<T>& graph)
        : Data(&graph)
        , size(graph.size) {
        // basic check
        Data->make_sure_non_empty();
        Data->make_sure_weighted();
        // init `Adj` and `Dist`
        Dist = Matrix<int>(size, SubMatrix<int>(size, 0));
        Adj  = Matrix<int>(size, SubMatrix<int>(size, 0));
        for (int source = 0; source < size; ++source) {
            for (int end = 0; end < size; ++end) {
                int source_to_end = Data->Mat[source][end];
                Dist[source][end] = source_to_end;
                if (source_to_end != Data->LIM) {
                    Adj[source][end] = source;
                } else {
                    Adj[source][end] = -1;
                }
            }
        }
    }
    void execute_algorithm() {
        for (int passed = 0; passed < size; ++passed) {
            for (int source = 0; source < size; ++source) {
                for (int end = 0; end < size; ++end) {
                    int source_to_end    = Dist[source][end];
                    int source_to_passed = Dist[source][passed];
                    int passed_to_end    = Dist[passed][end];
                    if (if_closer_judger(
                            source_to_passed,
                            passed_to_end,
                            source_to_end
                        )) {
                        Dist[source][end] = source_to_passed + passed_to_end;
                        Adj[source][end]  = passed;
                    }
                }
            }
        }
        // store the route
        build_all_min_route();
        // seek sp route
        build_sp_route_list();
    }
    void show_all_min_dist() {
        for (int source = 0; source < size; ++source) {
            for (int end = 0; end < size; ++end) {
                T& src = Data->Index_V_Map[source];
                T& ed  = Data->Index_V_Map[end];
                std::cout << "{ " << src << " -> " << ed << " } min distance : ";
                if (Dist[source][end] == Data->LIM) {
                    std::cout << "NaN" << std::endl;
                } else {
                    std::cout << Dist[source][end] << std::endl;
                }
            }
        }
        std::cout << std::endl;
    }
    void show_all_min_route() {
        for (int source = 0; source < size; ++source) {
            for (int end = 0; end < size; ++end) {
                auto key_pair = std::make_pair(source, end);
                T&   src      = Data->Index_V_Map[source];
                T&   ed       = Data->Index_V_Map[end];

                std::list<T>& curr_route = AllMinRoute.at(key_pair);
                std::cout << "{ " << src << " -> " << ed << " } min route : ";
                std::for_each(
                    curr_route.begin(),
                    curr_route.end(),
                    [](const T& vex) {
                        std::cout << vex << " ";
                    }
                );
                if (!curr_route.size()) {
                    std::cout << "No route!";
                }
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    }
    void show_num_of_sp_route() {
        std::cout << "Number of Special Routes: " << SpRouteList.size();
        std::cout << std::endl;
        std::cout << std::endl;
    }
};

} // namespace Utility