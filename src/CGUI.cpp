#include <ncurses.h>
#include <menu.h>
#include <string>
#include <string.h>

#include "CGUI.h"
#include "CNode.h"
#include "CXML.h"
#include "functions.h"
#include "CException.h"

///! When reallocing, how many times will new array will be bigger
#define REALLOC_CONSTANT 2
///! Start size of main XML tree
#define DEFAULT_MENU_ITEMS_COUNT 100
///! Height of control window
#define CONTROLS_LINES 3
///! Height of console window
#define CONSOLE_LINES 2

///! Specifier that i am inserting new root node
#define INSERTING_TO_ROOT -1

///! Maximum of chars in user input
#define MAX_INPUT 2500

/********************* PUBLIC METHODS *******************************/

/*! Initializes the curses environment and creates new windows.
 * \param openingXML Was the XML opened from the command line?.
 */
CGUI::CGUI(bool openingXML) {
    //allocates the space for tree
    m_menuItems = new ITEM * [DEFAULT_MENU_ITEMS_COUNT];
    m_nodes = new CNode * [DEFAULT_MENU_ITEMS_COUNT];
    for (int i = 0; i < DEFAULT_MENU_ITEMS_COUNT; i++) {
        m_menuItems[i] = NULL;
        m_nodes[i] = NULL;
    }
    m_cntNodes = 0;
    m_menuSize = DEFAULT_MENU_ITEMS_COUNT;

    //sets state
    m_xmlOpened = openingXML;
    m_treeInitialized = false;

    //starts ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    //counts window's size
    CountControlWindowSize();
    CountTreeWindowSize();
    CountConsoleWindowSize();
    refresh();

    //creates new windows
    CreateControlWindow();
    CreateConsoleWindow();
    CreateTreeWindow();

    //if there is no XML to open, starts handling curses user input
    if (!openingXML)
        Handler();
}

/*! Tidies up.
 */
CGUI::~CGUI() {
    //removes menu and the shown tree
    TreeDestroy();

    //ends curses
    attroff(COLOR_PAIR(1));
    delwin(m_tree.win);
    delwin(m_control.win);
    delwin(m_console.win);
    endwin();


    //deallocates used memory
    if (m_xmlOpened) {
        delete m_xmlfile;
    }
    delete [] m_menuItems;
    delete [] m_nodes;
}

/********************* INSERTING TO MENUS METHODS *******************************/

/*! Inserts an item to main tree.
 * \param name Text of the item.
 * \param node Pointer to node bound with this text.
 * \param type P means parent node, T means text node, S means simple node and C means comment node
 */
void CGUI::AddMenuItem(const string* name, CNode * node, const char * type) {
    ReallocItems(); //ensure that the array is not full
    m_menuItems[m_cntNodes] = new_item(name->c_str(), type);
    m_nodes[m_cntNodes++] = node;
}

/*! Inserts an item to an attributes menu.
 * \param name Name of attribute.
 * \param value Value of an attribute.
 */
void CGUI::AddAttributeItem(const string* name, const string* value) {
    ReallocAttributes(); //ensure that the array is not full
    m_attributesItems[m_cntAttributes++] = new_item(name->c_str(), value->c_str());
}

/********************* PUBLIC USER INPUT HANDLERS *******************************/

/*! An infinite loop, which allows the user to perform wanted actions while showing the XML file.
 */
void CGUI::TreeHandler() {
    int c; //to get id of pressed key
    int id; // id of current menu item
    char str[MAX_INPUT]; //for user string input
    string title; //for user string input

    m_treeInitialized = true; //set state

    PostDefaultControlWindow();

    //pressing F12 ends the program (and the loop)
    while ((c = wgetch(m_tree.win)) != KEY_F(12)) {
        switch (c) {
            case KEY_DOWN: //down arrow button
                menu_driver(m_menu, REQ_DOWN_ITEM);
                ConsolePrint("Console:");
                break;
            case KEY_UP: //up arrow button
                menu_driver(m_menu, REQ_UP_ITEM);
                ConsolePrint("Console:");
                break;
            case KEY_NPAGE: //page down button
                menu_driver(m_menu, REQ_SCR_DPAGE);
                ConsolePrint("Console:");
                break;
            case KEY_PPAGE://page up button
                menu_driver(m_menu, REQ_SCR_UPAGE);
                ConsolePrint("Console:");
                break;

            case KEY_F(2):// F2 - Expand current node
                id = item_index(current_item(m_menu));

                m_nodes[id]->Expand();

                //whole tree has to be build again
                TreeDestroy();
                m_xmlfile->Show();
                TreeInit();

                //highlight current item again
                while (item_index(current_item(m_menu)) != id) {
                    menu_driver(m_menu, REQ_DOWN_ITEM);
                }
                ConsolePrint("Console:");
                break;
            case KEY_F(3): //F3 - Collapse current node
                id = item_index(current_item(m_menu));

                m_nodes[id]->Collapse();

                //whole tree has to be build again
                TreeDestroy();
                m_xmlfile->Show();
                TreeInit();
                //highlight current item again
                while (item_index(current_item(m_menu)) != id) {
                    menu_driver(m_menu, REQ_DOWN_ITEM);
                }
                ConsolePrint("Console:");
                break;

            case KEY_F(4): // F4 - Show attributes of current node and enable their editing
                id = item_index(current_item(m_menu));

                //comment nodes doesn't have attributes
                if (m_nodes[id]->HasAttributes()) {
                    ConsolePrint("Console:");

                    //change the environment and handling
                    PostAttributesControlWindow();
                    AttributesHandler(id);

                    //rebuild the tree
                    m_xmlfile->Show();
                    TreeInit();

                    //scroll to the current item
                    while (item_index(current_item(m_menu)) != id) {
                        menu_driver(m_menu, REQ_DOWN_ITEM);
                    }
                    ConsolePrint("Console: Finished editing attributes.");
                    PostDefaultControlWindow();
                } else {
                    ConsolePrint("Console: Attributes are not allowed for comments.");
                }
                break;

            case KEY_F(5): //F5 - inserting new node
                if (m_cntNodes) { //if we are inserting to a given node
                    id = item_index(current_item(m_menu));

                    //inserting is allowed only for parent nodes
                    if (m_nodes[id]->HasChilds()) {
                        ConsolePrint("Console: Which node do you want to create?");

                        //change to inserting environment
                        PostInsertingControlWindow();
                        InsertingHandler(id);

                        //rebuild the tree
                        TreeDestroy();
                        m_xmlfile->Show();
                        TreeInit();

                        //scroll to the current item
                        while (item_index(current_item(m_menu)) != id) {
                            menu_driver(m_menu, REQ_DOWN_ITEM);
                        }
                        ConsolePrint("Console: Finished inserting node.");
                        PostDefaultControlWindow();

                    } else {
                        ConsolePrint("Console: Inserting is allowed only for parent nodes.");
                    }
                } else {
                    //inserting to root (the file is free now)
                    ConsolePrint("Console: Which node do you want to create?");

                    //change environment
                    PostInsertingControlWindow();
                    InsertingHandler(INSERTING_TO_ROOT);

                    //rebuild the tree
                    TreeDestroy();
                    m_xmlfile->Show();
                    TreeInit();

                    ConsolePrint("Console: Finished inserting node.");
                    PostDefaultControlWindow();
                }
                break;

            case KEY_F(6): //F6 - Removing nodes
                id = item_index(current_item(m_menu));
                //deleting a node with a parent
                if (m_nodes[id]->GetParent() != NULL)
                    m_nodes[id]->GetParent()->DeleteNode(m_nodes[id]->GetID());
                else {
                    //deleting root node
                    delete m_nodes[id];
                    m_xmlfile->SetRoot(NULL);
                }

                //rebuild the tree
                TreeDestroy();
                m_xmlfile->Show();
                TreeInit();

                //scroll to previous item (if there is any)
                if (m_cntNodes) {
                    while (item_index(current_item(m_menu)) != id - 1 && id != 0) {
                        menu_driver(m_menu, REQ_DOWN_ITEM);
                    }
                }
                ConsolePrint("Console: Node successfully deleted.");
                break;

            case KEY_F(7): //F7 - Filtering the tree
                ConsolePrint("Enter requested title: ");

                //user input
                echo();
                wgetnstr(m_console.win, str, MAX_INPUT);
                noecho();
                title = str;

                //process filtering 
                m_xmlfile->Filter(title);

                //rebuild the tree
                TreeDestroy();
                m_xmlfile->Show();
                TreeInit();
                ConsolePrint("Console: Filtered.");
                break;

            case KEY_F(8): //F8 - Saving the file
                m_xmlfile->Save();
                ConsolePrint("Console: File saved.");
                break;

            case KEY_F(9): //F9 - Opening new file
                ConsolePrint("Enter file path: ");

                //user input
                echo();
                wgetnstr(m_console.win, str, MAX_INPUT);
                noecho();
                title = str;

                //opening may fail, so change states first
                m_xmlOpened = false;
                m_treeInitialized = false;

                //try to open the file
                try {
                    m_xmlfile = OpenFile(m_xmlfile, title, this);
                } catch (const CException & e) {
                    wclear(m_tree.win);
                    wborder(m_tree.win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                    e.Print(this);
                    return;
                }
                m_xmlOpened = true;
                wclear(m_tree.win);
                wborder(m_tree.win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

                //if successful, rebuild the tree
                TreeDestroy();
                m_xmlfile->Show();
                TreeInit();
                break;
        }
        wrefresh(m_tree.win);
    }
}

/*! An infinite loop, which allows the user to perform wanted actions while there is no XML file.
 */
void CGUI::Handler() {
    int c; //to specify pressed key
    char str[MAX_INPUT]; //for user input
    string file; //for user input

    PostStartControlWindow();

    //program ends when F12 is pressed
    while ((c = wgetch(m_tree.win)) != KEY_F(12)) {
        switch (c) {
            case KEY_F(9): //F9 - open a file
                ConsolePrint("Enter file path: ");

                //user input
                echo();
                wgetnstr(m_console.win, str, MAX_INPUT);
                noecho();
                file = str;

                //opening may fail, setting states
                m_xmlOpened = false;
                m_treeInitialized = false;

                //trying to open the file
                try {
                    m_xmlfile = OpenFile(NULL, file, this);
                } catch (const CException & e) {
                    e.Print(this);
                    return;
                }
                
                m_xmlOpened = true;
                wclear(m_tree.win);
                wborder(m_tree.win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

                //if successful, rebuild the tree
                TreeDestroy();
                m_xmlfile->Show();
                TreeInit();

                //... and give handling to the Tree Handler
                TreeHandler();
                return;
        }
        wrefresh(m_tree.win);
    }
}


/********************* MENU INITIALIZERS AND DESTROYERS *******************************/

/*! Initializes the XML tree (creates the menu)
 */
void CGUI::TreeInit() {
    //load attributes
    m_menu = new_menu((ITEM **) m_menuItems);

    wclear(m_tree.win);

    //specify menu's details
    set_menu_win(m_menu, m_tree.win);
    set_menu_sub(m_menu, derwin(m_tree.win, m_tree.height, m_tree.width, 1, 1));
    set_menu_mark(m_menu, " -> ");
    set_menu_format(m_menu, m_tree.height, 1);

    post_menu(m_menu);
    wrefresh(m_tree.win);
}

/*! Initializes the attributes list for given node. (creates the menu)
 */
void CGUI::AttributesInit() {
    //load attributes
    m_attributes = new_menu((ITEM **) m_attributesItems);

    wclear(m_tree.win);

    //specify menu's details
    set_menu_win(m_attributes, m_tree.win);
    set_menu_sub(m_attributes, derwin(m_tree.win, m_tree.height, m_tree.width, 1, 1));
    set_menu_mark(m_attributes, " -> ");
    set_menu_format(m_attributes, m_tree.height, 1);

    post_menu(m_attributes);
    wrefresh(m_tree.win);
}

/*! Frees and removes the XML tree and menu.
 */
void CGUI::TreeDestroy() {
    if (m_xmlOpened) {
        unpost_menu(m_menu);
        free_menu(m_menu);
    }
    for (int i = 0; i < m_cntNodes; i++) {
        free_item(m_menuItems[i]);
    }

    //be ready for new items inserting
    m_menuItems = new ITEM * [m_menuSize];
    for (int i = 0; i < m_menuSize; i++) {

        m_menuItems[i] = NULL;
        m_nodes[i] = NULL;
    }
    m_cntNodes = 0;
}

/*! Frees and removes the attributes tree and menu.
 */
void CGUI::AttributesDestroy() {
    unpost_menu(m_attributes);
    free_menu(m_attributes);
    for (int i = 0; i < m_cntAttributes; i++) {
        free_item(m_attributesItems[i]);
    }

    //be ready for new items inserting
    m_attributesItems = new ITEM * [m_attributesSize];
    for (int i = 0; i < m_attributesSize; i++) {
        m_attributesItems[i] = NULL;
    }
    m_cntAttributes = 0;
}

/********************* TOOL FOR PRINTING TO CONSOLE *******************************/

/*! Prints the demanded message to the console window in appropriate format.
 * \param str The message.
 */
void CGUI::ConsolePrint(const char * str) {
    wclear(m_console.win);
    wborder(m_console.win, ' ', ' ', '-', ' ', '+', '+', ' ', ' ');
    //print file name only if it is opened
    if (m_xmlOpened) {
        mvwprintw(m_console.win, 1, 0, m_xmlfile->GetFilePath().c_str());
        mvwprintw(m_console.win, 1, m_xmlfile->GetFilePath().length() + 1, str);
    } else {
        mvwprintw(m_console.win, 1, 0, str);
    }
    wrefresh(m_console.win);
}

/********************* GETTERS *******************************/

/*! Gets the state of tree.
 */
bool CGUI::IsTreeInitialized() const {
    return m_treeInitialized;
}

/********************* SETTERS *******************************/

/*! Sets the current XML file.
 */
void CGUI::AddXML(CXML* xml) {
    m_xmlfile = xml;
}

/*! Sets state, if there is an opened XML file.
 */
void CGUI::SetXMLOpened(bool opened) {
    m_xmlOpened = opened;
}



/********************* PRIVATE METHODS *******************************/

/********************* WINDOW SIZE COUNTERS *******************************/

/*! Counts the size of control window.
 */
void CGUI::CountControlWindowSize() {
    //m_control window has 3 lines
    m_control.height = CONTROLS_LINES;
    m_control.width = COLS;
    m_control.y = LINES - m_control.height;
    m_control.x = 0;
}

/*! Counts the size of main tree window.
 */
void CGUI::CountTreeWindowSize() {
    m_tree.height = LINES - CONTROLS_LINES - CONSOLE_LINES;
    m_tree.width = COLS;
    m_tree.y = 0;
    m_tree.x = 0;
}

/*! Counts the size of console window.
 */
void CGUI::CountConsoleWindowSize() {
    m_console.height = CONSOLE_LINES;
    m_console.width = COLS;
    m_console.y = LINES - m_console.height - CONTROLS_LINES;
    m_console.x = 0;
}

/********************* WINDOW CREATORS *******************************/

/*! Creates control window.
 */
void CGUI::CreateControlWindow() {
    m_control.win = CreateNewWindow(m_control.width, m_control.height, m_control.x, m_control.y);
    PostDefaultControlWindow();
}

/*! Creates main tree window.
 */
void CGUI::CreateTreeWindow() {
    m_tree.win = CreateNewWindow(m_tree.width, m_tree.height, m_tree.x, m_tree.y);
    wborder(m_tree.win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    keypad(m_tree.win, TRUE);
}

/*! Creates console window.
 */
void CGUI::CreateConsoleWindow() {
    m_console.win = CreateNewWindow(m_console.width, m_console.height, m_console.x, m_console.y);
}

/********************* CONTROL HINTS POSTERS *******************************/

/*! Posts control hints for main XML tree handling.
 */
void CGUI::PostDefaultControlWindow() {
    wclear(m_control.win);
    wborder(m_control.win, ' ', ' ', '-', ' ', '+', '+', ' ', ' ');
    //the gap is count using the length of every option
    int gap = (COLS - 11 - 13 - 15 - 16 - 11) / 8;

    mvwprintw(m_control.win, 1, 0, "[F2] Expand");
    mvwprintw(m_control.win, 1, 11 + gap, "|");
    mvwprintw(m_control.win, 1, 12 + 2 * gap, "[F3] Collapse");
    mvwprintw(m_control.win, 1, 25 + 3 * gap, "|");
    mvwprintw(m_control.win, 1, 26 + 4 * gap, "[F4] Attributes");
    mvwprintw(m_control.win, 1, 41 + 5 * gap, "|");
    mvwprintw(m_control.win, 1, 42 + 6 * gap, "[F5] Insert Node");
    mvwprintw(m_control.win, 1, 58 + 7 * gap, "|");
    mvwprintw(m_control.win, 1, 59 + 8 * gap, "[F6] Remove");

    mvwprintw(m_control.win, 2, 0, "[F7] Filter");
    mvwprintw(m_control.win, 2, 11 + gap, "|");
    mvwprintw(m_control.win, 2, 12 + 2 * gap, "[F8] Save");
    mvwprintw(m_control.win, 2, 25 + 3 * gap, "|");
    mvwprintw(m_control.win, 2, 26 + 4 * gap, "[F9] Open");
    mvwprintw(m_control.win, 2, 41 + 5 * gap, "|");
    mvwprintw(m_control.win, 2, 42 + 6 * gap, "[F12] Quit");
    wrefresh(m_control.win);
}

/*! Posts control hints for inserting to tree handling.
 */
void CGUI::PostInsertingControlWindow() {
    wclear(m_control.win);
    wborder(m_control.win, ' ', ' ', '-', ' ', '+', '+', ' ', ' ');
    int gap = (COLS - 11 - 13 - 15 - 16 - 11) / 8;

    mvwprintw(m_control.win, 1, 0, "[P] Parent");
    mvwprintw(m_control.win, 1, 11 + gap, "|");
    mvwprintw(m_control.win, 1, 12 + 2 * gap, "[T] Text");
    mvwprintw(m_control.win, 1, 25 + 3 * gap, "|");
    mvwprintw(m_control.win, 1, 26 + 4 * gap, "[S] Simple");
    mvwprintw(m_control.win, 1, 41 + 5 * gap, "|");
    mvwprintw(m_control.win, 1, 42 + 6 * gap, "[C] Comment");
    mvwprintw(m_control.win, 1, 58 + 7 * gap, "|");
    mvwprintw(m_control.win, 1, 59 + 8 * gap, "[F12] Cancel");
    wrefresh(m_control.win);
}

/*! Posts control hints for attributes handling.
 */
void CGUI::PostAttributesControlWindow() {
    wclear(m_control.win);
    wborder(m_control.win, ' ', ' ', '-', ' ', '+', '+', ' ', ' ');
    int gap = (COLS - 11 - 13 - 15 - 16 - 11) / 8;

    mvwprintw(m_control.win, 1, 0, "[F5] Insert");
    mvwprintw(m_control.win, 1, 11 + gap, "|");
    mvwprintw(m_control.win, 1, 12 + 2 * gap, "[F6] Remove");
    mvwprintw(m_control.win, 1, 25 + 3 * gap, "|");
    mvwprintw(m_control.win, 1, 26 + 4 * gap, "[F12] Finish");
    wrefresh(m_control.win);
}

/*! Posts control hints for handling user input without an XML file.
 */
void CGUI::PostStartControlWindow() {
    wclear(m_control.win);
    wborder(m_control.win, ' ', ' ', '-', ' ', '+', '+', ' ', ' ');
    int gap = (COLS - 11 - 13 - 15 - 16 - 11) / 8;

    mvwprintw(m_control.win, 1, 0, "[F9] Open");
    mvwprintw(m_control.win, 1, 11 + gap, "|");
    mvwprintw(m_control.win, 1, 12 + 2 * gap, "[F12] Quit");
    wrefresh(m_control.win);
}

/********************* PRIVATE USER INPUT HANDLERS *******************************/

/*! An infinite loop, which allows the user to perform wanted actions while inserting node to an XML tree.
 * \param id To which node will it be inserted.
 */
void CGUI::InsertingHandler(int id) {
    int c; //for pressed key
    char str[MAX_INPUT], txt[MAX_INPUT]; //user input
    CNode * node; //for new node
    string title, text; //user input

    //inserting is canceled, when F12 is pressed
    while ((c = wgetch(m_tree.win)) != KEY_F(12)) {
        switch (c) {
            case 'P':
            case 'p': //P - inserting a parent node
                ConsolePrint("Enter node title: ");

                //user input
                echo();
                wgetnstr(m_console.win, str, MAX_INPUT);
                noecho();
                title = str;

                //creating new node
                node = new CParentNode(title);
                if (id == INSERTING_TO_ROOT) {
                    m_xmlfile->SetRoot(node);
                } else {
                    m_nodes[id]->InsertNode(node);
                }
                return;
            case 'T':
            case 't': //T - inserting new text node
                menu_driver(m_menu, REQ_UP_ITEM);
                ConsolePrint("Enter node title: ");

                //user input
                echo();
                wgetnstr(m_console.win, str, MAX_INPUT);
                noecho();
                ConsolePrint("Enter node text: ");
                echo();
                wgetnstr(m_console.win, txt, MAX_INPUT);
                noecho();
                title = str;
                text = txt;

                //creating new node
                node = new CTextNode(title, text);
                if (id == INSERTING_TO_ROOT) {
                    m_xmlfile->SetRoot(node);
                } else {
                    m_nodes[id]->InsertNode(node);
                }
                return;
            case 'C':
            case 'c': //C - comment node
                menu_driver(m_menu, REQ_UP_ITEM);
                ConsolePrint("Enter comment text: ");

                //user input
                echo();
                wgetnstr(m_console.win, txt, MAX_INPUT);
                noecho();
                text = txt;

                //inserting new node
                node = new CCommentNode(text);
                if (id == INSERTING_TO_ROOT) {
                    m_xmlfile->SetRoot(node);
                } else {
                    m_nodes[id]->InsertNode(node);
                }
                return;
            case 'S':
            case 's': //S - new simple node
                menu_driver(m_menu, REQ_UP_ITEM);
                ConsolePrint("Enter node title: ");

                //user input
                echo();
                wgetnstr(m_console.win, str, MAX_INPUT);
                noecho();
                title = str;

                //inserting new node
                node = new CSimpleNode(title);
                if (id == INSERTING_TO_ROOT) {
                    m_xmlfile->SetRoot(node);
                } else {
                    m_nodes[id]->InsertNode(node);
                }
                return;
        }
    }
}

/*! An infinite loop, which allows the user to perform wanted actions while managing the attributes.
 * \param id To which node will it be inserted.
 */
void CGUI::AttributesHandler(int id) {
    int c; //for key pressed
    int attid; //for current attribute highlighted
    char str[MAX_INPUT], val[MAX_INPUT]; //for user input
    string name, value; //for user input
    CAttribute * attribute; //for new attribute

    //allocates the attribute menu
    m_attributesItems = new ITEM * [DEFAULT_MENU_ITEMS_COUNT];
    for (int i = 0; i < DEFAULT_MENU_ITEMS_COUNT; i++) {
        m_attributesItems[i] = NULL;
    }
    m_cntAttributes = 0;
    m_attributesSize = DEFAULT_MENU_ITEMS_COUNT;

    //fills it with attributes
    m_nodes[id]->GetAttributes(this);

    //and creates the list of attributes
    TreeDestroy();
    AttributesInit();

    //managing attributes is finished, when F12 is pressed
    while ((c = wgetch(m_tree.win)) != KEY_F(12)) {
        switch (c) {
            case KEY_DOWN:
                menu_driver(m_attributes, REQ_DOWN_ITEM);
                ConsolePrint("Console:");
                break;
            case KEY_UP:
                menu_driver(m_attributes, REQ_UP_ITEM);
                ConsolePrint("Console:");
                break;
            case KEY_NPAGE:
                menu_driver(m_attributes, REQ_SCR_DPAGE);
                ConsolePrint("Console:");
                break;
            case KEY_PPAGE:
                menu_driver(m_attributes, REQ_SCR_UPAGE);
                ConsolePrint("Console:");
                break;
            case KEY_F(5): //F5 - new attribute
                ConsolePrint("Enter attribute name: ");

                //user input
                echo();
                wgetnstr(m_console.win, str, MAX_INPUT);
                noecho();
                name = str;
                ConsolePrint("Enter attribute value: ");
                echo();
                wgetnstr(m_console.win, val, MAX_INPUT);
                noecho();
                value = val;


                AttributesDestroy();
                //inserts the attribute
                attribute = new CAttribute(name, value);

                //it has to rebuild the tree to insert it
                m_xmlfile->Show();
                m_nodes[id]->InsertAttribute(attribute);
                m_nodes[id]->GetAttributes(this);
                TreeInit();
                TreeDestroy();

                //rebuilds the list of attributes
                AttributesInit();
                ConsolePrint("Console: Attribute successfully inserted.");
                break;
            case KEY_F(6): //F6 - removing the attribute
                attid = item_index(current_item(m_attributes));
                name = item_name(current_item(m_attributes));
                AttributesDestroy();

                //it has to rebuild the tree to insert it
                m_xmlfile->Show();
                m_nodes[id]->RemoveAttribute(name);
                m_nodes[id]->GetAttributes(this);
                TreeInit();
                TreeDestroy();

                //rebuilds the list
                AttributesInit();

                //scrolls to previous item, if possible
                if (m_cntAttributes) {
                    while (item_index(current_item(m_attributes)) != attid - 1 && attid != 0) {
                        menu_driver(m_attributes, REQ_DOWN_ITEM);
                    }
                }
                ConsolePrint("Console: Attribute successfully removed.");
                break;
        }

    }
    AttributesDestroy();
}

/********************* MEMORY MANAGEMENT TOOLS *******************************/

/*! Handles the arrays used by main tree.
 */
void CGUI::ReallocItems() {
    if (m_cntNodes > m_menuSize - 2) {
        ITEM ** tmp = new ITEM * [REALLOC_CONSTANT * m_menuSize];
        CNode ** tmp_node = new CNode * [REALLOC_CONSTANT * m_menuSize];

        for (int i = 0; i < REALLOC_CONSTANT * m_menuSize; i++) {
            tmp[i] = NULL;
            tmp_node[i] = NULL;
        }

        for (int i = 0; i < m_cntNodes; i++) {
            tmp[i] = m_menuItems[i];
            tmp_node[i] = m_nodes[i];
        }

        m_menuSize = REALLOC_CONSTANT*m_menuSize;
        delete [] m_menuItems;
        delete [] m_nodes;
        m_menuItems = tmp;
        m_nodes = tmp_node;
    }
}

/*! Handles the arrays used by attributes list.
 */
void CGUI::ReallocAttributes() {
    if (m_cntAttributes > m_attributesSize - 2) {
        ITEM ** tmp = new ITEM * [REALLOC_CONSTANT * m_attributesSize];

        for (int i = 0; i < REALLOC_CONSTANT * m_attributesSize; i++) {
            tmp[i] = NULL;
        }

        for (int i = 0; i < m_cntAttributes; i++) {
            tmp[i] = m_menuItems[i];
        }

        m_attributesSize = REALLOC_CONSTANT*m_attributesSize;
        delete [] m_attributesItems;
        m_attributesItems = tmp;
    }
}

/********************* WINDOW CREATING TOOL *******************************/

/*! Creates the window with specified parameters.
 * \param width Width of the window.
 * \param height Height of the window.
 * \param startx X coordinate of the window.
 * \param starty Y coordinate of the window.
 */
WINDOW * CGUI::CreateNewWindow(int width, int height, int startx, int starty) {
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);
    return local_win;
}
