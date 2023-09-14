#ifndef ALGOVIZ_VISUALIZER_H
#define ALGOVIZ_VISUALIZER_H
#include "Graph.h"
#include "Interface.h"

enum VisMode {
    BFS,
    DFS,
    DIJKSTRA
};



/** This class represents a graph visualization tool with various functionalities.
 *
 * Allows users to create, edit, and visualize graphs using a graphical user interface.
 */

class Visualizer {
public:
    /** Constructs a Visualizer object with an empty graph.
     *
     */
    explicit Visualizer(const Graph& graph = Graph());


    /** Default destructor.
     *
     */
    ~Visualizer();


    /** Starts the main loop of the visualization tool.
     *
     */
    void run();


private:
    sf::RenderWindow* window;
    sf::Event sfEvent{};
    sf::View original_view;
    sf::View current_view;
    Graph graph;
    Toolbar toolbar;
    SavedGraphsList saved_graphs_list;
    std::shared_ptr<Node> clicked_node;
    sf::Font* vis_font;
    bool node_is_clicked = false;
    float current_zoom_factor = 1.f;
    sf::FloatRect* grid_bounds;


    /** Parses the graph_literal parameter and creates Node objects for each node in the graph.
     *
     * Returns a vector of shared pointers to the created Node objects.
     */
    static std::vector<std::shared_ptr<Node>> parseNodesFromString(const std::string& graph_literal, sf::Font* font);


    /** Parses the graph_literal parameter and creates Edge objects for each edge in the graph.
     *
     * Returns a vector of shared pointers to the created Edge objects.
     */
    static std::vector<std::shared_ptr<Edge>> parseEdgesFromString(const std::string& graph_literal,
                                                                   std::vector<std::shared_ptr<Node>>& nodes);

    /** Checks whether the current view is within the bounds of the visualization window.
     *
     * @return True if the current view is within the bounds of the visualization window, false otherwise.
     */
    bool viewIsInBounds();


    /** Renders the visualization of the graph.
     *
     * @param load_list A boolean indicating whether to load the graph from saved_graphs_list.
     */
    void render(bool load_list = false);


    /** Executes an action when a node/edge/button is clicked.
     *
     */
    void executeClickAction();


    /** Runs the algorithm on the graph. This function applies the selected algorithm (BFS/DFS/DIJKSTRA) on the graph
     * and updates the visualization accordingly.
     * If no algorithm is selected, it does nothing.
     */
    void runAlgorithm();


    /** Performs the routine for the cursor, updating the position of the cursor on the screen.
     *
     * Helps move nodes and views around.
     *
     */
    void cursorRoutine();


    /** Adds a node to the graph and updates the graph accordingly.
     *
     */
    void addNodeRoutine();


    /** Adds an edge between two nodes in the graph.
     *
     */
    void addEdgeRoutine();


    /** Erases a node or an edge from the graph.
     *
     */
    void eraseRoutine();


    /** Changes the start node of the graph.
     *
     */
    void changeStartNodeRoutine();


    /** Ends the running algorithm, completing it without waiting after each step of the algorithm.
     *
     * It is useful for stopping a long-running algorithm prematurely.
     *
    */
    void endRoutine();


    /** Resets the current graph, clearing all algorithm-related changes, and stopping any running algorithm.
     *
     */
    void resetRoutine();


    /** Clears the window, creating a new empty graph, and stopping any running algorithm.
     *
     */
    void clearWindowRoutine();


    /** Chooses the target node by setting the node that is clicked on as the new target node.
     *
     */
    void chooseTargetNodeRoutine();


    /** Unsets the current target node.
     *
     */
    void removeTargetNodeRoutine();


    /** Saves the current graph to a file. Prompts the user to input a name for the graph.
     *
     * The saved graph is added to the list of saved graphs and saved to a file called "SavedGraphs.txt".
     *
     */
    void saveToFile();


    /** Loads a graph from "SavedGraphs.txt", selected by the user.
     *
     * Displays a list of saved graphs and prompts the user to select a graph. The selected graph is
     * parsed from a file called "SavedGraphs.txt" and added to the window. Any running algorithm is stopped
     * and the current graph is cleared.
     *
     */
    void loadFromFile();

};

#endif //ALGOVIZ_VISUALIZER_H
