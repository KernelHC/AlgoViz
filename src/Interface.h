//
// Created by User on 3/11/2023.
//

#ifndef ALGOVIZ_INTERFACE_H
#define ALGOVIZ_INTERFACE_H

#include <memory>
#include <SFML/Graphics.hpp>

class Button;
class Toolbar;
class TextBox;
class SavedGraphsList;

//-----------------------------------------------SavedGraphsList Class--------------------------------------------------

/** The SavedGraphsList class stores graphs and their names in a map and allows them to be saved to
 * and loaded from a file.
 * The class provides methods for inserting, erasing and checking if a graph is present.
 */
class SavedGraphsList {

private:
    std::map<std::string, std::shared_ptr<sf::RectangleShape>> saved_graphs;
    unsigned int saved_graphs_num = 0;
    sf::Font* text_font = nullptr;

public:
    /** Default constructor.
     *
     */
    SavedGraphsList() = default;


    /** Constructor for SavedGraphsList that loads saved graphs from a file.
     *
     * @param file - the file to load the saved graphs from.
     * @param font - the font used for rendering graph names.
     */
    SavedGraphsList(std::ifstream& file, sf::Font* font);


    /** Default destructor.
     *
     */
    ~SavedGraphsList() = default;


    /** Add a new graph to the saved graphs list.
     *
     * @param graph_name - the name of the graph to add
     */
    void addGraph(const std::string& graph_name);


    /** Delete a graph from the saved graphs list.
     *
     * @param graph_name - the name of the graph to delete.
     */
    void deleteGraph(const std::string& graph_name);


    /** Check if a given graph is present in the saved graphs list.
     *
     * @param graph_name - the name of the graph to check for presence.
     * @return True if the graph is present, false otherwise.
     */
    bool contains(const std::string& graph_name);


    /** Check if the saved graphs list is empty.
     *
     * @return True if the saved graphs list is empty, false otherwise
     */
    [[nodiscard]] bool empty() const;


    /** Render the saved graphs list to the given window.
     *
     * @param window - the window to render the saved graphs list to.
     */
    void render(sf::RenderWindow& window);


    /** Get the name of the graph that was clicked on in the saved graphs list.
     *
     * @param pos_x - the x position of the click in the window
     * @param pos_y - the y position of the click in the window
     * @return The name of the graph that was clicked on, or an empty string if no graph from
     * the saved graphs was clicked on
     */
    std::string getClickedGraph(float pos_x, float pos_y);

};

//-------------------------------------------------Button Class---------------------------------------------------------

enum ButtonId {
    CURSOR,
    ADD_NODE,
    ADD_EDGE,
    ERASE,
    CHANGE_START_NODE,
    CHOOSE_TARGET_NODE,
    REMOVE_TARGET_NODE,
    RUN_BFS, RUN_DFS,
    RUN_DIJKSTRA,
    END,
    RESET,
    CLEAR_WINDOW,
    SAVE_TO_FILE,
    LOAD_FROM_FILE
};

class Button {

private:
    sf::RectangleShape shape;
    std::string icon_path;
    ButtonId id;

public:
    /** Constructs a new Button object.
    *
    * @param x The x-coordinate of the button's top-left corner.
    * @param y The y-coordinate of the button's top-left corner.
    * @param width The width of the button.
    * @param height The height of the button.
    * @param txt The path of the button's icon.
    * @param id The unique identifier of the button.
    */
    Button(float x, float y, float width, float height, const std::string& txt, ButtonId id);


    /** Constructs a default Button object with the CURSOR id.
     *
     */
    Button(): id(CURSOR){}


    /** Assigns the values of one Button object to another.
     *
     * @param other The Button object to copy.
     * @return Button& The reference to the copied Button object.
     */
    Button& operator=(const Button& other);


    /** Checks if the mouse is hovering over the button and updates the clicked button (active button) accordingly.
     *
     * @param mousePosWindow The position of the mouse in the window.
     * @return Whether or not the button was clicked.
     */
    bool update(const sf::Vector2f& mousePosWindow);


    /** Disables the button by setting its color to fully opaque.
     *
     */
    void setButtonDisabled();


    /** Changes the fill color of the button to indicate that it is enabled.
     * This function sets the fill color of the button's shape to a semi-transparent color
     * to indicate that the button is enabled.
     */
    void setButtonEnabled();


    /** Renders the button onto the specified render target.
     *
     * @param target The render target to render the button onto.
     */
    void render(sf::RenderTarget& target);


    /** Returns the ID of the button.
     *
     * @return ButtonId of the button.
     */
    ButtonId getId() const;


    /** Default destructor.
     *
     */
    ~Button();

};


//-------------------------------------------------Toolbar Class--------------------------------------------------------


class Toolbar {
private:
    std::shared_ptr<Button> active_button;
    std::vector<std::shared_ptr<Button>> buttons;
    sf::RectangleShape rectangle;
    std::vector<sf::RectangleShape> horizontal_separators;

public:

    /** Constructs a new Toolbar object.
     *
    */
    Toolbar();


    /** Updates the active button in the toolbar according to the mouse position.
     *
     * @param mousePosWindow The position of the mouse in the window.
     * @return Whether or not a button was clicked.
     */
    bool updateActiveButton(const sf::Vector2f& mousePosWindow);


    /** Renders the toolbar and its buttons onto the specified render window.
     *
     * @param window The render window to render the toolbar onto.
     * @param mid_run Whether or not the function is called in the middle of an algorithm run.
     */
    void render(sf::RenderWindow& window, bool mid_run = false);


    /** Resets the active button to the cursor button after a clean operation.
     *
     */
    void resetActiveButton();


    /** Gets the ID of the active button.
     *
     * @return ButtonId of the active button.
     */
    ButtonId getActiveButtonId() const;


    /** Default destructor.
     *
    */
        ~Toolbar() = default;
};



//-------------------------------------------------TextBox Class--------------------------------------------------------


class TextBox {

private:
    sf::RectangleShape text_box;
    sf::RectangleShape text_field;
    sf::Font* text_font;

public:
    /** Constructs a new TextBox object with the given font and creates the necessary sf::RectangleShapes.
     *
     * @param window The window in which the text box will be displayed.
     * @param font A pointer to the font used to display the text.
     */
    TextBox(sf::RenderWindow& window, sf::Font* font);

    //todo: this
    bool close(float event_x, float event_y);


    /** Renders the text box on the given window and displays the given text.
     *
     * @param window The window on which the text box will be displayed.
     * @param txt The text to be displayed in the text box.
     */
    virtual void render(sf::RenderWindow& window, const std::string& txt);


    /** Default destructor.
     *
     */
    ~TextBox() = default;

};


//-------------------------------------------------InputBox Class--------------------------------------------------------

class InputBox : TextBox {

private:
    std::string input;
    /** Converts a Unicode character to an ASCII character.
    *
    * Is called to convert the input to the appropriate format before it is displayed on the screen.
    *
    * @param uni_char - the Unicode character to convert
    * @return the corresponding ASCII character, or 0 if the input is not valid
    */
    static char unicodeToAscii(unsigned int uni_char);

public:
    /** Construct a new InputBox object
     *
     * @param window The RenderWindow where the text box will be rendered.
     * @param font A pointer to the font to use for rendering text.
     */
    InputBox(sf::RenderWindow& window, sf::Font* font): TextBox(window, font){};


    /** Render the text box and the text entered by the user.
     *
     * @param window The RenderWindow where the text box will be rendered.
     * @param txt The text to display in the text box.
     */
    void render(sf::RenderWindow& window, const std::string& txt) override;


    /** Waits for the user to enter a string into the inputBox.
     *
     * @param window - the window to display the inputBox on.
     * @return a string containing the user's input.
     */
    std::string getInput(sf::RenderWindow& window);

};


#endif //ALGOVIZ_INTERFACE_H
