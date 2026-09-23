#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>

#include "graph.h"

using namespace std;

// ---------------------------------------------------------
// Helper: rebuild path from parent[] (source -> dest)
// ---------------------------------------------------------
static vector<int> rebuildPath(const int parent[], int dest) {
    vector<int> path;
    if (dest < 0) return path;

    int cur = dest;
    while (cur != -1) {
        path.push_back(cur);
        cur = parent[cur];
    }
    reverse(path.begin(), path.end());
    return path;
}

// ---------------------------------------------------------
// Helper: concatenate p then q without repeating join city
// ---------------------------------------------------------
static vector<int> concatNoDup(const vector<int>& p, const vector<int>& q) {
    if (p.empty() || q.empty()) return {};
    vector<int> out = p;
    if (!out.empty() && !q.empty() && out.back() == q.front()) {
        out.insert(out.end(), q.begin() + 1, q.end());
    }
    else {
        out.insert(out.end(), q.begin(), q.end());
    }
    return out;
}

// ---------------------------------------------------------
// Helper: print path as "CityA to CityB to CityC"
// ---------------------------------------------------------
static void printPathStyled(const Graph& g, const vector<int>& path) {
    for (size_t i = 0; i < path.size(); ++i) {
        cout << g.getCityName(path[i]);
        if (i + 1 < path.size()) cout << " to ";
    }
    cout << "\n";
}

// =========================================================
// MODE 1
// I am in city A, can I fly to city B with less than X
// connections? Print shortest route or "no such route".
// =========================================================
static int mode1(Graph& g, const string& A, const string& B, int X) {
    int src = g.getCityIndex(A.c_str());
    int dst = g.getCityIndex(B.c_str());

    if (src == -1 || dst == -1) {
        cout << "no such route\n";
        return 0;
    }

    int dist[MAX_CITIES], parent[MAX_CITIES];
    g.BFS(src, dist, parent);

    if (dist[dst] >= INF || dist[dst] > X) {
        cout << "no such route\n";
        return 0;
    }

    vector<int> path = rebuildPath(parent, dst);
    if (path.empty() || path.front() != src) {
        cout << "no such route\n";
        return 0;
    }

    printPathStyled(g, path);
    cout << "total connection: " << dist[dst] << "\n";
    return 0;
}

// =========================================================
// MODE 2
// Give me route with smallest number of connections from A
// to D through B and C (order of B and C not important).
// =========================================================
static pair<vector<int>, int> attemptOrder(Graph& g,
    int s1, int s2, int s3, int t) {
    // s1 -> s2
    int dist1[MAX_CITIES], par1[MAX_CITIES];
    g.BFS(s1, dist1, par1);
    if (dist1[s2] >= INF) return make_pair(vector<int>(), INF);
    vector<int> p1 = rebuildPath(par1, s2);

    // s2 -> s3
    int dist2[MAX_CITIES], par2[MAX_CITIES];
    g.BFS(s2, dist2, par2);
    if (dist2[s3] >= INF) return make_pair(vector<int>(), INF);
    vector<int> p2 = rebuildPath(par2, s3);

    // s3 -> t
    int dist3[MAX_CITIES], par3[MAX_CITIES];
    g.BFS(s3, dist3, par3);
    if (dist3[t] >= INF) return make_pair(vector<int>(), INF);
    vector<int> p3 = rebuildPath(par3, t);

    vector<int> all = concatNoDup(concatNoDup(p1, p2), p3);
    int hops = all.empty() ? INF : static_cast<int>(all.size()) - 1;

    return make_pair(all, hops);
}

static int mode2(Graph& g,
    const string& A,
    const string& B,
    const string& C,
    const string& D) {
    int a = g.getCityIndex(A.c_str());
    int b = g.getCityIndex(B.c_str());
    int c = g.getCityIndex(C.c_str());
    int d = g.getCityIndex(D.c_str());

    if (a < 0 || b < 0 || c < 0 || d < 0) {
        cout << "no such route\n";
        return 0;
    }

    pair<vector<int>, int> r1 = attemptOrder(g, a, b, c, d); // A -> B -> C -> D
    pair<vector<int>, int> r2 = attemptOrder(g, a, c, b, d); // A -> C -> B -> D

    vector<int> bestPath;
    int bestCost = INF;

    if (r1.second < bestCost) {
        bestCost = r1.second;
        bestPath = r1.first;
    }
    if (r2.second < bestCost) {
        bestCost = r2.second;
        bestPath = r2.first;
    }

    if (bestCost >= INF || bestPath.empty()) {
        cout << "no such route\n";
        return 0;
    }

    printPathStyled(g, bestPath);
    cout << "smallest number of connection: " << bestCost << "\n";
    return 0;
}

// =========================================================
// MODE 3
// Start from A, visit all reachable cities and come back to
// A using as few connections as possible (tree walk).
// =========================================================
static void dfsTree(int u,
    vector<int> children[],
    vector<int>& route) {
    for (size_t i = 0; i < children[u].size(); ++i) {
        int v = children[u][i];
        route.push_back(v);       // go down
        dfsTree(v, children, route);
        route.push_back(u);       // come back up
    }
}

static int mode3(Graph& g, const string& A) {
    int src = g.getCityIndex(A.c_str());
    if (src < 0) {
        cout << "no such route\n";
        return 0;
    }

    int n = g.getNumCities();

    int dist[MAX_CITIES], parent[MAX_CITIES];
    g.BFS(src, dist, parent);

    vector<int> children[MAX_CITIES];
    for (int v = 0; v < n; ++v) {
        if (v == src) continue;
        if (parent[v] != -1) {
            children[parent[v]].push_back(v);
        }
    }

    vector<int> route;
    route.push_back(src);
    dfsTree(src, children, route);

    printPathStyled(g, route);
    int hops = route.empty() ? 0 : static_cast<int>(route.size()) - 1;
    cout << "smallest number of connection: " << hops << "\n";
    return 0;
}

// =========================================================
// MODE 4
// Three friends in city A, B, C want to meet in a city D so
// the total number of connections is minimized.
// =========================================================
static int mode4(Graph& g,
    const string& A,
    const string& B,
    const string& C) {
    int a = g.getCityIndex(A.c_str());
    int b = g.getCityIndex(B.c_str());
    int c = g.getCityIndex(C.c_str());

    if (a < 0 || b < 0 || c < 0) {
        cout << "there is no such city\n";
        return 0;
    }

    int n = g.getNumCities();

    int distA[MAX_CITIES], parentA[MAX_CITIES];
    int distB[MAX_CITIES], parentB[MAX_CITIES];
    int distC[MAX_CITIES], parentC[MAX_CITIES];

    g.BFS(a, distA, parentA);
    g.BFS(b, distB, parentB);
    g.BFS(c, distC, parentC);

    int bestCity = -1;
    int bestSum = INF;

    for (int v = 0; v < n; ++v) {
        if (distA[v] >= INF || distB[v] >= INF || distC[v] >= INF) continue;
        int sum = distA[v] + distB[v] + distC[v];
        if (sum < bestSum) {
            bestSum = sum;
            bestCity = v;
        }
    }

    if (bestCity == -1) {
        cout << "there is no such city\n";
        return 0;
    }

    vector<int> pathA = rebuildPath(parentA, bestCity);
    vector<int> pathB = rebuildPath(parentB, bestCity);
    vector<int> pathC = rebuildPath(parentC, bestCity);

    cout << "You three should meet at " << g.getCityName(bestCity) << "\n";

    int connA = pathA.empty() ? 0 : static_cast<int>(pathA.size()) - 1;
    int connB = pathB.empty() ? 0 : static_cast<int>(pathB.size()) - 1;
    int connC = pathC.empty() ? 0 : static_cast<int>(pathC.size()) - 1;

    cout << "Route for first person: ";
    printPathStyled(g, pathA);
    cout << "Route for second person: ";
    printPathStyled(g, pathB);
    cout << "Route for third person: ";
    printPathStyled(g, pathC);

    cout << "Total number of connection: "
        << (connA + connB + connC) << "\n";
    return 0;
}

// =========================================================
// MAIN
// =========================================================
int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Graph g;
    g.loadFromFile("flight.txt");

    if (argc < 2) {
        cerr << "usage:\n"
            << "  routeSearch 1 <city_A> <city_B> <num_connection>\n"
            << "  routeSearch 2 <city_A> through <city_B> and <city_C> to <city_D>\n"
            << "  routeSearch 3 <city_A>\n"
            << "  routeSearch 4 <city_A> <city_B> <city_C>\n";
        return 1;
    }

    int mode = stoi(argv[1]);

    if (mode == 1) {
        if (argc < 5) {
            cerr << "not enough args for mode 1\n";
            return 1;
        }
        string A = argv[2], B = argv[3];
        int X = stoi(argv[4]);
        return mode1(g, A, B, X);

    }
    else if (mode == 2) {
        if (argc < 9) {
            cerr << "usage: routeSearch 2 <A> through <B> and <C> to <D>\n";
            return 1;
        }
        string A = argv[2], word1 = argv[3];
        string B = argv[4], word2 = argv[5];
        string C = argv[6], word3 = argv[7];
        string D = argv[8];
        (void)word1; (void)word2; (void)word3;
        return mode2(g, A, B, C, D);

    }
    else if (mode == 3) {
        if (argc < 3) {
            cerr << "usage: routeSearch 3 <city_A>\n";
            return 1;
        }
        string A = argv[2];
        return mode3(g, A);

    }
    else if (mode == 4) {
        if (argc < 5) {
            cerr << "usage: routeSearch 4 <city_A> <city_B> <city_C>\n";
            return 1;
        }
        string A = argv[2], B = argv[3], C = argv[4];
        return mode4(g, A, B, C);

    }
    else {
        cerr << "mode not implemented\n";
        return 1;
    }
}
