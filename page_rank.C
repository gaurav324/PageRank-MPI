// This is the MPI implementation of the page rank algorithm.

#include <iostream>
#include <vector>
#include <list>

#include <mpi.h>
#include <stdint.h>

#include "Graph.h"
#include "Node.h"

using namespace std;
int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);
     
    // Start-MPI
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int host_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &host_id);

    // Read in the file.
    string filePath = argv[1];
    filePath += ".split." + host_id;

    Graph g;
    g.ingestFile(filePath);

    Node* n = g.getNode(0);
    cout << "OutDegree: " << n->getOutDegree() << endl;

    vector<short>& out_cores = n->getOutCores();
    cout << "Out cores: " << endl;
    for (int i=0; i < out_cores.size(); ++i) {
        cout << out_cores[i] << "\t";
    }
    cout << endl;

    vector<pair<short, int> > in_nodes = n->getIncomingNodes();
    cout << "In-nodes: " << endl;
    for (int i=0; i < in_nodes.size(); ++i) {
        cout << in_nodes[i].first << "-" << in_nodes[i].second << "\t";
    }
    cout << endl;

    // Create appropriate data structures. 

    // Lets first start by sending all the updates to all other nodes.

    return 0;
}
