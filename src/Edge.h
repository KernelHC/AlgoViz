/**
 * This code represents the appearance and state of an Edge between 2 given nodes in a graph.
 * An Edge connects two nodes and can have an associated weight or length.
 *
 * This code uses std::shared_ptr to manage dynamically allocated objects.
 * std::shared_ptr provides automatic memory management by keeping track of the number
 * of shared pointers that point to the object. When there are no more shared pointers
 * pointing to the object, it is automatically deleted.
 *
 * This code uses sf::RectangleShape to represent an edge in order to provide a simple and flexible
 * way to display edges in a graphical user interface.
 *
 * Created by User on 12/19/2022.
 */


#ifndef ALGOVIZ_EDGE_H
#define ALGOVIZ_EDGE_H

#include "Node.h"
#include "SFML/Graphics.hpp"

#define UNDISCOVERED_EDGE_COLOR (sf::Color(150, 150, 150, 70))
#define DISCOVERED_EDGE_COLOR   (sf::Color::White)
#define SELECTED_EDGE_COLOR     (sf::Color::Red)
#define NEAREST_EDGE_COLOR      (sf::Color::Magenta)
#define NODE_RADIUS 30

enum EdgeState {
    EDGE_DISCOVERED,
    EDGE_UNDISCOVERED,
    EDGE_NEAREST,
    EDGE_SELECTED
};

class Edge {
    sf::RectangleShape shape;
    std::string literal;
    const std::shared_ptr<Node> first_node;
    const std::shared_ptr<Node> second_node;
    float length;

public:

    /** Creates an undirected edge between the first and second nodes.
     *
     * @param first_node
     * @param second_node
     * @param do_correct Set to true if  , false otherwise
     */
     //Todo: what's the purpose of do_correct?
    Edge(const std::shared_ptr<Node>& first_node, const std::shared_ptr<Node>& second_node, bool do_correct = true);


    /** Renders the shape of this edge to the specified render target.
     *
     * @param target The render window to which the contents will be drawn.
     */
    void render(sf::RenderTarget& target);


    /** Sets the color of this edge.
     *
     * To be used based on the current state.
     *
     * @param color Set according to the current state.
     */
    void setColor(const sf::Color& color);


    /** Sets the state of this edge.
     *
     * Used while traversing the graph in BFS/DFS/DIJKSTRA.
     *
     * @param state The current state, as defined by the algorithm's progression.
     */
    void setState(EdgeState state);


    /** Returns the length of this edge as an int.
     *
     * @return The length of this edge, casted to an integer.
     */
    int getLength() const;


    /** Returns true if this edge is shorter than the other edge, false otherwise.
     *
     * Compares this edge with another edge for ordering purposes in the context of Dijkstra's algorithm.
     *
     * @param other The other edge to compare with this edge.
     * @return True if this edge is shorter than the other edge, false otherwise.
     */
    bool operator<(const std::shared_ptr<Edge>& other) const;


    /** Returns true if the two edges have the same length.
     *
     * Compares this edge with another edge for equality in the context of Dijkstra's algorithm.
     *
     * @param other The other edge to compare with this edge.
     * @return True if this edge is the same length as the other edge, false otherwise.
     */
    bool operator==(const std::shared_ptr<Edge>& other) const;


    /** Returns the corrected endpoint coordinates as a vector of floats.
     *
     * This function takes the coordinates of the two nodes that the edge connects, calculates the angle and length
     * of the edge, and corrects the endpoint coordinates and rotation angle to ensure that the edge is properly
     * aligned and centered between the nodes.
     *
     * @return A vector of floats representing the corrected endpoint coordinates of the edge.
     */
    std::vector<float> correctEdgeCoordinates();


    /** Returns a shared pointer to the first node that this edge connects.
     *
     * @return A shared pointer to the first node that this edge connects.
     */
    std::shared_ptr<Node> getFirstNode() const;


    /** Returns a shared pointer to the second node that this edge connects.
     *
     * @return A shared pointer to the second node that this edge connects.
     */
    std::shared_ptr<Node> getSecondNode() const;


    /** Returns a copy of the shape object that represents this edge.
     *
     * This function returns a copy of the sf::RectangleShape object that represents this edge, which is a
     * rectangular shape that appears between the two nodes that the edge connects.
     *
     * @return A copy of the sf::RectangleShape object that represents this edge.
     */
    sf::RectangleShape getShape() const;


    /** Default destructor
     *
     */
    ~Edge() = default;


};

#endif //ALGOVIZ_EDGE_H
