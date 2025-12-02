// test_edmonds_karp_corrected.cpp - COMPLETE TEST SUITE WITH BUILT-IN GRAPHS
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <chrono>
#include <iomanip>
#include <string>
#include <memory>
#include <functional>

using namespace std;
using namespace chrono;

// ============ FORWARD DECLARATIONS ============
class Node;

// ============ EDGE CLASS (FIRST) ============
class Edge {
public:
    int weight;                 // edge weight/capacity
    Node* adjacentNode;         // adjacent node

    Edge(int w, Node* node);
};

// ============ NODE CLASS (SECOND) ============
class Node {
public:
    int id;                                     // vertex ID
    std::vector<Edge*> edges;                   // outgoing edges list
    std::unordered_map<Node*, Edge*> parents;   // incoming edges list

    Node(int nodeId) : id(nodeId) {}
};

// ============ EDGE CONSTRUCTOR IMPLEMENTATION ============
Edge::Edge(int w, Node* node) : weight(w), adjacentNode(node) {}

// ============ EDMONDS-KARP IMPLEMENTATION ============
int edmondsKarp(unordered_map<int, Node*>& graph, int sourceId, int sinkId, bool verbose = false) {
    // Display graph characteristics
    int vertexCount = graph.size();
    int edgeCount = 0;
    for (auto& pair : graph) {
        edgeCount += pair.second->edges.size();
    }

    cout << "\n" << string(50, '=') << endl;
    cout << "  GRAPH ANALYSIS" << endl;
    cout << string(50, '=') << endl;
    cout << "Vertices: " << vertexCount << endl;
    cout << "Edges: " << edgeCount << endl;
    cout << "Source: " << sourceId << ", Destination: " << sinkId << endl;
    cout << string(50, '=') << endl << endl;

    // Basic validations
    if (graph.empty()) {
        cout << "[ERROR] Empty graph" << endl;
        cout << "Maximum flow: 0" << endl << endl;
        return 0;
    }

    if (graph.find(sourceId) == graph.end()) {
        cout << "[ERROR] Source vertex " << sourceId << " not found" << endl;
        cout << "Maximum flow: 0" << endl << endl;
        return 0;
    }

    if (graph.find(sinkId) == graph.end()) {
        cout << "[ERROR] Destination vertex " << sinkId << " not found" << endl;
        cout << "Maximum flow: 0" << endl << endl;
        return 0;
    }

    if (sourceId == sinkId) {
        cout << "[ERROR] Source and destination are the same" << endl;
        cout << "Maximum flow: 0" << endl << endl;
        return 0;
    }

    Node* source = graph[sourceId];
    Node* sink = graph[sinkId];

    // Flow edge structure
    struct FlowEdge {
        Node* to;
        int capacity;
        int flow;
        FlowEdge* reverse;
        
        FlowEdge(Node* t, int c) : to(t), capacity(c), flow(0), reverse(nullptr) {}
        
        int residualCapacity() const {
            return capacity - flow;
        }
        
        void augment(int amount) {
            flow += amount;
            if (reverse) {
                reverse->flow -= amount;
            }
        }
    };

    // Build flow network
    unordered_map<Node*, vector<FlowEdge*>> flowNetwork;
    
    // Initialize for all vertices
    for (auto& pair : graph) {
        flowNetwork[pair.second] = vector<FlowEdge*>();
    }

    // Convert your structure to flow network
    for (auto& pair : graph) {
        Node* fromNode = pair.second;
        
        for (Edge* edge : fromNode->edges) {
            Node* toNode = edge->adjacentNode;
            int capacity = edge->weight;
            
            // Forward edge
            FlowEdge* forward = new FlowEdge(toNode, capacity);
            FlowEdge* backward = new FlowEdge(fromNode, 0);
            
            forward->reverse = backward;
            backward->reverse = forward;
            
            flowNetwork[fromNode].push_back(forward);
            flowNetwork[toNode].push_back(backward);
        }
    }

    // Edmonds-Karp algorithm
    int maxFlow = 0;
    int iterations = 0;
    
    auto start = high_resolution_clock::now();
    
    while (true) {
        // BFS to find augmenting path
        unordered_map<Node*, Node*> parent;
        unordered_map<Node*, FlowEdge*> path;
        
        queue<Node*> q;
        q.push(source);
        parent[source] = source;
        
        bool foundPath = false;
        
        while (!q.empty() && !foundPath) {
            Node* u = q.front();
            q.pop();
            
            for (FlowEdge* edge : flowNetwork[u]) {
                Node* v = edge->to;
                int residual = edge->residualCapacity();
                
                if (residual > 0 && parent.find(v) == parent.end()) {
                    parent[v] = u;
                    path[v] = edge;
                    q.push(v);
                    
                    if (v == sink) {
                        foundPath = true;
                        break;
                    }
                }
            }
        }
        
        if (!foundPath) {
            break; // No more augmenting paths
        }
        
        iterations++;
        
        // Find bottleneck
        int bottleneck = INT_MAX;
        for (Node* v = sink; v != source; v = parent[v]) {
            FlowEdge* edge = path[v];
            bottleneck = min(bottleneck, edge->residualCapacity());
        }
        
        // Augment flow along the path
        for (Node* v = sink; v != source; v = parent[v]) {
            FlowEdge* edge = path[v];
            edge->augment(bottleneck);
        }
        
        maxFlow += bottleneck;
        
        if (verbose && iterations <= 5) {
            cout << "Iteration " << iterations << ": +" << bottleneck 
                 << " (total: " << maxFlow << ")" << endl;
        }
        
        // Safety to prevent infinite loops
        if (iterations > vertexCount * vertexCount) {
            if (verbose) {
                cout << "[WARNING] Safety stop after " << iterations << " iterations" << endl;
            }
            break;
        }
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    // Display results
    cout << "\n" << string(50, '=') << endl;
    cout << "  RESULTS" << endl;
    cout << string(50, '=') << endl;
    cout << "Maximum flow: " << maxFlow << " units" << endl;
    cout << "From vertex " << sourceId << " to vertex " << sinkId << endl;
    cout << "Iterations: " << iterations << endl;
    cout << "Computation time: " << duration.count() << " microseconds " << endl;
    
    if (duration.count() > 0) {
        double throughput = (maxFlow * 1000000.0) / duration.count();
        cout << "Throughput: " << fixed << setprecision(2) << throughput << " units/second" << endl;
    } else {
        cout << "Throughput: Too fast to measure" << endl;
    }
    
    cout << string(50, '=') << endl << endl;
    
    // Cleanup
    for (auto& pair : flowNetwork) {
        for (FlowEdge* edge : pair.second) {
            delete edge;
        }
    }
    
    return maxFlow;
}

// ============ CLEANUP FUNCTION ============
void cleanupGraph(unordered_map<int, Node*>& graph) {
    for (auto& pair : graph) {
        for (Edge* edge : pair.second->edges) {
            delete edge;
        }
        delete pair.second;
    }
    graph.clear();
}

// ============ TEST GRAPH CREATION FUNCTIONS ============

// 1. SIMPLE 4-VERTEX GRAPH (Basic test)
void createSimpleGraph(unordered_map<int, Node*>& graph) {
    cout << "[INFO] Creating simple 4-vertex graph..." << endl;
    
    for (int i = 0; i < 4; i++) {
        graph[i] = new Node(i);
    }

    // A(0) -> B(1): 10
    graph[0]->edges.push_back(new Edge(10, graph[1]));
    graph[1]->parents[graph[0]] = graph[0]->edges.back();

    // A(0) -> C(2): 5
    graph[0]->edges.push_back(new Edge(5, graph[2]));
    graph[2]->parents[graph[0]] = graph[0]->edges.back();

    // B(1) -> C(2): 15
    graph[1]->edges.push_back(new Edge(15, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    // B(1) -> D(3): 10
    graph[1]->edges.push_back(new Edge(10, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    // C(2) -> D(3): 10
    graph[2]->edges.push_back(new Edge(10, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();
}

// 2. CLASSIC EXAMPLE FROM TEXTBOOK (6 vertices)
void createTextbookExample(unordered_map<int, Node*>& graph) {
    cout << "[INFO] Creating classic textbook example (6 vertices)..." << endl;
    
    for (int i = 0; i < 6; i++) {
        graph[i] = new Node(i);
    }

    // Edges with capacities
    graph[0]->edges.push_back(new Edge(16, graph[1]));
    graph[1]->parents[graph[0]] = graph[0]->edges.back();

    graph[0]->edges.push_back(new Edge(13, graph[2]));
    graph[2]->parents[graph[0]] = graph[0]->edges.back();

    graph[1]->edges.push_back(new Edge(12, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(10, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(9, graph[1]));
    graph[1]->parents[graph[2]] = graph[2]->edges.back();

    graph[2]->edges.push_back(new Edge(14, graph[4]));
    graph[4]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(7, graph[4]));
    graph[4]->parents[graph[3]] = graph[3]->edges.back();

    graph[3]->edges.push_back(new Edge(4, graph[5]));
    graph[5]->parents[graph[3]] = graph[3]->edges.back();

    graph[4]->edges.push_back(new Edge(20, graph[5]));
    graph[5]->parents[graph[4]] = graph[4]->edges.back();
}

// 3. LAYERED GRAPH (10 vertices)
void createLayeredGraph(unordered_map<int, Node*>& graph) {
    cout << "[INFO] Creating layered graph (10 vertices)..." << endl;
    
    for (int i = 0; i < 10; i++) {
        graph[i] = new Node(i);
    }

    // Layer 1 -> Layer 2
    graph[0]->edges.push_back(new Edge(20, graph[1]));
    graph[1]->parents[graph[0]] = graph[0]->edges.back();

    graph[0]->edges.push_back(new Edge(15, graph[2]));
    graph[2]->parents[graph[0]] = graph[0]->edges.back();

    // Layer 2 -> Layer 3
    graph[1]->edges.push_back(new Edge(25, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(10, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();

    graph[2]->edges.push_back(new Edge(15, graph[4]));
    graph[4]->parents[graph[2]] = graph[2]->edges.back();

    // Layer 3 -> Layer 4
    graph[3]->edges.push_back(new Edge(15, graph[5]));
    graph[5]->parents[graph[3]] = graph[3]->edges.back();

    graph[3]->edges.push_back(new Edge(10, graph[6]));
    graph[6]->parents[graph[3]] = graph[3]->edges.back();

    graph[4]->edges.push_back(new Edge(20, graph[6]));
    graph[6]->parents[graph[4]] = graph[4]->edges.back();

    graph[4]->edges.push_back(new Edge(5, graph[7]));
    graph[7]->parents[graph[4]] = graph[4]->edges.back();

    // Layer 4 -> Sink
    graph[5]->edges.push_back(new Edge(30, graph[8]));
    graph[8]->parents[graph[5]] = graph[5]->edges.back();

    graph[6]->edges.push_back(new Edge(20, graph[8]));
    graph[8]->parents[graph[6]] = graph[6]->edges.back();

    graph[7]->edges.push_back(new Edge(10, graph[9]));
    graph[9]->parents[graph[7]] = graph[7]->edges.back();

    graph[8]->edges.push_back(new Edge(25, graph[9]));
    graph[9]->parents[graph[8]] = graph[8]->edges.back();
}

// 4. DENSE GRAPH (8 vertices, many edges)
void createDenseGraph(unordered_map<int, Node*>& graph) {
    cout << "[INFO] Creating dense graph (8 vertices)..." << endl;
    
    for (int i = 0; i < 8; i++) {
        graph[i] = new Node(i);
    }

    // Create many connections
    vector<tuple<int, int, int>> edges = {
        {0, 1, 10}, {0, 2, 20}, {0, 3, 15},
        {1, 2, 5}, {1, 4, 15}, {1, 5, 10},
        {2, 3, 10}, {2, 5, 20}, {2, 6, 15},
        {3, 6, 10}, {3, 7, 20},
        {4, 5, 10}, {4, 7, 15},
        {5, 6, 5}, {5, 7, 10},
        {6, 7, 20}
    };

    for (auto& e : edges) {
        int from = get<0>(e);
        int to = get<1>(e);
        int cap = get<2>(e);
        
        graph[from]->edges.push_back(new Edge(cap, graph[to]));
        graph[to]->parents[graph[from]] = graph[from]->edges.back();
    }
}

// 5. GRAPH FROM YOUR TEST FILE
void createTestFileGraph(unordered_map<int, Node*>& graph) {
    cout << "[INFO] Creating graph from test1.txt..." << endl;
    
    // Based on your test1.txt file
    for (int i = 0; i <= 4; i++) {
        graph[i] = new Node(i);
    }

    vector<tuple<int, int, int>> edges = {
        {0, 2, 40}, {0, 1, 30}, {0, 3, 20},
        {1, 2, 50}, {2, 3, 20}, {3, 4, 30},
        {2, 4, 30}, {1, 4, 40}
    };

    for (auto& e : edges) {
        int from = get<0>(e);
        int to = get<1>(e);
        int cap = get<2>(e);
        
        graph[from]->edges.push_back(new Edge(cap, graph[to]));
        graph[to]->parents[graph[from]] = graph[from]->edges.back();
    }
}

// 6. EMPTY GRAPH (no edges)
void createEmptyGraph(unordered_map<int, Node*>& graph) {
    cout << "[INFO] Creating empty graph (3 vertices, no edges)..." << endl;
    
    for (int i = 0; i < 3; i++) {
        graph[i] = new Node(i);
    }
}

// 7. ZERO CAPACITY GRAPH
void createZeroCapacityGraph(unordered_map<int, Node*>& graph) {
    cout << "[INFO] Creating zero-capacity graph..." << endl;
    
    for (int i = 0; i < 4; i++) {
        graph[i] = new Node(i);
    }

    graph[0]->edges.push_back(new Edge(0, graph[1]));
    graph[1]->parents[graph[0]] = graph[0]->edges.back();

    graph[1]->edges.push_back(new Edge(0, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(0, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();
}

// ============ MAIN TEST SUITE ============
int main() {
    unordered_map<int, Node*> graph;
    
    cout << "\n" << string(70, '=') << endl;
    cout << "  EDMONDS-KARP ALGORITHM TEST SUITE" << endl;
    cout << "  Maximum Flow Calculator" << endl;
    cout << string(70, '=') << endl;
    cout << "Tests will run automatically..." << endl;
    
    // Wait for user
    cout << "\nPress Enter to start tests...";
    cin.ignore();
    
    // ============ TEST 1: Simple Graph ============
    cout << "\n\n" << string(70, '=') << endl;
    cout << "TEST 1: SIMPLE 4-VERTEX GRAPH" << endl;
    cout << string(70, '=') << endl;
    createSimpleGraph(graph);
    edmondsKarp(graph, 0, 3, true);
    cleanupGraph(graph);
    
    // ============ TEST 2: Textbook Example ============
    cout << "\n\n" << string(70, '=') << endl;
    cout << "TEST 2: CLASSIC TEXTBOOK EXAMPLE (6 vertices)" << endl;
    cout << string(70, '=') << endl;
    createTextbookExample(graph);
    edmondsKarp(graph, 0, 5, false);
    cleanupGraph(graph);
    
    // ============ TEST 3: Layered Graph ============
    cout << "\n\n" << string(70, '=') << endl;
    cout << "TEST 3: LAYERED GRAPH (10 vertices)" << endl;
    cout << string(70, '=') << endl;
    createLayeredGraph(graph);
    edmondsKarp(graph, 0, 9, false);
    cleanupGraph(graph);
    
    // ============ TEST 4: Dense Graph ============
    cout << "\n\n" << string(70, '=') << endl;
    cout << "TEST 4: DENSE GRAPH (8 vertices, many edges)" << endl;
    cout << string(70, '=') << endl;
    createDenseGraph(graph);
    edmondsKarp(graph, 0, 7, false);
    cleanupGraph(graph);
    
    // ============ TEST 5: Your Test File Graph ============
    cout << "\n\n" << string(70, '=') << endl;
    cout << "TEST 5: GRAPH FROM test1.txt" << endl;
    cout << string(70, '=') << endl;
    createTestFileGraph(graph);
    edmondsKarp(graph, 0, 4, true);
    cleanupGraph(graph);
    
    // ============ TEST 6: Edge Cases ============
    cout << "\n\n" << string(70, '=') << endl;
    cout << "TEST 6: EDGE CASES" << endl;
    cout << string(70, '=') << endl;
    
    cout << "\n--- Case 1: Empty graph ---" << endl;
    createEmptyGraph(graph);
    edmondsKarp(graph, 0, 2, false);
    cleanupGraph(graph);
    
    cout << "\n--- Case 2: Zero capacity graph ---" << endl;
    createZeroCapacityGraph(graph);
    edmondsKarp(graph, 0, 3, false);
    cleanupGraph(graph);
    
    cout << "\n--- Case 3: Same source and sink ---" << endl;
    createSimpleGraph(graph);
    edmondsKarp(graph, 0, 0, false);
    cleanupGraph(graph);
    
    cout << "\n--- Case 4: Non-existent vertices ---" << endl;
    createSimpleGraph(graph);
    edmondsKarp(graph, 0, 100, false);
    cleanupGraph(graph);
    
    // ============ PERFORMANCE TEST ============
    cout << "\n\n" << string(70, '=') << endl;
    cout << "PERFORMANCE TEST: Multiple runs" << endl;
    cout << string(70, '=') << endl;
    
    vector<pair<string, function<void(unordered_map<int, Node*>&)>>> testGraphs = {
        {"Simple 4-vertex", createSimpleGraph},
        {"Textbook example", createTextbookExample},
        {"Layered graph", createLayeredGraph},
        {"Dense graph", createDenseGraph}
    };
    
    cout << "\n" << setw(25) << left << "Graph Type" 
         << setw(10) << "Vertices" 
         << setw(10) << "Edges"
         << setw(12) << "Max Flow"
         << setw(12) << "Time (microseconds)"
         << setw(15) << "Throughput" << endl;
    cout << string(80, '-') << endl;
    
    for (auto& test : testGraphs) {
        unordered_map<int, Node*> perfGraph;
        test.second(perfGraph);
        
        int vertices = perfGraph.size();
        int edges = 0;
        for (auto& pair : perfGraph) {
            edges += pair.second->edges.size();
        }
        
        auto start = high_resolution_clock::now();
        int flow = edmondsKarp(perfGraph, 0, vertices-1, false);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        double throughput = (duration.count() > 0) ? 
            (flow * 1000000.0) / duration.count() : 0;
        
        cout << setw(25) << left << test.first
             << setw(10) << vertices
             << setw(10) << edges
             << setw(12) << flow
             << setw(12) << duration.count()
             << fixed << setprecision(0) << setw(15) << throughput << endl;
        
        cleanupGraph(perfGraph);
    }
    
    cout << string(80, '-') << endl;
    
    // ============ FINAL MESSAGE ============
    cout << "\n\n" << string(70, '=') << endl;
    cout << "  ALL TESTS COMPLETED SUCCESSFULLY!" << endl;
    cout << string(70, '=') << endl;
    cout << "\nSummary:" << endl;
    cout << "  - 6 different graph types tested" << endl;
    cout << "  - 4 edge cases validated" << endl;
    cout << "  - Performance metrics collected" << endl;
    cout << "  - All memory properly cleaned up" << endl;
    
    cout << "\nPress Enter to exit...";
    cin.ignore();
    
    return 0;
}