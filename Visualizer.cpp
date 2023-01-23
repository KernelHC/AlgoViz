#include "Visualizer.h"
#include <iostream>

#define MOUSE_X (this->sfEvent.mouseButton.x)
#define MOUSE_Y (this->sfEvent.mouseButton.y)

// use these only for drawing shapes (shapes drawn to wrong part of cursor)
#define MOUSE_X_CORRECTED (this->sfEvent.mouseButton.x-30)
#define MOUSE_Y_CORRECTED (this->sfEvent.mouseButton.y-30)

void Visualizer::initWindow() {
    this->window = new sf::RenderWindow(sf::VideoMode(1000, 600), "Graph Visualizer");
    this->window->setFramerateLimit(60);
}

Visualizer::Visualizer() {
    this->initWindow();
}

Visualizer::~Visualizer() {
  delete this->window;
}

void Visualizer::update() {
    while(this->window->pollEvent(this->sfEvent)) {
        switch (this->sfEvent.type) {
            case sf::Event::Closed: {
                this->window->close();
                break;
            }
            case sf::Event::MouseButtonPressed: {
                executeClickAction(MOUSE_X, MOUSE_Y);
                break;
            }
            case sf::Event::Resized: {
                sf::FloatRect view(0, 0, this->sfEvent.size.width, this->sfEvent.size.height);
                this->window->setView(sf::View(view));
                break;
            }
            default: {
                break;
            }
        }
    }
}

void Visualizer::render() {
    this->window->clear();
    this->toolbar.render(*this->window);
    this->graph.render(*this->window);
    this->window->display();
}

void Visualizer::run() {
    while (this->window->isOpen()) {
        this->update();
        this->render();
    }
}


void Visualizer::executeClickAction(float pos_x, float pos_y) {
    bool button_pressed = false;
    for (const auto &button: this->toolbar.buttons) {
        if (button->update(sf::Vector2i(MOUSE_X, MOUSE_Y))) {
            this->toolbar.update(sf::Vector2i(MOUSE_X, MOUSE_Y));
            button_pressed = true;
            break;
        }
    }
    button_id id = this->toolbar.getActiveButtonId();

    if (button_pressed) {
        if (id == CLEAN) {
            this->graph = Graph();
            this->node_is_clicked = false;
            this->clicked_node = nullptr;
            this->toolbar.updateActiveButton();
        }
        return;
    }

    switch (id) {
        case ADD_NODE: {
            this->graph.addNode(MOUSE_X_CORRECTED, MOUSE_Y_CORRECTED);
            break;
        }
        case ADD_EDGE: {
            if (!this->node_is_clicked) {
                this->clicked_node = this->graph.getNodeByPosition(MOUSE_X, MOUSE_Y);
                if (clicked_node) this->node_is_clicked = true;
            }
            else {
                std::shared_ptr<Node> dst = this->graph.getNodeByPosition(MOUSE_X, MOUSE_Y);
                if (dst) {
                    Edge to_add(this->clicked_node, dst);
                    this->graph.addEdge(to_add);
                }
                this->node_is_clicked = false;
            }
            break;
        }
        case ERASE: {
            std::shared_ptr<Node> node_to_delete = this->graph.getNodeByPosition(MOUSE_X, MOUSE_Y);
            if (node_to_delete)
                this->graph.removeNode(node_to_delete->getName());
            std::shared_ptr<Edge> edge_to_delete = this->graph.getEdgeByPosition(MOUSE_X, MOUSE_Y);
            if (edge_to_delete)
                this->graph.removeEdge(*edge_to_delete);
            break;
        }
        case CHANGE_START_NODE: {
            this->graph.setStartNode(this->graph.getNodeByPosition(MOUSE_X, MOUSE_Y));
            break;
        }
        case START: {
            runAlgorithm();
            break;
        }
        case PAUSE: {
            break;
        }
        case END: {
            break;
        }
        default: {
            break;
        }
    }

}

void Visualizer::runBFS() {
    sf::Clock clock;

}

void Visualizer::runDFS() {

}

void Visualizer::runMST() {

}

void Visualizer::runDijkstra() {

}

void Visualizer::runAlgorithm() {
    if (!this->graph.getStartNode()) return;
    vis_mode current_mode = this->mode;
    switch (current_mode) {
        case BFS: {
            runBFS();
            break;
        }
        case DFS: {
            runDFS();
            break;
        }
        case MST: {
            runMST();
            break;
        }
        case DIJKSTRA: {
            runDijkstra();
            break;
        }
    }
}


