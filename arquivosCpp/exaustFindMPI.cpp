#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <mpi.h>
#include <chrono>

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

bool isChecked(vector<int>& currentClique, vector<vector<int>>& graph){
    int sizeClique = currentClique.size();
    for (int i = 0; i < sizeClique; ++i) {
          for (int j = i + 1; j < sizeClique; ++j) {
              if (graph[currentClique[i]][currentClique[j]] == 0) {
                  return false;
              }
          }
    }
    return true;
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
      if (currentClique.size() > maxClique.size() && isChecked(currentClique, graph)){
        maxClique = currentClique;        
      }
    }

}

int main(int argc, char** argv) {

    chrono::high_resolution_clock::time_point startTime;
    chrono::high_resolution_clock::time_point endTime;

    int numVer;
    vector<vector<int>> graph;

    vector<int> possibles;

    vector<int> currentClique;
    vector<int> maxClique;

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) {

      startTime = chrono::high_resolution_clock::now();

      graph = LerGrafo(argv[1], numVer);

      MPI_Bcast(&numVer, 1, MPI_INT, 0, MPI_COMM_WORLD);

      for (int i = 0; i < numVer; ++i) {
        MPI_Bcast(graph[i].data(), numVer, MPI_INT, 0, MPI_COMM_WORLD);
      }

    }
    else {
      MPI_Bcast(&numVer, 1, MPI_INT, 0, MPI_COMM_WORLD);
      graph.resize(numVer, vector<int>(numVer, 0));
      for (int i = 0; i < numVer; ++i) {
          MPI_Bcast(graph[i].data(), numVer, MPI_INT, 0, MPI_COMM_WORLD);
      }
    }

    for (int clique = 0; clique < numVer; clique++){
        possibles.push_back(clique);
    }

    FindMaxClique(graph, possibles, currentClique, maxClique);

    if(rank != 0){
      MPI_Send(maxClique.data(), numVer, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }
    else{
      vector<int> tempCliqueMax(numVer, 0);

      for (int i = 1; i < size; ++i) {
        MPI_Recv(tempCliqueMax.data(), numVer, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (tempCliqueMax.size() > maxClique.size() && isChecked(tempCliqueMax, graph)){
            maxClique = tempCliqueMax;
        }
      }

      cout << "Clique máximo (tamanho " << maxClique.size() <<") achado com uma busca exaustiva: " << endl;

      for (int el : maxClique){
        //adicionando 1 para bater com o resultado do verifyClique
        cout << el + 1 << " ";
      }

      endTime = chrono::high_resolution_clock::now();
      chrono::duration<double> timeMPI = endTime - startTime;
      cout << endl;
      cout << timeMPI.count() << " seconds!" << endl;

    }

    MPI_Finalize();
    return 0;
}