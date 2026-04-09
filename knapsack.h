#ifndef KNAPSACK_H
#define KNAPSACK_H

#include "Data.h"
#include <vector>

// 0/1 Knapsack solver for optimal relief-resource allocation.
class KnapsackSolver {
public:
    explicit KnapsackSolver(const InputData& data);

    // Select the best set of resources that fit within the vehicle capacity.
    std::vector<int> allocateResources(int responderId);

private:
    const InputData& data_;
};

#endif // KNAPSACK_H