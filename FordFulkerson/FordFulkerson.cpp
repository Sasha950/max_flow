#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <clocale>

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

struct ResidualEdge {
    Node* from;
    Node* to;
    int capacity;
    Edge* originalEdge;
    bool isForward;
};

int fordFulkerson(std::unordered_map<int, Node*>& graph, int sourceId, int sinkId) {
    if (graph.empty()) {
        std::cout << "Граф пустой" << std::endl;
        return 0;
    }

    if (graph.find(sourceId) == graph.end()) {
        std::cout << "Вершина-источник " << sourceId << " не найдена" << std::endl;
        return 0;
    }

    if (graph.find(sinkId) == graph.end()) {
        std::cout << "Вершина-сток " << sinkId << " не найдена" << std::endl;
        return 0;
    }

    if (sourceId == sinkId) {
        std::cout << "Источник и сток - одна и та же вершина" << std::endl;
        return 0;
    }

    Node* source = graph[sourceId];
    Node* sink = graph[sinkId];

    std::unordered_map<Node*, std::vector<ResidualEdge>> residualGraph;

    for (auto& pair : graph) {
        Node* u = pair.second;

        for (Edge* edge : u->edges) {
            Node* v = edge->adjacentNode;

            ResidualEdge forward;
            forward.from = u;
            forward.to = v;
            forward.capacity = edge->weight;
            forward.originalEdge = edge;
            forward.isForward = true;

            residualGraph[u].push_back(forward);

            ResidualEdge backward;
            backward.from = v;
            backward.to = u;
            backward.capacity = 0;
            backward.originalEdge = nullptr;
            backward.isForward = false;

            residualGraph[v].push_back(backward);
        }
    }

    int maxFlow = 0;

    while (true) {
        std::queue<Node*> q;
        std::unordered_map<Node*, bool> visited;
        std::unordered_map<Node*, ResidualEdge*> parent;

        q.push(source);
        visited[source] = true;

        bool foundPath = false;

        while (!q.empty()) {
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
            pathFlow = std::min(pathFlow, re->capacity);
            v = re->from;
        }

        v = sink;
        while (v != source) {
            ResidualEdge* re = parent[v];

            re->capacity -= pathFlow;

            Node* u = re->from;

            for (ResidualEdge& reverseRe : residualGraph[v]) {
                if (reverseRe.to == u) {
                    reverseRe.capacity += pathFlow;
                    break;
                }
            }

            v = u;
        }

        maxFlow += pathFlow;
    }

    return maxFlow;
}

// ============ ТЕСТОВЫЕ ГРАФЫ ============

// 1. ПУСТОЙ ГРАФ (без ребер)
void createEmptyGraph(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 3; i++) {
        graph[i] = new Node(i);
    }
    // Никаких ребер не добавляем!
}

// 2. ОЧЕНЬ МАЛЕНЬКИЙ ГРАФ (2-3 вершины)
void createVerySmallGraph(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 3; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(10, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(5, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();
}

// 3. МАЛЕНЬКИЙ ГРАФ (классический пример)
void createSmallGraph(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 6; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(16, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(13, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(12, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();

    graph[2]->edges.push_back(new Edge(10, graph[4]));
    graph[4]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(9, graph[2]));
    graph[2]->parents[graph[3]] = graph[3]->edges.back();

    graph[3]->edges.push_back(new Edge(14, graph[5]));
    graph[5]->parents[graph[3]] = graph[3]->edges.back();

    graph[4]->edges.push_back(new Edge(7, graph[5]));
    graph[5]->parents[graph[4]] = graph[4]->edges.back();

    graph[4]->edges.push_back(new Edge(4, graph[6]));
    graph[6]->parents[graph[4]] = graph[4]->edges.back();

    graph[5]->edges.push_back(new Edge(20, graph[6]));
    graph[6]->parents[graph[5]] = graph[5]->edges.back();
}

// 4. СРЕДНИЙ ГРАФ (8-10 вершин)
void createMediumGraph(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 10; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(20, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(15, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(10, graph[4]));
    graph[4]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(25, graph[5]));
    graph[5]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(10, graph[5]));
    graph[5]->parents[graph[3]] = graph[3]->edges.back();

    graph[3]->edges.push_back(new Edge(15, graph[6]));
    graph[6]->parents[graph[3]] = graph[3]->edges.back();

    graph[4]->edges.push_back(new Edge(20, graph[6]));
    graph[6]->parents[graph[4]] = graph[4]->edges.back();

    graph[5]->edges.push_back(new Edge(15, graph[7]));
    graph[7]->parents[graph[5]] = graph[5]->edges.back();

    graph[5]->edges.push_back(new Edge(10, graph[8]));
    graph[8]->parents[graph[5]] = graph[5]->edges.back();

    graph[6]->edges.push_back(new Edge(20, graph[8]));
    graph[8]->parents[graph[6]] = graph[6]->edges.back();

    graph[6]->edges.push_back(new Edge(5, graph[9]));
    graph[9]->parents[graph[6]] = graph[6]->edges.back();

    graph[7]->edges.push_back(new Edge(30, graph[10]));
    graph[10]->parents[graph[7]] = graph[7]->edges.back();

    graph[8]->edges.push_back(new Edge(20, graph[10]));
    graph[10]->parents[graph[8]] = graph[8]->edges.back();

    graph[9]->edges.push_back(new Edge(10, graph[10]));
    graph[10]->parents[graph[9]] = graph[9]->edges.back();
}

// 5. БОЛЬШОЙ ГРАФ (15 вершин)
void createLargeGraph(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 15; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(50, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(40, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(30, graph[4]));
    graph[4]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(20, graph[5]));
    graph[5]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(15, graph[6]));
    graph[6]->parents[graph[2]] = graph[2]->edges.back();

    graph[2]->edges.push_back(new Edge(10, graph[7]));
    graph[7]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(20, graph[7]));
    graph[7]->parents[graph[3]] = graph[3]->edges.back();

    graph[3]->edges.push_back(new Edge(15, graph[8]));
    graph[8]->parents[graph[3]] = graph[3]->edges.back();

    graph[4]->edges.push_back(new Edge(25, graph[8]));
    graph[8]->parents[graph[4]] = graph[4]->edges.back();

    graph[4]->edges.push_back(new Edge(10, graph[9]));
    graph[9]->parents[graph[4]] = graph[4]->edges.back();

    graph[5]->edges.push_back(new Edge(30, graph[9]));
    graph[9]->parents[graph[5]] = graph[5]->edges.back();

    graph[5]->edges.push_back(new Edge(5, graph[10]));
    graph[10]->parents[graph[5]] = graph[5]->edges.back();

    graph[6]->edges.push_back(new Edge(40, graph[11]));
    graph[11]->parents[graph[6]] = graph[6]->edges.back();

    graph[7]->edges.push_back(new Edge(20, graph[11]));
    graph[11]->parents[graph[7]] = graph[7]->edges.back();

    graph[7]->edges.push_back(new Edge(15, graph[12]));
    graph[12]->parents[graph[7]] = graph[7]->edges.back();

    graph[8]->edges.push_back(new Edge(25, graph[12]));
    graph[12]->parents[graph[8]] = graph[8]->edges.back();

    graph[8]->edges.push_back(new Edge(10, graph[13]));
    graph[13]->parents[graph[8]] = graph[8]->edges.back();

    graph[9]->edges.push_back(new Edge(30, graph[13]));
    graph[13]->parents[graph[9]] = graph[9]->edges.back();

    graph[9]->edges.push_back(new Edge(5, graph[14]));
    graph[14]->parents[graph[9]] = graph[9]->edges.back();

    graph[10]->edges.push_back(new Edge(35, graph[14]));
    graph[14]->parents[graph[10]] = graph[10]->edges.back();

    graph[11]->edges.push_back(new Edge(50, graph[15]));
    graph[15]->parents[graph[11]] = graph[11]->edges.back();

    graph[12]->edges.push_back(new Edge(45, graph[15]));
    graph[15]->parents[graph[12]] = graph[12]->edges.back();

    graph[13]->edges.push_back(new Edge(35, graph[15]));
    graph[15]->parents[graph[13]] = graph[13]->edges.back();

    graph[14]->edges.push_back(new Edge(25, graph[15]));
    graph[15]->parents[graph[14]] = graph[14]->edges.back();

    graph[12]->edges.push_back(new Edge(5, graph[8]));
    graph[8]->parents[graph[12]] = graph[12]->edges.back();

    graph[14]->edges.push_back(new Edge(3, graph[10]));
    graph[10]->parents[graph[14]] = graph[14]->edges.back();
}

// 6. ГРАФ С НУЛЕВОЙ ПРОПУСКНОЙ СПОСОБНОСТЬЮ
void createZeroCapacityGraph(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 4; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(0, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(0, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(0, graph[4]));
    graph[4]->parents[graph[3]] = graph[3]->edges.back();
}


void cleanupGraph(std::unordered_map<int, Node*>& graph) {
    for (auto& pair : graph) {
        for (Edge* edge : pair.second->edges) {
            delete edge;
        }
        delete pair.second;
    }
    graph.clear();
}

void printGraphInfo(std::unordered_map<int, Node*>& graph, const std::string& name) {
    int vertices = graph.size();
    int edges = 0;

    for (auto& pair : graph) {
        edges += pair.second->edges.size();
    }

    std::cout << "\n" << name << ":" << std::endl;
    std::cout << "  Вершин: " << vertices << std::endl;
    std::cout << "  Ребер: " << edges << std::endl;
}

void runTest(std::unordered_map<int, Node*>& graph, const std::string& testName,
    int sourceId, int sinkId, int expected = -1) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "ТЕСТ: " << testName << std::endl;
    std::cout << "========================================" << std::endl;

    printGraphInfo(graph, "Характеристики графа");

    std::cout << "Источник: " << sourceId << ", Сток: " << sinkId << std::endl;

    int result = fordFulkerson(graph, sourceId, sinkId);

    std::cout << "Результат алгоритма: " << result << std::endl;;
}

int main() {
    setlocale(LC_ALL, "");

    std::cout << "ТЕСТИРОВАНИЕ АЛГОРИТМА ФОРДА-ФАЛКЕРСОНА" << std::endl;
    std::cout << "========================================" << std::endl;

    // Тест 1: Пустой граф (без ребер)
    {
        std::unordered_map<int, Node*> graph;
        createEmptyGraph(graph);
        runTest(graph, "ПУСТОЙ ГРАФ (без ребер)", 1, 3, 0);
        cleanupGraph(graph);
    }

    // Тест 2: Очень маленький граф
    {
        std::unordered_map<int, Node*> graph;
        createVerySmallGraph(graph);
        runTest(graph, "ОЧЕНЬ МАЛЕНЬКИЙ ГРАФ (3 вершины)", 1, 3, 5);
        cleanupGraph(graph);
    }

    // Тест 3: Граф с нулевой пропускной способностью
    {
        std::unordered_map<int, Node*> graph;
        createZeroCapacityGraph(graph);
        runTest(graph, "ГРАФ С НУЛЕВОЙ ПРОПУСКНОЙ СПОСОБНОСТЬЮ", 1, 4, 0);
        cleanupGraph(graph);
    }

    // Тест 4: Маленький граф (классический пример)
    {
        std::unordered_map<int, Node*> graph;
        createSmallGraph(graph);
        runTest(graph, "МАЛЕНЬКИЙ ГРАФ (6 вершин, классический)", 1, 6, 23);
        cleanupGraph(graph);
    }

    // Тест 5: Средний граф
    {
        std::unordered_map<int, Node*> graph;
        createMediumGraph(graph);
        runTest(graph, "СРЕДНИЙ ГРАФ (10 вершин)", 1, 10, -1); // Ожидаемое значение можно вычислить
        cleanupGraph(graph);
    }

    // Тест 6: Большой граф
    {
        std::unordered_map<int, Node*> graph;
        createLargeGraph(graph);
        runTest(graph, "БОЛЬШОЙ ГРАФ (15 вершин)", 1, 15, -1); // Сложный для ручного вычисления
        cleanupGraph(graph);
    }

    // Тест 7: Источник и сток совпадают
    {
        std::unordered_map<int, Node*> graph;
        createSmallGraph(graph);
        runTest(graph, "ИСТОЧНИК И СТОК СОВПАДАЮТ", 1, 1, 0);
        cleanupGraph(graph);
    }

    // Тест 8: Несуществующая вершина
    {
        std::unordered_map<int, Node*> graph;
        createSmallGraph(graph);
        runTest(graph, "НЕСУЩЕСТВУЮЩАЯ ВЕРШИНА", 1, 100, 0);
        cleanupGraph(graph);
    }

    // Тест 10: Полный граф K4

    std::cout << "\n========================================" << std::endl;
    std::cout << "ТЕСТИРОВАНИЕ ЗАВЕРШЕНО" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}