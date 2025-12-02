// edmonds_karp.cpp - IMPLÉMENTATION POUR VOTRE STRUCTURE
#include "edmonds_karp.h"
#include <algorithm>
#include <climits>
#include <iomanip>

using namespace std;

EdmondsKarp::EdmondsKarp() : networkBuilt(false) {}

EdmondsKarp::~EdmondsKarp() {
    clear();
}

Node* EdmondsKarp::getNode(int id) const {
    auto it = nodes.find(id);
    return (it != nodes.end()) ? it->second.get() : nullptr;
}

void EdmondsKarp::addEdge(int from, int to, int capacity) {
    // Créer les nœuds si nécessaire
    if (nodes.find(from) == nodes.end()) {
        nodes[from].reset(new Node(from));
    }
    if (nodes.find(to) == nodes.end()) {
        nodes[to].reset(new Node(to));
    }
    
    Node* fromNode = nodes[from].get();
    Node* toNode = nodes[to].get();
    
    // Ajouter l'arête dans VOTRE structure
    Edge* edge = new Edge(capacity, toNode);
    fromNode->edges.push_back(edge);
    toNode->parents[fromNode] = edge;
    
    // Ajouter l'arête inverse (pour Edmonds-Karp)
    Edge* reverseEdge = new Edge(0, fromNode);
    toNode->edges.push_back(reverseEdge);
    fromNode->parents[toNode] = reverseEdge;
    
    networkBuilt = false; // Nécessite reconstruction
}

void EdmondsKarp::buildFlowNetwork() {
    if (networkBuilt) return;
    
    clearFlowNetwork();
    
    // Convertir VOTRE structure en réseau de flux
    for (const auto& pair : nodes) {
        Node* fromNode = pair.second.get();
        
        for (Edge* edge : fromNode->edges) {
            Node* toNode = edge->adjacentNode;
            int capacity = edge->weight;
            
            // Créer FlowEdge forward
            FlowEdge* forward = new FlowEdge(toNode, capacity);
            FlowEdge* backward = new FlowEdge(fromNode, 0);
            
            forward->reverse = backward;
            backward->reverse = forward;
            
            flowNetwork[fromNode].push_back(forward);
            flowNetwork[toNode].push_back(backward);
        }
    }
    
    networkBuilt = true;
}

void EdmondsKarp::clearFlowNetwork() {
    for (auto& pair : flowNetwork) {
        for (FlowEdge* edge : pair.second) {
            delete edge;
        }
    }
    flowNetwork.clear();
}

bool EdmondsKarp::bfs(Node* source, Node* sink,
                     unordered_map<Node*, Node*>& parent,
                     unordered_map<Node*, FlowEdge*>& path) {
    parent.clear();
    path.clear();
    
    parent[source] = source;
    
    queue<Node*> q;
    q.push(source);
    
    while (!q.empty()) {
        Node* u = q.front();
        q.pop();
        
        if (u == sink) break;
        
        auto it = flowNetwork.find(u);
        if (it == flowNetwork.end()) continue;
        
        for (FlowEdge* edge : it->second) {
            Node* v = edge->to;
            int residual = edge->residualCapacity();
            
            if (residual > 0 && parent.find(v) == parent.end()) {
                parent[v] = u;
                path[v] = edge;
                q.push(v);
            }
        }
    }
    
    return parent.find(sink) != parent.end();
}

int EdmondsKarp::maxFlow(int source, int sink, bool verbose) {
    Node* sourceNode = getNode(source);
    Node* sinkNode = getNode(sink);
    
    if (!sourceNode || !sinkNode) {
        cout << "ERROR: Source or sink node not found!" << endl;
        return 0;
    }
    
    if (source == sink) {
        cout << "ERROR: Source and sink are the same!" << endl;
        return 0;
    }
    
    // Construire le réseau de flux
    buildFlowNetwork();
    
    unordered_map<Node*, Node*> parent;
    unordered_map<Node*, FlowEdge*> path;
    
    int totalFlow = 0;
    int iterations = 0;
    
    while (bfs(sourceNode, sinkNode, parent, path)) {
        iterations++;
        
        // Trouver le bottleneck
        int bottleneck = INT_MAX;
        for (Node* v = sinkNode; v != sourceNode; v = parent[v]) {
            FlowEdge* edge = path[v];
            bottleneck = min(bottleneck, edge->residualCapacity());
        }
        
        // Augmenter le flux
        for (Node* v = sinkNode; v != sourceNode; v = parent[v]) {
            FlowEdge* edge = path[v];
            edge->augment(bottleneck);
        }
        
        totalFlow += bottleneck;
        
        if (verbose && iterations <= 5) {
            cout << "Iteration " << iterations << ": +" << bottleneck 
                 << " (total: " << totalFlow << ")" << endl;
        }
        
        // Sécurité
        if (iterations > nodes.size() * nodes.size() * 2) {
            cout << "\nWARNING: Safe stop after " << iterations << " iterations" << endl;
            break;
        }
    }
    
    if (verbose) {
        cout << "\nMaximum flow: " << totalFlow << endl;
        cout << "Iterations: " << iterations << endl;
    }
    
    return totalFlow;
}

void EdmondsKarp::displayGraphStructure() const {
    cout << "\n" << string(60, '-') << endl;
    cout << "  GRAPH STRUCTURE" << endl;
    cout << string(60, '-') << endl;
    
    cout << "Vertices: " << nodes.size() << endl;
    cout << "Edges: " << getNumEdges() << endl << endl;
    
    // Trier les sommets par ID pour un affichage propre
    vector<int> vertexIds;
    for (const auto& pair : nodes) {
        vertexIds.push_back(pair.first);
    }
    sort(vertexIds.begin(), vertexIds.end());
    
    for (int vertexId : vertexIds) {
        const Node* node = nodes.at(vertexId).get();
        
        cout << "Vertex " << vertexId << ":" << endl;
        
        // Afficher seulement les arêtes sortantes avec capacité > 0
        bool hasOutgoing = false;
        for (const Edge* edge : node->edges) {
            if (edge->weight > 0) {
                if (!hasOutgoing) {
                    hasOutgoing = true;
                }
                cout << "    -> " << edge->adjacentNode->id 
                     << " [capacity: " << edge->weight << "]" << endl;
            }
        }
        
        if (!hasOutgoing) {
            cout << "    (no outgoing edges)" << endl;
        }
    }
    cout << string(60, '-') << endl;
}

void EdmondsKarp::displayFlowNetwork() const {
    cout << "\n" << string(70, '=') << endl;
    cout << "  FLOW NETWORK" << endl;
    cout << string(70, '=') << endl;
    
    cout << left << setw(8) << "From" 
         << setw(8) << "To" 
         << setw(12) << "Flow/Capacity"
         << setw(10) << "Usage" << endl;
    cout << string(70, '-') << endl;
    
    int totalFlow = 0;
    int totalCapacity = 0;
    
    for (const auto& pair : flowNetwork) {
        Node* fromNode = pair.first;
        
        for (const FlowEdge* edge : pair.second) {
            if (edge->capacity > 0) { // Arêtes forward seulement
                totalCapacity += edge->capacity;
                totalFlow += edge->flow;
                
                int usage = edge->capacity > 0 ? (edge->flow * 100) / edge->capacity : 0;
                
                cout << setw(8) << fromNode->id 
                     << setw(8) << edge->to->id
                     << setw(12) << (to_string(edge->flow) + "/" + to_string(edge->capacity))
                     << setw(10) << (to_string(usage) + "%") << endl;
            }
        }
    }
    
    cout << string(70, '-') << endl;
    cout << "Total flow: " << totalFlow << endl;
    cout << "Total capacity: " << totalCapacity << endl;
    if (totalCapacity > 0) {
        cout << "Utilization: " << (totalFlow * 100 / totalCapacity) << "%" << endl;
    }
    cout << string(70, '=') << endl;
}

void EdmondsKarp::displayStatistics() const {
    cout << "\n" << string(60, '=') << endl;
    cout << "  STATISTICS" << endl;
    cout << string(60, '=') << endl;
    
    // cout << "Graph size:" << endl;
    // cout << "  Vertices: " << nodes.size() << endl;
    // cout << "  Edges: " << getNumEdges() << endl;
    
    // Calculer les degrés
    unordered_map<int, int> outDegrees;
    unordered_map<int, int> inDegrees;
    
    for (const auto& pair : nodes) {
        int vertexId = pair.first;
        const Node* node = pair.second.get();
        
        outDegrees[vertexId] = 0;
        for (const Edge* edge : node->edges) {
            if (edge->weight > 0) {
                outDegrees[vertexId]++;
            }
        }
        
        inDegrees[vertexId] = node->parents.size();
    }
    
    int maxOut = 0, minOut = INT_MAX;
    int maxIn = 0, minIn = INT_MAX;
    
    for (const auto& pair : outDegrees) {
        maxOut = max(maxOut, pair.second);
        if (pair.second > 0) minOut = min(minOut, pair.second);
    }
    
    for (const auto& pair : inDegrees) {
        maxIn = max(maxIn, pair.second);
        if (pair.second > 0) minIn = min(minIn, pair.second);
    }
    
    cout << "\nVertex degrees:" << endl;
    cout << "  Max outgoing: " << maxOut << endl;
    cout << "  Min outgoing: " << (minOut == INT_MAX ? 0 : minOut) << endl;
    cout << "  Max incoming: " << maxIn << endl;
    cout << "  Min incoming: " << (minIn == INT_MAX ? 0 : minIn) << endl;
    
    cout << string(60, '=') << endl;
}

int EdmondsKarp::getNumVertices() const {
    return nodes.size();
}

int EdmondsKarp::getNumEdges() const {
    int count = 0;
    for (const auto& pair : nodes) {
        for (const Edge* edge : pair.second->edges) {
            if (edge->weight > 0) {
                count++;
            }
        }
    }
    return count;
}

void EdmondsKarp::clear() {
    // Nettoyer VOTRE structure
    for (auto& pair : nodes) {
        Node* node = pair.second.get();
        for (Edge* edge : node->edges) {
            delete edge;
        }
        node->edges.clear();
        node->parents.clear();
    }
    nodes.clear();
    
    // Nettoyer le réseau de flux
    clearFlowNetwork();
    
    networkBuilt = false;
}