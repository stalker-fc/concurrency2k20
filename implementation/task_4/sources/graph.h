#ifndef DJIKSTRA_GRAPH_H
#define DJIKSTRA_GRAPH_H


struct Graph {
    int n_vertices;
    int **adjacency_matrix;
};

Graph read_graph_from_file(char *filename);

#endif //DJIKSTRA_GRAPH_H
