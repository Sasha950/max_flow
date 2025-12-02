#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>
#include <climits>
#include <string>

class Node;
class Edge;

// --- YOUR STRUCTURES (UNCHANGED) ---

struct Node 
{
    int id;
    std::vector<Edge*> edges;
    std::unordered_map<Node*, Edge*> parents;

    Node(int nodeId) : id(nodeId) {}
};

struct Edge 
{
    int weight;
    Node* adjacentNode;

    Edge(int w, Node* node) : weight(w), adjacentNode(node) {}
};

// --- YOUR GRAPH CLASS ---

class Graph
{
private:
    std::vector<Node*> nodes;

    // Algorithm data
    std::unordered_map<Edge*, Edge*> reverseEdges;
    std::unordered_map<Edge*, int> edgeFlow;
    std::unordered_map<int, int> heights; 
    std::unordered_map<int, int> excess;  

    void push(Node* u, Edge* edge) {
        int capacity = edge->weight;
        int flow = edgeFlow[edge];
        int d = std::min(excess[u->id], capacity - flow);

        edgeFlow[edge] += d;
        Edge* rev = reverseEdges[edge];
        edgeFlow[rev] -= d;

        excess[u->id] -= d;
        excess[edge->adjacentNode->id] += d;
    }

    void relabel(Node* u) {
        int minHeight = INT_MAX;
        for (Edge* edge : u->edges) {
            int capacity = edge->weight;
            int flow = edgeFlow[edge];
            if (capacity - flow > 0) {
                int neighborId = edge->adjacentNode->id;
                if (heights.find(neighborId) == heights.end()) heights[neighborId] = 0;
                minHeight = std::min(minHeight, heights[neighborId]);
            }
        }
        if (minHeight != INT_MAX) {
            heights[u->id] = minHeight + 1;
        }
    }

public:
    ~Graph() {
        for (Node* node : nodes) {
            for (Edge* edge : node->edges) delete edge;
            delete node;
        }
    }

    Node* addNode(int id) {
        Node* newNode = new Node(id);
        nodes.push_back(newNode);
        return newNode;
    }

    void addEdge(Node* from, Node* to, int weight) {
        if (!from || !to) return;

        Edge* forward = new Edge(weight, to);
        Edge* backward = new Edge(0, from); 

        reverseEdges[forward] = backward;
        reverseEdges[backward] = forward;

        edgeFlow[forward] = 0;
        edgeFlow[backward] = 0;

        from->edges.push_back(forward);
        to->parents[from] = forward; 
        to->edges.push_back(backward);
        from->parents[to] = backward;
    }

    // Helper for stats
    void getGraphInfo(int& outVertices, int& outEdges) {
        outVertices = nodes.size();
        outEdges = 0;
        for(Node* n : nodes) {
            for(Edge* e : n->edges) {
                // Count only forward edges (original weight >= 0 and reverse weight 0)
                if (e->weight >= 0 && reverseEdges[e]->weight == 0) { 
                     outEdges++;
                }
            }
        }
    }

    void getMaxFlow(Node* source, Node* sink) {
        // 1. Validation
        if (nodes.empty()) {
            std::cout << "Error: The graph is empty." << std::endl;
            return;
        }
        if (source == nullptr || sink == nullptr) {
            std::cout << "Error: Invalid Source or Sink pointer." << std::endl;
            return;
        }
        if (source == sink) {
            std::cout << "Error: Source equals Sink." << std::endl;
            return;
        }

        // Reset state
        heights.clear();
        excess.clear();
        for (auto& pair : edgeFlow) pair.second = 0;

        heights[source->id] = nodes.size(); 

        // Preflow
        for (Edge* edge : source->edges) {
            if (edge->weight > 0) { 
                int d = edge->weight;
                edgeFlow[edge] += d;
                Edge* rev = reverseEdges[edge];
                edgeFlow[rev] -= d;
                excess[source->id] -= d;
                excess[edge->adjacentNode->id] += d;
            }
        }

        std::queue<Node*> activeNodes;
        for (Node* node : nodes) {
            if (node != source && node != sink && excess[node->id] > 0) {
                activeNodes.push(node);
            }
        }

        while (!activeNodes.empty()) {
            Node* u = activeNodes.front();
            bool pushed = false;

            for (Edge* edge : u->edges) {
                int cap = edge->weight;
                int flow = edgeFlow[edge];
                Node* v = edge->adjacentNode;

                if (cap - flow > 0 && heights[u->id] == heights[v->id] + 1) {
                    push(u, edge);
                    if (v != source && v != sink && excess[v->id] > 0) {
                        activeNodes.push(v);
                    }
                    pushed = true;
                    if (excess[u->id] == 0) break;
                }
            }

            if (excess[u->id] > 0) {
                if (!pushed) relabel(u);
                activeNodes.pop();
                activeNodes.push(u);
            } else {
                activeNodes.pop();
            }
        }

        std::cout << "Algorithm Result (Max Flow): " << excess[sink->id] << std::endl;
    }
};

// ============ TEST HELPERS ============

void printHeader(const std::string& name, Graph& g, int sId, int tId) {
    int v, e;
    g.getGraphInfo(v, e);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST: " << name << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nGraph Characteristics:" << std::endl;
    std::cout << "  Vertices: " << v << std::endl;
    std::cout << "  Edges: " << e << std::endl; 
    std::cout << "Source: " << sId << ", Sink: " << tId << std::endl;
}

int main() {
    // Locale is not needed for English output, removed.

    std::cout << "TESTING ALGORITHM (PUSH-RELABEL)" << std::endl;
    std::cout << "========================================" << std::endl;

    // Test 1: Empty Graph
    {
        Graph gEmpty;
        Node* s = gEmpty.addNode(1);
        gEmpty.addNode(2);
        Node* t = gEmpty.addNode(3);

        printHeader("EMPTY GRAPH (no edges)", gEmpty, 1, 3);
        gEmpty.getMaxFlow(s, t);
    }

    // Test 2: Very Small Graph
    {
        Graph g;
        std::unordered_map<int, Node*> n;
        for(int i=1; i<=3; ++i) n[i] = g.addNode(i);

        g.addEdge(n[1], n[2], 10);
        g.addEdge(n[2], n[3], 5);

        printHeader("VERY SMALL GRAPH (3 vertices)", g, 1, 3);
        g.getMaxFlow(n[1], n[3]);
    }

    // Test 3: Zero Capacity Graph
    {
        Graph g;
        std::unordered_map<int, Node*> n;
        for(int i=1; i<=4; ++i) n[i] = g.addNode(i);

        g.addEdge(n[1], n[2], 0);
        g.addEdge(n[2], n[3], 0);
        g.addEdge(n[3], n[4], 0);

        printHeader("ZERO CAPACITY GRAPH", g, 1, 4);
        g.getMaxFlow(n[1], n[4]);
    }

    // Test 4: Small Graph (Classic Example)
    {
        Graph g;
        std::unordered_map<int, Node*> n;
        for(int i=1; i<=6; ++i) n[i] = g.addNode(i);

        g.addEdge(n[1], n[2], 16);
        g.addEdge(n[1], n[3], 13);
        g.addEdge(n[2], n[3], 12); 
        g.addEdge(n[2], n[4], 10); 
        g.addEdge(n[3], n[2], 9);
        g.addEdge(n[3], n[5], 14);
        g.addEdge(n[4], n[5], 7);  
        g.addEdge(n[4], n[6], 4);
        g.addEdge(n[5], n[6], 20);

        printHeader("SMALL GRAPH (6 vertices, classic)", g, 1, 6);
        g.getMaxFlow(n[1], n[6]); // Expected: 23
    }

    // Test 5: Medium Graph
    {
        Graph g;
        std::unordered_map<int, Node*> n;
        for(int i=1; i<=10; ++i) n[i] = g.addNode(i);

        g.addEdge(n[1], n[2], 20);
        g.addEdge(n[1], n[3], 15);
        g.addEdge(n[1], n[4], 10);
        g.addEdge(n[2], n[5], 25);
        g.addEdge(n[3], n[5], 10);
        g.addEdge(n[3], n[6], 15);
        g.addEdge(n[4], n[6], 20);
        g.addEdge(n[5], n[7], 15);
        g.addEdge(n[5], n[8], 10);
        g.addEdge(n[6], n[8], 20);
        g.addEdge(n[6], n[9], 5);
        g.addEdge(n[7], n[10], 30);
        g.addEdge(n[8], n[10], 20);
        g.addEdge(n[9], n[10], 10);

        printHeader("MEDIUM GRAPH (10 vertices)", g, 1, 10);
        g.getMaxFlow(n[1], n[10]);
    }

    // Test 6: Large Graph
    {
        Graph g;
        std::unordered_map<int, Node*> n;
        for(int i=1; i<=15; ++i) n[i] = g.addNode(i);

        g.addEdge(n[1], n[2], 50);
        g.addEdge(n[1], n[3], 40);
        g.addEdge(n[1], n[4], 30);
        g.addEdge(n[1], n[5], 20);

        g.addEdge(n[2], n[6], 15);
        g.addEdge(n[2], n[7], 10);

        g.addEdge(n[3], n[7], 20);
        g.addEdge(n[3], n[8], 15);

        g.addEdge(n[4], n[8], 25);
        g.addEdge(n[4], n[9], 10);

        g.addEdge(n[5], n[9], 30);
        g.addEdge(n[5], n[10], 5);

        g.addEdge(n[6], n[11], 40);
        g.addEdge(n[7], n[11], 20);
        g.addEdge(n[7], n[12], 15);

        g.addEdge(n[8], n[12], 25);
        g.addEdge(n[8], n[13], 10);

        g.addEdge(n[9], n[13], 30);
        g.addEdge(n[9], n[14], 5);

        g.addEdge(n[10], n[14], 35);

        g.addEdge(n[11], n[15], 50);
        g.addEdge(n[12], n[15], 45);
        g.addEdge(n[13], n[15], 35);
        g.addEdge(n[14], n[15], 25);
        
        g.addEdge(n[12], n[8], 5);
        g.addEdge(n[14], n[10], 3);

        printHeader("LARGE GRAPH (15 vertices)", g, 1, 15);
        g.getMaxFlow(n[1], n[15]);
    }

    // Test 7: Source equals Sink
    {
        Graph g;
        Node* s = g.addNode(1);
        Node* t = g.addNode(2);
        g.addEdge(s, t, 10);

        printHeader("SOURCE AND SINK ARE THE SAME", g, 1, 1);
        g.getMaxFlow(s, s);
    }

    // Test 8: Invalid Vertex
    {
        Graph g;
        Node* s = g.addNode(1);
        Node* t = g.addNode(2);
        g.addEdge(s, t, 10);

        printHeader("NON-EXISTENT VERTEX (nullptr)", g, 1, 100);
        g.getMaxFlow(s, nullptr);
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "TESTING COMPLETED" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}