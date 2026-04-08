#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>

using namespace std;

class Dijkstra {
public:
    explicit Dijkstra(const Graph& graph);

    // Compute shortest paths from startNode to all nodes
    vector<int> computeShortestPaths(int startNode);

    // Get distance from startNode to targetNode
    int getDistance(int targetNode);

    // Get the actual shortest path as node indices
    vector<int> getPath(int targetNode);

private:
    const Graph& graph;
    vector<int> distances;
    unordered_map<int, int> predecessors;
    vector<bool> processed;

    void dijkstraStep(int currentNode);
};

#endif // DIJKSTRA_H