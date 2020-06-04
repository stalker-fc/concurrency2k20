package path;

import graph.Graph;

import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

public class DjikstraThread implements Runnable {
    private Graph graph;
    private Integer source;
    private Integer destination;

    private Set<Integer> visitedNodes;
    private Queue<Integer> unvisitedNodes;
    private Map<Integer, Integer> predecessors;
    private Map<Integer, Double> distances;

    private AtomicInteger lastVisitedNode;


    private AtomicBoolean stop;
    private AtomicInteger stopNode;
    private CopyOnWriteArraySet<Integer> visitedNodes;


    public DjikstraThread(Graph graph,
                          Integer source,
                          Integer destination,
                          List<Integer> path,
                          AtomicBoolean stop,
                          Integer stopNode,
                          CopyOnWriteArrayList<Integer> visitedNodes) {
        this.
        initialize(graph, source, destination, shortestPath,
                stop, stopPoint, visitedVertices);
    }

    @Override
    public void run() {
        Point target = findPath();
        retrievePath(target);
    }

    public Integer getLastVisitedNode() {
        return lastVisitedNode.get();
    }

    protected void initialize(WeightedGraph<Point, DefaultWeightedEdge> graph,
                              Point source, Point destination,
                              ArrayList<Point> shortestPath,
                              AtomicBoolean stop, Point stopPoint,
                              CopyOnWriteArrayList<VisitedVertex> visitedVertices) {
        super.initialize(graph, source, destination);
        this.shortestPath = shortestPath;
        this.stop = stop;
        this.stopPoint = stopPoint;
        this.visitedVertices = visitedVertices;
    }

    protected void visitNextNode() {
        Point current = unSettledNodes.poll().point;
        settledNodes.add(current);
        if (current.equals(destination)) {
            stopPoint = destination;
            stop.set(true);
            return;
        }
        VisitedVertex visitedVertex = new VisitedVertex(current, gScore.get(current), fScore.get(current));

        if (stop.get())
            return;

        List<Point> neighbors = getNeighbors(current);
        for (Point neighbor : neighbors) {
            if (stop.get())
                return;
            if (addNeighbor(current, neighbor))
                visitedVertex.addNeighbour(neighbor, getDistance(current, neighbor));
        }
        visitedVertices.add(visitedVertex);
    }

    protected Point findPath() {
        while (!unSettledNodes.isEmpty()) {
            visitNextNode();
            if (stop.get()) {
                return settledNodes.get(settledNodes.indexOf(stopPoint));
            }
        }
        return source;
    }
}
}
