// edmonds_karp.h - ADAPTÉ POUR VOTRE STRUCTURE
#ifndef EDMONDS_KARP_H
#define EDMONDS_KARP_H

#include "edge_improved.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>
#include <queue>

class EdmondsKarp {
private:
    // Stockage des nœuds avec VOTRE structure
    std::unordered_map<int, std::unique_ptr<Node>> nodes;
    
    // Structure interne pour gérer le flux
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
        
        bool isSaturated() const {
            return flow == capacity;
        }
        
        int utilization() const {
            if (capacity == 0) return 0;
            return (flow * 100) / capacity;
        }
    };
    
    // Réseau de flux (séparé de votre structure)
    std::unordered_map<Node*, std::vector<FlowEdge*>> flowNetwork;
    bool networkBuilt;
    
    // Méthodes privées
    void buildFlowNetwork();
    void clearFlowNetwork();
    bool bfs(Node* source, Node* sink, 
             std::unordered_map<Node*, Node*>& parent,
             std::unordered_map<Node*, FlowEdge*>& path);
    
public:
    EdmondsKarp();
    ~EdmondsKarp();
    
    // Interface utilisant VOTRE structure
    void addEdge(int from, int to, int capacity);
    
    // Calcul du flux maximum
    int maxFlow(int source, int sink, bool verbose = false);
    
    // Affichage
    void displayGraphStructure() const;
    void displayFlowNetwork() const;
    void displayStatistics() const;
    
    // Getters
    void clear();
    int getNumVertices() const;
    int getNumEdges() const;
    
    // Helper pour obtenir un nœud
    Node* getNode(int id) const;
    
    // Pour compatibilité
    int getVertexCount() const { return nodes.size(); }
};

#endif // EDMONDS_KARP_H