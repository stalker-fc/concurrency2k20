#include <climits>
#include <omp.h>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "graph.h"
#include "dijkstra.h"

#define INF INT_MAX


void Dijkstra::print_distances() {
    for (std::size_t i = 0; i < graph->n_vertices; ++i) {
        if (distances[i] == INF) {
            std::cout << i << "\t" << "INF" << std::endl;
        } else {
            std::cout << i << "\t" << distances[i] << std::endl;
        }
    }
}

void Dijkstra::save_distances_to_file(char *filename) {
    std::ofstream out;
    out.open(filename);
    for (std::size_t i = 0; i < graph->n_vertices; ++i) {
        if (distances[i] == INF) {
            out << i << "\t" << "INF" << std::endl;
        } else {
            out << i << "\t" << distances[i] << std::endl;
        }
    }
    out.close();
}


int *SequentialDijkstra::calculate_shortest_distances(int source) {
    distances = new int[graph->n_vertices];
    is_visited = new bool[graph->n_vertices];
    for (int i = 0; i < graph->n_vertices; ++i) {
        distances[i] = INF;
        is_visited[i] = false;
    }

    distances[source] = 0;
    for (int i = 0; i < graph->n_vertices - 1; ++i) {
        int u = get_closest_vertex();
        is_visited[u] = true;
        // если ближайшая вершина достижима
        if (distances[u] != INF) {
            for (int v = 0; v < graph->n_vertices; ++v) {
                if (!is_visited[v] // вершина v ранее не посещена
                    && graph->adjacency_matrix[u][v] // существует ребро между ближайшей вершиной и вершиной v
                    && distances[u] + graph->adjacency_matrix[u][v] <
                       distances[v] // общее расстояние через вершину u меньше существующего
                        )
                    distances[v] = distances[u] + graph->adjacency_matrix[u][v];
            }
        }
    }

    return distances;
}

int SequentialDijkstra::get_closest_vertex() {
    int min_distance = INF;
    int closest_vertex;

    for (int i = 0; i < graph->n_vertices; ++i) {
        if (!is_visited[i] && distances[i] <= min_distance) {
            min_distance = distances[i];
            closest_vertex = i;
        }
    }
    return closest_vertex;
}


int *ParallelDijkstra::calculate_shortest_distances(int source) {
    int n_vertices = graph->n_vertices;
    int **adjacency_matrix = graph->adjacency_matrix;
    distances = new int[n_vertices];
    is_visited = new bool[n_vertices];
    for (int i = 0; i < n_vertices; ++i) {
        distances[i] = INF;
        is_visited[i] = false;
    }

    int n_threads = omp_get_max_threads();
    auto min_thread_distances = new int[n_threads];
    auto closest_thread_vertices = new int[n_threads];
    for (int i = 0; i < n_threads; ++i) {
        min_thread_distances[i] = INF;
        closest_thread_vertices[i] = -1;
    }

    distances[source] = 0;
    for (int i = 0; i < n_vertices; ++i) {
#pragma omp parallel for default(none) shared(n_vertices, is_visited, distances, min_thread_distances, closest_thread_vertices)
        for (int k = 0; k < n_vertices; k++) {
            int cur_thread = omp_get_thread_num();
            if (!is_visited[k] &&
                (closest_thread_vertices[cur_thread] < 0 || min_thread_distances[cur_thread] > distances[k])) {
                min_thread_distances[cur_thread] = distances[k];
                closest_thread_vertices[cur_thread] = k;
            }
        }

        int closest_vertex = closest_thread_vertices[0];
        int min_distance = min_thread_distances[0];
        for (int j = 0; j < n_threads; ++j) {
            if (min_distance > min_thread_distances[j]) {
                min_distance = min_thread_distances[j];
                closest_vertex = closest_thread_vertices[j];
            }
        }
        is_visited[closest_vertex] = true;

#pragma omp parallel for default(none) shared(n_vertices, is_visited, distances, adjacency_matrix, closest_vertex)
        for (int v = 0; v < n_vertices; v++) {
            if (adjacency_matrix[closest_vertex][v] >= 0) {
                distances[v] = std::min(distances[v], distances[closest_vertex] + adjacency_matrix[closest_vertex][v]);
            }
        }
    }

    return distances;
}