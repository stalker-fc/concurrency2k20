package graph;

public class DistanceEstimator {
    public static double estimateDistance(Point a, Point b) {
        return Math.sqrt(
                (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)
        );
    }
}
