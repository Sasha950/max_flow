#include "Dinic.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <functional>


using namespace std;

// Вспомогательная структура для ребра в алгоритме Диница
struct FlowEdge {
    int to;
    int capacity;
    int rev;

    FlowEdge(int t, int c, int r) : to(t), capacity(c), rev(r) {}
};

int dinic(unordered_map<int, Node*>& graph, int sourceId, int sinkId) {
    // Проверка входных данных
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

    // Создаем сеть для алгоритма Диница
    unordered_map<int, vector<FlowEdge>> flowNetwork;
    unordered_map<int, int> nodeIdToIndex;
    unordered_map<int, int> indexToNodeId;

    // Сопоставляем ID вершинам индексы
    int index = 0;
    for (auto& pair : graph) {
        nodeIdToIndex[pair.first] = index;
        indexToNodeId[index] = pair.first;
        index++;
    }

    // Строим сеть потоков
    for (auto& pair : graph) {
        int u = nodeIdToIndex[pair.first];
        Node* uNode = pair.second;

        for (Edge* edge : uNode->edges) {
            int v = nodeIdToIndex[edge->adjacentNode->id];
            int capacity = edge->weight;

            // Прямое ребро
            flowNetwork[u].push_back(FlowEdge(v, capacity, flowNetwork[v].size()));
            // Обратное ребро
            flowNetwork[v].push_back(FlowEdge(u, 0, flowNetwork[u].size() - 1));
        }
    }

    int source = nodeIdToIndex[sourceId];
    int sink = nodeIdToIndex[sinkId];
    int n = graph.size();

    int maxFlow = 0;
    vector<int> level(n);
    vector<int> ptr(n);

    // BFS для построения слоистой сети
    auto bfs = [&]() -> bool {
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        q.push(source);
        level[source] = 0;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (const FlowEdge& e : flowNetwork[u]) {
                if (e.capacity > 0 && level[e.to] == -1) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }

        return level[sink] != -1;
        };

    // DFS для поиска блокирующего потока
    function<int(int, int)> dfs = [&](int u, int flow) -> int {
        if (u == sink) {
            return flow;
        }

        for (int& i = ptr[u]; i < flowNetwork[u].size(); ++i) {
            FlowEdge& e = flowNetwork[u][i];

            if (level[e.to] == level[u] + 1 && e.capacity > 0) {
                int pushed = dfs(e.to, min(flow, e.capacity));
                if (pushed > 0) {
                    e.capacity -= pushed;
                    flowNetwork[e.to][e.rev].capacity += pushed;
                    return pushed;
                }
            }
        }

        return 0;
        };

    // Основной цикл алгоритма Диница
    while (bfs()) {
        fill(ptr.begin(), ptr.end(), 0);

        int pushed;
        while ((pushed = dfs(source, INT_MAX)) > 0) {
            maxFlow += pushed;
        }
    }

    return maxFlow;
}