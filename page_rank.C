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

#define CONVERGENCE 1.0 / 10000
#define DAMPING_FACTOR 0.85
#define MAX_ITERATIONS 100
#define DEBUG 0
#define DEBUG_HOST 0

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

    if (host_id == DEBUG_HOST) {
        cout << "Reading " << filePath << endl;
    }
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
    
    double start_time_mpi = MPI_Wtime();

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
   
    if (DEBUG && (host_id == DEBUG_HOST)) { 
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
    
    if (DEBUG && (host_id == DEBUG_HOST)) {
        //for (int i=0; i < world_size; ++i) {
        //    cout << "Messages to: " << i << " would be " << message_count[i] << endl;
        //}
    }
    int status;
    int* receive_message_count = (int*) malloc(sizeof(int) * world_size);
    // Notice here, the message size is 1, not world size. Because we distribute 1 message to every other core.
    // This is pretty cool:
    // http://stackoverflow.com/questions/15049190/difference-between-mpi-allgather-and-mpi-alltoall-functions
    status = MPI_Alltoall(message_count, 1, MPI_INT, receive_message_count, 1, MPI_INT, MPI_COMM_WORLD);

    if (status != MPI_SUCCESS) {
        cout << "MPI_Alltoall for sending message counts failed" << endl;
        MPI_Finalize();
        exit(0);
    }
    if (DEBUG && (host_id == DEBUG_HOST)) {
        //for (int i=0; i < world_size; ++i) {
        //    cout << "Messages from: " << i << " would be " << receive_message_count[i] << endl;
        //}
    }

    // Create appropriate data structures. 
    // 1) Compute initial page ranks.
    // 2) Start iterations.
    // 3) Inside each iteration, find new rank of each node and write down 
    //    update to the array corresponding to the respective MPI process.

    ////////////////////////////////////////////////////////
    // Declare vectors to store updates from other hosts. //
    ////////////////////////////////////////////////////////
    float initial_rank = (1.0 / g.getTotalNodes());

    // All the rank related messages coming from other nodes would be kept here.
    // Rank message would be (nodes_rank / nodes_out_degree).
    vector<vector<float>*> rank_messages;
    for (int i=0; i < world_size; ++i) {
        rank_messages.push_back(new vector<float>(receive_message_count[i], initial_rank));
    }

    // This is where we store the rank of the final nodes.
    vector<float>* local_ranks = new vector<float>(g.getTotalLocalNodes(), initial_rank);
    
    //////////////////////////////////
    // Actual Page Rank computation //
    //////////////////////////////////
    if (DEBUG && (host_id == DEBUG_HOST)) {
        cout << "Checkpoint - 1" << endl;
    }
    // Compute the constant factor and store in advance,
    float const_factor = (1.0 - DAMPING_FACTOR) / g.getTotalNodes();
    for (int iteration=0; iteration < MAX_ITERATIONS; ++iteration) {
        vector<vector<float>*> outgoing_messages;
        for (int j=0; j < world_size; ++j) {
            outgoing_messages.push_back(new vector<float>());
        }

        if (DEBUG && (host_id == DEBUG_HOST)) {
            cout << "Checkpoint - 2" << endl;
        }
        float total_change = 0.0;
        // This is the main loop, where we find new values for
        // page rank and prepare vectors to be transferred to other
        // nodes. Also we collect vectors from other nodes.
        for (int j=0; j < total_local_nodes; ++j) {
            Node* node = g.getNode(j);

            vector<pair<short, int> >& in_nodes = node->getIncomingNodes();
            // Sum up the rank-factor of all the incoming nodes.
            float moving_factor = 0.0;
            for (std::vector<pair<short, int> >::iterator it=in_nodes.begin(); it != in_nodes.end(); ++it) {
                moving_factor += (*rank_messages[(*it).first])[(*it).second];    
            }

            float new_rank = const_factor + DAMPING_FACTOR * moving_factor;
            (*local_ranks)[j] = new_rank;

            // Update outgoing messages.
            float message = new_rank / node->getOutDegree();
            vector<short>& out_cores = node->getOutCores();
            for(std::vector<short>::iterator it = out_cores.begin(); it != out_cores.end(); ++it) {
                outgoing_messages[*it]->push_back(message);
            }
            //XXX TODO: Compute the total change in the final ranks.
        }

        if (DEBUG && (host_id == DEBUG_HOST)) {
            cout << "Checkpoint - 3" << endl;
        }
        // Once we have gone over all the nodes, we would dispatch outgoing messages.
//        int count = 1;
//        int sendTo, recvFrom;
//        while (count != world_size) {
//            sendTo = (host_id + count) % world_size;
//            recvFrom = host_id - count;
//            if (recvFrom < 0) {
//                recvFrom += world_size;
//            }
//            
//            if (outgoing_messages[sendTo]->size() != 0) {
//                if (DEBUG && (host_id == DEBUG_HOST)) {
//                    cout << "MPI Send from " << host_id << " to " << sendTo <<  " with message count " << outgoing_messages[sendTo]->size() << endl;
//                }
//                MPI_Send(&(outgoing_messages[sendTo]->front()), outgoing_messages[sendTo]->size(), MPI_FLOAT, sendTo, iteration, MPI_COMM_WORLD);
//            }
//            
//            if (rank_messages[recvFrom]->size() != 0) {
//                if (DEBUG && (host_id == DEBUG_HOST)) {
//                    cout << "MPI Receive from " << recvFrom << " to " << host_id <<  " with message count " << rank_messages[recvFrom]->size() << endl;
//                }
//                MPI_Recv(&(rank_messages[recvFrom]->front()), rank_messages[recvFrom]->size(), MPI_FLOAT, recvFrom, iteration, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//            }
//            count += 1;
//        }
//        // Locally copy messages for local host.
//        for (int i=0; i < outgoing_messages[host_id]->size(); ++i) {
//            (*rank_messages[host_id])[i] = (*outgoing_messages[host_id])[i];
//        }i

        MPI_Request *myRequest = new MPI_Request[world_size];
        for (int j=0; j < world_size; ++j) {
            if (host_id != j) {
                if (outgoing_messages[j]->size() != 0) {
                    if (DEBUG && (host_id == DEBUG_HOST)) {
                        cout << "MPI Send from " << host_id << " to " << j <<  " with message count " << outgoing_messages[j]->size() << endl;
                    }
                    MPI_Isend(&(outgoing_messages[j]->front()), outgoing_messages[j]->size(), MPI_FLOAT, j, iteration, MPI_COMM_WORLD, &myRequest[j]);
                }
            } else {
                for (int i=0; i < outgoing_messages[j]->size(); ++i) {
                    (*rank_messages[j])[i] = (*outgoing_messages[j])[i];
                }
            }
        }
        if (DEBUG && (host_id == DEBUG_HOST)) {
            cout << "Checkpoint - 4" << endl;
        }
        for (int j=0; j < world_size; ++j) {
            if (host_id != j) {
                if (rank_messages[j]->size() != 0) {
                    if (DEBUG && (host_id == DEBUG_HOST)) {
                        cout << "MPI Receive from " << j << " to " << host_id <<  " with message count " << rank_messages[j]->size() << endl;
                    }
                    MPI_Recv(&(rank_messages[j]->front()), rank_messages[j]->size(), MPI_FLOAT, j, iteration, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }
        }
        
        MPI_Status *status = new MPI_Status[world_size];
        for (int i=0; i < world_size; ++i) {
            if (host_id != i && outgoing_messages[i]->size() != 0) {
                MPI_Wait(&myRequest[i], &status[i]);
            }
        }

        if (host_id == DEBUG_HOST) {
            //cout << "Iteration " << iteration << " complete." << endl;
        }
    }

    if (DEBUG && (host_id == DEBUG_HOST)) {
        cout << "Checkpoint - 5" << endl;
    }
    if (DEBUG && host_id == DEBUG_HOST) {
        for (int i=0; i < local_ranks->size(); ++i) {
            //cout << "Rank of " << i << " is " << (*local_ranks)[i] << endl;
        }
    }
    double end_time_mpi = MPI_Wtime();
    MPI_Finalize();
    if (host_id == DEBUG_HOST) {
        cout << "Time to execute " << end_time_mpi - start_time_mpi << endl;
    }

    return 0;
}
