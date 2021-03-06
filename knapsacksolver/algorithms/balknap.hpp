#pragma once

#include "knapsacksolver/solution.hpp"
#include "knapsacksolver/part_solution_1.hpp"

#include <thread>
#include <map>

namespace knapsacksolver
{

struct BalknapOptionalParameters
{
    Info info = Info();

    char ub = 't';
    bool greedy = true;
    StateIdx greedynlogn = -1;
    StateIdx surrelax = -1;
    ItemPos partial_solution_size = 64;

    bool* end = NULL;
    bool stop_if_end = false;
    bool set_end = true;

    BalknapOptionalParameters& set_pure()
    {
        ub = 'b';
        greedy = false;
        greedynlogn = -1;
        surrelax = -1;
        partial_solution_size = 64;
        return *this;
    }

    BalknapOptionalParameters& set_combo()
    {
        ub = 't';
        greedy = -1;
        greedynlogn = 0;
        surrelax = 2000;
        partial_solution_size = 64;
        return *this;
    }

};

struct BalknapOutput: Output
{
    BalknapOutput(const Instance& instance, Info& info): Output(instance, info) { }
    Counter recursive_call_number = 0;

    BalknapOutput& algorithm_end(Info& info)
    {
        PUT(info, "Algorithm", "RecursiveCallNumber", recursive_call_number);
        Output::algorithm_end(info);
        VER(info, "Recursive call number: " << recursive_call_number << std::endl);
        return *this;
    }
};

BalknapOutput balknap(Instance& instance, BalknapOptionalParameters p = {});

}

