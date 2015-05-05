#include "Node.h"

Node::Node() {
    this->out_degree = 0;
}

Node::~Node() {
    // Nothing to destroy.
}

int Node::getOutDegree() {
    return this->out_degree;
}

void Node::setOutDegree(int out_degree) {
    this->out_degree = out_degree;
}

vector<short>& Node::getOutCores() {
    return out_cores;
}

void Node::addOutCore(short core_no) {
    this->out_cores.push_back(core_no);
}

vector<pair<short, int> >& Node::getIncomingNodes() {
    return in_nodes;
}

void Node::addIncomingNode(short core, int rank) {
    pair<short, int> cr(core, rank);
    this->in_nodes.push_back(cr);
}
