#include "Graph.h"
#include <iostream>

Graph::Graph(int numNodes)
    : numNodes_(numNodes), adj_(numNodes) {}

void Graph::addEdge(int u, int v, int w) {
    adj_[u].push_back({v, w});
    adj_[v].push_back({u, w});
}

void Graph::printGraph() const {
    std::cout << "Graph map (node names to ID):\n";
    for (int i = 0; i < numNodes_; ++i) {
        std::cout << "Node " << nodeNames_[i] << "(ID: " << i << "): ";
        for (const auto& [dest, weight] : adj_[i]) {
            std::cout << "-> Node " << nodeNames_[dest]
                      << " (Weight: " << weight << " mins)";
        }
        std::cout << "\n";
    }
    std::cout << "----------------------------------------------\n";
}

void Graph::visualizeRoute(const std::vector<int>& path) const {
    if (path.empty()) {
        std::cout << "No route found!\n";
        return;
    }

    std::cout << "RESCUE ROUTE:\n";
    for (size_t i = 0; i < path.size(); ++i) {
        int node = path[i];
        std::cout << "Step " << (i + 1) << ": " << nodeNames_[node] << "\n";

        if (i > 0) {
            int prev = path[i - 1];
            int travelTime = -1;
            for (const auto& [dest, weight] : adj_[prev]) {
                if (dest == node) {
                    travelTime = weight;
                    break;
                }
            }
            if (travelTime != -1) {
                std::cout << " (from " << nodeNames_[prev]
                          << " - Travel: " << travelTime << " mins)";
            } else {
                std::cout << " (Error: No direct connection!)";
            }
        }
    }
    std::cout << "--------------------------------------------------------\n";
}
