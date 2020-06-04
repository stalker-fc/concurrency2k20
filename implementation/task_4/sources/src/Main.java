import graph.Edge;
import graph.Graph;
import graph.GraphFileParser;
import path.DjikstraPathFinder;

import java.io.IOException;
import java.util.List;

public class Main {
    public static void main(String[] args) throws IOException {
        List<Edge> edges = GraphFileParser.getEdgesFromFile("/home/merlin/study/multiprocessing/concurrency2k20/implementation/task_4/sources/src/graph.txt");
        var graph = new Graph(edges);
        var djikstra = new DjikstraPathFinder();
        var path = djikstra.getShortestPath(graph, 0, 1);
    }
}
