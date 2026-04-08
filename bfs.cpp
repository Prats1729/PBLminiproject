#include "bfs.h"
#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>

// finding all responders reachable from disaster site
std::vector<int> bfsAllResponders(
    const Graph &graph,
    int startNode,
    const std::vector<int> &responderNodes
){
    if (responderNodes.empty() || startNode < 0 || startNode >= graph.getNumNodes()){
        return {}; // returns empty list
    }

    // set of responder node ids for fast lookup during BFS
    std::vector<bool> isResponder(graph.getNumNodes(), false);
    for (int r : responderNodes){
        if (r >= 0 && r < graph.getNumNodes()) {
            isResponder[r] = true;
        }
    }

    std::queue<int> q; // queue for bfs
    std::vector<bool> visited(graph.getNumNodes(), false); // visited vector

    // start the bfs
    q.push(startNode);
    visited[startNode] = true;

    std::vector<int> foundResponders;

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        // check if current node is a responder
        if (isResponder[current]){
            foundResponders.push_back(current);
        }

        // explore all adjacent nodes
        for (const auto& edge : graph.getAdj(current)){
            int neighbor = edge.first;
            if (!visited[neighbor]){
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    return foundResponders;
}