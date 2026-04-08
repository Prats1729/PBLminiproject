#include "knapsack.h"
#include <algorithm>

KnapsackSolver::KnapsackSolver(const InputData& data) : data(data) {
    // Allocate DP arrays sized to the knapsack capacity (+1 for zero weight)
    dp.resize(data.knapsackCapacity + 1, 0);
    parent.resize(data.knapsackCapacity + 1, -1);
}

std::vector<int> KnapsackSolver::allocateResources(int /*responderId*/) {
    // Build a list of resource indices that can possibly fit.
    std::vector<int> viable;
    for (size_t i = 0; i < data.resources.size(); ++i) {
        if (data.resources[i].weight <= data.knapsackCapacity) {
            viable.push_back(static_cast<int>(i));
        }
    }

    // 0/1 knapsack DP – fill dp[] from high weight to low
    for (int w = 0; w <= data.knapsackCapacity; ++w) {
        dp[w] = 0;
        parent[w] = -1;
    }

    for (int idx : viable) {
        int wt = data.resources[idx].weight;
        int val = data.resources[idx].value;

        for (int w = data.knapsackCapacity; w >= wt; --w) {
            if (dp[w - wt] + val > dp[w]) {
                dp[w] = dp[w - wt] + val;
                parent[w] = idx; // store which resource gave this improvement
            }
        }
    }

    // Reconstruct the selected resources from the parent array
    std::vector<int> selected;
    int w = data.knapsackCapacity;
    while (w > 0 && parent[w] != -1) {
        int resId = parent[w];
        selected.push_back(resId);
        w -= data.resources[resId].weight;
    }

    return selected;
}
