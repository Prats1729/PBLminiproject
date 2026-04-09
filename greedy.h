#ifndef GREEDY_H
#define GREEDY_H

#include "Data.h"
#include "Graph.h"
#include <vector>

// Struct representing the evaluation of a single disaster site
struct TriageResult {
    DisasterSite site;
    int nearestResponderTime;
    double triageScore;
};

// GreedySolver coordinates triage evaluations across all scenarios.
class GreedySolver {
public:
    // Computes triage score for all given sites based on proximity to nearest responders.
    // Returns the list of sites sorted by highest triage score descending.
    static std::vector<TriageResult> computeTriage(
        const std::vector<DisasterSite>& sites, 
        const Graph& graph, 
        const std::vector<Responder>& responders);
};

#endif // GREEDY_H
