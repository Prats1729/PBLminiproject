#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <utility>
#include <vector>

// Edge: (destination node index, travel-time weight)
using Edge = std::pair<int, int>;

// Weighted undirected graph representing the city road network.
class Graph {
public:
    explicit Graph(int numNodes);

    // Add a bidirectional road between nodes u and v with weight w.
    void addEdge(int u, int v, int w);

    // Print the full adjacency list to stdout.
    void printGraph() const;

    // Print a step-by-step rescue route for the given node path.
    void visualizeRoute(const std::vector<int>& path) const;

    // --- Accessors ---
    int getNumNodes() const { return numNodes_; }

    const std::vector<Edge>& getAdj(int node) const { return adj_[node]; }

    void setNodeNames(const std::vector<std::string>& names) { nodeNames_ = names; }

    const std::string& getNodeName(int id) const { return nodeNames_[id]; }

private:
    int numNodes_;
    std::vector<std::vector<Edge>> adj_;       // adjacency lists
    std::vector<std::string>       nodeNames_; // label per node
};

#endif // GRAPH_H