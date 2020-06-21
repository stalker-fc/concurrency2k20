package path;

import java.util.ArrayList;
import java.util.Set;

public class ConcurrentDjikstraPathFinder extends DjikstraPathFinder {
    private static final int MAX_THREADS_COUNT = 4;

    protected void findDistancesFromSource(Integer node) {
        Set<Integer> adjacentNodes = graph.getAdjacentNodes(node);
        ArrayList<Thread> threads = new ArrayList<>();

        for (var neighbour : adjacentNodes) {
            var thread = new Thread(new ConcurrentWorker(node, neighbour));
            threads.add(thread);
            thread.start();
        }
        for (var thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                System.out.println("Catched error in thread.");
            }
        }
    }

    private class ConcurrentWorker implements Runnable {
        private final Integer node;
        private final Integer neighbour;

        public ConcurrentWorker(Integer node, Integer neighbour) {
            this.node = node;
            this.neighbour = neighbour;
        }

        @Override
        public void run() {
            if (visitedNodes.contains(neighbour)) {
                return;
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
