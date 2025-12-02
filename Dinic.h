#pragma once

#include "graph.h"
#include <unordered_map>

int dinic(std::unordered_map<int, Node*>& graph, int sourceId, int sinkId);