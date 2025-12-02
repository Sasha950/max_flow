// edge_improved.h - VOTRE STRUCTURE EXACTE
#ifndef EDGE_IMPROVED_H
#define EDGE_IMPROVED_H

#include <iostream>
#include <unordered_map>
#include <vector>

// Déclarations anticipées
class Node;
class Edge;

class Edge {
public:
    int weight;                 // poids/capacité du bord
    Node* adjacentNode;         // noeud adjacent

    Edge(int w, Node* node) : weight(w), adjacentNode(node) {}
};

class Node {
public:
    int id;                                     // ID du sommet
    std::vector<Edge*> edges;                   // liste des bords sortants
    std::unordered_map<Node*, Edge*> parents;   // liste des bords entrants

    Node(int nodeId) : id(nodeId) {}
};

#endif // EDGE_IMPROVED_H