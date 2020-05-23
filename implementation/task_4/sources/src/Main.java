import graph.Edge;
import graph.GraphFileParser;

import java.io.IOException;
import java.util.List;

public class Main {
    public static void main(String[] args) throws IOException {
        List<Edge> edges = GraphFileParser.getEdgesFromFile("/home/merlin/study/multiprocessing/concurrency2k20/implementation/task_4/sources/src/graph.txt");
        System.out.println("Hello world!");
    }
}
