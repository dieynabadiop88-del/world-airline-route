#include "graph.h"
#include <fstream>
#include <sstream>

Graph::Graph() {
    cityCount = 0;
    for (int i = 0; i < MAX_CITIES; i++) {
        for (int j = 0; j < MAX_CITIES; j++) {
            adj[i][j] = 0;
        }
    }
}

void Graph::addCity(const char* name) {
    if (getCityIndex(name) != -1) return;
    strncpy_s(cityNames[cityCount], name, MAX_NAME_LEN - 1);
    cityNames[cityCount][MAX_NAME_LEN - 1] = '\0';
    cityCount++;
}

int Graph::getCityIndex(const char* name) {
    for (int i = 0; i < cityCount; i++) {
        if (strcmp(cityNames[i], name) == 0) return i;
    }
    return -1;
}

void Graph::addEdge(const char* src, const char* dest) {
    int u = getCityIndex(src);
    int v = getCityIndex(dest);
    if (u == -1 || v == -1) return;
    adj[u][v] = 1;
}

void Graph::loadFromFile(const char* filename) {
    std::ifstream fin(filename);
    if (!fin) {
        std::cout << "Error loading file\n";
        return;
    }

    std::string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        char src[MAX_NAME_LEN];
        ss >> src;
        addCity(src);

        char dest[MAX_NAME_LEN];
        while (ss >> dest) {
            addCity(dest);
            addEdge(src, dest);
        }
    }
}

void Graph::BFS(int src, int dist[], int parent[]) {
    bool visited[MAX_CITIES] = { false };
    for (int i = 0; i < cityCount; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    int q[MAX_CITIES];
    int front = 0, rear = 0;

    visited[src] = true;
    dist[src] = 0;
    q[rear++] = src;

    while (front < rear) {
        int u = q[front++];
        for (int v = 0; v < cityCount; v++) {
            if (adj[u][v] && !visited[v]) {
                visited[v] = true;
                dist[v] = dist[u] + 1;
                parent[v] = u;
                q[rear++] = v;
            }
        }
    }
}

void Graph::printPath(int parent[], int dest) {
    if (dest == -1) return;

    int stk[MAX_CITIES];
    int top = -1;

    int v = dest;
    while (v != -1) {
        stk[++top] = v;
        v = parent[v];
    }

    for (int i = top; i >= 0; i--) {
        std::cout << cityNames[stk[i]];
        if (i > 0) std::cout << " -> ";
    }
    std::cout << "\n";
}

void Graph::printCities() {
    for (int i = 0; i < cityCount; i++) {
        std::cout << i << ": " << cityNames[i] << "\n";
    }
}

const char* Graph::getCityName(int idx) const {
    if (idx < 0 || idx >= cityCount) return "";
    return cityNames[idx];
}
