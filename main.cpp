#include "graph.h"
#include "Dinic.h"
#include "edmonds_karp.h"
#include "FordFulkerson.h"
#include "Push-Relabel.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <chrono>
#include <iomanip>
#include <clocale>

using namespace std;

// Функция очистки графа
void cleanupGraph(unordered_map<int, Node*>& graph) {
    for (auto& pair : graph) {
        for (Edge* edge : pair.second->edges) {
            delete edge;
        }
        delete pair.second;
    }
    graph.clear();
}

// ============ ТЕСТОВЫЕ ГРАФЫ ============

// 1. ПУСТОЙ ГРАФ (без ребер)
void createEmptyGraph(unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 3; i++) {
        graph[i] = new Node(i);
    }
}

// 2. ОЧЕНЬ МАЛЕНЬКИЙ ГРАФ (2-3 вершины)
void createVerySmallGraph(unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 3; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(10, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(5, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();
}

// 3. МАЛЕНЬКИЙ ГРАФ (классический пример)
void createSmallGraph(unordered_map<int, Node*>& graph) {
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
void createMediumGraph(unordered_map<int, Node*>& graph) {
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
void createLargeGraph(unordered_map<int, Node*>& graph) {
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
}

// 6. ГРАФ С НУЛЕВОЙ ПРОПУСКНОЙ СПОСОБНОСТЬЮ
void createZeroCapacityGraph(unordered_map<int, Node*>& graph) {
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

// Функция для подсчета характеристик графа
void printGraphInfo(unordered_map<int, Node*>& graph, const string& name) {
    int vertices = graph.size();
    int edges = 0;

    for (auto& pair : graph) {
        edges += pair.second->edges.size();
    }

    cout << "\n" << name << ":" << endl;
    cout << "  Вершин: " << vertices << endl;
    cout << "  Ребер: " << edges << endl;
}

// Функция для запуска теста одного алгоритма
void runSingleTest(unordered_map<int, Node*>& graph,
    const string& algorithmName,
    int (*algorithm)(unordered_map<int, Node*>&, int, int),
    int sourceId, int sinkId) {

    auto start = chrono::high_resolution_clock::now();
    int result = algorithm(graph, sourceId, sinkId);
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    cout << "  " << algorithmName << ": " << result
        << " (время: " << duration.count() << " мкс)" << endl;
}

// Обертка для edmondsKarp без verbose параметра
int edmondsKarpSimple(unordered_map<int, Node*>& graph, int sourceId, int sinkId) {
    return edmondsKarp(graph, sourceId, sinkId, false);
}

// Основная функция тестирования
void runTestSuite(const string& testName,
    void (*createGraphFunc)(unordered_map<int, Node*>&),
    int sourceId, int sinkId,
    int expected = -1) {

    cout << "\n" << string(60, '=') << endl;
    cout << "ТЕСТ: " << testName << endl;
    cout << string(60, '=') << endl;

    unordered_map<int, Node*> graph;
    createGraphFunc(graph);
    printGraphInfo(graph, "Характеристики графа");
    cout << "Источник: " << sourceId << ", Сток: " << sinkId << endl;

    if (expected != -1) {
        cout << "Ожидаемый результат: " << expected << endl;
    }

    cout << "\nРезультаты алгоритмов:" << endl;

    // Тестируем все четыре алгоритма
    runSingleTest(graph, "Форд-Фалкерсон", fordFulkerson, sourceId, sinkId);
    runSingleTest(graph, "Эдмондс-Карп", edmondsKarpSimple, sourceId, sinkId);
    runSingleTest(graph, "Диниц", dinic, sourceId, sinkId);
    runSingleTest(graph, "Проталкивание предпотока", pushRelabel, sourceId, sinkId);

    cleanupGraph(graph);
}

int main() {
    setlocale(LC_ALL, "");
    cout << "ТЕСТИРОВАНИЕ АЛГОРИТМОВ ПОИСКА МАКСИМАЛЬНОГО ПОТОКА" << endl;
    cout << "=====================================================" << endl;
    cout << "Тестируются 4 алгоритма:" << endl;
    cout << "1. Форд-Фалкерсон" << endl;
    cout << "2. Эдмондс-Карп" << endl;
    cout << "3. Диниц" << endl;
    cout << "4. Проталкивание предпотока (Push-Relabel)" << endl;

    // Тест 1: Пустой граф
    runTestSuite("ПУСТОЙ ГРАФ (без ребер)", createEmptyGraph, 1, 3, 0);

    // Тест 2: Очень маленький граф
    runTestSuite("ОЧЕНЬ МАЛЕНЬКИЙ ГРАФ (3 вершины)", createVerySmallGraph, 1, 3, 5);

    // Тест 3: Граф с нулевой пропускной способностью
    runTestSuite("ГРАФ С НУЛЕВОЙ ПРОПУСКНОЙ СПОСОБНОСТЬЮ", createZeroCapacityGraph, 1, 4, 0);

    // Тест 4: Маленький граф (классический пример)
    runTestSuite("МАЛЕНЬКИЙ ГРАФ (6 вершин, классический)", createSmallGraph, 1, 6, 23);

    // Тест 5: Средний граф
    runTestSuite("СРЕДНИЙ ГРАФ (10 вершин)", createMediumGraph, 1, 10, -1);

    // Тест 6: Большой граф
    runTestSuite("БОЛЬШОЙ ГРАФ (15 вершин)", createLargeGraph, 1, 15, -1);

    // Тест 7: Краевые случаи
    cout << "\n" << string(60, '=') << endl;
    cout << "КРАЕВЫЕ СЛУЧАИ" << endl;
    cout << string(60, '=') << endl;

    // 7.1: Источник и сток совпадают
    {
        cout << "\nИсточник и сток совпадают:" << endl;
        unordered_map<int, Node*> graph;
        createSmallGraph(graph);
        cout << "  Форд-Фалкерсон: " << fordFulkerson(graph, 1, 1) << endl;
        cout << "  Эдмондс-Карп: " << edmondsKarp(graph, 1, 1, false) << endl;
        cout << "  Диниц: " << dinic(graph, 1, 1) << endl;
        cout << "  Проталкивание предпотока: " << pushRelabel(graph, 1, 1) << endl;
        cleanupGraph(graph);
    }

    // 7.2: Несуществующая вершина
    {
        cout << "\nНесуществующая вершина-сток:" << endl;
        unordered_map<int, Node*> graph;
        createSmallGraph(graph);
        cout << "  Форд-Фалкерсон: " << fordFulkerson(graph, 1, 100) << endl;
        cout << "  Эдмондс-Карп: " << edmondsKarp(graph, 1, 100, false) << endl;
        cout << "  Диниц: " << dinic(graph, 1, 100) << endl;
        cout << "  Проталкивание предпотока: " << pushRelabel(graph, 1, 100) << endl;
        cleanupGraph(graph);
    }

    cout << "\n" << string(60, '=') << endl;
    cout << "ТЕСТИРОВАНИЕ ЗАВЕРШЕНО" << endl;
    cout << string(60, '=') << endl;

    return 0;
}