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

#ifndef _MPI_GRAPH_H_
#define _MPI_GRAPH_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <stdint.h>
#include "Node.h"

using namespace std;
class Graph {
    private:
        // Vector of all the nodes on the local node.
        vector<Node*> nodes;
        
        // Total nodes. Sum of all the nodes on all the MPI tasks.
        int total_node_count;
    public:
        // Constructor.
        Graph();

        // Destructor.
        virtual ~Graph();

        // Read in graph file.
        // Each line in the graph file, would look like
        // OutDegree#Links with which MPIs(e.g Link1, Link2..)#(incoming_node_cluster-incoming
        //                                                      node_rank_in_that_cluster,...)
        void ingestFile(string path, int world_size);

        // Given the node index, it returns the Node*.
        Node* getNode(int index);

        // Return total local nodes;
        int getTotalLocalNodes();

        // Return the total number of nodes across all MPI tasks.
        int getTotalNodes();

        // Update the total number of nodes across all MPI tasks.
        void updateTotalNodes(int count);
};

#endif /* GRAPH_H_ */
