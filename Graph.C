#include <cstdlib>
#include <sstream>
#include "Graph.h"

// Constructor.
Graph::Graph() {
    this->total_node_count = 0;
}

// Destructor.
Graph::~Graph() {
    // Nothing to destroy.
}

// Read in graph.
void Graph::ingestFile(std::string path) {
    ifstream myfile (path.c_str());

    string line;
    if (myfile.is_open()) {
        char delimitor1 = '#';
        char delimitor2 = ',';
        while (getline(myfile, line)) {
            Node *n = new Node();

            int first_hash_index = line.find(delimitor1);
            int second_hash_index = line.rfind(delimitor1);

            // Set out degree.
            int out_degree = atol(line.substr(0, first_hash_index).c_str());
            n->setOutDegree(out_degree);

            // Set all the out cores for this node.
            std::stringstream ss(line.substr(first_hash_index + 1, second_hash_index - first_hash_index - 1));
            std::string item;
            while (std::getline(ss, item, delimitor2)) {
                short core_no = atoi(item.c_str());
                n->addOutCore(core_no);
            }

            // Set all the incoming nodes.
            if (line.length() > (second_hash_index + 1)) {
                std::stringstream ss1(line.substr(second_hash_index + 1, line.length() - 1).c_str());
                std::string item1 = "";
                while (std::getline(ss1, item1, delimitor2)) {
                    int core_rank_split_index = item1.find("-");
                    short core = atoi(item1.substr(0, core_rank_split_index).c_str());
                    int rank = atoi(item1.substr(core_rank_split_index + 1, item1.length() - 1).c_str());
                     n->addIncomingNode(core, rank);
                }
            }

            this->nodes.push_back(n);
        }
    }
}

// Return node on index.
Node* Graph::getNode(int index) {
    return this->nodes[index];
}

// Return total local nodes;
int Graph::getTotalLocalNodes() {
    return this->nodes.size();
}

// Return total number of nodes across all MPI nodes.
int Graph::getTotalNodes() {
    return this->total_node_count;
}

// Update total nodes.
void Graph::updateTotalNodes(int count) {
    this->total_node_count += count;
}
