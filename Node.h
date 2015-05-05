/**
 * Node.h
 * 
 * Node represents a vertex in the graph.
 *
 * Created on: May 3, 2015
 * Author: Gaurav Nanda
 */

#ifndef _MPI_NODE_H_
#define _MPI_NODE_H_

#include <vector>
#include <utility>

using namespace std;

class Node {
private:
    // Total number of edges going out of the edge.
    int out_degree;

    // Each node maintains which all cores it would have to send updates to.
    vector<short> out_cores;
    
    // Store all of the incoming nodes. Each node is represented 
    vector < pair<short, int> > in_nodes;
public:
    // Constructor.
    Node();

    // Destructor.
    virtual ~Node();

    // Return the number of out going edges from the node.
    int getOutDegree();

    // Sets out degree.
    void setOutDegree(int out_degree);
    
    // Get the vector having list of cores, to which page rank
    // would need to send updates.
    vector<short>& getOutCores();

    // Add mpi node to the out_core.
    void addOutCore(short core_no);

    // Get list of all the incoming nodes.
    vector< pair<short, int> >& getIncomingNodes();

    // Add one incoming node.
    void addIncomingNode(short core, int rank);
};

#endif /* NODE_H_ */
