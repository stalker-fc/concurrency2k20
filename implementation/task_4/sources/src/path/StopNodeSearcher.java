package path;

import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.atomic.AtomicBoolean;

public class StopNodeSearcher implements Runnable {
    protected AtomicBoolean stopFlag;
    protected Integer stopNodeForward;
    protected Integer stopNodeReverse;
    protected CopyOnWriteArraySet<Integer> visitedNodesForward;
    protected CopyOnWriteArraySet<Integer> visitedNodesReverse;

    protected Double minLength;
    protected double topF = 0.0;
    protected double topR = 0.0;
    protected double topFHeuristic = Double.MAX_VALUE;
    protected double topRHeuristic = Double.MAX_VALUE;
    protected boolean flag = false;

    public StopNodeSearcher(AtomicBoolean stop,
                             Integer stopNodeForward,
                             Integer stopNodeReverse,
                             CopyOnWriteArraySet<Integer> visitedNodesForward,
                             CopyOnWriteArraySet<Integer> visitedNodesReverse) {
        this.stopFlag = stop;
        this.stopNodeForward = stopNodeForward;
        this.stopNodeReverse = stopNodeReverse;
        this.visitedNodesForward = visitedNodesForward;
        this.visitedNodesReverse = visitedNodesReverse;
        minLength = Double.MAX_VALUE;
    }


    protected int containsVertex(CopyOnWriteArraySet<In> visitedNodes, Point p){
        for (int i = 0; i < visitedNodes.size(); i++) {
            if (visitedNodes.get(i).vertex.equals(p)) {
                return i;
            }
        }
        return -1;
    }
    protected boolean checkStop() {
        if (stopFlag.get()) {
            if (!stopNodeForward.equals(-1))
                stopNodeReverse = visitedNodesReverse.get(0);
            if (!stopNodeReverse.equals(-1))
                stopNodeForward.setNode(visitedNodesForward.get(0).vertex);
            return true;
        }
        return false;
    }

    protected void findStopNode(int oldSizeVisitedForward,
                                 int oldSizeVisitedReverse){

        if (checkStop())
            return;

        int curSizeVisitedForward = visitedNodesForward.size();
        for (int i = oldSizeVisitedForward; i < curSizeVisitedForward; i++){
            VisitedVertex v = visitedNodesForward.get(i);

            for (var neighbour : v.neighbours.keySet()) {
                int index = containsVertex(visitedNodesReverse, neighbour);
                if (index != -1) {
                    double tmpLength = v.distance + v.neighbours.get(neighbour) + visitedNodesReverse.get(index).distance;
                    if (tmpLength < minLength) {
                        minLength = tmpLength;
                        flag = true;
                        stopNodeForward.setNode(v.vertex);
                        stopNodeReverse.setNode(neighbour);
                        topFHeuristic = v.heuristic;
                        topRHeuristic = visitedNodesReverse.get(index).heuristic;
                    }
                }
            }
            if ((v.distance > topF) && (v.heuristic < topFHeuristic) && flag) {
                topF = v.distance;
                //topFHeuristic = v.heuristic;
            }
        }

        if (checkStop())
            return;

        int curSizeVisitedReverse = visitedNodesReverse.size();
        for (int i = oldSizeVisitedReverse; i < curSizeVisitedReverse; i++){
            VisitedVertex v = visitedNodesReverse.get(i);
            for (Node neighbour : v.neighbours.keySet()) {
                int index = containsVertex(visitedNodesForward, neighbour);
                if (index != -1) {
                    double tmpLength = v.distance + v.neighbours.get(neighbour) + visitedNodesForward.get(index).distance;
                    if (tmpLength < minLength) {
                        minLength = tmpLength;
                        flag = true;
                        stopNodeReverse.setNode(v.vertex);
                        stopNodeForward.setNode(neighbour);
                        topRHeuristic = v.heuristic;
                        topFHeuristic = visitedNodesForward.get(index).heuristic;
                    }
                }
            }
            if((v.distance > topR) && (v.heuristic < topRHeuristic) && flag) {
                topR = v.distance;
                //topRHeuristic = v.heuristic;
            }
        }

        if (checkStop())
            return;

        oldSizeVisitedForward = curSizeVisitedForward;
        oldSizeVisitedReverse = curSizeVisitedReverse;
        /*if (flag)
            stop.set(true);*/
        if (topF + topR > minLength)
            stopFlag.set(true);

    }
    @Override
    public void run() {
        int oldSizeVisitedForward = 0;
        int oldSizeVisitedReverse = 0;
        while(!stopFlag.get()) {
            findStopNode(oldSizeVisitedForward, oldSizeVisitedReverse);
        }
    }
}