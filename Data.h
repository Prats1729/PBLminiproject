#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <tuple>

using namespace std;

// This struct represents the actual disaster site
struct DisasterSite{
    int nodeId;  // This is the graphs node id, its the position in the map
    string name; // name of the location eg- hospital/school
    int severity; // on a scale of 1-10 how critical is the situation
    int population; // number of people affected there
};

// This structure represents a rescue team
struct Resource
{
    string name;  // Type of resource medkits, water tanker etc
    int weight;  // space/effort required to carry
    int value;  // Effectiveness score 1-100
};

// This struct is about the Responders
struct Responder
{
    int nodeId;  // Where the team is situated at
    string name;  // name of team, eg - ambulance, resquesquad
    string type;  // type of assistance, fire, mmedical, rescue
};

// All the info that needs to be loaded for a disater
struct InputData
{
    int numNodes; // Number of locations in the map
    vector<string> nodeNames; // Name of each node eg- hosptial/park
    vector<tuple<int,int,int>> edges; // These are the Road connections (u, v, weight)
                                      // u and v are node indices, weight = travel time in minutes
    vector<DisasterSite> sites; // List of all the disaster sites
    vector<Responder> reponders; // List of rescue teams ready to help
    vector<Resource> resources; // all the items that can be shipped in relief vehicles
    int knapsackCapacity;  // Max weight a vehicle is allowed to carry
};

InputData loadHardcodedData(); // fix sample data for now anyway


#endif