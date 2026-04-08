#include "dijkstra.h"
#include <algorithm>
#include <unordered_set>
#include <limits>

Dijkstra::Dijkstra(const Graph& graph) : graph(graph) {
    int n = graph.getNumNodes();
    distances.resize(n, std::numeric_limits<int>::max());
    processed.resize(n, false);
    // predecessors map will be filled lazily
}

std::vector<int> Dijkstra::computeShortestPaths(int startNode) {
    distances[startNode] = 0;
    // priority queue ordered by smallest distance first
    using PQItem = std::pair<int, int>; // {distance, node}
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;
    pq.push({0, startNode});
    std::unordered_set<int> visited;

    while (!pq.empty()) {
        auto [currentDist, currentNode] = pq.top();
        pq.pop();
        if (visited.find(currentNode) != visited.end()) continue;
        visited.insert(currentNode);
        if (processed[currentNode]) continue;
        processed[currentNode] = true;

        // Relax edges from currentNode
        for (const auto& edge : graph.getAdj(currentNode)) {
            int neighbor = edge.first;
            int weight = edge.second;
            if (distances[currentNode] + weight < distances[neighbor]) {
                distances[neighbor] = distances[currentNode] + weight;
                predecessors[neighbor] = currentNode;
                pq.push({distances[neighbor], neighbor});
            }
        }
    }
    return distances;
}

int Dijkstra::getDistance(int targetNode) {
    return distances[targetNode];
}

std::vector<int> Dijkstra::getPath(int targetNode) {
    std::vector<int> path;
    // if there is no predecessor for targetNode and it's not the start node, path is empty
    if (predecessors.find(targetNode) == predecessors.end() && distances[targetNode] == 0) {
        path.push_back(targetNode);
        return path;
    }
    for (int cur = targetNode; ; cur = predecessors[cur]) {
        path.push_back(cur);
        if (predecessors.find(cur) == predecessors.end()) break;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void Dijkstra::dijkstraStep(int currentNode) {
    // Not used in current implementation; kept for potential custom step extensions.
}
