// This is the MPI implementation of the page rank algorithm.

#include <mpi.h>

#include <iostream>
#include <vector>
#include <list>

using namespace std;

class Node {
    uint64_t vertex;
    float rank;
    std::list<unsigned> edges;
};

int main() {
    MPI_Init(NULL, NULL);
     
    // Start-MPI
    uint64_t size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    uint64_t host_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &host_id);

    // Read in the file.
    std::vector<Node> graph;

    // Create appropriate data structures. 

    // Lets first start by sending all the updates to all other nodes.

    return 0;
}
