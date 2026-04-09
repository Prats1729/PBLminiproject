#include <iostream>
#include <limits>
#include <vector>

#include "Data.h"
#include "Graph.h"
#include "bfs.h"
#include "dijkstra.h"
#include "knapsack.h"
#include "greedy.h"

using namespace std;

int main() {
    // ── Load scenario ──────────────────────────────────────────
    cout << "---Loading Disaster Scenario---\n";
    InputData disasterData = loadHardcodedData();

    // ── Build road network ─────────────────────────────────────
    cout << "---Creating Graph---\n";
    Graph disasterGraph(disasterData.numNodes);
    disasterGraph.setNodeNames(disasterData.nodeNames);

    cout << "---Building Road Network---\n";
    for (const auto& [u, v, weight] : disasterData.edges) {
        disasterGraph.addEdge(u, v, weight);
        cout << "Added road: " << disasterData.nodeNames[u] << " <--> "
             << disasterData.nodeNames[v] << " ( " << weight
             << " mins travel )\n";
    }

    // ── Display graph ──────────────────────────────────────────
    cout << "---Displaying Graph Map---\n";
    disasterGraph.printGraph();

    // ── BFS: all reachable responders ──────────────────────────
    vector<int> responderNodeIds;
    for (const auto& r : disasterData.responders) {
        responderNodeIds.push_back(r.nodeId);
    }

    // ── Greedy Triage Prioritization ───────────────────────────
    cout << "\n---Greedy Optimization: Triage Priorities---\n";
    vector<TriageResult> triageResults = GreedySolver::computeTriage(
        disasterData.sites, disasterGraph, disasterData.responders);

    for (size_t i = 0; i < triageResults.size(); ++i) {
        cout << i + 1 << ". " << triageResults[i].site.name 
             << " [Severity: " << triageResults[i].site.severity 
             << ", Pop: " << triageResults[i].site.population 
             << "] -> Nearest Rsp: " << triageResults[i].nearestResponderTime 
             << "m  |  Score: " << triageResults[i].triageScore << "\n";
    }

    // Use the highest-priority disaster site as the origin
    int disasterOrigin = triageResults.empty() ? 0 : triageResults[0].site.nodeId;

    vector<int> allReachable = bfsAllResponders(
        disasterGraph, disasterOrigin, responderNodeIds);

    cout << "\n---BFS: All Reachable Responders---\n";
    if (!allReachable.empty()) {
        cout << "Responders reachable from " << disasterData.nodeNames[disasterOrigin] << ":\n";
        for (int id : allReachable) {
            cout << "  - Responder at " << disasterData.nodeNames[id] << "\n";
        }
    } else {
        cout << "No responders reachable from the start node.\n";
    }

    // ── Dijkstra: nearest responder by travel time ─────────────
    Dijkstra dijkstra(disasterGraph);
    dijkstra.computeShortestPaths(disasterOrigin);

    int closestResponder = -1;
    int minDistance = numeric_limits<int>::max();
    for (int rid : responderNodeIds) {
        int dist = dijkstra.getDistance(rid);
        if (dist < minDistance) {
            minDistance       = dist;
            closestResponder  = rid;
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

    // ── Knapsack: optimal resource allocation ──────────────────
    if (closestResponder != -1) {
        KnapsackSolver solver(disasterData);
        vector<int> optimalResources = solver.allocateResources(closestResponder);

        cout << "\n---Optimal Resource Allocation---\n";
        cout << "Responder ID " << closestResponder << " can carry:\n";
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