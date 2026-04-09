#ifndef BFS_H
#define BFS_H

#include "Graph.h"
#include <vector>

// Find all responder nodes reachable from startNode via BFS traversal.
std::vector<int> bfsAllResponders(
    const Graph& graph,
    int startNode,
    const std::vector<int>& responderNodes
);

#endif // BFS_H