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

	std::cout << "Максимальный поток от вершины " << sourceId << " до вершины " << sinkId << ": " << maxFlow << std::endl;
	return maxFlow;
}

void createTestGraph(std::unordered_map<int, Node*>& graph) {
	for (int i = 1; i <= 5; i++) {
		graph[i] = new Node(i);
	}

	graph[1]->edges.push_back(new Edge(40, graph[3]));
	graph[3]->parents[graph[1]] = graph[1]->edges.back();

	graph[1]->edges.push_back(new Edge(30, graph[2]));
	graph[2]->parents[graph[1]] = graph[1]->edges.back();

	graph[1]->edges.push_back(new Edge(20, graph[4]));
	graph[4]->parents[graph[1]] = graph[1]->edges.back();

	graph[2]->edges.push_back(new Edge(50, graph[3]));
	graph[3]->parents[graph[2]] = graph[2]->edges.back();

	graph[3]->edges.push_back(new Edge(20, graph[4]));
	graph[4]->parents[graph[3]] = graph[3]->edges.back();

	graph[4]->edges.push_back(new Edge(30, graph[5]));
	graph[5]->parents[graph[4]] = graph[4]->edges.back();

	graph[3]->edges.push_back(new Edge(30, graph[5]));
	graph[5]->parents[graph[3]] = graph[3]->edges.back();

	graph[2]->edges.push_back(new Edge(40, graph[5]));
	graph[5]->parents[graph[2]] = graph[2]->edges.back();
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

int main() {
	setlocale(LC_ALL, "");
	std::unordered_map<int, Node*> graph;
	createTestGraph(graph);
	fordFulkerson(graph, 5, 1);
	cleanupGraph(graph);
	return 0;
}