Node::Node() {
    this->out_degree = 0;
}

Node::~Node() {
    // Nothing to destroy.
}

uint64_t getOutDegree() {
    return this->out_degree;
}

void setOutDegree(uint64_t out_degree) {
    this->out_degree = out_degree;
}

vector<uint8_t>& getOutCores() {
    return out_cores;
}

void addOutCore(uint8_t core_no) {
    this->out_cores.push_back(core_no);
}

vector<pair<uint8_t, uint64_t>> getIncomingNodes() {
    return in_nodes;
}

void addIncomingNode(uint8_t core, uint64_t rank) {
    pair<uint8_t, uint64_t> cr(core, rank);
    this->in_nodes.push_back(cr);
}
