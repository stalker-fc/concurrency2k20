#include <climits>
#include <omp.h>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "graph.h"
#include "djikstra.h"
#define INF INT_MAX



void print_distances(int *distances, int n_vertices, char *filename) {
    for (std::size_t i = 0; i < n_vertices; ++i) {
        if (distances[i] == INF) {
            std::cout << i << "\t" << "INF" << std::endl;
        } else {
            std::cout << i << "\t" << distances[i] << std::endl;
        }
    }
}

int get_closest_vertex(int *distances, bool *is_visited, int n_vertices) {
    int min_distance = INF;
    int closest_vertex;

    for (int i = 0; i < n_vertices; ++i) {
        if (is_visited[i] == false && distances[i] <= min_distance) {
            min_distance = distances[i];
            closest_vertex = i;
        }
    }

    return closest_vertex;
}


int* calculate_shortest_distances_sequential(Graph graph, int source) {
    auto distances = new int [graph.n_vertices];
    auto is_visited = new bool [graph.n_vertices];
    for (int i = 0; i < graph.n_vertices; ++i) {
        distances[i] = INF;
        is_visited[i] = false;
    }

    distances[source] = 0;
    for (int i = 0; i < graph.n_vertices - 1; ++i) {
        int u = get_closest_vertex(distances, is_visited, graph.n_vertices);
        is_visited[u] = true;
        // если ближайшая вершина достижима
        if (distances[u] != INF) {
            for (int v = 0; v < graph.n_vertices; ++v) {
                if (!is_visited[v] // вершина v ранее не посещена
                    && graph.adjacency_matrix[u][v] // существует ребро между ближайшей вершиной и вершиной v
                    && distances[u] + graph.adjacency_matrix[u][v] < distances[v] // общее расстояние через вершину u меньше существующего
                    )
                    distances[v] = distances[u] + graph.adjacency_matrix[u][v];
            }
        }
    }

    return distances;
}