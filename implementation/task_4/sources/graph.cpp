#include <chrono>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "graph.h"


Graph read_graph_from_file(char *filename) {
    std::ifstream input;
    input.open(filename);
    if (!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }
    int n_vertices;
    input >> n_vertices;

    int **adjacency_matrix = new int*[n_vertices];
    for (std::size_t i = 0; i < n_vertices; i++) {
        adjacency_matrix[i] = new int [n_vertices];
    }
    for (std::size_t i = 0; i < n_vertices; ++i)
        input >> adjacency_matrix[i / n_vertices][i % n_vertices];

    Graph graph{n_vertices, adjacency_matrix};

    return graph;
}

