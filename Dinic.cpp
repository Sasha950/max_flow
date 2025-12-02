#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <clocale>
#include <functional>

// Структура для остаточного ребра
struct ResidualEdge {
    int capacity;     // остаточная пропускная способность
    int to;           // вершина назначения
    int rev;          // индекс обратного ребра в списке смежности

    ResidualEdge(int cap = 0, int toNode = -1, int reverseIndex = -1)
        : capacity(cap), to(toNode), rev(reverseIndex) {
    }
};

// Вспомогательная функция для подсчета ребер
int countEdges(std::unordered_map<int, std::vector<ResidualEdge>>& graph) {
    int edgeCount = 0;
    for (auto& pair : graph) {
        edgeCount += pair.second.size();
    }
    return edgeCount;
}

// Алгоритм Диница
int dinic(std::unordered_map<int, std::vector<ResidualEdge>>& graph, int sourceId, int sinkId) {
    // Вывод характеристик графа как у коллеги
    int vertexCount = graph.size();
    int edgeCount = countEdges(graph) / 2; // делим на 2, так как каждое ребро учитывается дважды

    std::cout << "Характеристики графа:" << std::endl;
    std::cout << "    Вершин: " << vertexCount << std::endl;
    std::cout << "    Ребер: " << edgeCount << std::endl;
    std::cout << "Источник: " << sourceId << ", Сток: " << sinkId << std::endl;

    // Проверка входных данных
    if (graph.empty()) {
        std::cout << "Граф пустой" << std::endl;
        std::cout << "Результат алгоритма: 0" << std::endl << std::endl;
        return 0;
    }

    if (graph.find(sourceId) == graph.end()) {
        std::cout << "Вершина-источник " << sourceId << " не найдена" << std::endl;
        std::cout << "Результат алгоритма: 0" << std::endl << std::endl;
        return 0;
    }

    if (graph.find(sinkId) == graph.end()) {
        std::cout << "Вершина-сток " << sinkId << " не найдена" << std::endl;
        std::cout << "Результат алгоритма: 0" << std::endl << std::endl;
        return 0;
    }

    if (sourceId == sinkId) {
        std::cout << "Источник и сток – одна и та же вершина" << std::endl;
        std::cout << "Результат алгоритма: 0" << std::endl << std::endl;
        return 0;
    }

    int maxFlow = 0;

    // Для уровней
    std::unordered_map<int, int> level;

    // Для указателей
    std::unordered_map<int, int> ptr;

    // 2. BFS для построения слоистой сети
    auto bfs = [&]() -> bool {
        // Очищаем уровни
        for (auto& pair : graph) {
            level[pair.first] = -1;
        }

        std::queue<int> q;
        q.push(sourceId);
        level[sourceId] = 0;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (const ResidualEdge& re : graph[u]) {
                if (re.capacity > 0 && level[re.to] == -1) {
                    level[re.to] = level[u] + 1;
                    q.push(re.to);
                }
            }
        }

        return level[sinkId] != -1;
    };

    // 3. DFS для поиска блокирующего потока
    std::function<int(int, int)> dfs = [&](int u, int flow) -> int {
        if (u == sinkId) {
            return flow;
        }

        for (int& i = ptr[u]; i < graph[u].size(); ++i) {
            ResidualEdge& re = graph[u][i];

            if (level[re.to] == level[u] + 1 && re.capacity > 0) {
                int pushed = dfs(re.to, std::min(flow, re.capacity));
                if (pushed > 0) {
                    re.capacity -= pushed;
                    graph[re.to][re.rev].capacity += pushed;
                    return pushed;
                }
            }
        }

        return 0;
    };

    // 4. Основной цикл алгоритма Диница
    while (bfs()) {
        // Сбрасываем указатели
        for (auto& pair : graph) {
            ptr[pair.first] = 0;
        }

        int pushed;
        while ((pushed = dfs(sourceId, INT_MAX)) > 0) {
            maxFlow += pushed;
        }
    }

    std::cout << "Результат алгоритма: " << maxFlow << std::endl << std::endl;
    return maxFlow;
}

// Вспомогательная функция для добавления ребра в граф
void addEdge(std::unordered_map<int, std::vector<ResidualEdge>>& graph, 
             int from, int to, int capacity) {
    // Прямое ребро
    graph[from].push_back(ResidualEdge(capacity, to, graph[to].size()));
    // Обратное ребро
    graph[to].push_back(ResidualEdge(0, from, graph[from].size() - 1));
}

// 1. ПУСТОЙ ГРАФ (без ребер)
void createEmptyGraph(std::unordered_map<int, std::vector<ResidualEdge>>& graph) {
    for (int i = 1; i <= 3; i++) {
        graph[i] = std::vector<ResidualEdge>();
    }
}

// 2. ГРАФ С 3 ВЕРШИНАМИ
void createGraph3Vertices(std::unordered_map<int, std::vector<ResidualEdge>>& graph) {
    for (int i = 1; i <= 3; i++) {
        graph[i] = std::vector<ResidualEdge>();
    }

    addEdge(graph, 1, 2, 10);
    addEdge(graph, 2, 3, 5);
}

// 3. ТЕСТОВЫЙ ГРАФ ИЗ ЗАДАНИЯ
void createTestGraph(std::unordered_map<int, std::vector<ResidualEdge>>& graph) {
    for (int i = 1; i <= 5; i++) {
        graph[i] = std::vector<ResidualEdge>();
    }

    addEdge(graph, 1, 3, 40);
    addEdge(graph, 1, 2, 30);
    addEdge(graph, 1, 4, 20);
    addEdge(graph, 2, 3, 50);
    addEdge(graph, 3, 4, 20);
    addEdge(graph, 4, 5, 30);
    addEdge(graph, 3, 5, 30);
    addEdge(graph, 2, 5, 40);
}

// 4. ГРАФ С 6 ВЕРШИНАМИ (классический пример)
void createGraph6Vertices(std::unordered_map<int, std::vector<ResidualEdge>>& graph) {
    for (int i = 1; i <= 6; i++) {
        graph[i] = std::vector<ResidualEdge>();
    }

    addEdge(graph, 1, 2, 16);
    addEdge(graph, 1, 3, 13);
    addEdge(graph, 2, 3, 12);
    addEdge(graph, 2, 4, 10);
    addEdge(graph, 3, 2, 9);
    addEdge(graph, 3, 5, 14);
    addEdge(graph, 4, 5, 7);
    addEdge(graph, 4, 6, 4);
    addEdge(graph, 5, 6, 20);
}

// 5. ГРАФ С 10 ВЕРШИНАМИ
void createGraph10Vertices(std::unordered_map<int, std::vector<ResidualEdge>>& graph) {
    for (int i = 1; i <= 10; i++) {
        graph[i] = std::vector<ResidualEdge>();
    }

    addEdge(graph, 1, 2, 20);
    addEdge(graph, 1, 3, 15);
    addEdge(graph, 1, 4, 10);
    addEdge(graph, 2, 5, 25);
    addEdge(graph, 3, 5, 10);
    addEdge(graph, 3, 6, 15);
    addEdge(graph, 4, 6, 20);
    addEdge(graph, 5, 7, 15);
    addEdge(graph, 5, 8, 10);
    addEdge(graph, 6, 8, 20);
    addEdge(graph, 6, 9, 5);
    addEdge(graph, 7, 10, 30);
    addEdge(graph, 8, 10, 20);
    addEdge(graph, 9, 10, 10);
}

// 6. ГРАФ С 15 ВЕРШИНАМИ
void createGraph15Vertices(std::unordered_map<int, std::vector<ResidualEdge>>& graph) {
    for (int i = 1; i <= 15; i++) {
        graph[i] = std::vector<ResidualEdge>();
    }

    addEdge(graph, 1, 2, 50);
    addEdge(graph, 1, 3, 40);
    addEdge(graph, 1, 4, 30);
    addEdge(graph, 1, 5, 20);
    addEdge(graph, 2, 6, 15);
    addEdge(graph, 2, 7, 10);
    addEdge(graph, 3, 7, 20);
    addEdge(graph, 3, 8, 15);
    addEdge(graph, 4, 8, 25);
    addEdge(graph, 4, 9, 10);
    addEdge(graph, 5, 9, 30);
    addEdge(graph, 5, 10, 5);
    addEdge(graph, 6, 11, 40);
    addEdge(graph, 7, 11, 20);
    addEdge(graph, 7, 12, 15);
    addEdge(graph, 8, 12, 25);
    addEdge(graph, 8, 13, 10);
    addEdge(graph, 9, 13, 30);
    addEdge(graph, 9, 14, 5);
    addEdge(graph, 10, 14, 35);
    addEdge(graph, 11, 15, 50);
    addEdge(graph, 12, 15, 45);
    addEdge(graph, 13, 15, 35);
    addEdge(graph, 14, 15, 25);
    addEdge(graph, 12, 8, 5);
    addEdge(graph, 14, 10, 3);
}

// 7. ГРАФ С НУЛЕВОЙ ПРОПУСКНОЙ СПОСОБНОСТЬЮ
void createZeroCapacityGraph(std::unordered_map<int, std::vector<ResidualEdge>>& graph) {
    for (int i = 1; i <= 4; i++) {
        graph[i] = std::vector<ResidualEdge>();
    }

    addEdge(graph, 1, 2, 0);
    addEdge(graph, 2, 3, 0);
    addEdge(graph, 3, 4, 0);
}

// ============ ОСНОВНАЯ ПРОГРАММА С ВСЕМИ ТЕСТАМИ ============
int main() {
    setlocale(LC_ALL, "");
    std::unordered_map<int, std::vector<ResidualEdge>> graph;

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С 3 ВЕРШИНАМИ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph3Vertices(graph);
    dinic(graph, 1, 3);
    graph.clear();

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С 6 ВЕРШИНАМИ (классический)" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph6Vertices(graph);
    dinic(graph, 1, 6);
    graph.clear();

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С 10 ВЕРШИНАМИ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph10Vertices(graph);
    dinic(graph, 1, 10);
    graph.clear();

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С 15 ВЕРШИНАМИ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph15Vertices(graph);
    dinic(graph, 1, 15);
    graph.clear();

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ПУСТОЙ ГРАФ (без ребер)" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createEmptyGraph(graph);
    dinic(graph, 1, 3);
    graph.clear();

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С НУЛЕВОЙ ПРОПУСКНОЙ СПОСОБНОСТЬЮ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createZeroCapacityGraph(graph);
    dinic(graph, 1, 4);
    graph.clear();

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ИСТОЧНИК И СТОК СОВПАДАЮТ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph6Vertices(graph);
    dinic(graph, 1, 1);
    graph.clear();

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: НЕСУЩЕСТВУЮЩАЯ ВЕРШИНА" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph6Vertices(graph);
    dinic(graph, 1, 100);
    graph.clear();

    return 0;
}
