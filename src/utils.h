//
// Created by User on 2/14/2023.
//

#ifndef ALGOVIZ_UTILS_H
#define ALGOVIZ_UTILS_H
#include "SFML/Graphics.hpp"
#include <vector>

    /** Get the angle between two points (in degrees).
     *
     * @param x1 - the x-coordinate of the first point
     * @param y1 - the y-coordinate of the first point
     * @param x2 - the x-coordinate of the second point
     * @param y2 - the y-coordinate of the second point
     * @return The angle (in degrees) between the two points.
     */
    float getAngle(float x1, float y1, float x2, float y2);


    /** Get the distance between two points.
     *
     * @param x1 - the x-coordinate of the first point
     * @param y1 - the y-coordinate of the first point
     * @param x2 - the x-coordinate of the second point
     * @param y2 - the y-coordinate of the second point
     * @return the distance between the two points
     */
    float getDistance(float x1, float y1, float x2, float y2);


    /** Get the corrected x-coordinate of a point after rotating it by an angle.
     *
     * @param x1 - the original x-coordinate of the point
     * @param angle - the angle (in degrees) to rotate the point by
     * @return The new x-coordinate of the point after rotation
     */
    float getCorrectedX1(float x1, float angle);


    /** Get the corrected y-coordinate of a point after rotating it by an angle.
     *
     * @param y1 - the original y-coordinate of the point
     * @param angle - the angle (in degrees) to rotate the point by
     * @return The new y-coordinate of the point after rotation
     */
    float getCorrectedY1(float y1, float angle);


    /** Get the closest non-collision point to a given point.
     *
     * By using this function during node movement, we can move nodes without causing them to touch other nodes
     * on the screen.
     *
     * @param node_x - the x-coordinate of the starting point
     * @param node_y - the y-coordinate of the starting point
     * @param cursor_x - the x-coordinate of the target point
     * @param cursor_y - the y-coordinate of the target point
     * @return a vector containing the x and y coordinates of the closest non-collision point
     */
    std::vector<float> getClosestNonCollision(float node_x, float node_y, float cursor_x, float cursor_y);


    /** Draw a grid on a given window using a given view.
     *
     * @param window - the window to draw the grid on
     * @param view - the view to use for positioning the grid
     */
    void drawGrid(sf::RenderWindow& window, sf::View& view, sf::FloatRect& gridBounds);


    /** Get the names of graphs stored in a file.
     *
     * @param file - the file to read from.
     * @return a vector containing the names of the graphs stored in the file.
     */
    std::vector<std::string> getGraphNamesFromFile(std::ifstream& file);

#endif //ALGOVIZ_UTILS_H
