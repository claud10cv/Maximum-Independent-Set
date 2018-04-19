#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include "Reductions.hpp"

using namespace std;

void Reductions::run() {
    cout << " \nReductions\n";
    //printCC();
    reduce();
    //graph.printEdgeCounts();
    //graph.printWithGraphTraversal(true);
    graph.print(true);
    printCCSizes();
}

void Reductions::reduce() {
    bool firstTime = true;
    while (removeUnconfinedNodes() || firstTime) {
        if (firstTime) {
            firstTime = false;
        }
        if (!foldCompleteKIndependentSets()) {
            break;
        }
    }
    buildCC();
    printCCSizes();
    removeLineGraphs();
    graph.rebuild(nodesWithoutSortedNeighbors, reduceInfo);
}

bool Reductions::foldCompleteKIndependentSets() {
    cout << "\n**Performing K-Independent set folding reduction**" << endl;
    ReduceInfo old = reduceInfo;
    foldCompleteKIndependentSets2();
    if (old.nodesRemoved == reduceInfo.nodesRemoved) {
        return false;
    }
    do {
        reduceInfo.print(&old);
        old = reduceInfo;
        foldCompleteKIndependentSets2();
    } while (old.nodesRemoved != reduceInfo.nodesRemoved);
    return true;
}


bool Reductions::removeUnconfinedNodes() {
    cout << "\n**Performing unconfined nodes reduction**" << endl;
    ReduceInfo old = reduceInfo;
    removeUnconfinedNodes2();
    if (old.nodesRemoved == reduceInfo.nodesRemoved) {
        return false;
    }
    do {
        reduceInfo.print(&old);
        old = reduceInfo;
        removeUnconfinedNodes2();
        //assert(old.nodesRemoved == reduceInfo.nodesRemoved);
    } while (old.nodesRemoved != reduceInfo.nodesRemoved);
    return true;
}

void Reductions::foldCompleteKIndependentSets2() {
    Graph::GraphTraversal graphTraversal(graph);
    uint32_t bound = 25000;
    while (graphTraversal.curNode != NONE) {
        for (uint32_t k = 1 ; k <= 2 ; k++) {
            if (graph.getNodeDegree(graphTraversal.curNode) == k+1) {
                if (graphTraversal.curNode >= bound) {
                    cout << "Node " << graphTraversal.curNode << "\n";
                    bound += 25000;
                }
                vector<uint32_t> nodes;
                vector<uint32_t> neighbors;
                nodes.push_back(graphTraversal.curNode);
                graph.gatherNeighbors(graphTraversal.curNode, neighbors);
                if (k == 2) {
                    uint32_t secondNode = graph.getNextNodeWithIdenticalNeighbors(graphTraversal.curNode, neighbors);
                    if (secondNode != NONE) {
                        nodes.push_back(secondNode);
                    }
                }
                if (k == 1 || k == 2 && nodes.size() == 2) {
                    vector<uint32_t> &mis = this->mis.getMis();
                    if (graph.isIndependentSet(neighbors, nodesWithoutSortedNeighbors)) {
                        uint32_t newNode = graph.contractToSingleNode(nodes, neighbors, nodesWithoutSortedNeighbors, reduceInfo);
                        this->mis.markHypernode(newNode, nodes, neighbors);
                    } else {
                        mis.insert(mis.end(), nodes.begin(), nodes.end());
                    }
                    neighbors.insert(neighbors.end(), nodes.begin(), nodes.end());
                    graph.remove(neighbors, reduceInfo);
                    //graph.print(true);
                }
                break;
            }
        }
        graph.getNextNode(graphTraversal);
    }
}

void Reductions::removeUnconfinedNodes2() {
    Graph::GraphTraversal graphTraversal(graph);
    while (graphTraversal.curNode != NONE) {
        //cout << "node " << graphTraversal.curNode << "\n";
        bool isUnconfined = false;
        vector<uint32_t> extendedGrandchildren;
        while (graphTraversal.curEdgeOffset != NONE) {
            uint32_t neighbor = graph.edgeBuffer[graphTraversal.curEdgeOffset];
            uint32_t outerNeighbor;
            bool exactlyOne;
            graph.getOuterNeighbor(graphTraversal.curNode, neighbor, nodesWithoutSortedNeighbors, outerNeighbor, exactlyOne);
            if (outerNeighbor == NONE) {
                //cout << "none\n";
                isUnconfined = true;
                break;
            } else if (exactlyOne) {
                //cout << "exactly one\n";
                extendedGrandchildren.push_back(outerNeighbor);
            }
            graph.getNextEdge(graphTraversal);
        }
        if (isUnconfined || !graph.isIndependentSet(extendedGrandchildren, nodesWithoutSortedNeighbors)) {
            cout << "Unconfined node " << graphTraversal.curNode << "\n";
            graph.remove(graphTraversal.curNode, reduceInfo);
        }
        graph.getNextNode(graphTraversal);
    }
}

void Reductions::removeLineGraphs() {
    vector<unordered_map<uint32_t, vector<uint32_t>* >::iterator> removedCCs;
    for (auto it = ccToNodes.begin() ; it != ccToNodes.end() ; it++) {
        vector<uint32_t> *cc = it->second;
        vector<Graph::GraphTraversal> clique1;
        vector<Graph::GraphTraversal> clique2;
        clique1.reserve(5);
        clique2.reserve(5);
        for (uint32_t i = 0 ; i < 3 ; i++) {
            uint32_t nodes, degree, clique1Size, clique2Size;
            switch (i) {
                case 0:
                    nodes = 10;
                    degree = 6;
                    clique1Size = clique2Size = 4;
                    break;
                case 1:
                    nodes = 15;
                    degree = 8;
                    clique1Size = clique2Size = 5;
                    break;
                case 2:
                    nodes = 20;
                    degree = 7;
                    clique1Size = 4;
                    clique2Size = 5;
                    break;
            }
            if (cc->size() == nodes && nodeDegreesEqualTo(*cc, degree, graph)) {
                bool foundCliques = true;
                for (auto node : *cc) {
                    clique1.clear();
                    clique2.clear();
                    clique1.push_back(Graph::GraphTraversal(graph, node));
                    if (!findClique(clique1, NULL, clique1Size)) {
                        foundCliques = false;
                        break;
                    }
                    cout << "Clique1: " << endl;
                    for (uint32_t i = 0 ; i < clique1.size() ; i++) {
                        cout << clique1[i].curNode << endl;
                    }
                    clique2.push_back(Graph::GraphTraversal(graph, node));
                    if (!findClique(clique2, &clique1, clique2Size)) {
                        foundCliques = false;
                        break;
                    }
                    cout << "Clique2: " << endl;
                    for (uint32_t i = 0 ; i < clique2.size() ; i++) {
                        cout << clique2[i].curNode << endl;
                    }
                }
                if (foundCliques) {
                    cout << "Removing component " << it->first << "\n";
                    findMisInComponent(*cc);
                    graph.remove(*cc, reduceInfo, true);
                    for (auto node : *cc) {
                        nodeToCC.erase(node);
                    }
                    removedCCs.push_back(it);
                    break;
                } else {
                    break;
                }
            }
        }
    }
    for (auto cc : removedCCs) {
        ccToNodes.erase(cc);
    }
}

void Reductions::findMisInComponent(const vector<uint32_t> &cc) {
    unordered_set<uint32_t> removedNodes;
    for (auto node : cc) {
        if (removedNodes.find(node) == removedNodes.end()) {
            mis.getMis().push_back(node);
            Graph::GraphTraversal graphTraversal(graph, node);
            while (graphTraversal.curEdgeOffset != NONE) {
                removedNodes.insert(graph.edgeBuffer[graphTraversal.curEdgeOffset]);
                graph.getNextEdge(graphTraversal);
            }
        }
    }
}

bool Reductions::findClique(vector<Graph::GraphTraversal> &clique, vector<Graph::GraphTraversal> *previousClique, const uint32_t &cliqueSize) {
    Graph::GraphTraversal graphTraversal = clique[0];
    if (graphTraversal.curNode == NONE) {
        return false;
    }
    while (clique.size() < cliqueSize) {
        //if (clique[0].curNode == 11) {
        //cout << "Cur clique: \n";
        //for (uint32_t i = 0 ; i < clique.size() ; i++) {
        //cout << clique[i].curNode << endl;
        //}
        //}
        uint32_t neighbor = graph.edgeBuffer[graphTraversal.curEdgeOffset];
        bool existsInPreviousClique = (previousClique == NULL ? false : find(neighbor, *previousClique));
        //cout << "node " << graphTraversal.curNode << " neighbor " << neighbor << endl;
        if (!existsInPreviousClique && !find(neighbor, clique) && isSubsetOfNeighbors(clique, neighbor, graph)) {
            //cout << "going to " << neighbor << endl;
            //cout << "commonNode is " << commonNode << endl;
            graph.goToNode(neighbor, graphTraversal);
            clique.push_back(graphTraversal);
        } else {
            if (!advance(clique, graphTraversal, graph)) {
                return false;
            }
        }
    }
    return true;
}

void Reductions::buildCC() {
    uint32_t component = 0;
    unordered_set<uint32_t> exploredSet;
    stack<uint32_t> frontier;
    Graph::GraphTraversal graphTraversal(graph);
    while (graphTraversal.curNode != NONE) {
        if (exploredSet.insert(graphTraversal.curNode).second) {
            vector<uint32_t> *componentNodes = new vector<uint32_t>();
            componentNodes->push_back(graphTraversal.curNode);
            nodeToCC.insert({graphTraversal.curNode, component});
            frontier.push(graphTraversal.curNode);
            while (!frontier.empty()) {
                uint32_t node = frontier.top();
                frontier.pop();
                Graph::GraphTraversal neighbors(graph, node);
                while (neighbors.curEdgeOffset != NONE) {
                    node = graph.edgeBuffer[neighbors.curEdgeOffset];
                    if (exploredSet.insert(node).second) {
                        frontier.push(node);
                        nodeToCC.insert({node, component});
                        componentNodes->push_back(node);
                    }
                    graph.getNextEdge(neighbors);
                }
            }
            ccToNodes.insert({component, componentNodes});
            component++;
        }
        graph.getNextNode(graphTraversal);
    }
}

Reductions::~Reductions() {
    for (auto &cc : ccToNodes) {
        delete cc.second;
    }
}

void Reductions::printCC() const {
    for (auto &cc : ccToNodes) {
        cout << "\nCC " << cc.first << ":\n";
        for (auto node : *(cc.second)) {
            cout << node << ", belongs to cc " << nodeToCC.at(node) << "\n";
        }
    }
}

void Reductions::printCCSizes() const {
    for (auto &cc : ccToNodes) {
        cout << "CC " << cc.first << " size: " << (cc.second)->size() << "\n";
    }
}