#include <iostream>
#include "djikstra.h"
#include "graph.h"


int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " "
                  << "`path/to/graph.txt` `path/to/solution.txt`"
                  << "mode: [`1` for sequential multiplication, "
                  << "`2` for openmp multiplication, "
                  << std::endl;
        std::exit(1);
    }
    struct Graph graph = read_graph_from_file(argv[1]);

    int mode = atoi(argv[2]);

    int *distances;
    int source = 0;
    switch (mode) {
        case 1:
            distances = calculate_shortest_distances_sequential(graph, source);
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            std::cerr << "Incorrect mode value. It must be [1, 2, 3, 4, 5], but it was " << mode << "." << std::endl;
            std::exit(1);
    }
    print_distances(distances, graph.n_vertices, argv[2]);
    return 0;
}