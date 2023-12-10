#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

// Função para ler o grafo a partir do arquivo de entrada
std::vector<std::vector<int>> LerGrafo(const std::string& nomeArquivo, int& numVertices) {
    std::ifstream arquivo(nomeArquivo);
    int numArestas;
    arquivo >> numVertices >> numArestas;

    std::vector<std::vector<int>> grafo(numVertices, std::vector<int>(numVertices, 0));

    for (int i = 0; i < numArestas; ++i) {
        int u, v;
        arquivo >> u >> v;
        grafo[u - 1][v - 1] = 1;
        grafo[v - 1][u - 1] = 1;  // O grafo é não direcionado
    }

    arquivo.close();

    return grafo;
}

vector<int> FindMaxClique(vector<vector<int>> &graph, int numVer){

    vector<int> maxClique;
    vector<int> possibles;

    for (int clique = 0; clique < numVer; clique++){
        possibles.push_back(clique);
    }

    while (!possibles.empty()){
        int lastClique = possibles.back();
        possibles.pop_back();

        bool canAdd = true;

        for (int u : maxClique){
            if (graph[u][lastClique] == 0){
                canAdd = false;
                break;
            }
        }

        if (canAdd == true){
            maxClique.push_back(lastClique);
            vector<int> newPossibles;

            for (int u : possibles){
                bool isAdjacent = true;

                for (int c: maxClique){
                    if (graph[u][c] == 0){
                        isAdjacent = false;
                        break;
                    }
                }

                if (isAdjacent){
                    newPossibles.push_back(u);
                }
            }

            possibles = newPossibles;
        }
    }

    return maxClique;
}

int main(int argc, char* argv[]){
    int numVer;
    vector<vector<int>> graph;

    graph = LerGrafo(argv[1], numVer);

    vector<int> maxClique;

    maxClique = FindMaxClique(graph, numVer);

    cout << "Clique máximo achado com uma busca que pega o último vértice: " << endl;

    for (auto el : maxClique){
        cout << el << " ";
    }
    return 0;

}