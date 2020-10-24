#ifndef DJIKSTRA_DIJKSTRA_H
#define DJIKSTRA_DIJKSTRA_H

class Dijkstra {
public:
    Graph *graph;

    Dijkstra(Graph *graph1) : graph(graph1) {}

    virtual int *calculate_shortest_distances(int source) = 0;

    void print_distances();

    void save_distances_to_file(char *filename);

protected:
    int *distances;
    bool *is_visited;
};

class SequentialDijkstra : public Dijkstra {
public:
    SequentialDijkstra(Graph *graph1) : Dijkstra(graph1) {}

    int *calculate_shortest_distances(int source);

    int get_closest_vertex();
};


class ParallelDijkstra : public Dijkstra {
public:
    ParallelDijkstra(Graph *graph1) : Dijkstra(graph1) {}

    int *calculate_shortest_distances(int source);

    int get_closest_vertex();
};

#endif //DJIKSTRA_DIJKSTRA_H
