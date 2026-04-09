# Smart Disaster Response & Control Optimization

A robust, modular C++ application designed to optimize resource allocation and route planning during city-wide disaster scenarios. This project uses graph algorithms and dynamic programming to ensure efficient response to multiple critical incidents.

## Features

- **Realistic Map Simulation**: Models an 8-node city with 12 interconnected roads, travel times, multiple disaster sites (flooding, fires), and responder bases.
- **BFS (Breadth-First Search)**: Rapidly discovers all emergency responders reachable from a given disaster origin. 
- **Dijkstra's Algorithm**: Calculates the absolute fastest rescue route (minimal travel time) from a disaster site to the nearest responder.
- **0/1 Knapsack Optimization**: Intelligently packs an emergency vehicle with the optimal combination of relief supplies based on a strict weight capacity vs. item value trade-off.
- **Greedy Triage Prioritization**: Disasters are ranked based on a heuristic score evaluating severity, population density, and $O((V+E) \log V)$ minimum travel time to the nearest emergency responder.
- **Interactive Control Center Visualization**: Built with Qt5, featuring a dynamic dark-themed dashboard, color-coded node interactions, and animated algorithmic workflows.

## Algorithms Breakdown

- **Graph Structure**: Built using a modern C++17 `std::vector<std::vector<std::pair<int, int>>>` pattern for highly efficient adjacency lists.
- **Dijkstra Time Complexity**: $O((V+E) \log V)$ using a `std::priority_queue`.
- **BFS Time Complexity**: $O(V+E)$ using a standard `std::queue`.
- **Greedy Algorithm**: Heuristic evaluation $O(|S| \times (V+E) \log V)$ for optimal triage scheduling across $|S|$ sites.

## Building and Running

### Prerequisites
- `g++` (Compiler with C++17 support)
- `pkg-config`
- Qt5 Widgets library

On Debian/Ubuntu:
```bash
sudo apt-get install build-essential qtbase5-dev qt5-qmake pkg-config
```

### Building the Console Binary
To build the lightweight, text-only console application:
```bash
g++ -std=c++17 -o disaster main.cpp Data.cpp Graph.cpp bfs.cpp dijkstra.cpp knapsack.cpp greedy.cpp
./disaster
```

### Building the Interactive Qt5 UI
The rich user interface uses the Qt Framework:
```bash
# 1. Run the Meta-Object Compiler
moc $(pkg-config --cflags Qt5Widgets) ui_main.cpp -o ui_main.moc

# 2. Compile and link the GUI app
g++ -std=c++17 -fPIC ui_main.cpp Data.cpp Graph.cpp bfs.cpp dijkstra.cpp knapsack.cpp greedy.cpp $(pkg-config --cflags --libs Qt5Widgets) -o disaster_ui

# 3. Launch
./disaster_ui
```

## Project Structure
- `Data.h / Data.cpp` - Shared state and hardcoded city graph data.
- `Graph.h / Graph.cpp` - Core data structure logic and graph-building helpers.
- `bfs.h / bfs.cpp` - Reachability algorithms.
- `dijkstra.h / dijkstra.cpp` - Minimal travel-time computations.
- `greedy.h / greedy.cpp` - Triage prioritization and heuristic decision making.
- `knapsack.h / knapsack.cpp` - Relief vehicle resource optimization.
- `main.cpp` - Console-based execution flow.
- `ui_main.cpp` - The full Qt5 interactive dashboard.
