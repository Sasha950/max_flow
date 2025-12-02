#pragma once

#include "graph.h"
#include <unordered_map>

int edmondsKarp(std::unordered_map<int, Node*>& graph, int sourceId, int sinkId, bool verbose = false);