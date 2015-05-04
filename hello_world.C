// This is the MPI implementation of the page rank algorithm.

#include <iostream>
#include <vector>
#include <list>

#include <mpi.h>

using namespace std;
int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);
     
    // Start-MPI
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int host_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &host_id);

    cout << "Size: " << size;
    // Create appropriate data structures. 

    // Lets first start by sending all the updates to all other nodes.

    return 0;
}
