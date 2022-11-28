/**
 * @file GraphPool.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "../Utility/ConceptLib.hpp"
#include "../Utility/Graph.hpp"

#include <memory>
#include <stdexcept>

namespace Resource {

template <typename T>
requires Utility::StdOut<T>
class GraphPool {
    using Graph      = Utility::Graph<T>;
    using EdgeList   = typename Graph::EdgeList;
    using WEdgeList  = typename Graph::WEdgeList;
    using VertexList = typename Graph::VertexList;
    using EL         = EdgeList;
    using WEL        = WEdgeList;
    using VL         = VertexList;

    static void UnregisteredException() {
        throw std::runtime_error("You've not registered a Graph!");
    }

    static std::shared_ptr<Graph> TheGraph;

public:
    // You can't use ConstructorFunc
    GraphPool() = delete;

    /// @brief @b Register
    static void Register(
        const VL&   vl,
        const WEL&  wel,
        const bool& directed = false
    ) {
        if (TheGraph) {
            TheGraph.reset();
        }
        TheGraph = std::make_shared<Graph>(vl, wel, directed);
    }

    /// @brief @b get
    static std::shared_ptr<Graph> get() {
        if (!TheGraph) {
            UnregisteredException();
        }
        return TheGraph;
    }

    /// @brief @b set
    static void set(
        const VL&   vl,
        const EL&   el,
        const bool& directed = false
    ) {
        TheGraph.reset();
        TheGraph = std::make_shared<Graph>(vl, el, directed);
    }
};

template <typename T>
requires Utility::StdOut<T>
std::shared_ptr<Utility::Graph<T>> GraphPool<T>::TheGraph = nullptr;

} // namespace Resource