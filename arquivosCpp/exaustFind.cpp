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

void FindMaxClique(vector<vector<int>>& graph, vector<int>& possibles, vector<int>& currentClique, vector<int>& maxClique){

    bool canAdd = true;

    if (!possibles.empty()){
      int lastClique = possibles.back();
      possibles.pop_back();
      currentClique.push_back(lastClique);

      vector<int> newPossibles;

      for (int cliquePossible : possibles){
        if (graph[lastClique][cliquePossible] == 1){
          newPossibles.push_back(cliquePossible);
        }
      }

      FindMaxClique(graph, newPossibles, currentClique, maxClique);
      currentClique.pop_back();
      FindMaxClique(graph, possibles, currentClique, maxClique);
      possibles.push_back(lastClique);

    }
    else {
      int size = currentClique.size();
      if (currentClique.size() > maxClique.size()){
        for (int i = 0; i < size; ++i) {
          for (int j = i + 1; j < size; ++j) {
              if (graph[currentClique[i]][currentClique[j]] == 0) {
                  canAdd = false;
                  break;
              }
          }
        }
        if (canAdd){
          maxClique = currentClique;
        }
      }
    }

}

int main(int argc, char* argv[]){
    int numVer;
    vector<vector<int>> graph;

    graph = LerGrafo(argv[1], numVer);
    vector<int> currentClique;
    vector<int> possibles;
    vector<int> maxClique;

    for (int clique = 0; clique < numVer; clique++){
        possibles.push_back(clique);
    }

    FindMaxClique(graph, possibles, currentClique, maxClique);

    cout << "Clique máximo (tamanho " << maxClique.size() <<") achado com uma busca exaustiva: " << endl;

    for (int el : maxClique){
        //adicionando 1 para bater com o resultado do verifyClique
        cout << el + 1 << " ";
    }
    return 0;
}