#include <iostream>
#include "omp.h"
#include "graph.h"
#include "dijkstra.h"


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " "
                  << "`path/to/graph.txt`"
                  << "mode: [`1` for sequential dijkstra, "
                  << "`2` for parallel dijkstra]" << std::endl;
        std::exit(1);
    }
    auto graph = Graph(argv[1]);
    Dijkstra *dijkstra;

    int mode = atoi(argv[2]);

    int *distances;
    int source = 0;

    switch (mode) {
        case 1:
            dijkstra = new SequentialDijkstra(&graph);
            distances = dijkstra->calculate_shortest_distances(source);
            break;
        case 2:
            dijkstra = new ParallelDijkstra(&graph);
            distances = dijkstra->calculate_shortest_distances(source);
            break;
        default:
            std::cerr << "Incorrect mode value. It must be [1, 2, 3, 4, 5], but it was " << mode << "." << std::endl;
            std::exit(1);
    }
//    char *filename = "data.txt";
//    dijkstra->save_distances_to_file(filename);
    return 0;
}