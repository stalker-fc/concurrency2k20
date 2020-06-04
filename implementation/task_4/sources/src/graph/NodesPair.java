package graph;

public class NodesPair {
    private Integer u;
    private Integer v;

    public NodesPair(Integer u, Integer v) {
        this.u = u;
        this.v = v;
    }

    public int hashCode() {
        int hashU = u != null ? u.hashCode() : 0;
        int hashV = v != null ? v.hashCode() : 0;

        return hashU * hashV;
    }

    public boolean equals(Object other) {
        if (other instanceof NodesPair) {
            NodesPair otherNodesPair = (NodesPair) other;
            return (this.u.equals(otherNodesPair.u) && this.v.equals(otherNodesPair.v)) ||
                    (this.u.equals(otherNodesPair.v) && this.v.equals(otherNodesPair.u));
        }
        return false;
    }
}