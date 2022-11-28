/**
 * @file Initializer.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "../Resource/GraphPool.hpp"

#include "../Utility/ConceptLib.hpp"
#include "../Utility/FileManager.hpp"
#include "../Utility/Graph.hpp"

#include <iostream>
#include <vector>

namespace Module {

class Initializer {
    using VexList   = Utility::Graph<int>::VertexList;
    using WEdgeList = Utility::Graph<int>::WEdgeList;

    int              size = 0;
    std::vector<int> relation;

    VexList   Vexes;
    WEdgeList Edges;
    int       source_vex = 0;

private:
    void InitVex() {
        Vexes.reserve(size);
        for (int i = 0; i < size; ++i) {
            int vex = i + 1;
            Vexes.emplace_back(vex);
        }
    }
    void InitEdge() {
        for (int i = 0; i < size; ++i) {
            int from_vex = relation.at(i);
            int to_vex   = i + 1;
            if (from_vex == 0) {
                source_vex = to_vex;
            } else {
                Edges.push_back(
                    std::make_tuple(
                        from_vex,
                        to_vex,
                        1 /* set weight always equal to 1 */
                    )
                );
            }
        }
    }

private:
    bool IfHaveRoot() {
        // TODO(eden):
        return true;
    }
    bool IfOnlyOneRoot() {
        // TODO(eden):
        return true;
    }
    void InputSize() {
        int tmp = 0;
        while (true) {
            std::cout << "Input number of vertex(>=1): ";
            std::cin >> tmp;
            if (tmp >= 1) {
                break;
            } else {
                std::cout << "Input number should >=1, try to input again... ";
                std::cout << std::endl;
                std::cout << std::endl;
            }
        }
        size = tmp;
    }
    void InputRelation() {
        std::vector<int> tmp(size, 0);
        while (true) {
            std::cout << "Input the relation of vertex: ";
            std::cout << std::endl;
            std::cout << ">>> ";
            for (int i = 0; i < size; ++i) {
                int in = 0;
                std::cin >> in;
                tmp.at(i) = in;
            }
            if (IfHaveRoot() && IfOnlyOneRoot()) {
                break;
            } else {
                std::cout << "Invalid relation, try to input again... ";
                std::cout << std::endl;
                std::cout << std::endl;
            }
        }
        relation = tmp;
        std::cout << std::endl;
    }

public:
    static auto KeyboardInput() {
        Initializer init;
        // input
        init.InputSize();
        init.InputRelation();
        // init
        init.InitVex();
        init.InitEdge();
        // register
        Resource::GraphPool<int>::Register(
            init.Vexes,
            init.Edges,
            false
        );

        // return source_vex
        return init.source_vex;
    }
    static auto FileInit() {
        // TODO(eden):
    }
};

} // namespace Module