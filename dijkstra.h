#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"
#include <limits>
#include <unordered_map>
#include <vector>

// Computes single-source shortest paths on the city graph using Dijkstra's algorithm.
class Dijkstra {
public:
    explicit Dijkstra(const Graph& graph);

    // Compute shortest distances from startNode to every reachable node.
    std::vector<int> computeShortestPaths(int startNode);

    // Distance from the last-computed source to targetNode (INT_MAX if unreachable).
    int getDistance(int targetNode) const;

    // Reconstruct the shortest path as a sequence of node indices.
    std::vector<int> getPath(int targetNode) const;

private:
    const Graph& graph_;
    std::vector<int>  distances_;
    std::vector<bool> processed_;
    std::unordered_map<int, int> predecessors_;
};

#endif // DIJKSTRA_H