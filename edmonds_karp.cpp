#include "graph.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

// ============ EDMONDS-KARP IMPLEMENTATION ============
int edmondsKarp(unordered_map<int, Node*>& graph, int sourceId, int sinkId, bool verbose) {
    // Basic validations
    if (graph.empty()) {
        return 0;
    }

    if (graph.find(sourceId) == graph.end()) {
        return 0;
    }

    if (graph.find(sinkId) == graph.end()) {
        return 0;
    }

    if (sourceId == sinkId) {
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

        // Safety to prevent infinite loops
        if (iterations > graph.size() * graph.size()) {
            break;
        }
    }

    // Cleanup
    for (auto& pair : flowNetwork) {
        for (FlowEdge* edge : pair.second) {
            delete edge;
        }
    }

    return maxFlow;
}