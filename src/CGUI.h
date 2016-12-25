/*
 * NCurses GUI implementation was inspired by some examples on 
 * http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html   
 */

#ifndef CGUI_H
#define	CGUI_H

#include <ncurses.h>
#include <menu.h>
#include <cstdlib>

using namespace std;

class CNode;
class CXML;

///! Structure stores the pointer to a window, its position and size.
struct Window {
    ///! Pointer to the window
    WINDOW * win;
    ///! Window information
    int x, y, width, height;
};

///! Class for managing graphic user interface (powered by ncurses).
class CGUI {
public:
    CGUI(bool openingXML);
    ~CGUI();

    //functions for inserting menu items
    void AddMenuItem(const string * name, CNode * node, const char * type);
    void AddAttributeItem(const string * name, const string * value);

    //user input handlers
    void Handler();
    void TreeHandler();

    //menu initializes and destroyers
    void TreeInit();
    void TreeDestroy();
    void AttributesInit();
    void AttributesDestroy();

    //tool for printing to console
    void ConsolePrint(const char * str);

    //getters
    bool IsTreeInitialized() const;

    //setter
    void AddXML(CXML * xml);
    void SetXMLOpened(bool opened);

protected:
    //functions to count size of windows
    void CountControlWindowSize();
    void CountTreeWindowSize();
    void CountConsoleWindowSize();

    //functions to create new windows
    void CreateControlWindow();
    void CreateTreeWindow();
    void CreateConsoleWindow();

    //functions for posting different hints for controlling the program
    void PostDefaultControlWindow();
    void PostInsertingControlWindow();
    void PostAttributesControlWindow();
    void PostStartControlWindow();

    //user input handlers
    void InsertingHandler(int id);
    void AttributesHandler(int id);

    //memory management tools
    void ReallocItems();
    void ReallocAttributes();

    //tool for creating new window
    WINDOW * CreateNewWindow(int width, int height, int startx, int starty);

    //windows
    ///! Window with control hint
    Window m_control;
    ///! Main window for viewing tree
    Window m_tree;
    ///! Console window
    Window m_console;

    //arrays of menus items
    ///! XML printed strings
    ITEM ** m_menuItems;
    ///! Pointers to nodes connected to strings
    CNode ** m_nodes; //every item in menu represents a node
    ///! Attributes of given node (its strings)
    ITEM ** m_attributesItems;

    //menus to be shown
    ///! Main XML tree
    MENU *m_menu;
    ///! List of attributes for given node
    MENU *m_attributes;


    //menus information
    ///! Count of shown nodes
    int m_cntNodes;
    ///! Current max count of shown nodes
    int m_menuSize;
    ///! Count of shown attributes
    int m_cntAttributes;
    ///! Current max count of attributes
    int m_attributesSize;

    ///! XML file currently open
    CXML * m_xmlfile;

    //variables representing current state
    ///! is an XML opened?
    bool m_xmlOpened;
    ///! is the XML shown?
    bool m_treeInitialized; 
};

#endif	/* CGUI_H */

