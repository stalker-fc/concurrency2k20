package path;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Set;

public class ConcurrentDjikstraPathFinder extends DjikstraPathFinder {
    private static final int MAX_THREADS_COUNT = 4;

    protected void findDistancesFromSource(Integer node) {
        var adjacentNodes = graph.getAdjacentNodes(node);
        ArrayList<Thread> threads = new ArrayList<>();
        int numberOfThreads = 2;
        Integer[] firstHalf = Arrays.copyOfRange(adjacentNodes, 0, adjacentNodes.length / 2);
        Integer[] secondHalf = Arrays.copyOfRange(adjacentNodes, adjacentNodes.length / 2, adjacentNodes.length - 1);

        var firstThread = new Thread(new ConcurrentWorker(node, firstHalf));
        var secondThread = new Thread(new ConcurrentWorker(node, secondHalf));
        firstThread.start();
        secondThread.start();
        try {
            firstThread.join();
            secondThread.join();
        } catch (InterruptedException e) {
            System.out.println("Catched error in thread.");
        }
    }

    private class ConcurrentWorker implements Runnable {
        private final Integer node;
        private final Integer[] neighbours;

        public ConcurrentWorker(Integer node, Integer[] neighbours) {
            this.node = node;
            this.neighbours = neighbours;
        }

        @Override
        public void run() {
            for (var neighbour : neighbours) {
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
    }
}
