#ifndef DATA_H
#define DATA_H

#include <string>
#include <tuple>
#include <vector>

// Represents a disaster-affected location on the city graph.
struct DisasterSite {
    int nodeId;           // Graph node index (position on the map)
    std::string name;     // Human-readable label, e.g. "Hospital"
    int severity;         // Criticality on a 1–10 scale
    int population;       // Number of people affected
};

// A single relief item that can be loaded onto a rescue vehicle.
struct Resource {
    std::string name;     // Type of resource, e.g. "Medkits"
    int weight;           // Space / effort required to carry
    int value;            // Effectiveness score (1–100)
};

// A rescue team stationed at a graph node.
struct Responder {
    int nodeId;           // Where the team is stationed
    std::string name;     // Team label, e.g. "Ambulance 1"
    std::string type;     // Assistance category: "Medical", "Fire", "Rescue"
};

// Complete scenario data loaded at startup.
struct InputData {
    int numNodes;                                   // Locations in the map
    std::vector<std::string> nodeNames;             // Label per node
    std::vector<std::tuple<int, int, int>> edges;   // Roads: (u, v, travel-time)
    std::vector<DisasterSite> sites;                // Disaster sites
    std::vector<Responder> responders;              // Rescue teams
    std::vector<Resource> resources;                // Relief items
    int knapsackCapacity;                           // Max vehicle payload
};

// Load a fixed sample scenario for testing.
InputData loadHardcodedData();

#endif // DATA_H