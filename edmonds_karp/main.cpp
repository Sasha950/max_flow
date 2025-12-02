// main.cpp - VERSION COMPLÈTE CORRIGÉE AVEC CHOIX DES SOMMETS
#include "edmonds_karp.h"
#include "graph_generator.h"
#include "file_reader.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <limits>

using namespace std;
using namespace chrono;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader(const string& title) {
    clearScreen();
    cout << "\n" << string(70, '=') << endl;
    cout << "  " << title << endl;
    cout << string(70, '=') << endl;
}

void printSection(const string& section) {
    cout << "\n" << string(50, '-') << endl;
    cout << "  " << section << endl;
    cout << string(50, '-') << endl;
}

void displayMainMenu() {
    printHeader("EDMONDS-KARP ALGORITHM - MAXIMUM FLOW");
    
    cout << "1. GENERATE RANDOM GRAPH" << endl;
    cout << "   Create a graph with custom parameters" << endl;
    cout << endl;
    
    cout << "2. GENERATE LAYERED GRAPH" << endl;
    cout << "   Structured organization by levels" << endl;
    cout << endl;
    
    cout << "3. LOAD GRAPH FROM FILE" << endl;
    cout << "   Format: source destination capacity" << endl;
    cout << endl;
    
    cout << "4. VISUALIZE CURRENT GRAPH" << endl;
    cout << "   Display structure, flow and statistics" << endl;
    cout << endl;
    
    cout << "5. PERFORMANCE TEST" << endl;
    cout << "   Compare different graph sizes" << endl;
    cout << endl;
    
    cout << "6. EXIT" << endl;
    cout << "\n" << string(70, '-') << endl;
    cout << "Your choice [1-6]: ";
}

void generateRandomGraphMenu(EdmondsKarp& graph) {
    printHeader("RANDOM GRAPH GENERATION");
    
    int nodes, edges, source, sink;
    bool verbose = false;
    
    cout << "Number of vertices (2-500): ";
    cin >> nodes;
    
    if (nodes < 2 || nodes > 500) {
        cout << "\n[ERROR] Invalid number of vertices!" << endl;
        return;
    }
    
    int maxEdges = nodes * (nodes - 1);
    cout << "Number of edges (1-" << maxEdges << "): ";
    cin >> edges;
    
    if (edges < 1 || edges > maxEdges) {
        cout << "\n[ERROR] Invalid number of edges!" << endl;
        return;
    }
    
    cout << "\n--- Source and Destination Selection ---" << endl;
    cout << "Note: You can choose any vertex from 0 to " << (nodes-1) << endl;
    
    cout << "Source vertex (0-" << (nodes-1) << "): ";
    cin >> source;
    if (source < 0 || source >= nodes) {
        cout << "\n[ERROR] Invalid source vertex!" << endl;
        return;
    }
    
    cout << "Destination vertex (0-" << (nodes-1) << "), different from source: ";
    cin >> sink;
    if (sink < 0 || sink >= nodes || sink == source) {
        cout << "\n[ERROR] Invalid destination vertex!" << endl;
        return;
    }
    
    cout << "\nVerbose mode (1=Yes, 0=No): ";
    cin >> verbose;
    
    printSection("GENERATION IN PROGRESS");
    // Passer source et sink au générateur
    GraphGenerator::generateRandomGraph(graph, nodes, edges, 100, source, sink);
    
    printSection("ANALYSIS OF GENERATED GRAPH");
    graph.displayGraphStructure();
    
    printSection("MAXIMUM FLOW CALCULATION");
    cout << "Source: " << source << ", Destination: " << sink << endl;
    
    auto start = steady_clock::now();
    int flow = graph.maxFlow(source, sink, verbose);
    auto end = steady_clock::now();
    
    auto durationMicro = duration_cast<microseconds>(end - start);
    
    printSection("RESULTS");
    cout << fixed << setprecision(2);
    cout << "Maximum flow: " << flow << " units" << endl;
    cout << "From vertex " << source << " to vertex " << sink << endl;
    
    if (durationMicro.count() > 0) {
        cout << "Computation time: " << durationMicro.count() << " microseconds" << endl;
        
        auto durationMilli = duration_cast<milliseconds>(end - start);
        if (durationMilli.count() > 0) {
            cout << "                  " << durationMilli.count() << " milliseconds" << endl;
        }
        
        cout << "Throughput: " << (flow * 1000000.0 / durationMicro.count()) 
             << " units/second" << endl;
    } else {
        auto durationNano = duration_cast<nanoseconds>(end - start);
        cout << "Computation time: " << durationNano.count() << " nanoseconds" << endl;
        cout << "                  " << (durationNano.count() / 1000.0) << " microseconds" << endl;
        
        if (durationNano.count() > 0) {
            cout << "Throughput: " << (flow * 1000000000.0 / durationNano.count()) 
                 << " units/second" << endl;
        } else {
            cout << "Throughput: Too fast to measure" << endl;
        }
    }
    
    if (verbose) {
        graph.displayFlowNetwork();
    }
    
    graph.displayStatistics();
}

void generateLayeredGraphMenu(EdmondsKarp& graph) {
    printHeader("LAYERED GRAPH GENERATION");
    
    int layers, nodesPerLayer, source, sink;
    bool verbose = false;
    
    cout << "Number of layers (2-10): ";
    cin >> layers;
    
    cout << "Nodes per layer (1-10): ";
    cin >> nodesPerLayer;
    
    int totalNodes = layers * nodesPerLayer + 2;
    
    cout << "\n--- Source and Destination Selection ---" << endl;
    cout << "Note: You can choose any vertex from 0 to " << (totalNodes-1) << endl;
    
    cout << "Source vertex (0-" << (totalNodes-1) << "): ";
    cin >> source;
    if (source < 0 || source >= totalNodes) {
        cout << "\n[ERROR] Invalid source vertex!" << endl;
        return;
    }
    
    cout << "Destination vertex (0-" << (totalNodes-1) << "), different from source: ";
    cin >> sink;
    if (sink < 0 || sink >= totalNodes || sink == source) {
        cout << "\n[ERROR] Invalid destination vertex!" << endl;
        return;
    }
    
    cout << "\nVerbose mode (1=Yes, 0=No): ";
    cin >> verbose;
    
    printSection("GENERATION IN PROGRESS");
    GraphGenerator::generateLayeredGraph(graph, layers, nodesPerLayer, 100, source, sink);
    
    printSection("ANALYSIS OF GENERATED GRAPH");
    graph.displayGraphStructure();
    
    printSection("MAXIMUM FLOW CALCULATION");
    cout << "Source: " << source << ", Destination: " << sink << endl;
    
    auto start = steady_clock::now();
    int flow = graph.maxFlow(source, sink, verbose);
    auto end = steady_clock::now();
    
    auto durationMicro = duration_cast<microseconds>(end - start);

    printSection("RESULTS");
    cout << fixed << setprecision(2);
    cout << "Maximum flow: " << flow << " units" << endl;
    cout << "From vertex " << source << " to vertex " << sink << endl;
    
    if (durationMicro.count() > 0) {
        cout << "Computation time: " << durationMicro.count() << " microseconds" << endl;
        
        cout << "Performance: " << (flow * 1000000.0 / durationMicro.count()) 
             << " units/second" << endl;
    } else {
        auto durationNano = duration_cast<nanoseconds>(end - start);
        cout << "Computation time: " << durationNano.count() << " nanoseconds" << endl;
        
        if (durationNano.count() > 0) {
            cout << "Performance: " << (flow * 1000000000.0 / durationNano.count()) 
                 << " units/second" << endl;
        } else {
            cout << "Performance: Too fast to measure" << endl;
        }
    }
    
    if (verbose) {
        graph.displayFlowNetwork();
    }
    
    graph.displayStatistics();
}

void loadGraphFromFile(EdmondsKarp& graph) {
    printHeader("LOAD GRAPH FROM FILE");
    
    string filename;
    cout << "File name: ";
    cin >> filename;
    
    // VIDER LE GRAPHE AVANT DE CHARGER
    graph.clear();
    
    printSection("LOADING");
    if (FileReader::readFromFile(graph, filename)) {
        cout << "File loaded successfully!" << endl;
        
        // Afficher la structure du graphe
        graph.displayGraphStructure();
        
        int source, sink;
        cout << "\n--- Source and Destination Selection ---" << endl;
        
        // Afficher les sommets disponibles (si la méthode existe)
        // graph.displayAvailableVertices(); // Décommentez si vous avez ajouté cette méthode
        
        cout << "Source vertex: ";
        cin >> source;
        
        // Vérifier si le sommet source existe
        if (graph.getNode(source) == nullptr) {
            cout << "\n[ERROR] Source vertex " << source << " does not exist!" << endl;
            cout << "Check the graph structure above for available vertices." << endl;
            return;
        }
        
        cout << "Destination vertex: ";
        cin >> sink;
        
        // Vérifier si le sommet destination existe
        if (graph.getNode(sink) == nullptr) {
            cout << "\n[ERROR] Destination vertex " << sink << " does not exist!" << endl;
            cout << "Check the graph structure above for available vertices." << endl;
            return;
        }
        
        if (source == sink) {
            cout << "\n[ERROR] Source and destination must be different!" << endl;
            return;
        }
        
        bool verbose;
        cout << "Verbose mode for calculation details (1=Yes, 0=No): ";
        cin >> verbose;
        
        // AFFICHAGE DANS LE FORMAT DEMANDÉ
        cout << "\n" << string(50, '-') << endl;
        cout << " MAXIMUM FLOW CALCULATION" << endl;
        cout << string(50, '-') << endl;
        cout << "Source: " << source << ", Destination: " << sink << endl;
        
        auto start = steady_clock::now();
        int flow = graph.maxFlow(source, sink, verbose);
        auto end = steady_clock::now();
        
        auto durationMicro = duration_cast<microseconds>(end - start);
        auto durationNano = duration_cast<nanoseconds>(end - start);
        
        cout << "\n" << string(50, '-') << endl;
        cout << "  RESULTS" << endl;
        cout << string(50, '-') << endl;
        
        cout << fixed << setprecision(2);
        cout << "Maximum flow: " << flow << " units" << endl;
        cout << "From vertex " << source << " to vertex " << sink << endl;
        
        // Affichage du temps de calcul
        cout << "Computation time: " << durationNano.count() << " nanoseconds" << endl;
        cout << "                  " << (durationNano.count() / 1000.0) << " microseconds" << endl;
        
        // Calcul et affichage du throughput
        if (durationNano.count() > 0) {
            double throughput = (flow * 1000000000.0) / durationNano.count();
            cout << "Throughput: " << throughput << " units/second" << endl;
        } else {
            cout << "Throughput: Too fast to measure" << endl;
        }
        
        // Afficher le réseau de flux si verbose
        if (verbose) {
            graph.displayFlowNetwork();
        }
        
        // Afficher les statistiques dans le format demandé
        cout << "\n" << string(60, '=') << endl;
        cout << "  STATISTICS" << endl;
        cout << string(60, '=') << endl;
        
        // Pour obtenir les degrés, nous avons besoin d'une méthode spécifique
        // Créons une fonction temporaire pour calculer les degrés
        cout << "\nVertex degrees:" << endl;
        
        // Calcul des degrés (code similaire à displayStatistics mais formaté différemment)
        unordered_map<int, int> outDegrees;
        unordered_map<int, int> inDegrees;
        
        // Pour accéder aux nodes, nous avons besoin d'une méthode publique
        // Solution temporaire : afficher un message
        // cout << "  Max outgoing: N/A (add displayAvailableVertices to EdmondsKarp)" << endl;
        // cout << "  Min outgoing: N/A" << endl;
        // cout << "  Max incoming: N/A" << endl;
        // cout << "  Min incoming: N/A" << endl;
        
        cout << string(60, '=') << endl;
        
    } else {
        cout << "[ERROR] Could not load the file!" << endl;
    }
}

void visualizeCurrentGraph(EdmondsKarp& graph) {
    printHeader("CURRENT GRAPH VISUALIZATION");
    
    if (graph.getNumVertices() == 0) {
        cout << "\n[INFO] No graph loaded." << endl;
        cout << "Please generate or load a graph first." << endl;
        return;
    }
    
    int choice;
    cout << "\nVisualization options:" << endl;
    cout << "1. Graph structure" << endl;
    cout << "2. Flow network (current state)" << endl;
    cout << "3. Detailed statistics" << endl;
    cout << "4. Calculate maximum flow with custom vertices" << endl;
    cout << "5. Show everything" << endl;
    cout << "\nYour choice [1-5]: ";
    cin >> choice;
    
    switch (choice) {
        case 1:
            graph.displayGraphStructure();
            break;
        case 2:
            graph.displayFlowNetwork();
            break;
        case 3:
            graph.displayStatistics();
            break;
        case 4:
            {
                int source, sink;
                bool verbose;
                
                cout << "\n--- Source and Destination Selection ---" << endl;
                cout << "Available vertices: 0 to " << (graph.getNumVertices()-1) << endl;
                cout << "Source vertex: ";
                cin >> source;
                
                if (source < 0 || source >= graph.getNumVertices()) {
                    cout << "\n[ERROR] Invalid source vertex!" << endl;
                    return;
                }
                
                cout << "Destination vertex: ";
                cin >> sink;
                
                if (sink < 0 || sink >= graph.getNumVertices()) {
                    cout << "\n[ERROR] Invalid destination vertex!" << endl;
                    return;
                }
                
                if (source == sink) {
                    cout << "\n[ERROR] Source and destination must be different!" << endl;
                    return;
                }
                
                cout << "\nVerbose mode (1=Yes, 0=No): ";
                cin >> verbose;
                
                printSection("MAXIMUM FLOW CALCULATION");
                cout << "Source: " << source << ", Destination: " << sink << endl;
                
                auto start = steady_clock::now();
                int flow = graph.maxFlow(source, sink, verbose);
                auto end = steady_clock::now();
                
                auto duration = duration_cast<microseconds>(end - start);
                
                cout << "\nMaximum flow from " << source << " to " << sink << ": " << flow << " units" << endl;
                cout << "Computation time: ";
                
                if (duration.count() > 1000) {
                    cout << (duration.count() / 1000.0) << " milliseconds" << endl;
                } else {
                    cout << duration.count() << " microseconds" << endl;
                }
                
                if (verbose) {
                    graph.displayFlowNetwork();
                }
                
                graph.displayStatistics();
            }
            break;
        case 5:
            graph.displayGraphStructure();
            graph.displayFlowNetwork();
            graph.displayStatistics();
            break;
        default:
            cout << "[ERROR] Invalid choice!" << endl;
    }
}

void performanceTest() {
    printHeader("PERFORMANCE TEST");
    
    cout << "This test generates several graphs of different sizes" << endl;
    cout << "and measures the algorithm's execution time." << endl;
    cout << "NOTE: Use larger graphs for meaningful time measurements." << endl;
    
    vector<pair<int, int>> testCases = {
        {20, 50},     // Très petit
        {50, 200},    // Petit
        {100, 500},   // Moyen
        {200, 1000},  // Grand
        {300, 1500}   // Très grand
    };
    
    int source, sink;
    cout << "\n--- Test Configuration ---" << endl;
    cout << "Choose source vertex for all tests: ";
    cin >> source;
    
    cout << "Choose destination vertex for all tests: ";
    cin >> sink;
    
    if (source == sink) {
        cout << "\n[ERROR] Source and destination must be different!" << endl;
        cout << "Using default values (0 and n-1)" << endl;
        source = 0;
        sink = -1; // Utiliser -1 pour indiquer qu'on utilisera n-1
    }
    
    cout << "\n" << string(80, '-') << endl;
    cout << setw(10) << "Vertices" 
         << setw(10) << "Edges" 
         << setw(12) << "Source"
         << setw(12) << "Destination"
         << setw(15) << "Max Flow"
         << setw(15) << "Time (µs)"
         << setw(15) << "Throughput" << endl;
    cout << string(80, '-') << endl;
    
    for (const auto& test : testCases) {
        int nodes = test.first;
        int edges = test.second;
        
        EdmondsKarp graph;
        
        // Déterminer la destination
        int actualSink = sink;
        if (sink == -1 || sink >= nodes) {
            actualSink = nodes - 1;
        }
        
        int actualSource = source;
        if (source >= nodes) {
            actualSource = 0;
        }
        
        if (actualSink == actualSource) {
            actualSink = (actualSource + 1) % nodes;
        }
        
        // Générer le graphe
        GraphGenerator::generateRandomGraph(graph, nodes, edges, 100, actualSource, actualSink);
        
        long long totalTime = 0;
        int repetitions = 3;
        int flow = 0;
        
        for (int i = 0; i < repetitions; i++) {
            auto start = steady_clock::now();
            flow = graph.maxFlow(actualSource, actualSink, false);
            auto end = steady_clock::now();
            totalTime += duration_cast<microseconds>(end - start).count();
        }
        
        long long avgTimeMicro = totalTime / repetitions;
        
        double throughput = 0;
        if (avgTimeMicro > 0) {
            throughput = (flow * 1000000.0) / avgTimeMicro;
        }
        
        cout << setw(10) << nodes 
             << setw(10) << edges 
             << setw(12) << actualSource
             << setw(12) << actualSink
             << setw(15) << flow 
             << setw(15) << avgTimeMicro
             << fixed << setprecision(0) << setw(15) << throughput << endl;
    }
    
    cout << string(80, '-') << endl;
    cout << "\n[INFO] µs = microseconds" << endl;
    cout << "[INFO] Test completed. Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void showWelcomeMessage() {
    clearScreen();
    cout << "\n" << string(70, '=') << endl;
    cout << "  WELCOME TO MAXIMUM FLOW ANALYZER" << endl;
    cout << "  Edmonds-Karp Algorithm" << endl;
    cout << string(70, '=') << endl;
    
    cout << "\nThis tool allows you to:" << endl;
    cout << "1. Generate graphs of different sizes" << endl;
    cout << "2. Calculate maximum flow between any two vertices" << endl;
    cout << "3. Visualize network structure" << endl;
    cout << "4. Analyze algorithm performance" << endl;
    
    cout << "\nNew feature: You can now choose any source and destination vertices!" << endl;
    
    cout << "\n" << string(70, '-') << endl;
    cout << "Press Enter to start...";
    cin.ignore();
    cin.get();
}

int main() {
    GraphGenerator::seedRandom();
    EdmondsKarp graph;
    
    showWelcomeMessage();
    
    int choice;
    do {
        displayMainMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                generateRandomGraphMenu(graph);
                break;
            case 2:
                generateLayeredGraphMenu(graph);
                break;
            case 3:
                loadGraphFromFile(graph);
                break;
            case 4:
                visualizeCurrentGraph(graph);
                break;
            case 5:
                performanceTest();
                break;
            case 6:
                printHeader("GOODBYE");
                cout << "\nThank you for using the flow analyzer!" << endl;
                cout << "Developed as part of C++ project" << endl;
                cout << string(70, '=') << endl;
                break;
            default:
                cout << "\n[ERROR] Invalid choice! Please try again." << endl;
        }
        
        if (choice != 6) {
            cout << "\n" << string(70, '-') << endl;
            cout << "Press Enter to return to menu...";
            cin.ignore();
            cin.get();
        }
        
    } while (choice != 6);
    
    return 0;
}