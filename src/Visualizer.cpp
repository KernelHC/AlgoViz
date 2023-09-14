#include "Visualizer.h"
#include "utils.h"
#include "Node.h"
#include "Interface.h"
#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <cmath>
#include <regex>


#define EVENT_X (this->window->mapPixelToCoords( \
sf::Vector2i(this->sfEvent.mouseButton.x,this->sfEvent.mouseButton.y)).x)

#define EVENT_Y (this->window->mapPixelToCoords( \
sf::Vector2i(this->sfEvent.mouseButton.x,this->sfEvent.mouseButton.y)).y)

#define MOUSE_X (this->window->mapPixelToCoords( \
sf::Vector2i((float)sf::Mouse::getPosition(*this->window).x, \
(float)sf::Mouse::getPosition(*this->window).y)).x)

#define MOUSE_Y (this->window->mapPixelToCoords( \
sf::Vector2i((float)sf::Mouse::getPosition(*this->window).x, \
(float)sf::Mouse::getPosition(*this->window).y)).y)

#define CORRECTED_EVENT_X   (this->window->mapPixelToCoords( \
sf::Vector2i(this->sfEvent.mouseButton.x-(30*this->current_zoom_factor), \
this->sfEvent.mouseButton.y-(30*this->current_zoom_factor))).x)

#define CORRECTED_EVENT_Y   (this->window->mapPixelToCoords( \
sf::Vector2i(this->sfEvent.mouseButton.x-(30*this->current_zoom_factor), \
this->sfEvent.mouseButton.y-(30*this->current_zoom_factor))).y)

#define CORRECTED_MOUSE_X   (this->window->mapPixelToCoords( \
sf::Vector2i((float)sf::Mouse::getPosition(*this->window).x-(30*this->current_zoom_factor), \
(float)sf::Mouse::getPosition(*this->window).y-(30*this->current_zoom_factor))).x)

#define CORRECTED_MOUSE_Y   (this->window->mapPixelToCoords( \
sf::Vector2i((float)sf::Mouse::getPosition(*this->window).x-(30*this->current_zoom_factor), \
(float)sf::Mouse::getPosition(*this->window).y-(30*this->current_zoom_factor))).y)



#define MAX_ZOOM    5
#define MIN_ZOOM    0.2





std::mutex window_lock; // prevents multiple threads from accessing window resource
bool algo_thread_is_running = false; // true when a thread is currently running runBfs
bool algo_thread_is_finished = false; // true when a thread ran and ended execution of runBfs, so we need to call join()
bool algo_thread_should_end = false;

bool is_immediate = false;

std::thread algo_thread; // the thread we use to execute run<Algorithm> methods

VisMode current_algo_mode;



Visualizer::Visualizer(const Graph& graph): graph(graph) {
    this->vis_font = new sf::Font();
    this->vis_font->loadFromFile("fonts/Raleway-Regular.ttf");

    this->window = new sf::RenderWindow(sf::VideoMode(1400, 1000), "Graph Visualizer");
    this->window->setFramerateLimit(120);

    this->current_view = this->window->getView();
    this->original_view = this->window->getDefaultView();

    this->grid_bounds = new sf::FloatRect();

    std::ifstream save_file("SavedGraphs.txt", std::ios::in);
    this->saved_graphs_list = SavedGraphsList(save_file, this->vis_font);
    save_file.close();
}



Visualizer::~Visualizer() {
  delete this->window;
  delete this->vis_font;
  delete this->grid_bounds;
}


void Visualizer::run() {
    this->render();
    while(this->window->waitEvent(this->sfEvent)) {
        if (algo_thread_is_finished) {
            algo_thread.join();
            algo_thread_is_finished = false;
        }

        switch (this->sfEvent.type) {
            case sf::Event::Closed: {
                if (algo_thread_is_running || algo_thread_is_finished) {
                    algo_thread_should_end = true;
                    algo_thread.join();
                    algo_thread_is_finished = false;
                }
                this->window->close();
                break;
            }
            case sf::Event::MouseButtonPressed: {
                executeClickAction();
                break;
            }
            case sf::Event::Resized: {
                sf::FloatRect view_rect(0, 0, (float)this->sfEvent.size.width, (float)this->sfEvent.size.height);
                auto resized_view = sf::View(view_rect);
                this->original_view = resized_view;
                resized_view.zoom(1.f/this->current_zoom_factor);
                this->current_view = resized_view;
                this->window->setView(this->current_view);
                break;
            }
            case sf::Event::MouseWheelScrolled: {
                if (this->sfEvent.mouseWheelScroll.delta > 0 && current_zoom_factor < MAX_ZOOM) {
                    this->current_view.zoom(0.875);
                    this->current_zoom_factor *= 1.142857;
                }
                else if (this->sfEvent.mouseWheelScroll.delta < 0 && current_zoom_factor > MIN_ZOOM){
                    this->current_view.zoom(1.142857);
                    this->current_zoom_factor *= 0.875;
                }
                if (!algo_thread_is_running) {
                    this->render();
                }
                break;
            }
            default: {
                break;
            }
        }
        if (!algo_thread_is_running) {
            this->render();
        }
    }
}


void Visualizer::render(bool load_list) {
    window_lock.lock();
    window->setActive(true);
    this->window->clear(BG_COLOR);
    drawGrid((*this->window), this->original_view, *this->grid_bounds);
    this->graph.render(*this->window, this->vis_font);
    this->window->setView(this->original_view);
    this->toolbar.render(*this->window, false);
    if (load_list) this->saved_graphs_list.render(*(this->window));
    if (this->toolbar.getActiveButtonId() == ADD_NODE) {
        float corrected_radius = this->current_zoom_factor * NODE_RADIUS;
        sf::CircleShape hover_node(corrected_radius);
        hover_node.setFillColor(sf::Color(255, 255, 255, 100));
        hover_node.setPosition(CORRECTED_MOUSE_X, CORRECTED_MOUSE_Y);
        this->window->draw(hover_node);
    }
    this->window->setView(this->current_view);
    if (this->toolbar.getActiveButtonId() == ADD_EDGE && this->node_is_clicked) {
        std::shared_ptr<Node> tmp_node = std::make_shared<Node>(Node("tmp", CORRECTED_MOUSE_X, CORRECTED_MOUSE_Y, vis_font));
        std::shared_ptr<Edge> tmp_edge = std::make_shared<Edge>(Edge(this->clicked_node, tmp_node, false));
        tmp_edge->render(*this->window);
    }
    this->window->display();
    window->setActive(false);
    window_lock.unlock();
}



void Visualizer::executeClickAction() {
    //Didn't use macros because this case is shared, other class functions work on position relative to window
    // and not global or world position:
    auto window_x = (float)this->sfEvent.mouseButton.x;
    auto window_y = (float)this->sfEvent.mouseButton.y;

    is_immediate = this->toolbar.updateActiveButton(sf::Vector2f(window_x,window_y));
    ButtonId id = this->toolbar.getActiveButtonId();

    switch (id) {
        case REMOVE_TARGET_NODE: {
            removeTargetNodeRoutine();
            break;
        }
        case RUN_BFS: {
            current_algo_mode = BFS;
            runAlgorithm();
            break;
        }
        case RUN_DFS: {
            current_algo_mode = DFS;
            runAlgorithm();
            break;
        }
        case RUN_DIJKSTRA: {
            current_algo_mode = DIJKSTRA;
            runAlgorithm();
            break;
        }
        case END: {
            endRoutine();
            break;
        }
        case RESET: {
            resetRoutine();
            break;
        }
        case CLEAR_WINDOW: {
            clearWindowRoutine();
            break;
        }
        case SAVE_TO_FILE: {
            saveToFile();
            break;
        }
        case LOAD_FROM_FILE: {
            loadFromFile();
            break;
        }
        default: {
            break;
        }

    }

    if (is_immediate || algo_thread_is_running) return;
    // the following cases are not allowed while an algorithm is running
    std::shared_ptr<Node> to_toggle = this->graph.getNodeByPosition(EVENT_X, EVENT_Y);
    this->graph.setToggledNode(to_toggle);
    switch (id) {
        case CURSOR: {
            cursorRoutine();
            break;
        }
        case ADD_NODE: {
            addNodeRoutine();
            break;
        }
        case ADD_EDGE: {
            addEdgeRoutine();
            break;
        }
        case ERASE: {
            eraseRoutine();
            break;
        }
        case CHANGE_START_NODE: {
            changeStartNodeRoutine();
            break;
        }
        case CHOOSE_TARGET_NODE: {
            chooseTargetNodeRoutine();
            break;
        }
        default: {
            break;
        }
    }

}




void Visualizer::runAlgorithm() {
    if (!this->graph.getStartNode() || algo_thread_is_running) {
        this->toolbar.resetActiveButton();
        return;
    }
    auto window_x = (float) this->sfEvent.mouseButton.x;
    auto window_y = (float) this->sfEvent.mouseButton.y;
    sf::Vector2f original_active_button_coordinates = sf::Vector2f(window_x, window_y);
    while (this->sfEvent.type != sf::Event::MouseButtonReleased) {
        this->window->waitEvent(this->sfEvent);
    }
    if (!this->toolbar.updateActiveButton(sf::Vector2f(window_x, window_y))) {
        this->toolbar.updateActiveButton(original_active_button_coordinates);
        return;
    }
    this->graph.reset();
    this->toolbar.resetActiveButton();

    VisMode current_mode = current_algo_mode;
    bool should_wait = true;
    switch (current_mode) {
        case BFS: {
            this->window->setActive(false);
            algo_thread = std::thread(&Graph::runBFS, std::ref(this->graph), std::ref(*this->window),
                                      std::ref(this->toolbar), std::ref(this->original_view),
                                      std::ref(this->current_view), std::ref(this->vis_font), std::ref(*this->grid_bounds), should_wait);
            break;
        }
        case DFS: {
            this->window->setActive(false);
            algo_thread = std::thread(&Graph::runDFS, std::ref(this->graph), std::ref(*this->window),
                                      std::ref(this->toolbar), std::ref(this->original_view),
                                      std::ref(this->current_view), std::ref(this->vis_font), std::ref(*this->grid_bounds), should_wait);
            break;
        }
        case DIJKSTRA: {
            this->window->setActive(false);
            algo_thread = std::thread(&Graph::runDijkstra, std::ref(this->graph), std::ref(*this->window),
                                      std::ref(this->toolbar), std::ref(this->original_view),
                                      std::ref(this->current_view), std::ref(this->vis_font), std::ref(*this->grid_bounds), should_wait);
            break;
        }
    }
}




void Visualizer::cursorRoutine() {
    std::shared_ptr<Node> moving_node = this->graph.getNodeByPosition(EVENT_X, EVENT_Y);
    if (!moving_node) {
        // didn't click on a node, which means we're moving the view
        float window_x = (float)sf::Mouse::getPosition().x;
        float window_y = (float)sf::Mouse::getPosition().y;
        sf::Vector2f prev_pos = sf::Vector2f (window_x, window_y);
        sf::Vector2f current_pos, delta_pos;
        while (this->sfEvent.type != sf::Event::MouseButtonReleased) {
            window_x = (float) sf::Mouse::getPosition().x;
            window_y = (float) sf::Mouse::getPosition().y;
            current_pos = sf::Vector2f(window_x, window_y);
            delta_pos = prev_pos - current_pos; // inverted because when we "look" to the left things "move" to the right
            this->current_view.move(delta_pos / this->current_zoom_factor);
            if (!this->viewIsInBounds()) {
                this->current_view.move(-delta_pos / this->current_zoom_factor);
            }
            prev_pos = current_pos;
            this->render();
            this->window->waitEvent(this->sfEvent);
        }
        return;
    }

    // Clicked on a node, which means we're moving a node:
    while (this->sfEvent.type != sf::Event::MouseButtonReleased) {
        moving_node->setPosition(CORRECTED_MOUSE_X, CORRECTED_MOUSE_Y);
        if (!this->graph.checkValidPosition(*moving_node)) {
            std::shared_ptr<Node> collided_node = this->graph.getCollidedNode(moving_node);
            if (collided_node) {
                float collided_x = collided_node->getPosition().x;
                float collided_y = collided_node->getPosition().y;
                std::vector<float> closest_pos = getClosestNonCollision(collided_x, collided_y,
                                                                        CORRECTED_MOUSE_X,
                                                                        CORRECTED_MOUSE_Y);
                moving_node->setPosition(closest_pos[0], closest_pos[1]);
                if (!this->graph.checkValidPosition(*moving_node)) continue;
            }
        }
        if (current_algo_mode == DIJKSTRA && (this->graph.getStartNode()->getState() == NODE_DONE ||
                                              this->graph.getStartNode()->getState() == NODE_NEAREST)) {
            this->graph.runDijkstra(*(this->window), this->toolbar, this->original_view,
                                    this->current_view, this->vis_font, *grid_bounds, false);
            this->graph.setToggledNode(moving_node);
        }
        this->render();
        this->window->pollEvent(this->sfEvent);
    }
}


void Visualizer::addNodeRoutine() {
    std::shared_ptr<Node> node_exists = this->graph.addNode(CORRECTED_EVENT_X, CORRECTED_EVENT_Y, this->vis_font);
    if (node_exists) {
        this->graph.reset();
    }
}


void Visualizer::addEdgeRoutine() {
    bool edge_was_added = false;
    std::shared_ptr<Node> dst = nullptr;

    if (this->node_is_clicked) {
        // a node was clicked and the mouse was released
        // the next node to be clicked will be connected by an edge to the already clicked one
        dst = this->graph.getNodeByPosition(EVENT_X, EVENT_Y);
        if (dst && dst != clicked_node) {
            std::shared_ptr<Edge> to_add = std::make_shared<Edge>(this->clicked_node, dst);
            this->graph.addEdge(to_add);
            edge_was_added = true;
        }
        this->node_is_clicked = false;
        this->clicked_node = nullptr;
        if (edge_was_added) {
            this->graph.reset();
        }
        return;
    }
    // The following is in case no node was previously clicked to draw an edge
    // so drawing an edge is going to happen by selecting a node after clicking
    // and connecting it to another node that is going to be selected after releasing
    this->clicked_node = this->graph.getNodeByPosition(EVENT_X, EVENT_Y);
    if (!clicked_node) return;
    this->node_is_clicked = true;

    while (this->sfEvent.type != sf::Event::MouseButtonReleased) {
        // As long as the mouse wasn't released:
        while (!dst || dst == clicked_node) {
            // Keep rendering an edge animation that follows the mouse from the clicked node:
            this->render();
            dst = this->graph.getNodeByPosition(MOUSE_X, MOUSE_Y);
            this->window->waitEvent(this->sfEvent);
            if (this->sfEvent.type == sf::Event::MouseButtonReleased) {
                if (dst == clicked_node) {
                    if (edge_was_added) {
                        this->clicked_node = nullptr;
                        this->node_is_clicked = false;
                        this->graph.reset();
                    }
                    return;
                }
                else if (!dst) {
                    edge_was_added = false;
                }
                else {
                    edge_was_added = true;
                }
                break;
            }
        }
        if (!edge_was_added && !dst) {
            this->clicked_node = nullptr;
            this->node_is_clicked = false;
            return;
        }
        if (dst && dst != clicked_node) {
            std::shared_ptr<Edge> to_add = std::make_shared<Edge>(this->clicked_node, dst);
            this->graph.addEdge(to_add);
            this->clicked_node = dst;
            this->graph.setToggledNode(dst);
            this->render();
            edge_was_added = true;
        }
        this->window->waitEvent(this->sfEvent);
    }
    if (edge_was_added) {
        this->graph.reset();
    }
}




void Visualizer::eraseRoutine() {
    std::shared_ptr<Node> node_to_delete = this->graph.getNodeByPosition(EVENT_X, EVENT_Y);
    std::shared_ptr<Edge> edge_to_delete = this->graph.getEdgeByPosition(EVENT_X, EVENT_Y);
    if (node_to_delete) {
        this->graph.removeNode(node_to_delete->getName());
    }
    else if (edge_to_delete) {
        this->graph.removeEdge(edge_to_delete);
    }
    if (node_to_delete || edge_to_delete) {
        this->graph.reset();
    }
}


void Visualizer::changeStartNodeRoutine() {
    std::shared_ptr<Node> node_exists = this->graph.getNodeByPosition(EVENT_X, EVENT_Y);
    if (node_exists) {
        this->graph.reset();
    }
    this->graph.setStartNode(this->graph.getNodeByPosition(EVENT_X, EVENT_Y));
}

void Visualizer::chooseTargetNodeRoutine() {
    std::shared_ptr<Node> node_exists = this->graph.getNodeByPosition(EVENT_X, EVENT_Y);
    if (node_exists) {
        this->graph.reset();
    }
    this->graph.setTargetNode(this->graph.getNodeByPosition(EVENT_X, EVENT_Y));
}

void Visualizer::removeTargetNodeRoutine() {
    if (algo_thread_is_running || !is_immediate) return;
    this->graph.reset();
    this->graph.removeTargetNode();
    this->toolbar.resetActiveButton();
}


void Visualizer::endRoutine() {
    this->toolbar.resetActiveButton();
    if (!algo_thread_is_running) return;
    algo_thread_should_end = true;
    algo_thread.join();
    algo_thread_is_finished = false;
    algo_thread_should_end = false;

    this->graph.reset();
    if (current_algo_mode == BFS) {
        this->graph.runBFS(*this->window, this->toolbar, this->original_view,
                           this->current_view, this->vis_font, *grid_bounds, false);
    }
    else if (current_algo_mode == DFS) {
        this->graph.runDFS(*this->window, this->toolbar, this->original_view,
                           this->current_view, this->vis_font, *grid_bounds, false);
    }
    else {
        this->graph.runDijkstra(*this->window, this->toolbar, this->original_view,
                                this->current_view, this->vis_font, *grid_bounds, false);
    }
    this->toolbar.resetActiveButton();
}


void Visualizer::resetRoutine() {
    if (algo_thread_is_running) {
        algo_thread_should_end = true;
        algo_thread.join();
        algo_thread_is_finished = false;
        algo_thread_should_end = false;
    }
    this->graph.reset();
    this->toolbar.resetActiveButton();
}


void Visualizer::clearWindowRoutine() {
    if (algo_thread_is_running) {
        algo_thread_should_end = true;
        algo_thread.join();
        algo_thread_is_finished = false;
        algo_thread_should_end = false;
    }
    this->graph = Graph();
    this->node_is_clicked = false;
    this->clicked_node = nullptr;
    this->toolbar.resetActiveButton();
}



void Visualizer::saveToFile() {
    std::string graph_name;
    this->window->waitEvent(this->sfEvent);
    InputBox inputBox(*(this->window), this->vis_font);
    this->window->setView(this->original_view);
    graph_name = inputBox.getInput(*(this->window));

    if (graph_name.empty() || this->saved_graphs_list.contains(graph_name)) {
        this->toolbar.resetActiveButton();
        return;
    }

    std::ofstream save_file("SavedGraphs.txt", std::ios::out | std::ios::app);
    std::string graph_literal = this->graph.getLiteral();
    graph_literal = "svg:" + graph_name + graph_literal;
    save_file.write(graph_literal.c_str(), (long long)graph_literal.size());
    save_file.close();
    this->toolbar.resetActiveButton();
}



void Visualizer::loadFromFile() {
    bool load_list = true;
    std::string graph_literal;
    std::string graph_name;
    this->toolbar.resetActiveButton();

    std::ifstream save_file("SavedGraphs.txt", std::ios::in);
    save_file.clear();
    save_file.seekg(0);

    if (!algo_thread_is_running) this->render(load_list);
    while (this->window->waitEvent(this->sfEvent)) {
        if (this->sfEvent.type == sf::Event::MouseButtonPressed) {
            graph_name = this->saved_graphs_list.getClickedGraph(EVENT_X, EVENT_Y);
            break;
        }
    }

    if (graph_name.empty()) {
        save_file.close();
        return;
    }

    while(std::getline(save_file, graph_literal)) {
        bool found = false;
        for (int i = 0; i < graph_literal.size() - graph_name.size(); i++) {
            if (graph_literal.substr(i, graph_name.size()) == graph_name) {
                found = true;
                break;
            }
        }
        if (found) break;
    }
    save_file.close();

    std::vector<std::shared_ptr<Node>> nodes = parseNodesFromString(graph_literal, this->vis_font);
    std::vector<std::shared_ptr<Edge>> edges = parseEdgesFromString(graph_literal, nodes);
    this->clearWindowRoutine();
    for (auto& node : nodes) {
        this->graph.addNode(node);
        if (node->getState() == NODE_START) this->graph.setStartNode(node);
    }
    for (auto& edge : edges) {
        this->graph.addEdge(edge);
    }
    this->toolbar.resetActiveButton();
}


//**********************************************Auxiliary Functions***************************************************//

bool Visualizer::viewIsInBounds() {
    sf::Vector2f viewCenter = this->current_view.getCenter();
    float viewWidth = this->current_view.getSize().x;
    float viewHeight = this->current_view.getSize().y;

// Calculate the bounds of the view
    sf::FloatRect viewBounds(viewCenter.x - viewWidth / 2, viewCenter.y - viewHeight / 2, viewWidth, viewHeight);

    if (!grid_bounds->contains(viewBounds.left, viewBounds.top) || !grid_bounds->contains(viewBounds.width + viewBounds.left, viewBounds.height + viewBounds.top)) {
        return false;
    }

    return true;
}




std::vector<std::shared_ptr<Node>> Visualizer::parseNodesFromString(const std::string& graph_literal, sf::Font* font) {
    std::string tmp_literal = graph_literal;

    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::string> node_names;
    std::vector<std::string> node_positions;

    std::regex start_node_expression("\\|(node_[0-9]+)*\\}");
    std::regex name_expression("(node_[0-9]+):");
    std::regex pos_expression("<((-)*[0-9]+,(-)*[0-9]+)>");

    std::smatch start_node_match;
    std::smatch name_matches;
    std::smatch pos_matches;

    std::string start_node_name;

    while (std::regex_search(tmp_literal, start_node_match, start_node_expression)) {
        start_node_name = start_node_match.str(1);
        tmp_literal = start_node_match.suffix().str();
    }
    tmp_literal = graph_literal;
    while (std::regex_search(tmp_literal, name_matches, name_expression)) {
        node_names.push_back(name_matches.str(1));
        tmp_literal = name_matches.suffix().str();
    }
    tmp_literal = graph_literal;
    while (std::regex_search(tmp_literal, pos_matches, pos_expression)) {
        node_positions.push_back(pos_matches.str(1));
        tmp_literal = pos_matches.suffix().str();
    }
    

    for (int i = 0; i < node_names.size(); i++) {
        std::string pos_str = node_positions[i];
        std::string x_str = pos_str.substr(0, pos_str.find(','));
        std::string y_str = pos_str.substr(pos_str.find(',')+1);
        float pos_x = (float)std::stoi(x_str.substr(std::min(0, (int)pos_str.find('-')+1)));
        float pos_y = (float)std::stoi(y_str.substr(std::min(0, (int)pos_str.find('-')+1)));
        if (x_str[0] == '-') pos_x = -pos_x;
        if (y_str[0] == '-') pos_y = -pos_y;
        std::shared_ptr<Node> to_add = std::make_shared<Node>(Node(node_names[i], pos_x, pos_y, font));
        if (to_add->getName() == start_node_name) to_add->setState(NODE_START);
        nodes.push_back(to_add);
    }
    return nodes;
}




std::vector<std::shared_ptr<Edge>> Visualizer::parseEdgesFromString(const std::string& graph_literal,
                                                                    std::vector<std::shared_ptr<Node>>& nodes) {
    std::string tmp_literal = graph_literal;
    std::vector<std::shared_ptr<Edge>> edges;
    std::vector<std::vector<std::string>> node_pairs;
    std::shared_ptr<Node> node1, node2;

    std::regex pair_expression("(\\(node_[0-9]+,node_[0-9]+)");
    std::smatch pair_matches;

    while (std::regex_search(tmp_literal, pair_matches, pair_expression)) {
        std::string pair_str = pair_matches.str(1);
        std::string node1_name = pair_str.substr(pair_str.find('(')+1, pair_str.find(',')-1);
        std::string node2_name = pair_str.substr(pair_str.find(',')+1, pair_str.find(')')-1);
        node_pairs.push_back({node1_name, node2_name});
        tmp_literal = pair_matches.suffix().str();
    }

    for (auto& pair : node_pairs) {
        for (auto& node : nodes) {
            if (pair[0] == node->getName()) node1 = node;
            else if (pair[1] == node->getName()) node2 = node;
        }
        edges.push_back(std::make_shared<Edge>(node1, node2));
    }

    return edges;
}
