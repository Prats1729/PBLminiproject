#include <iostream>
#include "Data.h"
#include "Graph.h"
#include "bfs.h"
#include "dijkstra.h"
#include "knapsack.h"

using namespace std;

int main() {
    cout << "---Loading Disaster Scenario---\n";
    InputData disasterData = loadHardcodedData();

    cout << "---Creating Graph---\n";
    Graph disasterGraph(disasterData.numNodes);
    disasterGraph.setNodeNames(disasterData.nodeNames);

    cout << "---Building Road Network---\n";
    for (const auto& edge : disasterData.edges) {
        int u = get<0>(edge);
        int v = get<1>(edge);
        int weight = get<2>(edge);

        disasterGraph.addEdge(u, v, weight);
        cout << "Added road: " << disasterData.nodeNames[u] << " <--> "
             << disasterData.nodeNames[v] << " ( " << weight
             << " mins travel )\n";
    }

    cout << "---Displaying Graph Map---\n";
    disasterGraph.printGraph();

    // ---------------------------------------------------------
    //  BFS: Print **all** responders reachable from start node
    // ---------------------------------------------------------
    vector<int> responderNodeIds = {0, 1};  // known responder IDs
    vector<int> allReachable = bfsAllResponders(
        disasterGraph,        // graph
        0,                    // start node (disaster origin)
        responderNodeIds);    // list of possible responders

    cout << "\n---BFS: All Reachable Responders---\n";
    if (!allReachable.empty()) {
        cout << "Responders reachable from node 0:\n";
        for (int id : allReachable) {
            cout << "  - Responder ID " << id
                 << " (" << disasterData.nodeNames[id] << ")\n";
        }
    } else {
        cout << "No responders reachable from the start node.\n";
    }

    // ---------------------------------------------------------
    // Dijkstra: Find the *fastest* responder (minimum travel time)
    // ---------------------------------------------------------
    Dijkstra dijkstra(disasterGraph);
    dijkstra.computeShortestPaths(0);   // pre‑compute distances from start node

    int closestResponder = -1;
    int minDistance = numeric_limits<int>::max();
    for (int responderId : responderNodeIds) {
        int dist = dijkstra.getDistance(responderId);
        if (dist < minDistance) {
            minDistance = dist;
            closestResponder = responderId;
        }
    }

    cout << "\n---Dijkstra: Nearest Responder (by time)---\n";
    if (closestResponder != -1) {
        cout << "Nearest responder found in " << minDistance
             << " minutes (ID " << closestResponder << ")\n";
        vector<int> path = dijkstra.getPath(closestResponder);
        disasterGraph.visualizeRoute(path);
    } else {
        cout << "No responder reachable.\n";
    }

    // ---------------------------------------------------------
    //  Knapsack: Optimal resource allocation for rescue truck
    // ---------------------------------------------------------
    if (closestResponder != -1) {
        // Create knapsack solver for this responder
        KnapsackSolver solver(disasterData);
        vector<int> optimalResources = solver.allocateResources(closestResponder);

        cout << "\n---Optimal Resource Allocation---\n";
        cout << "Responder ID " << closestResponder
             << " can carry:\n";
        for (int resourceId : optimalResources) {
            const Resource& res = disasterData.resources[resourceId];
            cout << "  - " << res.name
                 << " (weight=" << res.weight
                 << ", value=" << res.value << ")\n";
        }
    }

    cout << "\n---System is ready---\n";
    return 0;
}