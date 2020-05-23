package graph;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

public class GraphFileParser {
    public static ArrayList<Edge> getEdgesFromFile(String fileName) throws IOException {
        FileInputStream stream = new FileInputStream(fileName);
        BufferedReader reader = new BufferedReader(new InputStreamReader(stream, "UTF-8"));

        ArrayList<Edge> edges = new ArrayList<Edge>();
        String line;
        while ((line = reader.readLine()) != null) {
            String[] components = line.split(" ");
            edges.add(new Edge(
                    Integer.parseInt(components[0]),
                    Integer.parseInt(components[1]),
                    Double.parseDouble(components[2])
            ));
        }

        reader.close();
        return edges;
    }
}
