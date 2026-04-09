#include "dijkstra.h"
#include <algorithm>
#include <queue>

Dijkstra::Dijkstra(const Graph& graph)
    : graph_(graph),
      distances_(graph.getNumNodes(), std::numeric_limits<int>::max()),
      processed_(graph.getNumNodes(), false) {}

std::vector<int> Dijkstra::computeShortestPaths(int startNode) {
    distances_[startNode] = 0;

    // Min-heap: {distance, node}
    using PQItem = std::pair<int, int>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<>> pq;
    pq.push({0, startNode});

    while (!pq.empty()) {
        auto [dist, node] = pq.top();
        pq.pop();

        if (processed_[node]) continue;
        processed_[node] = true;

        for (const auto& [neighbor, weight] : graph_.getAdj(node)) {
            int newDist = distances_[node] + weight;
            if (newDist < distances_[neighbor]) {
                distances_[neighbor]    = newDist;
                predecessors_[neighbor] = node;
                pq.push({newDist, neighbor});
            }
        }
    }

    return distances_;
}

int Dijkstra::getDistance(int targetNode) const {
    return distances_[targetNode];
}

std::vector<int> Dijkstra::getPath(int targetNode) const {
    std::vector<int> path;

    // If target is the source itself (distance 0, no predecessor).
    if (predecessors_.find(targetNode) == predecessors_.end()
        && distances_[targetNode] == 0) {
        path.push_back(targetNode);
        return path;
    }

    // Walk the predecessor chain backwards.
    for (int cur = targetNode; ; cur = predecessors_.at(cur)) {
        path.push_back(cur);
        if (predecessors_.find(cur) == predecessors_.end()) break;
    }

    std::reverse(path.begin(), path.end());
    return path;
}
