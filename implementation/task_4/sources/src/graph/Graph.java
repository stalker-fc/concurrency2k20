package graph;

import java.util.*;

public class Graph {
    private Map<Integer, Set<Integer>> nodeNeighbours;
    private Map<NodesPair, Double> edges;

    public Graph(List<Edge> edges) {
        this.edges = new HashMap<>();
        this.nodeNeighbours = new HashMap<>();
        for (var edge : edges) {
            this.edges.put(new NodesPair(edge.u, edge.v), edge.weight);
        }
        for (var edge : edges) {
            var neighboursU = nodeNeighbours.get(edge.u);
            var neighboursV = nodeNeighbours.get(edge.v);
            if (neighboursU == null) {
                var neighbours = new HashSet<Integer>();
                neighbours.add(edge.v);
                nodeNeighbours.put(edge.u, neighbours);
            } else {
                neighboursU.add(edge.v);
            }
            if (neighboursV == null) {
                var neighbours = new HashSet<Integer>();
                neighbours.add(edge.u);
                nodeNeighbours.put(edge.v, neighbours);
            } else {
                neighboursV.add(edge.v);
            }
        }
    }

    public Integer[] getAdjacentNodes(Integer node) {
        return nodeNeighbours.get(node).toArray(Integer[]::new);
    }

    public Double getWeightOfEdge(Integer source, Integer target) {
        return edges.getOrDefault(new NodesPair(source, target), Double.MAX_VALUE);
    }
}
