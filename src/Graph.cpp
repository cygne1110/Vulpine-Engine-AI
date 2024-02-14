#include <Graph.hpp>

Node::Node(int _id, vec3 _position) {

    id = _id;
    position = _position;
    for(int i = 0; i < MAX_NEIGHBORS; i++) links[i] = std::make_pair(-1, -1);
    neighborsN = 0;

}

Node::~Node() {

}

void Node::connectNode(int other, int cost) {

    if(neighborsN >= MAX_NEIGHBORS) {

        // FLAG_CERR
        std::cerr << "Node " << id << " has already 8 neighbors\n";
        return;

    }

    links[neighborsN] = std::make_pair(other, cost);

    neighborsN++;

}

void Node::print() {

    std::cout << "Node "
    << id << ", in pos ("
    << position[0] << ", "
    << position[1] << ", "
    << position[2] << "), "
    << "is connected to :";

    for(int i = 0; i < neighborsN; i++) {

        std::cout << " (" << links[i].first << ", c: " << links[i].second << ")";

    }

    std::cout << "\n";

}

Graph::Graph(int _id) {

    id = _id;
    nodes.reserve(START_GRAPH);
    nodesN = 0;

}

void Graph::addNode(Node& node, link links[8]) {

    for(int i = 0; i < MAX_NEIGHBORS; i++) {

        if(links[i].first == -1) break;
        node.connectNode(links[i].first, links[i].second);

    }

    nodes.push_back(node);
    nodesN++;

}

void Graph::print() {

    std::cout << "Graph "
    << id << ", nodes are: \n";
    for(int i = 0; i < nodesN; i++) {

        nodes[i].print();

    }

    std::cout << "\n";

}