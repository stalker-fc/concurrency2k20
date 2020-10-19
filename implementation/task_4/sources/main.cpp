#include <iostream>
#include "graph.h"
#include "djikstra.cpp"



int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " "
                  << "`path/to/graph.txt` `path/to/solution.txt`"
                  << "mode: [`1` for sequential multiplication, "
                  << "`2` for openmp multiplication, "
                  << std::endl;
        std::exit(1);
    }
    auto graph = Graph(argv[1]);
    Djikstra *djikstra;

    int mode = atoi(argv[2]);

    int *distances;
    int source = 0;
    switch (mode) {
        case 1:
            djikstra = new SequentialDjikstra(&graph);
            distances = djikstra->calculate_shortest_distances(source);
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
    djikstra->print_distances();
    return 0;
}