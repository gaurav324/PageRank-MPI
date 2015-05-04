#include <cstdlib>

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
        string deleimitor1 = "#";
        string delimitor2 = ",";
        while (getline(myfile, line)) {
            Node *n = new Node();

            int first_hash_index = line.find(delimitor1);
            int second_hash_index = line.find(delimitor2);

            // Set out degree.
            uint64_t out_degree = atol(line.substr(0, first_hash_index).c_str());
            n->setOutDegree(out_degree);

            // Set all the out cores for this node.
            std::stringstream ss(line.substr(first_hash_index, second_hash_index - first_hash_index));
            std::string item;
            while (std::getline(ss, item, delimitor2)) {
                uint8_t core_no = atoi(item);
                n->addOutCore(core_no);
            }

            // Set all the incoming nodes.
            std::stringstream ss(line.substr(second_hash_index, line.length()).c_str());
            std::string item;
            while (std::getline(ss, item, delimitor2)) {
                int core_rank_split_index = item.split("-");
                uint8_t core = atoi(item.substr(0, core_rank_split_index).c_str());
                uint64_t rank = atoi(item.substr(core_rank_split_index, item.length()).cstr());
                n->addIncomingNode(core, rank);
            }
    }
}

// Return node on index.
Node* getNode(int index) {
    return this->nodes[index];
}

// Return total local nodes;
uint64_t getTotalLocalNodes() {
    return this->nodes.size();
}

// Return total number of nodes across all MPI nodes.
uint64_t getTotalNodes() {
    return this->total_node_count;
}

// Update total nodes.
void updateTotalNodes(uint64_t count) {
    this->total_node_count += count;
}
