package path;

import graph.Graph;

import java.util.*;

public class DjikstraPathFinder {
    protected Graph graph;
    protected Integer source;
    protected Integer destination;

    protected Set<Integer> visitedNodes;
    protected Queue<Integer> unvisitedNodes;
    protected Map<Integer, Integer> predecessors;
    protected Map<Integer, Double> distances;

    public List<Integer> getShortestPath(Graph graph, Integer source, Integer destination) {
        init(graph, source, destination);
        findPath();
        return retrievePath();
    }

    protected void init(Graph graph, Integer source, Integer destination) {
        this.graph = graph;
        this.source = source;
        this.destination = destination;
        visitedNodes = new HashSet<Integer>();
        unvisitedNodes = new PriorityQueue<Integer>();
        predecessors = new HashMap<Integer, Integer>();
        distances = new HashMap<Integer, Double>();

        unvisitedNodes.add(this.source);
        distances.put(this.source, 0.0);
    }

    protected void findPath() {
        while (unvisitedNodes.size() > 0) {
            var node = unvisitedNodes.poll();
            visitedNodes.add(node);
            findDistancesFromSource(node);
        }
    }

    protected void findDistancesFromSource(Integer node) {
        var adjacentNodes = graph.getAdjacentNodes(node);
        for (var neighbour : adjacentNodes) {
            if (visitedNodes.contains(neighbour)) {
                continue;
            }
            var distanceNodeFromSource = distances.getOrDefault(node, Double.MAX_VALUE);
            var distanceNeighbourFromSource = distances.getOrDefault(neighbour, Double.MAX_VALUE);
            var edgeWeight = graph.getWeightOfEdge(node, neighbour);
            if (distanceNeighbourFromSource > distanceNodeFromSource + edgeWeight) {
                distances.put(neighbour, distanceNodeFromSource + edgeWeight);
                predecessors.put(neighbour, node);
                unvisitedNodes.add(neighbour);
            }
        }
    }

    protected List<Integer> retrievePath() {
        List<Integer> shortestPath = new LinkedList<Integer>();
        Integer step = destination;
        // check if a path exists
        if (predecessors.get(step) == null) {
            return null;
        }
        shortestPath.add(step);
        while (predecessors.get(step) != null) {
            step = predecessors.get(step);
            shortestPath.add(step);
        }
        // Put it into the correct order
        Collections.reverse(shortestPath);
        return shortestPath;
    }
}
