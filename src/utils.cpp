//
// Created by User on 2/14/2023.
//
#include <cmath>
#include <numbers>
#include "utils.h"
#include <iostream>
#include <fstream>

#ifndef NODE_RADIUS
#define NODE_RADIUS 30
#endif

#define ROWS (120)
#define COLS (120*((float)window.getView().getSize().x/(float)window.getView().getSize().y))

float getAngle(float x1, float y1, float x2, float y2) {
    float rads = std::atan((y2 - y1) / (x2 - x1));
    float degrees = (rads * 180)/(float)std::numbers::pi;
    float angle = degrees - 90; // correction for shape::setRotation function
    if (x1 > x2) angle += 180; // another correction
    return angle;
}

float getDistance(float x1, float y1, float x2, float y2) {
    auto distance = (float)sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return distance;
}

float getCorrectedX1(float x1, float angle) {
    float rads = ((angle + 90) * (float)std::numbers::pi) / 180;
    float offset = NODE_RADIUS * std::cos(rads);
    return x1 + offset;
}

float getCorrectedY1(float y1, float angle) {
    float rads = ((angle+90) * (float)std::numbers::pi) / 180;
    float offset = NODE_RADIUS * std::sin(rads);
    return y1 + offset;
}

std::vector<float> getClosestNonCollision(float node_x, float node_y, float cursor_x, float cursor_y) {
    std::vector<float> res(2);
    if (node_x == cursor_x && node_y == cursor_y) node_x += 0.1;
    float dist = std::sqrt(std::pow(cursor_x-node_x,2) + std::pow(cursor_y-node_y,2));
    float coefficient = (2 * NODE_RADIUS) / dist;
    res[0] = node_x + ((cursor_x-node_x) * coefficient);
    res[1] = node_y + ((cursor_y-node_y) * coefficient);
    return res;
}







void drawGrid(sf::RenderWindow& window, sf::View& view, sf::FloatRect& gridBounds) {
    // initialize values
    int numLines = ROWS+COLS-2;
    sf::VertexArray grid(sf::Lines, 2*(numLines));
    auto size = view.getSize();
    size.x *= 5;
    size.y *= 5;
    float rowH = 2 * NODE_RADIUS;
    float colW = 2 * NODE_RADIUS;
    // row separators
    for(int i=0; i < ROWS-1; i++){
        int r = i+1;
        float rowY = (float)r * rowH;
        grid[i*2].position = {-size.x/2-660, rowY-size.y/2-360};
        grid[i*2].color = sf::Color(255,255,255,20);
        grid[i*2+1].position = {size.x-1110, rowY-size.y/2-360};
        grid[i*2+1].color = sf::Color(255,255,255,20);
    }
    // column separators
    for(int i=ROWS-1; i < numLines; i++){
        int c = i-ROWS+2;
        float colX = (float)c * colW;
        grid[i*2].position = {colX-size.x/2-660, -size.y/2-360};
        grid[i*2].color = sf::Color(255,255,255,20);
        grid[i*2+1].position = {colX-size.x/2-660, size.y-710};
        grid[i*2+1].color = sf::Color(255,255,255,20);
    }
    // draw it
    window.draw(grid);
    gridBounds = {-size.x/2-660, -size.y/2-360, size.x-1110 + size.x/2-660 ,size.y-710 + size.y/2-360};
}


std::vector<std::string> getGraphNamesFromFile(std::ifstream& file) {
    std::vector<std::string> graph_names;
    std::string current_graph, current_name;
    int i = 0;
    while (std::getline(file, current_graph)) {
        while (i < current_graph.size() && current_graph[i] != ':') i++;
        i++;
        while (i < current_graph.size() && current_graph[i] != '{') {
            current_name += current_graph[i];
            i++;
        }
        if (current_name.empty()) continue;
        graph_names.push_back(current_name);
        current_name = "";
    }
    return graph_names;
}