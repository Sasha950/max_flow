#pragma once

#include "graph.h"
#include <unordered_map>

int pushRelabel(std::unordered_map<int, Node*>& graph, int sourceId, int sinkId);