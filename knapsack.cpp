#include "knapsack.h"
#include <vector>

KnapsackSolver::KnapsackSolver(const InputData& data) : data_(data) {}

std::vector<int> KnapsackSolver::allocateResources(int /*responderId*/) {
    const int capacity = data_.knapsackCapacity;

    // Collect resource indices that can physically fit.
    std::vector<int> viable;
    for (size_t i = 0; i < data_.resources.size(); ++i) {
        if (data_.resources[i].weight <= capacity) {
            viable.push_back(static_cast<int>(i));
        }
    }

    int n = static_cast<int>(viable.size());

    // 0/1 Knapsack DP with a 2D keep-table for correct reconstruction.
    std::vector<int> dp(capacity + 1, 0);
    std::vector<std::vector<bool>> keep(n, std::vector<bool>(capacity + 1, false));

    for (int i = 0; i < n; ++i) {
        int idx = viable[i];
        int wt  = data_.resources[idx].weight;
        int val = data_.resources[idx].value;

        for (int w = capacity; w >= wt; --w) {
            if (dp[w - wt] + val > dp[w]) {
                dp[w]      = dp[w - wt] + val;
                keep[i][w] = true;
            }
        }
    }

    // Reconstruct: walk backwards through items.
    std::vector<int> selected;
    int w = capacity;
    for (int i = n - 1; i >= 0; --i) {
        if (keep[i][w]) {
            selected.push_back(viable[i]);
            w -= data_.resources[viable[i]].weight;
        }
    }

    return selected;
}
