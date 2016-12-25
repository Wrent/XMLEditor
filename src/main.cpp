/**
 *
 * Simple XML Editor in NCurses
 * 
 * @author Adam Kucera (kucerad5)
 * @version 1.0 ( 5.6.2012 )
 * 
 */

#include <cstdlib>
#include <iostream>
#include <ncurses.h>
#include <menu.h>

#include "CXML.h"
#include "CException.h"
#include "CGUI.h"

using namespace std;

int main(int argc, char** argv) {
    bool openingXML;
    if (argc > 1) // was file name specified from the command line
        openingXML = true;
    else
        openingXML = false;

    //init GUI
    CGUI * interface = new CGUI(openingXML);

    CXML *xmlFile = NULL;
    if (openingXML) {
        try {
            string filePath = argv[argc - 1];
            //open the file and show it
            xmlFile = new CXML(filePath, interface);
            xmlFile->Show();
            interface->TreeInit();
            interface->TreeHandler();
        } catch (const CException & e) {
            //error handler
            e.Print(interface);
        }
    }

    delete interface;
    return 0;
}
