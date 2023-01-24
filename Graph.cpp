//
// Created by User on 12/23/2022.
//
#include <queue>
#include "Graph.h"
#include "Node.h"
#include <thread>
#include <mutex>

std::mutex m1;
extern bool algo_thread_is_running;

void Graph::render(sf::RenderTarget& target) {
    for (auto &node: this->nodes_list) {
        node.second->render(target);
        for (auto &edge: this->edges_list[node.first]) {
            edge->render(target);
        }
    }
}

std::string Graph::generateName() const {
    return "node_" + std::to_string(this->name_count);
}

bool Graph::checkValidPos(const Node& node) const {
    for (auto& pair : this->nodes_list) {
        if (node.checkCollision(pair.second)) return false;
    }
    //Todo: set x and y limits
    return true;
}

void Graph::addNode(float pos_x, float pos_y) {
    std::string node_name = generateName();
    std::shared_ptr<Node> node_ptr = std::make_shared<Node>(node_name, pos_x, pos_y);
    if (!checkValidPos(*node_ptr)) return; //TODO: exception
    this->nodes_list[node_name] = node_ptr;
    this->neighbors_list[node_name] = {};
    this->nodes_num++;
    this->name_count++;
    if (this->nodes_num == 1) setStartNode(node_ptr);
}


void Graph::removeNode(const std::string& node_name) {
    if (this->nodes_list.find(node_name) == this->nodes_list.end()) return;

    this->nodes_list.erase(node_name);
    this->nodes_num--;

    // update the start node if it's the deleted node
    if (!this->nodes_list.contains(this->start_node->getName())) {
        if (this->nodes_list.empty()) this->start_node = nullptr;
        else setStartNode(nodes_list.begin()->second);
    }

    // go over all the nodes' neighbors and delete the edges between them
    for (const auto& neighbor : this->neighbors_list[node_name]) { // for every neighbor of the node with the given name
        for (const auto& edge : this->edges_list[neighbor->getName()]) { // for every edge touching the neighbor
            if (edge->getNode1()->getName() == node_name || edge->getNode2()->getName() == node_name) { // if it also touches the node with the given name
                this->edges_list[neighbor->getName()].erase(edge); // delete the edge
                this->edges_num--;
            }
        }
    }

    this->edges_list.erase(node_name); // delete edges list entry

    this->neighbors_list.erase(node_name); // erase the node's neighbors entry from hashmap

    // erase the node from every neighboring node's list
    for (auto neighbors: neighbors_list) { // tried to use structured binding which requires c++20, but didn't work for some reason
        for (const auto& node : neighbors.second) {
            if (node->getName() == node_name) neighbors.second.erase(node);
        }
    }

}


void Graph::addEdge(const std::shared_ptr<Edge>& edge) {
    if (this->containsEdge(edge)) return;
    std::string node1_name = edge->getNode1()->getName();
    std::string node2_name = edge->getNode2()->getName();
    this->edges_num++;
    this->edges_list[edge->getNode1()->getName()].insert(edge);
    this->edges_list[edge->getNode2()->getName()].insert(edge); // done twice because each edge exists in 2 lists, one for each node it connects
    this->neighbors_list[node1_name].insert(edge->getNode2());
    this->neighbors_list[node2_name].insert(edge->getNode1());
}


void Graph::removeEdge(const std::shared_ptr<Edge>& to_delete) {
    this->edges_num--;
    for (const auto& edge : this->edges_list[to_delete->getNode1()->getName()]) {
        if (edge->getNode1()->getName() == to_delete->getNode1()->getName())
            this->edges_list[to_delete->getNode1()->getName()].erase(edge);
    }

    for (const auto& edge : this->edges_list[to_delete->getNode2()->getName()]) {
        if (edge->getNode2()->getName() == to_delete->getNode2()->getName())
            this->edges_list[to_delete->getNode2()->getName()].erase(edge);
    }
}

const std::shared_ptr<Node>& Graph::getStartNode() const {
    return this->start_node;
}

int Graph::getNodesNum() const {
    return nodes_num;
}

int Graph::getEdgesNum() const {
    return edges_num;
}

bool Graph::containsNode(const std::string& node_name) {
    return (this->nodes_list.find(node_name) != this->nodes_list.end());
}



bool Graph::containsEdge(const std::shared_ptr<Edge>& edge) {
    std::string node1_name = edge->getNode1()->getName();
    std::string node2_name = edge->getNode2()->getName();
    Edge reverse_edge(edge->getNode2(), edge->getNode1());

    if (this->edges_list.find(node1_name) == this->edges_list.end() ||
        this->edges_list.find(node2_name) == this->edges_list.end())
        return false;

    for (const auto& current_edge: edges_list[edge->getNode1()->getName()]) {
        if (current_edge->getNode1()->getName() == node1_name && current_edge->getNode2()->getName() == node2_name ||
            current_edge->getNode2()->getName() == node1_name && current_edge->getNode1()->getName() == node2_name)
            return true;
    }

    return false;
}

void Graph::setStartNode(const std::shared_ptr<Node>& new_start_node) {
    if (!new_start_node) return;
    if (this->start_node) this->start_node->setState(NODE_UNDISCOVERED);
    this->start_node = new_start_node;
    this->start_node->setDistance(0);
    this->start_node->setPathWeight(0);
    this->start_node->setState(NODE_START);
    this->start_node->setColor(sf::Color::Yellow);
}

bool Graph::hasNegativeCircle() {
    return false;
}

std::shared_ptr<Node> Graph::getNodeByPosition(float pos_x, float pos_y) {
    for (const auto& node: this->nodes_list) {
        if (node.second->getShape().getGlobalBounds().contains(pos_x, pos_y)) {
            return node.second;
        }
    }
    return nullptr;
}

std::shared_ptr<Edge> Graph::getEdgeByPosition(float pos_x, float pos_y) {
    for (const auto& edges: edges_list) {
        for (const auto& edge: edges.second) {
            if (edge->getShape().getGlobalBounds().contains(pos_x, pos_y)) {
                return edge;
            }
        }
    }
    return nullptr;
}

void Graph::runBFS() {
//    m1.lock();
//    algo_thread_is_running = true;
//    std::queue<std::shared_ptr<Node>> bfs_q;
//
//
//    bfs_q.push(this->start_node);
//    bfs_q.front()->setState(NODE_CURRENT);
//    std::this_thread::sleep_for(std::chrono::milliseconds(150));
//    while (!bfs_q.empty()) {
//        std::shared_ptr current_node = bfs_q.front();
//        for (const auto& node: this->neighbors_list[current_node->getName()]) {
//
//        }
//    }
//    m1.unlock();
}

void Graph::runDFS() {

}

void Graph::runMST() {

}

void Graph::runDijkstra() {

}
