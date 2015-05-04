// This is the MPI implementation of the page rank algorithm.

#include <iostream>
#include <vector>
#include <list>
#include <sstream>
#include <cstdlib>

#include <mpi.h>
#include <stdint.h>

#include "Graph.h"
#include "Node.h"

using namespace std;
int main(int argc, char *argv[]) {
    // Start-MPI
    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int host_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &host_id);

    ///////////////////////
    // Read in the file. //
    ///////////////////////
    string filePath = argv[1];
    stringstream ss;
    ss << host_id;
    filePath += (".split." + ss.str());

    cout << "Reading " << filePath << endl;
    Graph g;
    g.ingestFile(filePath);

    /* Just print some nodes of the graph, to make sure that is being read correctly.
    for (int i=0; i < 1; ++i) {
        cout << "For Node " << i << endl;
        Node* n = g.getNode(i);
        cout << "OutDegree: " << n->getOutDegree() << endl;
    
        vector<short>& out_cores = n->getOutCores();
        cout << "Out cores: " << endl;
        for (int i=0; i < out_cores.size(); ++i) {
            cout << out_cores[i] << "\t";
        }
        cout << endl;
    
        vector<pair<short, int> >& in_nodes = n->getIncomingNodes();
        cout << "In-nodes: " << endl;
        for (int i=0; i < in_nodes.size(); ++i) {
            cout << in_nodes[i].first << "-" << in_nodes[i].second << "\t";
        }
        cout << endl;
    }
    */
    
    ////////////////////////////////////
    // Read in total number of nodes. //
    ////////////////////////////////////
    
    // Declare an array of ints of size equals to number of MPI processes.
    int* vertex_count = (int *)malloc(sizeof(int) * world_size);
    // MPI All gather.
    int total_local_nodes = g.getTotalLocalNodes();
    MPI_Allgather(&total_local_nodes, 1, MPI_INT, vertex_count, 1, MPI_INT,
                  MPI_COMM_WORLD);

    for (int i=0; i < world_size; ++i) {
        g.updateTotalNodes(vertex_count[i]);        
    }
    MPI_Barrier(MPI_COMM_WORLD);
   
    if (host_id == 1) { 
        cout << "Total global nodes are: " << g.getTotalNodes() << endl;
    }
    
    /////////////////////////////////////////////////////////////////////
    // Agree upon number of messages to be received in each iteration. //
    /////////////////////////////////////////////////////////////////////
    
    // 1) Dispatch number of messages we would be sending out to all the cores.
    // 2) Initialize arrays of sizes received from the other MPI tasks.
    int* message_count = (int*) calloc(world_size, sizeof(int));
    for (int i=0; i < total_local_nodes; ++i) {
        Node* n = g.getNode(i);
        vector<short>& out_cores = n->getOutCores();
        for(std::vector<short>::iterator it = out_cores.begin(); it != out_cores.end(); ++it) {
            message_count[*it] += 1;
        }
    }
    
    if (host_id == 1) {
        for (int i=0; i < world_size; ++i) {
            cout << "Messages to: " << i << " would be " << message_count[i] << endl;
        }
    }
    int status;
    int* receive_message_count = (int*) malloc(sizeof(int) * world_size);
    // Notice here, the message size is 1, not world size. Because we distribute 1 message to every other core.
    // This is pretty cool:
    // http://stackoverflow.com/questions/15049190/difference-between-mpi-allgather-and-mpi-alltoall-functions
    status = MPI_Alltoall(message_count, 1, MPI_INT, receive_message_count, 1, MPI_INT, MPI_COMM_WORLD);

    if (status != MPI_SUCCESS) {
        cout << "MPI_Alltoall for sending message counts failed" << endl;
        exit(0);
    }
    if (host_id == 1) {
        for (int i=0; i < world_size; ++i) {
            cout << "Messages from: " << i << " would be " << receive_message_count[i] << endl;
        }
    }

    // Create appropriate data structures. 
    // 1) Compute initial page ranks.
    // 2) Start iterations.
    // 3) Inside each iteration, find new rank of each node and write down 
    //    update to the array corresponding to the respective MPI process.

    // Lets first start by sending all the updates to all other nodes.

    return 0;
}
