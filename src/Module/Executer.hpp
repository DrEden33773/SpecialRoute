/**
 * @file Executer.hpp
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
#include "../Utility/DijkstraPlus.hpp"

#include <iostream>
#include <memory>
#include <vector>

namespace Module {

class Executer {
    using core_t = Utility::DijkstraPlus<int>;

    std::unique_ptr<core_t> Core = nullptr;

    Executer() = default;

    void register_Core() {
        Core = std::make_unique<core_t>(
            *Resource::GraphPool<int>::get()
        );
    }
    void run_Core() {
        Core->execute_algorithm();
    }
    void debug_show_all_min_route() {
        Core->show_all_min_route();
    }
    void show_numeric_result() {
        Core->show_num_of_sp_route();
    }

public:
    static void exec() {
        Executer exec;
        exec.register_Core();
        exec.run_Core();
        exec.debug_show_all_min_route();
        exec.show_numeric_result();
    }
};

} // namespace Module