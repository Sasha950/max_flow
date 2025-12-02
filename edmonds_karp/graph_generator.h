// graph_generator.h - ADAPTÉ POUR VOTRE STRUCTURE AVEC CHOIX DES SOMMETS
#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include "edmonds_karp.h"
#include <random>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <set>

using namespace std;

class GraphGenerator {
private:
    static mt19937 rng;
    
public:
    static void seedRandom() {
        rng.seed(time(nullptr));
    }
    
    static void generateRandomGraph(EdmondsKarp& graph, int nodes, int edges, 
                                   int maxCapacity = 100, int source = 0, int sink = -1) {
        if (nodes < 2) {
            cout << "[ERROR] Need at least 2 vertices!" << endl;
            return;
        }
        
        if (sink == -1) {
            sink = nodes - 1; // Valeur par défaut
        }
        
        if (source < 0 || source >= nodes || sink < 0 || sink >= nodes || source == sink) {
            cout << "[ERROR] Invalid source or sink vertices!" << endl;
            cout << "Source: " << source << ", Sink: " << sink << endl;
            cout << "Valid range: 0 to " << (nodes-1) << endl;
            return;
        }
        
        graph.clear();
        
        cout << "\n[INFO] Generating random graph with your improved structure..." << endl;
        cout << "[INFO] Source: " << source << ", Destination: " << sink << endl;
        cout << "[INFO] Nodes: " << nodes << ", Edges: " << edges << endl;
        
        uniform_int_distribution<int> nodeDist(0, nodes-1);
        uniform_int_distribution<int> capDist(1, maxCapacity);
        
        set<pair<int, int>> addedEdges;
        int edgesAdded = 0;
        int attempts = 0;
        const int MAX_ATTEMPTS = edges * 3;
        
        while (edgesAdded < edges && attempts < MAX_ATTEMPTS) {
            int from = nodeDist(rng);
            int to = nodeDist(rng);
            
            if (from != to && addedEdges.find({from, to}) == addedEdges.end()) {
                int capacity = capDist(rng);
                graph.addEdge(from, to, capacity);
                addedEdges.insert({from, to});
                edgesAdded++;
            }
            
            attempts++;
        }
        
        // Assurer connectivité source-sink
        if (addedEdges.find({source, sink}) == addedEdges.end()) {
            cout << "[INFO] Adding connectivity between source and sink..." << endl;
            
            // Ajouter un chemin direct
            int capacity = capDist(rng);
            graph.addEdge(source, sink, capacity);
            addedEdges.insert({source, sink});
            edgesAdded++;
            
            // Ajouter aussi des chemins indirects pour plus de robustesse
            int pathsToAdd = min(3, nodes - 2);
            for (int i = 0; i < pathsToAdd; i++) {
                int intermediate = nodeDist(rng);
                while (intermediate == source || intermediate == sink) {
                    intermediate = nodeDist(rng);
                }
                
                // S'assurer que les arêtes n'existent pas déjà
                if (addedEdges.find({source, intermediate}) == addedEdges.end()) {
                    graph.addEdge(source, intermediate, capDist(rng));
                    addedEdges.insert({source, intermediate});
                    edgesAdded++;
                }
                
                if (addedEdges.find({intermediate, sink}) == addedEdges.end()) {
                    graph.addEdge(intermediate, sink, capDist(rng));
                    addedEdges.insert({intermediate, sink});
                    edgesAdded++;
                }
            }
        }
        
        cout << "[SUCCESS] Generated " << edgesAdded << " edges" << endl;
    }
    
    static void generateLayeredGraph(EdmondsKarp& graph, int layers, int nodesPerLayer, 
                                    int maxCapacity = 100, int source = -1, int sink = -1) {
        if (layers < 2 || nodesPerLayer < 1) {
            cout << "[ERROR] Invalid parameters!" << endl;
            return;
        }
        
        graph.clear();
        
        uniform_int_distribution<int> capDist(1, maxCapacity);
        int totalNodes = layers * nodesPerLayer + 2;
        
        // Déterminer source et sink
        int actualSource = (source == -1 || source >= totalNodes) ? 0 : source;
        int actualSink = (sink == -1 || sink >= totalNodes) ? totalNodes - 1 : sink;
        
        if (actualSource == actualSink) {
            cout << "[ERROR] Source and sink must be different!" << endl;
            actualSink = (actualSource + 1) % totalNodes;
        }
        
        cout << "\n[INFO] Generating layered graph..." << endl;
        cout << "[INFO] Layers: " << layers << ", Nodes per layer: " << nodesPerLayer << endl;
        cout << "[INFO] Total nodes: " << totalNodes << endl;
        cout << "[INFO] Source: " << actualSource << ", Destination: " << actualSink << endl;
        
        // Source vers première couche
        for (int i = 0; i < nodesPerLayer; i++) {
            int target = i + 1;
            if (target != actualSource) { // Éviter les boucles
                graph.addEdge(actualSource, target, capDist(rng));
            }
        }
        
        // Connexions entre couches
        for (int layer = 0; layer < layers - 1; layer++) {
            int startCurrent = layer * nodesPerLayer + 1;
            int startNext = (layer + 1) * nodesPerLayer + 1;
            
            for (int i = 0; i < nodesPerLayer; i++) {
                int from = startCurrent + i;
                
                // Ne pas créer d'arête si from est le sink
                if (from == actualSink) continue;
                
                int connections = 1 + (rng() % 3);
                for (int c = 0; c < connections; c++) {
                    int target = startNext + (rng() % nodesPerLayer);
                    
                    // Ne pas créer d'arête si target est la source ou si from == target
                    if (target != actualSource && target != from) {
                        graph.addEdge(from, target, capDist(rng));
                    }
                }
            }
        }
        
        // Dernière couche vers sink
        int lastLayerStart = (layers - 1) * nodesPerLayer + 1;
        for (int i = 0; i < nodesPerLayer; i++) {
            int from = lastLayerStart + i;
            
            // Ne pas créer d'arête si from est le sink
            if (from != actualSink) {
                graph.addEdge(from, actualSink, capDist(rng));
            }
        }
        
        // S'assurer qu'il y a un chemin de source à sink
        // Vérifier si source a des arêtes sortantes
        bool sourceHasEdges = false;
        bool sinkHasEdges = false;
        
        // On peut ajouter des arêtes supplémentaires si nécessaire
        if (!sourceHasEdges && actualSource != actualSink) {
            // Ajouter une arête directe si possible
            if (actualSource < totalNodes && actualSink < totalNodes) {
                graph.addEdge(actualSource, actualSink, capDist(rng));
            }
        }
        
        cout << "[SUCCESS] Layered graph generated with " << totalNodes << " vertices" << endl;
    }
};

mt19937 GraphGenerator::rng(random_device{}());

#endif // GRAPH_GENERATOR_H