#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>

/// Implementation of Graph data structure
/// based on adjacency list. Also, covers
/// basic algorithms like dfs, bfs, reachibility
/// etc.
/// Adjacency matrix implementation should be similar.
/// Graph is both directed and undirected
/// Graph has node keys and weighted edges
/// Edges can be unweighted but that just means
/// the weight is set to 1.

using namespace std;

// fwd decl
class Node;

class NodeWeightPairHashFctor {
  public:
    size_t operator()(const pair<shared_ptr<Node>, int>&
                          given_pair) const {
        return hash<shared_ptr<Node>>{}(given_pair.first);
    }
};

class NodeWeightPairEqualFctor {
  public:
    bool operator()(
        const pair<shared_ptr<Node>, int>& pair1,
        const pair<shared_ptr<Node>, int>& pair2) const {
        return pair1.first == pair2.first;
    }
};

using NeighborsSet =
    unordered_set<pair<shared_ptr<Node>, int>,
                  NodeWeightPairHashFctor,
                  NodeWeightPairEqualFctor>;

// Adjacency List Type
// Map from a node to a set of neighboring nodes
// Why hash map? Can lookup a node/vertex in the whole
// graph O(1) time.
// Why hash set? Can lookup a given node's child in
// O(1) time.
using AdjacencyList = unordered_set<shared_ptr<Node>>;

/// Node for graph.
/// Why string typed key? Can templatize easily.
class Node {
  public:
    Node(string given_key)
        : key(given_key)
        , neighbors() {}

    bool operator==(const Node& other) {
        return this->key == other.key;
    }

    //
    /// Node's key value
    //
    string key;

    //
    /// Node API's
    //

    // Add neighbor if there isn't one already.
    // Else asserts (neighbor already exists)
    void add_neighbor(shared_ptr<Node> node,
                      int weight = 1) {
        assert(node);
        auto pair = make_pair(node, weight);
        if (neighbors.find(pair) != neighbors.end())
            assert(false); // neighbor already exists
        neighbors.insert(pair);
    }

    // Removes neighbor if there exists one.
    // Else, silently returns.
    void remove_neighbor(shared_ptr<Node> node) {
        assert(node);
        auto pair = make_pair(node, -1);
        if (neighbors.find(pair) != neighbors.end())
            neighbors.erase(pair);
    }

    NeighborsSet neighbors;
};

class Graph {
  public:
    Graph()
        : adj_list() {}

    void add_node(shared_ptr<Node> node) {
        assert(node);
        if (adj_list.find(node) != adj_list.end())
            assert(false); // already exists
        adj_list.insert(node);
    }

    void add_edge(shared_ptr<Node> source,
                  shared_ptr<Node> dest, int weight = 1) {
        assert(source && dest);
        if (adj_list.find(source) == adj_list.end())
            add_node(source);
        if (adj_list.find(source) == adj_list.end())
            add_node(dest);
        source->add_neighbor(dest, weight);
    }

    void add_undirected_edge(shared_ptr<Node> source,
                             shared_ptr<Node> dest,
                             int weight = 1) {
        assert(source && dest);
        add_edge(source, dest);
        add_edge(dest, source);
    }

    //
    /// Graph Algorithms
    //
    /// Basic DFS - O(V + E). E = # edges
    /// Not O(V) where V = # nodes/vertices because one
    /// node can have multiple incoming nodes as opposed
    /// to 1 in for example binary trees.
    /// Space is O(V) because of implicit recursive stack.
    /// Note: DFS has to start from *some* node. For trees
    /// it's the root but for graphs, it's more generic
    /// since also because graphs can have unconnected
    /// components.
    void dfs(shared_ptr<Node> node) const {
        cout << node->key << " ";
        for (const auto& neighbor : node->neighbors)
            dfs(neighbor.first);
    }

    /// Is node2 reachable from node1?
    /// In other words does path node1->node2 exist?
    /// O(E) because of DFS. Although can be made more
    /// efficient by caching what nodes are visited. Athough
    /// worst case still O(E). Space is O(V) because of
    /// implicit recursive stack.
    bool is_reachable(shared_ptr<Node> node1,
                      shared_ptr<Node> node2) const {
        // Can do a dfs or bfs and see if we get node2
        // starting
        // from node1. Will do dfs for now.
        for (const auto& neighbor : node1->neighbors) {
            if (neighbor.first == node2)
                return true;
            return is_reachable(neighbor.first, node2);
        }
        return false;
    }

    /// One application: dependency analysis (e.g. build
    /// systems)
    /// Stack and hashset implementation
    /// Idea: Pick any node, and rescursively explore
    /// it fully. While doing that cache visited nodes.
    /// Once node fully explored, add it to explored
    /// stack. Then pick another node and explore it fully.
    /// In the process, don't revisit. This works because
    /// once a given area of graph is explored, we have
    /// covered
    /// *all* downstream nodes starting from the initial
    /// node.
    /// After that the only unvisited nodes are the ones
    /// upstream which we visit and explore again.
    /// Note: there can be multiple valid topological sorted
    /// sequences for a given graph.
    /// Note: We must explore a given node in a DFS manner
    /// because doing it that way will take care of node
    /// dependencies (directions) inherently.
    /// Time: O(E + V), Space: O(V)
    /// Same as DFS
    /// Note: Graph must be a DAG otherwise topological
    /// sort isn't defined i.e. can't have cycles or be
    /// undirected.
    void topological_sort() const {
        unordered_set<shared_ptr<Node>> visited;
        // explored at the end will be filled
        // with nodes in topologically sorted
        // order.
        stack<shared_ptr<Node>> explored;
        // Pick any node at random ===
        // traverse hashset.
        // Note: This won't be deterministic across builds
        // but will still give a valid topo sort.
        for (const auto node : adj_list) {
            topological_sort_helper(node, visited,
                                    explored);
        }
        // print populated stack
        while (!explored.empty()) {
            cout << explored.top()->key << " ";
            explored.pop();
        }
        cout << endl;
    }

    void topological_sort_helper(
        const shared_ptr<Node> node,
        unordered_set<shared_ptr<Node>>& visited,
        stack<shared_ptr<Node>>& explored) const {
        // if visited, return early
        if (visited.find(node) != visited.end())
            return;
        // else, make node visited
        visited.insert(node);
        // now start exploring
        for (const auto neighbor : node->neighbors) {
            topological_sort_helper(neighbor.first, visited,
                                    explored);
        }
        // add to stack once done exploring this node
        explored.push(node);
    }

    /// Cycle detection algorithm in an undirected graph
    /// Multiple ways:
    /// 1) disjoint sets
    /// 2) dfs
    /// 3) topological sort
    /// Will use dfs here
    /// Note: This algorithm has to be provided a starting
    /// node
    /// It tells if a cycle exists if we traverse in any
    /// order starting from the given node.
    bool cycle_exists(shared_ptr<Node> node) const {
        unordered_set<shared_ptr<Node>> visited;
        return cycle_exists_helper(node, visited);
    }

    bool cycle_exists_helper(
        shared_ptr<Node> node,
        unordered_set<shared_ptr<Node>> visited) const {
        if (visited.find(node) != visited.end())
            return true;
        visited.insert(node);
        for (const auto neighbor : node->neighbors) {
            if (cycle_exists_helper(neighbor.first,
                                    visited))
                return true;
        }
        return false;
    }

    /// Graph starting at node can contain cycles
    /// It can be either directed or undirected
    /// Time: O(V + E)
    /// Space: O(V)
    /// Same as DFS
    /// Note: Using graph with adjacency list
    void bfs(shared_ptr<Node> node) const {
        if (!node)
            return;
        // to preserve level order, use a queue
        queue<shared_ptr<Node>> q;
        // to take care of cycles, use visited set
        unordered_set<shared_ptr<Node>> visited;
        // push node in the queue to start with
        q.push(node);
        while (!q.empty()) {
            // push neighbors to queue if node not visited
            if (visited.find(q.front()) == visited.end()) {
                // mark visited
                visited.insert(q.front());
                // explore node but just for 1 level i.e. no
                // recursion here and push the
                // children/neighbor
                // of this node to the end of queue
                for (const auto neighbor :
                     q.front()->neighbors) {
                    q.push(neighbor.first);
                }
                // now populate output sequencex; this will
                // be part of bfs sequence; can store in
                // another container or just cout for now.
                cout << q.front()->key << " ";
            }
            // finally, pop from queue since we have
            // explored this and moved to output
            // if needed.
            q.pop();
        }
        cout << endl;
    }

  private:
    AdjacencyList adj_list;
};

int main() {
    /// Main Output:

    /// Without cycles:
    /// Dfs starting from chelsea node is:
    /// Chelsea Barcelona ManCity ManUtd RealMadrid

    /// Bfs starting from chelsea node is:
    /// Chelsea Barcelona ManCity RealMadrid ManUtd

    /// Chelsea defeated Manchester United.

    /// Topological sort of teams:
    /// Chelsea Tottenham Arsenal Liverpool Juventus
    /// Barcelona RealMadrid ManCity ManUtd

    /// With cycles:
    /// Cucle exists in graph.
    /// Note: g.add_edge(liverpool, chelsea) introduces the
    /// cycle

    Graph g;
    auto arsenal = make_shared<Node>("Arsenal");
    auto chelsea = make_shared<Node>("Chelsea");
    auto barcelona = make_shared<Node>("Barcelona");
    auto realmadrid = make_shared<Node>("RealMadrid");
    auto tottenham = make_shared<Node>("Tottenham");
    auto manutd = make_shared<Node>("ManUtd");
    auto mancity = make_shared<Node>("ManCity");
    auto liverpool = make_shared<Node>("Liverpool");
    auto juventus = make_shared<Node>("Juventus");
    g.add_node(juventus);
    g.add_edge(arsenal, liverpool);
    g.add_edge(tottenham, arsenal);
    g.add_edge(chelsea, barcelona);
    g.add_edge(barcelona, realmadrid);
    g.add_edge(mancity, manutd);
    g.add_edge(barcelona, mancity);
    g.add_edge(realmadrid, tottenham);
    g.add_edge(liverpool, chelsea);
    g.add_edge(chelsea, juventus);

    if (g.cycle_exists(barcelona)) {
        cout << "Cycle exists in graph" << endl;
    } else {
        cout << "Dfs starting from chelsea node is:\n";
        g.dfs(chelsea);
        cout << endl << endl;

        cout << "Bfs starting from chelsea node is:\n";
        g.bfs(chelsea);
        cout << endl;

        if (g.is_reachable(chelsea, manutd))
            cout << "Chelsea defeated Manchester United.\n";

        cout << "\nTopological sort of teams:\n";
        if (!g.cycle_exists(chelsea))
            g.topological_sort();
    }
    return 0;
}
