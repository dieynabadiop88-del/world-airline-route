#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <cstring>

const int MAX_CITIES = 200;
const int MAX_NAME_LEN = 50;
const int INF = 999999;

class Graph {
private:
    int adj[MAX_CITIES][MAX_CITIES];
    char cityNames[MAX_CITIES][MAX_NAME_LEN];
    int cityCount;

public:
    Graph();

    // city and graph management
    void addCity(const char* name);
    int getCityIndex(const char* name);
    void addEdge(const char* src, const char* dest);
    void loadFromFile(const char* filename);

    // algorithms
    void BFS(int src, int dist[], int parent[]);
    void printPath(int parent[], int dest);
    void printCities();

    int getNumCities() const { return cityCount; }
    const char* getCityName(int idx) const;
};

#endif
