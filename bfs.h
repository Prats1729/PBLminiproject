#ifndef BFS_H
#define BFS_H

#include "Data.h"
#include "Graph.h"
#include <vector>

std::vector<int> bfsAllResponders(
    const Graph &graph, // a readonly reference to the graph
    int startNode, // node where disaster starts
    const std::vector<int> &responderNodes // list of responder IDs
);

#endif