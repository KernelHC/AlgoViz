//
// Created by User on 12/23/2022.
//
#include <iostream>
#include <queue>
#include "Graph.h"
#include "Node.h"
#include <thread>
#include <mutex>

#define WAIT_TIME_MS 100

#define CHECK_IF_SHOULD_END \
if (should_end) {           \
    algo_thread_is_running = false; \
    is_finished = true;\
    return;                 \
    }



extern std::mutex window_lock;
extern bool algo_thread_is_running;
extern bool is_finished;
extern bool should_end;




Graph::Graph(const Graph &other) {
    this->nodes_num = other.nodes_num;
    this->edges_num = other.edges_num;
    this->name_count = other.name_count;
    this->start_node = other.start_node;
    this->nodes_list = other.nodes_list;
    this->neighbors_list = other.neighbors_list;
    this->edges_list = other.edges_list;
    this->directed = other.directed;
}

void Graph::render(sf::RenderTarget& target) {
    for (auto &node: this->nodes_list) {
        node.second->render(target);
    }
// rendered nodes first then edges to show edges when they cross nodes
    for (auto &node: this->nodes_list) {
        for (auto &edge: this->edges_list[node.first]) {
            edge->correctEdgeCoordinates();
            edge->render(target);
        }
    }
}


std::shared_ptr<Node> Graph::addNode(float pos_x, float pos_y) {
    std::string node_name = generateNodeName();
    std::shared_ptr<Node> node_ptr = std::make_shared<Node>(node_name, pos_x, pos_y);
    if (!checkValidPosition(*node_ptr)) return nullptr;
    this->nodes_list[node_name] = node_ptr;
    this->neighbors_list[node_name] = {};
    this->nodes_num++;
    this->name_count++;
    if (this->nodes_num == 1) setStartNode(node_ptr);
    return node_ptr;
}


void Graph::removeNode(const std::string& node_name) {
    if (this->nodes_list.find(node_name) == this->nodes_list.end()) return;
    bool start_node_isChanged;

    std::shared_ptr<Node> node_to_delete = this->nodes_list[node_name];

    // update the start node if it's the deleted node
    if (node_name == this->start_node->getName()) {
        start_node_isChanged = true;
    }

    // go over all the nodes' neighbors and delete the edges between them
    for (const std::shared_ptr<Edge>& edge : this->edges_list[node_name]) {
        if (edge->getFirstNode()->getName() == node_name) {
            this->edges_list[edge->getSecondNode()->getName()].erase(edge);
        }
        else {
            this->edges_list[edge->getFirstNode()->getName()].erase(edge);
        }
    }

    // erase the node from every neighboring node's list
    for (const std::shared_ptr<Node>& neighbor_node : this->neighbors_list[node_name]) {
        neighbors_list[neighbor_node->getName()].erase(node_to_delete);
    }

    this->nodes_list.erase(node_name);
    this->edges_list.erase(node_name); // delete edges list entry
    this->neighbors_list.erase(node_name); // erase the node's neighbors entry from hashmap
    if (start_node_isChanged) {
        if (this->nodes_list.empty()) this->start_node = nullptr;
        else setStartNode(nodes_list.begin()->second);
    }
    this->nodes_num--;
}


void Graph::addEdge(std::shared_ptr<Edge>& edge) {
    if (this->containsEdge(edge)) return;
    std::shared_ptr<Node> node1 = edge->getFirstNode();
    std::shared_ptr<Node> node2 = edge->getSecondNode();
    this->edges_num++;
    this->edges_list[edge->getFirstNode()->getName()].insert(edge);
    this->edges_list[edge->getSecondNode()->getName()].insert(edge); // done twice because each edge exists in 2 lists, one for each node it connects
    this->neighbors_list[node1->getName()].insert(node2);
    this->neighbors_list[node2->getName()].insert(node1);
}


void Graph::removeEdge(const std::shared_ptr<Edge>& to_delete) {
    this->edges_list[to_delete->getFirstNode()->getName()].erase(to_delete);
    this->edges_list[to_delete->getSecondNode()->getName()].erase(to_delete);
    std::shared_ptr<Node> firstNode = to_delete->getFirstNode();
    std::shared_ptr<Node> secondNode = to_delete->getSecondNode();
    this->neighbors_list[firstNode->getName()].erase(secondNode);
    this->neighbors_list[secondNode->getName()].erase(firstNode);
    this->edges_num--;
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
    std::string node1_name = edge->getFirstNode()->getName();
    std::string node2_name = edge->getSecondNode()->getName();

    if (this->edges_list.find(node1_name) == this->edges_list.end() ||
        this->edges_list.find(node2_name) == this->edges_list.end())
        return false;

    for (const auto& current_edge: edges_list[edge->getFirstNode()->getName()]) {
        if (current_edge->getFirstNode()->getName() == node1_name && current_edge->getSecondNode()->getName() == node2_name ||
            current_edge->getSecondNode()->getName() == node1_name && current_edge->getFirstNode()->getName() == node2_name)
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

void Graph::setToggledNode(std::shared_ptr<Node>& to_toggle) {
    this->untoggle();
    if (to_toggle)
        to_toggle->toggle();
    this->toggled_node = to_toggle;
}

void Graph::runBFS(sf::RenderWindow& window, Toolbar& toolbar, bool wait) {
    algo_thread_is_running = true;
    this->untoggle();
    if (!this->start_node) return;

    std::queue<std::shared_ptr<Node>> bfs_q;
    std::shared_ptr<Node> previous_node;
    std::shared_ptr<Edge> current_edge;
    bfs_q.push(this->start_node);
    bfs_q.front()->setState(NODE_CURRENT);
    CHECK_IF_SHOULD_END;
    this->renderAndWait(window, toolbar, wait);

    while (!bfs_q.empty()) {
        CHECK_IF_SHOULD_END;
        previous_node = bfs_q.front();
        if (previous_node->getState() == NODE_DONE) {
            bfs_q.pop();
            continue;
        }
        previous_node->setState(NODE_CURRENT);
        for (const std::shared_ptr<Node>& current_node: this->neighbors_list[previous_node->getName()]) {
            CHECK_IF_SHOULD_END;
            if (current_node->getState() != NODE_DONE && current_node->getState() != NODE_DISCOVERED) {
                current_edge = getEdgeByNodes(previous_node, current_node);
                current_edge->setState(EDGE_DISCOVERED);
                bfs_q.push(current_node);
                current_node->setState(NODE_DISCOVERED);
                current_node->setDistance(1);
                this->renderAndWait(window, toolbar, wait);
            }
        }
        previous_node->setState(NODE_DONE);
        bfs_q.pop();
        CHECK_IF_SHOULD_END;
        this->renderAndWait(window, toolbar, wait);
    }

    if (wait) {
        is_finished = true;
    }
    algo_thread_is_running = false;
}


void Graph::runDFS(sf::RenderWindow& window, Toolbar& toolbar, bool wait) {
    algo_thread_is_running = true;
    this->untoggle();
    if (!this->start_node) return;

    dfs(nullptr, this->start_node, window, toolbar, wait);

    if (wait) {
        is_finished = true;
    }
    algo_thread_is_running = false;
}


void Graph::dfs(const std::shared_ptr<Node>& prev_node, const std::shared_ptr<Node>& curr_node, sf::RenderWindow& window, Toolbar& toolbar, bool wait) {
    CHECK_IF_SHOULD_END
    if (prev_node) {
        getEdgeByNodes(prev_node, curr_node)->setState(EDGE_DISCOVERED);
    }
    curr_node->setState(NODE_DISCOVERED);
    this->renderAndWait(window, toolbar, wait);
    for (const std::shared_ptr<Node>& neighbor_node : this->neighbors_list[curr_node->getName()]) {
        CHECK_IF_SHOULD_END
        if (neighbor_node->getState() != NODE_DISCOVERED && neighbor_node->getState() != NODE_DONE)
            dfs(curr_node, neighbor_node, window, toolbar, wait);
    }
    curr_node->setState(NODE_DONE);
    CHECK_IF_SHOULD_END
    this->renderAndWait(window, toolbar, wait);
}

void Graph::runMST(sf::RenderWindow& window, Toolbar& toolbar, bool wait) {

}


std::shared_ptr<Node> Graph::dijkstraMinDistance() const {
    int min = INT_MAX;
    std::shared_ptr<Node> min_distance_node;
    // finding the node with the min distance
    for (const auto& node: nodes_list) {
        if (node.second->getState() != NODE_DONE && node.second->getState() != NODE_DISCOVERED && node.second->getDistance() <= min) {
            min = node.second->getDistance();
            min_distance_node = node.second;
        }
    }
    return min_distance_node;
}

void Graph::runDijkstra(sf::RenderWindow& window, Toolbar& toolbar, bool wait) {
    algo_thread_is_running = true;
    this->untoggle();
    if (!this->start_node) return;

    std::unordered_map<std::shared_ptr<Node>, std::shared_ptr<Edge>> discovered_edges;
    this->start_node->setDistance(0);

    for (int i = 0; i < nodes_list.size(); i++) {
        std::shared_ptr<Node> current_node = dijkstraMinDistance();
        current_node->setState(NODE_DISCOVERED);
        if (current_node != this->start_node) {
            discovered_edges[current_node]->setState(EDGE_DISCOVERED);
        }
        CHECK_IF_SHOULD_END
        this->renderAndWait(window, toolbar, wait);
        if (current_node->getDistance() < INT_MAX) {
            for (const std::shared_ptr<Node> &neighbor_node: this->neighbors_list[current_node->getName()]) {
                // updating the distance of neighboring nodes
                std::shared_ptr<Edge> edge = getEdgeByNodes(current_node, neighbor_node);
                int edge_weight = edge->getLength();
                if (neighbor_node->getState() == NODE_UNDISCOVERED && current_node->getDistance() + edge_weight < neighbor_node->getDistance()) {
                    neighbor_node->setDistance(current_node->getDistance() + edge_weight);
                    discovered_edges[neighbor_node] = edge;
                }
            }
        }
        current_node->setState(NODE_DONE);
        CHECK_IF_SHOULD_END
        this->renderAndWait(window, toolbar, wait);
    }

    if (wait) is_finished = true;
    algo_thread_is_running = false;
}

void Graph::reset() {
    this->untoggle();
    for (const auto& node: nodes_list) {
        if (node.second == this->start_node) {
            node.second->setState(NODE_START);
        }
        else {
            node.second->setState(NODE_UNDISCOVERED);
        }
        node.second->setDistance(INT_MAX);
    }

    for (const auto& node: edges_list) {
        for (const auto& edge: node.second) {
            edge->setState(EDGE_UNDISCOVERED);
        }
    }
}

//**********************************************************************************************************************
// private helper functions:
void Graph::renderAndWait(sf::RenderWindow& window, Toolbar& toolbar, bool wait) {
    if (!wait) return;
    window_lock.lock();
    window.setActive(true);
    window.clear(BG_COLOR);
    this->render(window);
    toolbar.render(window);
    window.display();
    window.setActive(false);
    window_lock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_MS));
}


std::string Graph::generateNodeName() const {
    return "node_" + std::to_string(this->name_count);
}

bool Graph::checkValidPosition(const Node& node) const {
    for (auto& pair : this->nodes_list) {
        if (node.checkBoundsCollision(pair.second) && node.getName() != pair.second->getName()) {
            return false;
        }
    }
    return true;
}


std::shared_ptr<Edge> Graph::getEdgeByNodes(const std::shared_ptr<Node>& node1, const std::shared_ptr<Node>& node2) {
    for (const auto& edge: this->edges_list[node1->getName()]) {
        if (edge->getSecondNode()->getName() == node2->getName() || edge->getFirstNode()->getName() == node2->getName())
            return edge;
    }
    return nullptr;
}

void Graph::untoggle() {
    if (this->toggled_node)
        this->toggled_node->untoggle();
    this->toggled_node = nullptr;
}

