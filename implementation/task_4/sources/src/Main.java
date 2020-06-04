import graph.Edge;
import graph.Graph;
import graph.GraphFileParser;
import path.DjikstraPathFinder;

import java.io.IOException;
import java.util.List;

public class Main {
    public static void main(String[] args) throws IOException {
        Integer mode = null;
        String path_to_graph_data = null;
        Integer source = null;
        Integer destination = null;

        if (args.length != 4) {
            System.err.println("`mode` [1 for sequential, 2 for concurrent] " +
                    "`path/to/graph/data.txt` `source` `destination`");
            System.exit(1);
        } else {
            mode = Integer.parseInt(args[0]);
            path_to_graph_data = args[1];
            source = Integer.parseInt(args[2]);
            destination = Integer.parseInt(args[3]);
        }

        List<Edge> edges = GraphFileParser.getEdgesFromFile(path_to_graph_data);
        var graph = new Graph(edges);
        List<Integer> path = null;
        if (mode == 0) {
            var djikstra = new DjikstraPathFinder();
            path = djikstra.getShortestPath(graph, source, destination);
        } else if (mode == 1) {
            System.out.println("Nothing to do here");
        } else {
            System.err.println("Incorrect `mode` value. It must be `1` for sequential or `2` for concurrent");
            System.exit(1);
        }
        if (path == null) {
            System.err.println("Can`t build path for such source and destination.");
            System.exit(1);
        }
    }
}
