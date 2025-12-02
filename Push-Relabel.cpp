#include "Push-Relabel.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;

// Вспомогательная функция для добавления ребра в остаточную сеть
void addResidualEdge(unordered_map<int, unordered_map<int, int>>& residual,
    int from, int to, int capacity) {
    residual[from][to] = capacity;
}

int pushRelabel(unordered_map<int, Node*>& graph, int sourceId, int sinkId) {
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

    int n = graph.size();

    // Создаем остаточную сеть
    unordered_map<int, unordered_map<int, int>> residual;

    // Инициализация остаточных пропускных способностей
    for (auto& pair : graph) {
        int u = pair.first;
        Node* uNode = pair.second;

        for (Edge* edge : uNode->edges) {
            int v = edge->adjacentNode->id;
            addResidualEdge(residual, u, v, edge->weight);
            // Инициализируем обратное ребро если еще не существует
            if (residual[v].find(u) == residual[v].end()) {
                addResidualEdge(residual, v, u, 0);
            }
        }
    }

    // Высоты вершин
    unordered_map<int, int> height;

    // Избыточный поток в вершинах
    unordered_map<int, int> excess;

    // Инициализация высот
    height[sourceId] = n;

    // Инициализация избыточного потока
    for (auto& edge : residual[sourceId]) {
        int v = edge.first;
        int capacity = edge.second;
        if (capacity > 0) {
            residual[sourceId][v] = 0;
            residual[v][sourceId] += capacity;
            excess[v] = capacity;
            excess[sourceId] -= capacity;
        }
    }

    // Очередь активных вершин
    queue<int> activeVertices;
    for (auto& pair : graph) {
        int u = pair.first;
        if (u != sourceId && u != sinkId && excess[u] > 0) {
            activeVertices.push(u);
        }
    }

    // Основной цикл алгоритма
    while (!activeVertices.empty()) {
        int u = activeVertices.front();
        activeVertices.pop();

        // Пытаемся протолкнуть поток
        bool pushed = false;

        for (auto& edge : residual[u]) {
            int v = edge.first;
            int capacity = edge.second;

            // Можем протолкнуть только если есть остаточная пропускная способность
            // и высота u больше высоты v на 1
            if (capacity > 0 && height[u] == height[v] + 1) {
                int flow = min(excess[u], capacity);

                residual[u][v] -= flow;
                residual[v][u] += flow;

                excess[u] -= flow;
                excess[v] += flow;

                if (v != sourceId && v != sinkId && excess[v] > 0) {
                    activeVertices.push(v);
                }

                pushed = true;

                if (excess[u] == 0) {
                    break;
                }
            }
        }

        // Если не удалось протолкнуть, поднимаем вершину
        if (!pushed && excess[u] > 0) {
            // Находим минимальную высоту среди соседей с положительной остаточной пропускной способностью
            int minHeight = INT_MAX;
            for (auto& edge : residual[u]) {
                int v = edge.first;
                int capacity = edge.second;
                if (capacity > 0) {
                    minHeight = min(minHeight, height[v]);
                }
            }

            if (minHeight != INT_MAX) {
                height[u] = minHeight + 1;
            }

            // Возвращаем вершину в очередь
            activeVertices.push(u);
        }
    }

    // Максимальный поток равен избыточному потоку в стоке
    return excess[sinkId];
}