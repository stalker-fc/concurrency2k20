#ifndef DJIKSTRA_GRAPH_H
#define DJIKSTRA_GRAPH_H

class Graph {
public:
    int n_vertices;
    int **adjacency_matrix;

    Graph() : Graph(0, nullptr) {}

    Graph(int n_vertices1, int **adjacency_matrix1);

    Graph(char *filename);
};


#endif //DJIKSTRA_GRAPH_H
