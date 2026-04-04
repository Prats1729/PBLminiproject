#include <iostream>
#include "Data.h"
#include "Graph.h"

int main(){
    std::cout <<"---Loading Disaster Scenario---\n";
    InputData disasterData = loadHardcodedData();

    std::cout << "---Creating Graph---\n";
    Graph disasterGraph(disasterData.numNodes);
    disasterGraph.setNodeNames(disasterData.nodeNames);

    std::cout << "---Building Road Network---\n";
    for (const auto& edge : disasterData.edges){
        //Extracting tuple components u,v w
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        int weight = std::get<2>(edge);

        // adding the biderectional road b/w u and v witht the travel time
        disasterGraph.addEdge(u, v, weight);
        std::cout << "Added road: " << disasterData.nodeNames[u] << " <--> " <<        disasterData.nodeNames[v] << "( " << weight << " mins travell)\n";

    }

    std::cout << "---Displaying Graph Map---\n";
    disasterGraph.printGraph();

    // Just for testing im not using any actuall algo
    std::vector<int> samplePath = {0, 1, 2};

    std::cout << "\n---Sample Route Display---\n";
    disasterGraph.visualizeRoute(samplePath);

    // data summary
    std::cout << "---Data Summary---\n";
    std::cout << "Nodes: " << disasterData.numNodes << "\n";
    std::cout << "disasterData.sites size: " << disasterData.sites.size() << endl;
    std::cout << "responders size: "<< disasterData.responders.size() << endl;

    std::cout << "\n---System is kinda ready  ig---\n";
    std::cout << "Exitinf";

    return 0;
}