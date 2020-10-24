#include <chrono>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "graph.h"


Graph::Graph(int n_vertices1, int **adjacency_matrix1) : n_vertices(n_vertices1) {
    adjacency_matrix = new int *[n_vertices];
    for (int i = 0; i < n_vertices; ++i) {
        adjacency_matrix[i] = new int[n_vertices];
        for (auto j = 0; j < n_vertices; ++j) {
            adjacency_matrix[i][j] = adjacency_matrix1[i][j];
        }
    }
}

Graph::Graph(char *filename) {
    std::ifstream input;
    input.open(filename);
    if (!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }
    input >> n_vertices;

    adjacency_matrix = new int *[n_vertices];
    for (std::size_t i = 0; i < n_vertices; i++) {
        adjacency_matrix[i] = new int[n_vertices];
    }
    for (std::size_t i = 0; i < n_vertices * n_vertices; ++i)
        input >> adjacency_matrix[i / n_vertices][i % n_vertices];
}

