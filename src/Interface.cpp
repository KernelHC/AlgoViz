//
// Created by User on 3/11/2023.
//
#include "Interface.h"
#include "utils.h"
#include <iostream>
#include <filesystem>


extern bool algo_thread_is_running;


//--------------------------------------------Button methods------------------------------------------------------------

Button::Button(float x, float y, float width, float height, const std::string& icon_path, ButtonId id) : id(id) {
    auto* texture = new sf::Texture();
    this->icon_path = icon_path;
    texture->loadFromFile(this->icon_path);
    this->shape.setTexture(texture);
    this->shape.setPosition(sf::Vector2f(x, y));
    this->shape.setSize(sf::Vector2f(width, height));
}

Button::~Button() {
    delete this->shape.getTexture();
}


Button& Button::operator=(const Button& other) {
    if (this != &other) {
        this->icon_path = other.icon_path;
        delete this->shape.getTexture();
        auto* texture = new sf::Texture();
        texture->loadFromFile(this->icon_path);
        this->shape.setTexture(texture);
        this->shape.setPosition(other.shape.getPosition());
        this->shape.setSize(sf::Vector2f(other.shape.getSize()));
    }
    return *this;
}


ButtonId Button::getId() const {
    return this->id;
}

bool Button::update(const sf::Vector2f& mousePosWindow) {
    //Mouse click is in bounds
    if (this->shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosWindow))) {
        return true;
    }
    return false;
}

void Button::setButtonEnabled() {
    this->shape.setFillColor(sf::Color(this->shape.getFillColor().r, this->shape.getFillColor().g,
                                       this->shape.getFillColor().b, 100));
}

void Button::setButtonDisabled() {
    this->shape.setFillColor(sf::Color(this->shape.getFillColor().r, this->shape.getFillColor().g,
                                       this->shape.getFillColor().b, 255));
}

void Button::render(sf::RenderTarget& target) {
    target.draw(this->shape);
}

//-------------------------------------------Toolbar Methods------------------------------------------------------------

#define TOOLBAR_COLOR    sf::Color(107, 107, 107, 255)
#define SEPARATOR_COLOR  sf::Color(81, 78, 78, 200)

Toolbar::Toolbar() {
    std::vector<ButtonId> id_list = {CURSOR, ADD_NODE, ADD_EDGE, ERASE, CHANGE_START_NODE, CHOOSE_TARGET_NODE,
                                     REMOVE_TARGET_NODE, RUN_BFS, RUN_DFS, RUN_DIJKSTRA, END, RESET, CLEAR_WINDOW,
                                     SAVE_TO_FILE, LOAD_FROM_FILE};
    this->buttons.push_back(std::make_shared<Button>(32.5, 15, 30, 30, "./images/cursor.png", CURSOR));
    this->buttons.push_back(std::make_shared<Button>(35, 60, 30, 30, "./images/add_node.png", ADD_NODE));
    this->buttons.push_back(std::make_shared<Button>(35, 110, 30, 30, "./images/add_edge.png", ADD_EDGE));
    this->buttons.push_back(std::make_shared<Button>(35, 162.5, 30, 30, "./images/erase.png", ERASE));
    this->buttons.push_back(std::make_shared<Button>(35, 210, 30, 30, "./images/change_start_node.png",
                                                     CHANGE_START_NODE));
    this->buttons.push_back(std::make_shared<Button>(35, 260, 28, 28, "./images/choose_target_node.png",
                                                     CHOOSE_TARGET_NODE));
    this->buttons.push_back(std::make_shared<Button>(35, 310, 28, 28, "./images/remove_target_node.png",
                                                     REMOVE_TARGET_NODE));
    this->buttons.push_back(std::make_shared<Button>(25, 360, 50, 30, "./images/run_bfs.png", RUN_BFS));
    this->buttons.push_back(std::make_shared<Button>(25, 410, 50, 30, "./images/run_dfs.png", RUN_DFS));
    this->buttons.push_back(std::make_shared<Button>(15, 460, 70, 30, "./images/run_dijkstra.png", RUN_DIJKSTRA));
    this->buttons.push_back(std::make_shared<Button>(35, 510, 30, 30, "./images/end.png", END));
    this->buttons.push_back(std::make_shared<Button>(35, 560, 30, 30, "./images/reset.png", RESET));
    this->buttons.push_back(std::make_shared<Button>(35, 610, 30, 30, "./images/clear_window.png", CLEAR_WINDOW));
    this->buttons.push_back(std::make_shared<Button>(30, 655, 45, 45, "./images/save_to_file.png", SAVE_TO_FILE));
    this->buttons.push_back(std::make_shared<Button>(30, 705, 45, 45, "./images/load_from_file.png", LOAD_FROM_FILE));
    this->rectangle.setPosition(10, 10);
    this->rectangle.setSize({80, 745});
    this->rectangle.setFillColor(TOOLBAR_COLOR);
    this->rectangle.setOutlineColor(sf::Color::White);
    this->rectangle.setOutlineThickness(2.f);

    for (int i = 0; i < id_list.size() - 1; i++) {
        sf::RectangleShape s1;
        s1.setSize(sf::Vector2f(2, 60));
        s1.setFillColor(SEPARATOR_COLOR);
        s1.setPosition(80, (float)(50 * (i + 1)));
        s1.setRotation(90);
        this->horizontal_separators.push_back(s1);
    }
    this->active_button = buttons[0];
}

ButtonId Toolbar::getActiveButtonId() const {
    return this->active_button->getId();
}

bool Toolbar::updateActiveButton(const sf::Vector2f& mousePosWindow) {
    for (auto& button : this->buttons) {
        if (button->update(mousePosWindow)) {
            this->active_button->setButtonDisabled();
            this->active_button = button;
            this->active_button->setButtonEnabled();
            return true;
        }
    }

    return false;
}

void Toolbar::render(sf::RenderWindow& window, bool is_mid_run) {
    std::vector<ButtonId> not_to_render_while_running = {CURSOR, ADD_NODE, ADD_EDGE, ERASE, CHANGE_START_NODE,
                                                         CHOOSE_TARGET_NODE, REMOVE_TARGET_NODE, RUN_BFS, RUN_DFS,
                                                         RUN_DIJKSTRA, SAVE_TO_FILE, LOAD_FROM_FILE};
    window.draw(this->rectangle);

    for (const auto & horizontal_separator : this->horizontal_separators) {
        window.draw(horizontal_separator);
    }

    for (const auto& button : this->buttons) {
        if (    is_mid_run &&
                find(not_to_render_while_running.begin(), not_to_render_while_running.end(), button->getId()) !=
                not_to_render_while_running.end()
                ) {
            continue;
        }
        button->render(window);
    }
}

void Toolbar::resetActiveButton() {
    this->active_button->setButtonDisabled();
    this->active_button = buttons[0];
    this->active_button->setButtonEnabled();
}


//------------------------------------------------TextBox Methods-------------------------------------------------------


TextBox::TextBox(sf::RenderWindow& window, sf::Font* font): text_font(font) {
    this->text_box.setPosition((float)window.getDefaultView().getCenter().x+(float)window.getDefaultView().getSize().x/4,
                               (float)window.getDefaultView().getCenter().y+(float)window.getDefaultView().getSize().y/4);
    this->text_field.setPosition((float)this->text_box.getPosition().x+15, (float)this->text_box.getPosition().y+70);
    this->text_box.setSize(sf::Vector2f(200, 120));
    this->text_field.setSize(sf::Vector2f(170, 30));
    this->text_box.setFillColor(sf::Color(100,100,100,255));
    this->text_field.setFillColor(sf::Color(150,150,150,255));
    this->text_box.setOutlineThickness(1);
    this->text_box.setOutlineColor(sf::Color::White);
}


bool TextBox::close(float event_x, float event_y) {
    return (!this->text_box.getGlobalBounds().contains(event_x, event_y));
}

void TextBox::render(sf::RenderWindow& window, const std::string& txt) {
    sf::Text t;
    t.setString(txt);
    t.setFont(*this->text_font);
    t.setFillColor(sf::Color::White);
    t.setCharacterSize(20);
    t.setPosition(this->text_field.getPosition().x+10, this->text_field.getPosition().y);
    window.draw(this->text_box);
    window.draw(this->text_field);
    window.draw(t);
    window.display();
}


void InputBox::render(sf::RenderWindow &window, const std::string &txt) {
    TextBox::render(window, txt);
}

std::string InputBox::getInput(sf::RenderWindow& window) {
    this->input = "";
    sf::Event event{};
    window.waitEvent(event);
    while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        if (event.type == sf::Event::KeyPressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            char c = unicodeToAscii(event.text.unicode);
            if (c == 8) {
                this->input = this->input.substr(0, this->input.size()-1);
            }
            else if (c != 0) {
                this->input += unicodeToAscii(event.text.unicode);
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::Resized) {
            if (close((float)event.mouseButton.x, (float)event.mouseButton.y)) return "";
        }
        this->render(window, this->input);
        window.waitEvent(event);
    }
    return this->input;
}


char InputBox::unicodeToAscii(unsigned int uni_char) {
    if (uni_char <= 25) {
        return (char)('a' + uni_char);
    }
    else if (uni_char <= 35) {
        return (char)('0' + uni_char-26);
    }
    else if (uni_char >= 75 && uni_char <= 84) {
        return (char)('0' + uni_char-75);
    }
    else if (uni_char == 56) {
        return '-';
    }
    else if (uni_char == 57) {
        return ' ';
    }
    else if (uni_char == 59) {
        return 8;
    }

    return 0;
}

//----------------------------------------SavedGraphsList methods-------------------------------------------------------

SavedGraphsList::SavedGraphsList(std::ifstream &file, sf::Font *font): text_font(font) {
    std::vector<std::string> graph_names = getGraphNamesFromFile(file);
    for (const auto& graph_name : graph_names) {
        this->addGraph(graph_name);
    }
}


void SavedGraphsList::render(sf::RenderWindow& window) {
    for (const auto & saved_graph: this->saved_graphs) {
        window.draw(*(saved_graph.second));
        sf::Text graph_name;
        graph_name.setString(saved_graph.first);
        graph_name.setFont(*this->text_font);
        graph_name.setFillColor(sf::Color::White);
        graph_name.setCharacterSize(20);
        graph_name.setPosition(saved_graph.second->getPosition().x + (float)(10*saved_graph.first.size()),
                               saved_graph.second->getPosition().y + 20);
        window.draw(graph_name);
    }
}

void SavedGraphsList::addGraph(const std::string& graph_name) {
    if (this->saved_graphs.find(graph_name) != this->saved_graphs.end()) return;
    sf::RectangleShape frame;
    frame.setSize(sf::Vector2f(150, 50));
    frame.setFillColor(TOOLBAR_COLOR);
    frame.setPosition(92, (float)(695 - this->saved_graphs_num * 50));
    frame.setOutlineThickness(1);
    frame.setOutlineColor(sf::Color::White);
    this->saved_graphs[graph_name] = std::make_shared<sf::RectangleShape>(frame);
    this->saved_graphs_num++;
}

void SavedGraphsList::deleteGraph(const std::string& graph_name) {
    if (this->saved_graphs.find(graph_name) == this->saved_graphs.end()) return;
    this->saved_graphs.erase(graph_name);
    this->saved_graphs_num--;
    int i = 0;
    for (const auto& saved_graph : this->saved_graphs) {
        saved_graph.second->setPosition(92, (float)(795 - 50*i));
    }
}

bool SavedGraphsList::empty() const {
    return this->saved_graphs_num == 0;
}

bool SavedGraphsList::contains(const std::string& graph_name) {
    for (const auto& graph : this->saved_graphs) {
        if (graph.first == graph_name) return true;
    }
    return false;
}

std::string SavedGraphsList::getClickedGraph(float pos_x, float pos_y) {
    std::string clicked_graph_name;
    for (const auto& graph : saved_graphs) {
        if (graph.second->getGlobalBounds().contains(pos_x, pos_y)) clicked_graph_name = graph.first;
    }
    return clicked_graph_name;
}

