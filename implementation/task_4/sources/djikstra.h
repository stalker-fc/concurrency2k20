#ifndef DJIKSTRA_DJIKSTRA_H
#define DJIKSTRA_DJIKSTRA_H

void print_distances(int *distances, int n_vertices, char *filename);

int get_closest_vertex(int *distances, bool *is_visited, int n_vertices);

int* calculate_shortest_distances_sequential(Graph graph, int source);

#endif //DJIKSTRA_DJIKSTRA_H
