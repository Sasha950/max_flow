#include "FordFulkerson.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>

using namespace std;

struct ResidualEdge {
    Node* from;
    Node* to;
    int capacity;
    Edge* originalEdge;
    bool isForward;

    ResidualEdge(Node* f = nullptr, Node* t = nullptr, int cap = 0, Edge* orig = nullptr, bool forward = true)
        : from(f), to(t), capacity(cap), originalEdge(orig), isForward(forward) {
    }
};

int fordFulkerson(unordered_map<int, Node*>& graph, int sourceId, int sinkId) {
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

    unordered_map<Node*, vector<ResidualEdge>> residualGraph;

    for (auto& pair : graph) {
        Node* u = pair.second;

        for (Edge* edge : u->edges) {
            Node* v = edge->adjacentNode;

            ResidualEdge forward(u, v, edge->weight, edge, true);
            residualGraph[u].push_back(forward);

            ResidualEdge backward(v, u, 0, nullptr, false);
            residualGraph[v].push_back(backward);
        }
    }

    int maxFlow = 0;

    while (true) {
        queue<Node*> q;
        unordered_map<Node*, bool> visited;
        unordered_map<Node*, ResidualEdge*> parent;

        q.push(source);
        visited[source] = true;

        bool foundPath = false;

        while (!q.empty() && !foundPath) {
            Node* u = q.front();
            q.pop();

            if (residualGraph.find(u) == residualGraph.end()) continue;

            for (ResidualEdge& re : residualGraph[u]) {
                if (!visited[re.to] && re.capacity > 0) {
                    q.push(re.to);
                    visited[re.to] = true;

                    parent[re.to] = &re;

                    if (re.to == sink) {
                        foundPath = true;
                        break;
                    }
                }
            }
            if (foundPath) break;
        }

        if (!foundPath) break;

        int pathFlow = INT_MAX;
        Node* v = sink;

        while (v != source) {
            ResidualEdge* re = parent[v];
            pathFlow = min(pathFlow, re->capacity);
            v = re->from;
        }

        v = sink;
        while (v != source) {
            ResidualEdge* re = parent[v];
            re->capacity -= pathFlow;

            for (ResidualEdge& reverseRe : residualGraph[v]) {
                if (reverseRe.to == re->from) {
                    reverseRe.capacity += pathFlow;
                    break;
                }
            }

            v = re->from;
        }

        maxFlow += pathFlow;
    }

    return maxFlow;
}