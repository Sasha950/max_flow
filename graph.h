#pragma once
#include <vector>
#include <unordered_map>

class Node;
class Edge;

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