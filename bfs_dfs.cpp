#include <iostream>
#include <vector>
#include <queue>
#include <omp.h>
using namespace std;

class Graph {
    int V;
    vector<vector<int>> adj;

public:
    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    // Add undirected edge
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // ─── Parallel BFS ───────────────────────────────────────────────
    void parallelBFS(int start) {
        vector<bool> visited(V, false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        cout << "\nParallel BFS Traversal: ";

        while (!q.empty()) {
            int size = q.size();
            vector<int> toVisit;  // FIX: collect next nodes safely

            #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < size; i++) {

                int node = -1;

                // FIX 1: pop node inside critical section
                #pragma omp critical
                {
                    if (!q.empty()) {
                        node = q.front();
                        q.pop();
                        cout << node << " ";
                    }
                }

                // FIX 2: skip if queue was empty for this thread
                if (node == -1) continue;

                for (int j = 0; j < (int)adj[node].size(); j++) {
                    int neighbor = adj[node][j];

                    // FIX 3: protect visited[] and toVisit together
                    #pragma omp critical
                    {
                        if (!visited[neighbor]) {
                            visited[neighbor] = true;
                            toVisit.push_back(neighbor);
                        }
                    }
                }
            }

            // FIX 4: push to queue sequentially (queue is not thread-safe)
            for (int n : toVisit)
                q.push(n);
        }
    }

    // ─── Parallel DFS ───────────────────────────────────────────────
    void parallelDFSUtil(int node, vector<bool>& visited) {

        // FIX 5: print inside critical to avoid garbled output
        #pragma omp critical
        {
            cout << node << " ";
        }

        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < (int)adj[node].size(); i++) {
            int neighbor = adj[node][i];

            // FIX 6: check-and-set visited[] atomically to prevent double visit
            bool doVisit = false;
            #pragma omp critical
            {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    doVisit = true;
                }
            }

            if (doVisit)
                parallelDFSUtil(neighbor, visited);
        }
    }

    void parallelDFS(int start) {
        vector<bool> visited(V, false);
        visited[start] = true;  // mark before first call
        cout << "\nParallel DFS Traversal: ";
        parallelDFSUtil(start, visited);
    }
};

// ─── Main ────────────────────────────────────────────────────────────
int main() {
    int V = 7;
    Graph g(V);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);
    g.addEdge(2, 6);

    /*
            0
           / \
          1   2
         / \ / \
        3  4 5  6
    */

    double start, end;

    // BFS
    start = omp_get_wtime();
    g.parallelBFS(0);
    end = omp_get_wtime();
    cout << "\nBFS Time: " << end - start << " seconds";

    // DFS
    start = omp_get_wtime();
    g.parallelDFS(0);
    end = omp_get_wtime();
    cout << "\nDFS Time: " << end - start << " seconds\n";

    return 0;
}
