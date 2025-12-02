// file_reader.h - ADAPTÉ POUR VOTRE STRUCTURE
#ifndef FILE_READER_H
#define FILE_READER_H

#include "edmonds_karp.h"
#include <fstream>
#include <sstream>
#include <iostream>

class FileReader {
public:
    static bool readFromFile(EdmondsKarp& graph, const std::string& filename) {
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "[ERROR] Cannot open file " << filename << std::endl;
            return false;
        }
        
        std::cout << "[DEBUG] Opening file: " << filename << std::endl;
        
        std::string line;
        int lineNumber = 0;
        int edgesLoaded = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            if (line.empty() || line[0] == '#') {
                std::cout << "[DEBUG] Line " << lineNumber << ": Comment/empty" << std::endl;
                continue;
            }
            
            std::istringstream iss(line);
            int from, to, capacity;
            
            if (iss >> from >> to >> capacity) {
                std::cout << "[DEBUG] Loading edge: " << from << " -> " << to 
                          << " (cap: " << capacity << ")" << std::endl;
                graph.addEdge(from, to, capacity);
                edgesLoaded++;
            } else {
                std::cerr << "[ERROR] Line " << lineNumber << ": Invalid format" << std::endl;
                std::cerr << "[ERROR] Content: '" << line << "'" << std::endl;
            }
        }
        
        file.close();
        std::cout << "[INFO] Loaded " << edgesLoaded << " edges from " 
                  << lineNumber << " lines" << std::endl;
        return true;
    }
};

#endif // FILE_READER_H