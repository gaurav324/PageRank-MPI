/*
 * Graph.h
 *
 * This is highly customized to work for Page Rank algorithm
 * implemented using MPI and with minimum communication across
 * hosts.
 *
 * Created May 3, 2015
 * Author: Gaurav Nanda
 *
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>

using namespace std;
class Graph {
    private:
        // Vector of all the nodes on the local node.
        vector<Node*> nodes;
        
        // Total nodes. Sum of all the nodes on all the MPI tasks.
        uint64_t total_node_count;
    public:
        // Constructor.
        Graph();

        // Destructor.
        virtual ~Graph();

        // Read in graph file.
        // Each line in the graph file, would look like
        // OutDegree#Links with which MPIs(e.g Link1, Link2..)#(incoming_node_cluster-incoming
        //                                                      node_rank_in_that_cluster,...)
        void ingestFile(string path);

        // Given the node index, it returns the Node*.
        Node* getNode(int index);

        // Return total local nodes;
        uint64_t getTotalLocalNodes();

        // Return the total number of nodes across all MPI tasks.
        uint64_t getTotalNodes();

        // Update the total number of nodes across all MPI tasks.
        void updateTotalNodes(uint64_t count);
};

#endif /* GRAPH_H_ */
