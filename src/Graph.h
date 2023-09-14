/**
 * This code represents a graph that contains nodes and edges.
 * This code provides various methods to manipulate the graph, such as adding and removing nodes and edges,
 * setting start and target nodes, performing different types of graph search algorithms and rendering the graph.
 * The Graph class is implemented using smart pointers and the nodes and edges are stored as
 * std::shared_ptr objects, allowing for automatic memory management and preventing memory leaks.
 *
 * Created by User on 12/23/2022.
 */

#ifndef ALGOVIZ_GRAPH_H
#define ALGOVIZ_GRAPH_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "Node.h"
#include "Edge.h"
#include "Interface.h"
#include <SFML/Graphics.hpp>
#include <thread>


#define BG_COLOR (sf::Color(0,0,50,255))

class Graph {
    int nodes_num = 0;
    int edges_num = 0;
    int name_count = 0;
    bool directed = false;

    std::shared_ptr<Node> start_node = nullptr; // pointer to the starting node for pathfinding algorithms
    std::shared_ptr<Node> target_node = nullptr; // pointer to the target node for pathfinding algorithms
    std::shared_ptr<Node> toggled_node = nullptr; // pointer to a node that has been toggled by the user
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes_list; // map of nodes using their names
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Node>>> neighbors_list; // map of nodes and their neighbor nodes
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Edge>>> edges_list; // map of nodes and their connected edges


    /** Helper function for rendering the graph and waiting after each object rendering.
       *
       * @param window - the window to render the graph on
       * @param toolbar - the toolbar for user interaction
       * @param original_view - the original view of the window before any zooming
       * @param current_view - the current view of the window
       * @param font - pointer to the font used for rendering text
       * @param wait - flag indicating whether to wait after each object rendering or not
       * @param is_mid_run - flag indicating whether the rendering is part of a pathfinding algorithm. if it's true,
       * the grid and other parts won't get rendered.
       */
    void renderAndWait(sf::RenderWindow& window, Toolbar& toolbar, sf::View original_view, sf::View current_view,
                       sf::Font* font, sf::FloatRect& grid_bounds, bool wait = true, bool is_mid_run = true);


    /** Recursive function for performing depth-first search on the graph.
     *
     * @param prev - the previous node in the DFS traversal
     * @param start - the source node for the DFS traversal
     * @param window - the window to render the graph on
     * @param toolbar - the toolbar for user interaction
     * @param original_view - the original view of the window before any zooming
     * @param current_view - the current view of the window
     * @param font - pointer to the font used for rendering text
     * @param wait - flag indicating whether to wait after rendering a new state of some object in the graph or not
     *
     * @return True if the target node is found during the traversal, false otherwise.
     */
    bool dfs(const std::shared_ptr<Node>& prev, const std::shared_ptr<Node>& start, sf::RenderWindow& window, Toolbar& toolbar, sf::View& original_view, sf::View& current_view, sf::Font* font, sf::FloatRect& grid_bounds, bool wait = false);


    /** Helper function for finding the node with the minimum distance in Dijkstra's algorithm.
      *
      * @return pointer to the node with the minimum distance
      */
    std::shared_ptr<Node> dijkstraMinDistance() const;


    /** Helper function for generating a unique node name.
     *
     * @return a unique node name string
     */
    std::string generateNodeName() const;


    /** Helper function for calculating the distances between the start node and all other nodes in the graph
     *
     */
    void calculate_distances();

    public:
    explicit Graph() = default;


    /** Copy constructor for the Graph class.
     *
     * @param other - the Graph object to copy from
     */
    Graph(const Graph& other);


    /** Default destructor
     *
     */
    ~Graph() = default;


    /** Sets the starting node for the graph traversal.
     *
     * @param newStartNode - the new starting node
     */
    void setStartNode(const std::shared_ptr<Node>& newStartNode);


    /** Sets the target node for the graph traversal.
     *
     * @param target_node - the new target node
     */
    void setTargetNode(const std::shared_ptr<Node>& target_node);


    /** Renders the graph on a specified window using the specified font for texts.
     *
     * @param target - the window to render the graph on
     * @param font - pointer to the font used for rendering text
     */
    void render(sf::RenderWindow& target, sf::Font* font);


    /** Adds a node to the graph.
     *
     * @param node - the node to add
     */
    void addNode(std::shared_ptr<Node>& node);


    /** Removes a node from the graph by its name.
     *
     * @param node_name - the name of the node to remove
     */
    void removeNode(const std::string& node_name);


    /** Adds an edge to the graph.
     *
     * @param edge - the edge to add
     */
    void addEdge(std::shared_ptr<Edge>& edge);



    /** Removes an edge from the graph.
     *
     * @param edge - the edge to remove
     */
    void removeEdge(const std::shared_ptr<Edge>& edge);


    /** Sets the target node pointer to nullptr.
     *
    */
    void removeTargetNode();


    /** Checks if an edge exists in the graph.
     *
     * @param edge - the edge to check for existence
     *
     * @return true if the edge exists, false otherwise
     */
    bool containsEdge(const std::shared_ptr<Edge>& edge);


    /** Runs Breadth-First Search on the graph.
     *
     * @param window - the window to render the graph on
     * @param toolbar - the toolbar for user interaction
     * @param original_view - the original view of the window before any zooming
     * @param current_view - the current view of the window
     * @param font - pointer to the font used for rendering text
     * @param wait - flag indicating whether to wait after each object rendering or not. set to false when
     * called by END-operation.
     */
    void runBFS(sf::RenderWindow& window, Toolbar& toolbar, sf::View& original_view, sf::View& current_view, sf::Font* font, sf::FloatRect& grid_bounds, bool wait = false);


    /** Runs Depth-First Search on the graph.
     *
     * @param window - the window to render the graph on
     * @param toolbar - the toolbar for user interaction
     * @param original_view - the original view of the window before any zooming
     * @param current_view - the current view of the window
     * @param font - pointer to the font used for rendering text
     * @param wait - flag indicating whether to wait after each object rendering or not. set to false when
     * called by END-operation.
     */
    void runDFS(sf::RenderWindow& window, Toolbar& toolbar, sf::View& original_view, sf::View& current_view, sf::Font* font, sf::FloatRect& grid_bounds, bool wait = false);


    /** Runs Dijkstra's algorithm on the graph.
     *
     * @param window - the window to render the graph on
     * @param toolbar - the toolbar for user interaction
     * @param original_view - the original view of the window before any zooming or panning
     * @param current_view - the current view of the window
     * @param font - pointer to the font used for rendering text
     * @param wait - flag indicating whether to wait after each object rendering or not. set to false when
     * called by END-operation.
     */
    void runDijkstra(sf::RenderWindow& window, Toolbar& toolbar, sf::View& original_view, sf::View& current_view, sf::Font* font, sf::FloatRect& grid_bounds, bool wait = false);


    /** Resets the graph, clearing all nodes and edges.
     *
     */
    void reset();


    /** Sets the node to be toggled for user interaction.
     *
     * @param to_toggle - the node to be toggled
     */
    void setToggledNode(std::shared_ptr<Node>& to_toggle);


    /** Clears the toggled node.
     *
     */
    void untoggle();


    /** Checks if a given node position is valid in the graph.
     *
     * @param node - the node to check for validity
     *
     * @return true if the position is valid, false otherwise
     */
    bool checkValidPosition(const Node& node) const;


    /** Returns a const reference to the source node of the graph.
     *
     * @return const reference to the source node
     */
    const std::shared_ptr<Node>& getStartNode() const;


    /** Returns a pointer to a node located at the given position on the window.
     *
     * @param pos_x - the x-coordinate of the position to check for a node
     * @param pos_y - the y-coordinate of the position to check for a node
     * @return pointer to the node at the given position, or nullptr if none exists
     */
    std::shared_ptr<Node> getNodeByPosition(float pos_x, float pos_y);


    /** Returns a pointer to an edge located at the given position on the window.
     *
     * @param pos_x - the x-coordinate of the position to check for an edge
     * @param pos_y - the y-coordinate of the position to check for an edge
     * @return pointer to the edge at the given position, or nullptr if none exists
     */
    std::shared_ptr<Edge> getEdgeByPosition(float pos_x, float pos_y);


    /** Returns a pointer to the edge that connects the given nodes in the graph.
     *
     * @param node1 - pointer to one of the nodes to check for a connecting edge
     * @param node2 - pointer to the other node to check for a connecting edge
     * @return pointer to the edge connecting the given nodes, or nullptr if none exists
     */
    std::shared_ptr<Edge> getEdgeByNodes(const std::shared_ptr<Node>& node1, const std::shared_ptr<Node>& node2);


    /** Adds a new node to the graph with the given position and font for rendering text.
     *
     * @param pos_x - the x-coordinate of the position to add the new node
     * @param pos_y - the y-coordinate of the position to add the new node
     * @param text_font - pointer to the font used for rendering text
     * @return pointer to the newly added node
     */
    std::shared_ptr<Node> addNode(float pos_x, float pos_y, sf::Font* text_font);


    /** Returns a pointer to the node in the graph that collides with the given node.
     *
     * @param moving_node - pointer to the node to check for collisions with other nodes
     * @return pointer to the colliding node, or nullptr if none exists
     */
    std::shared_ptr<Node> getCollidedNode(const std::shared_ptr<Node>& moving_node) const;



//Todo: wha2 this?
std::string getLiteral();

};


#endif //ALGOVIZ_GRAPH_H
