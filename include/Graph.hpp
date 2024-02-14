#pragma once

#include <../Engine/include/MathsUtils.hpp>
#include <vector>
#include <iostream>

#define MAX_NEIGHBORS 8
#define START_GRAPH 16

typedef std::pair<int, int> link;

class Node {

    private:

        vec3 position;
        link links[MAX_NEIGHBORS];
        int id;
        int neighborsN;


    public:

        Node(int, vec3);

        ~Node();

        vec3 getPosition() {return position;};
        link getLink(int i) {return links[i];};
        int getId() {return id;};
        int getNeighborsN() {return neighborsN;};

        void connectNode(int, int);

        void print();
        

};

class Graph {

    private:

        std::vector<Node> nodes;
        int nodesN;
        int id;

    public:

        Graph(int);

        void addNode(Node&, link[8]);

        void print();

};
