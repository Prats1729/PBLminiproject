#include "Data.h"
#include <vector>
#include <tuple>

InputData loadHardcodedData(){
    InputData data;

    // dummy data for testing
    data.numNodes = 5;
    data.nodeNames = {"Hospital", "School", "Bridge", "Park", "Factory"};

    // sample edges
    data.edges = {
        {0, 1, 10}, // hospital->school (10 min road)
        {1, 2, 15}, // School- Bridge (15 min)
        {2, 3, 20} // Bridge-Park (20 min)
    };

    // dummy sites
    data.sites = {
        {0, "Hospital Site", 10, 300},
        {1, "School Site", 8, 200}
    };

    // Dummy respnders

    data.responders = {
        {0, "Ambulance 1", "Medical"},
        {1, "Rescure Team A", "Rescue"}
    };

    data.resources = {
        {"Medkits", 5, 30},
        {"Water Tanks", 3, 60}
    };

    data.knapsackCapacity = 100;
    return data;
}