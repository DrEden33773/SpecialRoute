/**
 * @file main.cpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "TaskManager.hpp"
#include <iostream>

int main(int argc, char** argv) {
    // std::cout << "hello world!" << std::endl;
    // std::cout << __cplusplus;
    Task::run_all_task();
    return 0;
}
