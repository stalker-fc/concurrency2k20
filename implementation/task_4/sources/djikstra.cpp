#include <climits>
#include <omp.h>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "graph.h"

#define INF INT_MAX


class Djikstra {
public:
    Graph *graph;

    Djikstra(Graph *graph1) : graph(graph1) {}

    virtual int *calculate_shortest_distances(int source) = 0;

    void print_distances() {
        for (std::size_t i = 0; i < graph->n_vertices; ++i) {
            if (distances[i] == INF) {
                std::cout << i << "\t" << "INF" << std::endl;
            } else {
                std::cout << i << "\t" << distances[i] << std::endl;
            }
        }
    }

    void save_distances_to_file(char *filename) {
        std::ofstream out;
        out.open (filename);
        for (std::size_t i = 0; i < graph->n_vertices; ++i) {
            if (distances[i] == INF) {
                out << i << "\t" << "INF" << std::endl;
            } else {
                out << i << "\t" << distances[i] << std::endl;
            }
        }
        out.close();
    }

protected:
    int *distances;
    bool *is_visited;
};


class SequentialDjikstra : public Djikstra {
public:
    SequentialDjikstra(Graph *graph1) : Djikstra(graph1) {}

    int *calculate_shortest_distances(int source) {
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

    int get_closest_vertex() {
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
};


