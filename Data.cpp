#include "Data.h"

InputData loadHardcodedData() {
    InputData data;

    // ── 8-node city map inspired by a mid-sized Indian city ──
    //
    //  0 = City Hospital        (north)
    //  1 = Fire Station         (north-east)
    //  2 = Central Market       (center)
    //  3 = Railway Station      (east)
    //  4 = Riverside Colony     (south-east, flood-prone)
    //  5 = Government School    (south)
    //  6 = Industrial Area      (west)
    //  7 = IT Park              (north-west)

    data.numNodes  = 20;
    data.nodeNames = {
        "City Hospital",       // 0 (Responder)
        "North West Suburb",   // 1
        "Northern Ridge",      // 2
        "IT Park",             // 3
        "Airport Base",        // 4 (Responder)
        "West Bridge",         // 5
        "University Campus",   // 6
        "Downtown",            // 7 (Disaster 1)
        "Central Plaza",       // 8
        "East Tech Hub",       // 9
        "Old Town",            // 10
        "Lower West Side",     // 11
        "Central Market",      // 12 (Disaster 2)
        "Logistics Center",    // 13
        "Port District",       // 14
        "South Highway",       // 15 (Responder)
        "Riverside Fringe",    // 16
        "Riverside Colony",    // 17 (Disaster 3)
        "Industrial Park",     // 18
        "East Train Station"   // 19 (Responder)
    };

    // Roads: {source, destination, travel-time in minutes}
    data.edges = {
        // Horizontals
        {0,1,10}, {1,2,12}, {2,3,11}, {3,4,18},
        {5,6,9},  {6,7,10}, {7,8,12}, {8,9,11},
        {10,11,11},{11,12,9},{12,13,10},{13,14,12},
        {15,16,12},{16,17,11},{17,18,10},{18,19,11},
        // Verticals
        {0,5,10}, {5,10,11}, {10,15,12},
        {1,6,10}, {6,11,11}, {11,16,8},
        {2,7,11}, {7,12,7},  {12,17,12},
        {3,8,10}, {8,13,11}, {13,18,10},
        {4,9,12}, {9,14,10}, {14,19,10},
        // Cross Diagonals (Downtown & Market hubs)
        {6,12,10},{7,11,10},{7,13,12},{8,12,11},
        {2,6,14}, {1,7,13}, {8,3,14}, {11,17,14},
        {13,17,11}
    };

    // Exactly 3 Disaster sites, placed in the center/vulnerable spots
    data.sites = {
        {7,  "Downtown — Skyscraper Fire",       10, 3500},
        {17, "Riverside Colony — Flash Flood",    9,  1800},
        {12, "Central Market — Structural Ocr",   8,  2200},
    };

    // Responders placed far away on the perimeter
    data.responders = {
        {0,  "Fire Engine Alpha",  "Fire"},
        {4,  "Rescue Unit Delta",  "Rescue"},
        {15, "Ambulance Bravo",    "Medical"},
        {19, "Hazmat Echo",        "Chemical"},
    };

    // Relief resources available at the dispatch center
    data.resources = {
        {"First-Aid Kits",      4,  35},
        {"Water Tanker",       25,  80},
        {"Blanket Bundles",     8,  25},
        {"Food Ration Packs",  12,  50},
        {"Tarpaulin Sheets",    6,  40},
        {"Portable Generator", 30,  70},
        {"Inflatable Boats",   20,  90},
        {"Oxygen Cylinders",   10,  55},
        {"Fire Extinguishers", 15,  45},
        {"Comm Radio Sets",     3,  60},
    };

    data.knapsackCapacity = 80;
    return data;
}