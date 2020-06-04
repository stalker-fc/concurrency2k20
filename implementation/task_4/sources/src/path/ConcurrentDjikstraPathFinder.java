package path;

import graph.Graph;

import java.util.*;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

public class ConcurrentDjikstraPathFinder {
    protected Graph graph;
    protected Graph graphForward;
    protected Graph graphReverse;
    protected Integer source;
    protected Integer destination;
    private List<Integer> path;

    public List<Integer> getShortestPath(Graph graph, Integer source, Integer destination) {
        init(graph, source, destination);
        try {
            findPath();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return path;
    }

    private void init(Graph graph, Integer source, Integer destination) {
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

    protected void findPath() throws InterruptedException {
        ArrayList<Integer> pathForward = new ArrayList<Integer>();
        ArrayList<Integer> pathReverse = new ArrayList<Integer>();
        AtomicBoolean stopFlag = new AtomicBoolean(false);
        AtomicInteger stopNodeForward = new AtomicInteger(-1);
        AtomicInteger stopNodeReverse = new AtomicInteger(-1);
        CopyOnWriteArraySet<Integer> visitedForward = new CopyOnWriteArraySet<>();
        CopyOnWriteArraySet<Integer> visitedReverse = new CopyOnWriteArraySet<>();

        Runnable partSource = new DjikstraThread(graphForward, source, destination,
                pathForward, stopFlag, stopNodeForward, visitedForward);
        Runnable partDestination = new DjikstraThread(graphReverse, destination, source,
                pathReverse, stopFlag, stopNodeReverse, visitedReverse);
        Runnable stopRunnable = new StopNodeSearcher(stopFlag, stopNodeForward, stopNodeReverse,
                visitedForward, visitedReverse);

        Thread threadSource = new Thread(partSource);
        Thread threadDestination = new Thread(partDestination);
        Thread stopThread = new Thread(stopRunnable);
        threadSource.start();
        threadDestination.start();
        stopThread.start();

        threadSource.join();
        threadDestination.join();

        retrievePath(pathForward, pathReverse);
        FinalMerge();
    }
    protected List<Integer> retrievePath(List<Integer> pathForward, List<Integer> pathReverse) {
        return new ArrayList<>();
    }
}
