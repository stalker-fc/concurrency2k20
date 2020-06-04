package graph;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;

public class Graph {
    private List<Edge> edges;

    public Graph(List<Edge> edges) {
        this.edges = edges;
    }

    public void saveToFile(String fileName) throws IOException {
        BufferedWriter writer = new BufferedWriter(new FileWriter(fileName));
        for (Edge edge : edges) {
            writer.write(String.format("%d %d %f\n", edge.u, edge.v, edge.weight));
        }
        writer.close();
    }
}
