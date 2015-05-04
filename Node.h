/**
 * Node.h
 * 
 * Node represents a vertex in the graph.
 *
 * Created on: May 3, 2015
 * Author: Gaurav Nanda
 */
#ifdef NODE_H_
#define NODE_H_

#include <vector>
#include <utility>

using namespace std;

class Node {
private:
    // Total number of edges going out of the edge.
    uint64_t out_degree;

    // Each node maintains which all cores it would have to send updates to.
    vector<uint8_t> out_cores;
    
    // Store all of the incoming nodes. Each node is represented 
    vector<pair<uint8_t, uint64_t>> in_nodes;
public:
    // Constructor.
    Node();

    // Destructor.
    virtual ~Node();

    // Return the number of out going edges from the node.
    uint64_t getOutDegree();

    // Sets out degree.
    void setOutDegree(uint64_t out_degree);
    
    // Get the vector having list of cores, to which page rank
    // would need to send updates.
    vector<uint8_t>& getOutCores();

    // Add mpi node to the out_core.
    void addOutCore(uint8_t core_no);

    // Get list of all the incoming nodes.
    vector<pair<uint8_t, uint64_t>> getIncomingNodes();

    // Add one incoming node.
    void addIncomingNode(uint8_t core, uint64_t rank);
};

#endif /* NODE_H_ */
