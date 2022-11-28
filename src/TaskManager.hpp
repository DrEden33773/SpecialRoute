/**
 * @file TaskManager.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "Module/Executer.hpp"
#include "Module/Initializer.hpp"

namespace Task {

void run_all_task() {
    Module::Initializer::KeyboardInput();
    Module::Executer::exec();
}

} // namespace Task