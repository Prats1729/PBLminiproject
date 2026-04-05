#ifndef BFS_H
#define BFS_H

#include "Data.h"
#include "Graph.h"
#include <vector>

std::vector<int> bfsNearestResponder(
    const Graph &graph, // a readonly refernce to the graph
    int startNode, // node wehre disaster startes
    const std::vector<int> &responderNodes, // list of reponder IDs
    int &outDistance // how many steps to nearest reponder
);

#endif