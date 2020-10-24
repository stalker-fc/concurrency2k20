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
                if (!is_visited[v] && graph->adjacency_matrix[u][v])
                    distances[v] = std::min(distances[v], distances[u] + graph->adjacency_matrix[u][v]);
            }
        }
    }

    return distances;
}


int *ParallelDijkstra::calculate_shortest_distances(int source) {
    distances = new int[graph->n_vertices];
    is_visited = new bool[graph->n_vertices];
    for (int i = 0; i < graph->n_vertices; ++i) {
        distances[i] = INF;
        is_visited[i] = false;
    }

    distances[source] = 0;
    for (int i = 0; i < graph->n_vertices; ++i) {
        int u = get_closest_vertex();
        if (u < 0) // если вершина достижима
            std::cerr << "Broken" << std::endl;
        break;
        is_visited[u] = true;
        // ближайшая вершина достижима
        if (distances[u] != INF) {
#pragma omp parallel for default(none) shared(graph, is_visited, distances, u)
            for (int v = 0; v < graph->n_vertices; ++v) {
                if (!is_visited[v] && graph->adjacency_matrix[u][v])
                    distances[v] = std::min(distances[v], distances[u] + graph->adjacency_matrix[u][v]);
            }
        }
    }
    delete[] is_visited;
    return distances;
}

int ParallelDijkstra::get_closest_vertex() {
    int n_threads = omp_get_max_threads();
    auto min_thread_distances = new int[n_threads];
    auto closest_thread_vertices = new int[n_threads];
    for (int i = 0; i < n_threads; ++i) {
        min_thread_distances[i] = INF;
        closest_thread_vertices[i] = -1;
    }

#pragma omp parallel for default(none) shared(graph, is_visited, distances, min_thread_distances, closest_thread_vertices)
    for (int i = 0; i < graph->n_vertices; i++) {
        int cur_thread = omp_get_thread_num();
        if (!is_visited[i] &&
            (closest_thread_vertices[cur_thread] < 0 || min_thread_distances[cur_thread] > distances[i])) {
            min_thread_distances[cur_thread] = distances[i];
            closest_thread_vertices[cur_thread] = i;
        }
    }

    int closest_vertex = closest_thread_vertices[0];
    int min_distance = INF;
    for (int i = 0; i < n_threads; ++i) {
        if (min_distance >= min_thread_distances[i]) {
            min_distance = min_thread_distances[i];
            closest_vertex = closest_thread_vertices[i];
        }
    }
    delete[] min_thread_distances;
    delete[] closest_thread_vertices;
    return closest_vertex;
}