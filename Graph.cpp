#include "Graph.h"
#include <iostream>
#include <sstream>

Graph::Graph(int n)
{
    numNodes = n;
    adj = new std::vector<Edge>[n]; // this vector holds n adjacency lists ie adjacency list for each node of the graph
}

void Graph::addEdge(int u, int v, int w)
{
        // since this is an undirected graph, and roads have 2 lanes so im storing in both directions
        adj[u].push_back({v, w}); // edge from u -> v
        adj[v].push_back({u, w}); // edge from v -> u
    
}

void Graph::printGraph(){
    std::cout << "Graph map (node names to ID): \n";
    for (int i = 0; i < numNodes; ++i){
        std::cout << "Node " << nodeNames[i] << "(ID: "<<i<< "): "; // this shows the name and the node ID
        for (const auto& edge : adj[i]) { // iterates through each edge from the curr node
            // edge.first = destination node index
            // edge.second = weight (travel time)
            std::cout << "-> Node " << nodeNames[edge.first] << " (Weight: " << edge.second << " mins)";
        }
        std::cout << "\n";
    }
    std::cout << "----------------------------------------------\n";
}

void Graph::visualizeRoute(const std::vector<int>& path) {
    if (path.empty()){
        std::cout << "No route found!\n";
        return;
    }
    std::cout << "RESCUE ROUTE:\n";
    for (size_t i = 0; i < path.size(); ++i){ // loop through each node
        int currentNode = path[i];
        std::string nodeName = nodeNames[currentNode]; // name of current node
        std::cout << "Step "<< (i+1) << ": "<< nodeName << "\n"; // for sequential steps
        if (i > 0){
            int prevNode = path[i-1];
            // Find the weight between prevNode and currentNode
            int weight = -1;
            for (const auto& edge : adj[prevNode]){ // searching for edge weight b/w prev and curr Node
                if (edge.first == currentNode){// check prev to curr edge present or not
                    weight = edge.second; // if found then store
                    break;
                }
            }
            // if edge was found then travel time 
            if (weight != -1) {
                std::cout << " (from " << nodeNames[prevNode] << " - Travel: "<< weight << " mins)";
            }
            else{
                std::cout << " (Error: No direct COnnection!)";
            }
        }
    }
    std::cout << "--------------------------------------------------------\n";

}
