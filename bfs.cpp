#include "bfs.h"
#include <queue>

std::vector<int> bfsAllResponders(
    const Graph& graph,
    int startNode,
    const std::vector<int>& responderNodes
) {
    if (responderNodes.empty() || startNode < 0 || startNode >= graph.getNumNodes()) {
        return {};
    }

    // Build a fast-lookup table for responder node IDs.
    std::vector<bool> isResponder(graph.getNumNodes(), false);
    for (int r : responderNodes) {
        if (r >= 0 && r < graph.getNumNodes()) {
            isResponder[r] = true;
        }
    }

    // Standard BFS traversal.
    std::queue<int> frontier;
    std::vector<bool> visited(graph.getNumNodes(), false);

    frontier.push(startNode);
    visited[startNode] = true;

    std::vector<int> found;
    while (!frontier.empty()) {
        int current = frontier.front();
        frontier.pop();

        if (isResponder[current]) {
            found.push_back(current);
        }

        for (const auto& [neighbor, weight] : graph.getAdj(current)) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                frontier.push(neighbor);
            }
        }
    }

    return found;
}