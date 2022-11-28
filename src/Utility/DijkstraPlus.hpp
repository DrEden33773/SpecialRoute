/**
 * @file DijkstraPlus.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022
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
class DijkstraPlus {
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
    Matrix<int> Flag;

    std::unordered_map<IntPair, VexList, IntPairHash> AllMinRoute;
    std::unordered_set<IntPair, IntPairHash>          NoRouteIdxPair;

    std::vector<std::pair<T, T>> SpRouteList;

    bool if_closer_judger(
        int joined_to_curr,
        int source_to_curr
    ) {
        bool res = false;
        // joined_to_curr < source_to_curr
        // b < c
        const int& b            = joined_to_curr;
        const int& c            = source_to_curr;
        bool       if_b_lim     = b == Data->LIM;
        bool       if_c_lim     = c == Data->LIM;
        bool       if_bc_no_lim = !if_b_lim && !if_c_lim;
        if (if_bc_no_lim) {
            res = ((b) < (c));
        } else {
            if (!if_b_lim && if_c_lim) {
                res = true;
            }
            // all cases else should be `false`
        }
        return res;
    }
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
    int find_closest_unjoined_idx(const int& source) {
        int min_idx = 0;
        // 1. locate first available idx
        for (int idx = 0; idx < Data->size; ++idx) {
            if (Flag[source][idx]) {
                continue;
            }
            min_idx = idx;
            break;
        }
        // 2. find
        for (int idx = 0; idx < Data->size; ++idx) {
            if (Flag[source][idx]) {
                continue;
            }
            if (Dist[source][idx] == Data->LIM) {
                continue;
            }
            if (if_closer_judger(
                    Dist[source][idx],
                    Dist[source][min_idx]
                )) {
                min_idx = idx;
            }
        }
        // 3. return
        return min_idx;
    }
    void build_min_route_of(const int& source) {
        for (int end = 0; end < size; ++end) {
            auto key_pair = std::make_pair(source, end);
            T&   end_vex  = Data->Index_V_Map[end];

            std::list<T>& curr = AllMinRoute.at(key_pair);

            int  trace_back            = end;
            bool if_no_route_to_source = false;

            while (trace_back != source) {
                int adj = Adj[source][trace_back];
                if (adj == -1) {
                    curr.clear();
                    if_no_route_to_source = true;
                    break;
                }
                T& adj_vex = Data->Index_V_Map[adj];
                curr.push_front(adj_vex);
                trace_back = adj;
            }
            if (if_no_route_to_source) {
                NoRouteIdxPair.insert(key_pair);
            } else {
                curr.push_back(end_vex);
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

    DijkstraPlus() = default;

public:
    explicit DijkstraPlus(Utility::Graph<T>& graph)
        : Data(&graph)
        , size(graph.size) {
        // basic check
        Data->make_sure_non_empty();
        Data->make_sure_weighted();
        // init `Adj` and `Dist`
        Dist = Matrix<int>(size, SubMatrix<int>(size, 0));
        Adj  = Matrix<int>(size, SubMatrix<int>(size, 0));
        Flag = Matrix<int>(size, SubMatrix<int>(size, 0));
        for (int source = 0; source < size; ++source) {
            // flag
            Flag[source][source] = 1;
            // dist, adj, MinRouteList
            for (int end = 0; end < size; ++end) {
                // dist
                int source_to_end = Data->Mat[source][end];
                Dist[source][end] = source_to_end;
                // adj
                if (source_to_end != Data->LIM) {
                    Adj[source][end] = source;
                } else {
                    Adj[source][end] = -1;
                }
                // MinRoute
                auto key_pair         = std::make_pair(source, end);
                AllMinRoute[key_pair] = std::list<T>();
            }
        }
    }
    void execute_algorithm() {
        for (int source = 0; source < size; ++source) {
            int unjoined_num = Data->size;
            while (unjoined_num > 0) {
                // 1) find closest of `passed-->source`
                int passed = find_closest_unjoined_idx(source);
                // 2) set visited
                Flag[source][passed] = 1;
                int source_to_passed = Dist[source][passed];
                if (source_to_passed == Data->LIM) {
                    continue;
                }
                // 3) update all other dist
                for (int curr = 0; curr < Data->size; ++curr) {
                    int source_to_curr = Dist[source][curr];
                    int passed_to_curr = Data->Mat[passed][curr];
                    // source->passed->curr < source-->curr
                    bool if_unvisited = Flag[source][curr] == 0;
                    bool if_closer    = if_closer_judger(
                        source_to_passed, /* a */
                        passed_to_curr,   /* b */
                        source_to_curr    /* c */
                        /* Judge => a + b < c (consider LIM)*/
                    );
                    if (if_unvisited && if_closer) {
                        // need to update
                        Dist[source][curr] = source_to_passed + passed_to_curr;
                        Adj[source][curr]  = passed;
                    }
                }
                // 4) update tag
                --unjoined_num;
            }
            // could update path
            build_min_route_of(source);
        }
        // build_sp_route
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
