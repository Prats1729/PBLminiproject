#ifndef KNAPSACK_H
#define KNAPSACK_H

#include "Data.h"

class KnapsackSolver {
public:
    KnapsackSolver(const InputData& data);

    // Compute optimal resource allocation for a given responder
    std::vector<int> allocateResources(int responderId);

private:
    const InputData& data;
    std::vector<int> dp; // DP array for knapsack
    std::vector<int> parent; // Parent pointers for path reconstruction
};

#endif // KNAPSACK_H