#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <utility>
#include <iostream>

using Edge = std::pair<int, int>; // Edge = (destination node index, weight)

// This is the graph class, it holds all graph related data and operations
class Graph {
    private:
        int numNodes;  // total nodes in graph
        std::vector<Edge>* adj; // Pointer to an array of adjacency lists
        std::vector<std::string> nodeNames; // name of each node of graph

    public:
        // COnstructor for the graph
        explicit Graph(int n);

        // Destructor to free the adj vector
        ~Graph() {
            delete[] adj;
        }

        // Add a bidirectional edge between u and v
        void addEdge(int u, int v, int w);

        // For visualisation of the graph
        void printGraph();

        // For visualisation of routes
        void visualizeRoute(const std::vector<int>& path);
};

#endif