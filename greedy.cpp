#include "greedy.h"
#include "dijkstra.h"
#include <algorithm>
#include <limits>

std::vector<TriageResult> GreedySolver::computeTriage(
    const std::vector<DisasterSite>& sites, 
    const Graph& graph, 
    const std::vector<Responder>& responders) {
    
    std::vector<TriageResult> results;

    // Cache responder node IDs for fast lookup
    std::vector<int> responderIds;
    for (const auto& r : responders) {
        responderIds.push_back(r.nodeId);
    }

    // For each site, compute time to nearest responder
    for (const auto& site : sites) {
        Dijkstra dijk(graph);
        dijk.computeShortestPaths(site.nodeId);

        int minTime = std::numeric_limits<int>::max();
        for (int rid : responderIds) {
            int dist = dijk.getDistance(rid);
            if (dist < minTime) {
                minTime = dist;
            }
        }

        // Fallback for unreachable components to not crash the division
        if (minTime == std::numeric_limits<int>::max()) {
            minTime = 9999;
        }

        // Triage Heuristic: (Severity * Population) / (Time + 1)
        double score = (static_cast<double>(site.severity) * site.population) / 
                       (static_cast<double>(minTime) + 1.0);

        results.push_back({site, minTime, score});
    }

    // Sort descending by highest score
    std::sort(results.begin(), results.end(), 
        [](const TriageResult& a, const TriageResult& b) {
            return a.triageScore > b.triageScore;
        });

    return results;
}
