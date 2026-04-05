#include "bfs.h"
#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>

// finding the nearest repondeer to the disater sire
std::vector<int> bfsNearestResponder(
    const Graph &graph,
    int startNode,
    const std::vector<int> &responderNodes,
    int &outDistance
){
    if (responderNodes.empty() || startNode < 0){
        outDistance = -1; // no valid responders
        return {}; // returns empty path
    }

    // set of responer nodes ids for fat looup during BFS
    std::vector<bool> isResponder(graph.getNumNodes(), false);
    for (int r : responderNodes){
        isResponder[r] = true;
    }

    std::queue<int> q; // queue for bfs
    std::vector<bool> visited(graph.getNumNodes(), false); // visited vector
    std::vector<int> parent(graph.getNumNodes(), -1); // stores parents

    // satrt the bfs

    q.push(startNode);
    visited[startNode] = true;

    int foundNode = -1; // this will store the responder we find fisrt

    while (!q.empty()) {
        int current = q.front(); 
        q.pop();

        // check if node is responder or not
        if (isResponder[current]){
            foundNode = current;
            break;
        }

        // explore all adjacent nodes
        for (const auto& edge : graph.getAdj(current)){
            int neighbor = edge.first;
            if (!visited[neighbor]){
                visited[neighbor] = true;
                parent[neighbor] = current; // tracking parent so we can reconstruct the path later
                q.push(neighbor);
            }
        }

    }

    // if responder was found then reconstruct the path from the start to responder
    if (foundNode != -1){
        // count the steps from start
        outDistance = 0;
        int temp = foundNode;
        while (temp != startNode){
            temp = parent[temp];
            outDistance++;
        }

        // reconstruct path by walking reverse from responder to start via parent pointers
        std::vector<int> path;
        for (int node = foundNode; node != -1; node = parent[node]){
            path.push_back(node);
        }

        // reverse path to start from start
        std::reverse(path.begin(), path.end());
        return path;
    }

    // if no responder found ie graph could be disconnected
    outDistance = -1;
    return {};
}