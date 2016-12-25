#include <iostream>

#include "CGUI.h"
#include <cstdlib>

#include "CException.h"

using namespace std;

/********************* INVALID FILE NAME *******************************/

/*! Creates new exception for invalid file name.
 * \param fileName Name of the file.
 */
InvalidFileNameException::InvalidFileNameException(const string & fileName) {
    m_fileName = fileName;
}

/*! Virtual method for printing the message about invalid file name to the console.
 * \param interface Pointer to the interface, where the message will be displayed.
 */
void InvalidFileNameException::Print(CGUI * interface) const {
    string str;
    interface->SetXMLOpened(false);
    str.append("Console: ");
    str.append("File");
    str.append(" does not exist!");
    interface->ConsolePrint(str.c_str());
    interface->Handler();
}

/********************* INVALID XML TITLE OR ATTRIBUTE *******************************/

/*! Creates new exception for invalid XML title or attribute name.
 * \param title Title of a XML node or name of an attribute.
 */
InvalidXMLTitleException::InvalidXMLTitleException(const string & title) {
    m_title = title;
}

/*! Virtual method for printing the message about invalid XML title or attribute to the console.
 * \param interface Pointer to the interface, where the message will be displayed.
 */
void InvalidXMLTitleException::Print(CGUI * interface) const {
    string str;
    str.append("Console: ");
    str.append(m_title);
    str.append(" is not valid XML element or attribute title!");
    if (interface->IsTreeInitialized()) {
        interface->ConsolePrint(str.c_str());
        interface->TreeHandler();
    } else {
        interface->SetXMLOpened(false);
        interface->ConsolePrint(str.c_str());
        interface->Handler();
    }
}

/********************* DUPLICATE ATTRIBUTE *******************************/

/*! Creates new exception for duplicate attribute.
 * \param name Name of the attribute.
 */
AttributeAlreadyExistsException::AttributeAlreadyExistsException(const string & name) {
    m_name = name;
}

/*! Virtual method for printing the message about duplicate attribute to the console.
 * \param interface Pointer to the interface, where the message will be displayed.
 */
void AttributeAlreadyExistsException::Print(CGUI * interface) const {
    string str;
    str.append("Console: ");
    str.append("Attribute ");
    str.append(m_name);
    str.append(" already exists!");
    if (interface->IsTreeInitialized()) {
        interface->ConsolePrint(str.c_str());
        interface->TreeHandler();
    } else {
        interface->SetXMLOpened(false);
        interface->ConsolePrint(str.c_str());
        interface->Handler();
    }
}

/********************* NONEXISTING ATTRIBUTE *******************************/

/*! Creates new exception for missing given attribute.
 * \param name Name of the attribute.
 */
AttributeDoesNotExistException::AttributeDoesNotExistException(const string & name) {
    m_name = name;
}

/*! Virtual method for printing the message about not existing attribute to the console.
 * \param interface Pointer to the interface, where the message will be displayed.
 */
void AttributeDoesNotExistException::Print(CGUI * interface) const {
    string str;
    str.append("Console: ");
    str.append("Attribute ");
    str.append(m_name);
    str.append(" does not exist!");
    if (interface->IsTreeInitialized()) {
        interface->ConsolePrint(str.c_str());
        interface->TreeHandler();
    } else {
        interface->SetXMLOpened(false);
        interface->ConsolePrint(str.c_str());
        interface->Handler();
    }
}

/********************* INVALID CHILD ID *******************************/

/*! Creates new exception for child id of a node.
 * \param id ID of a node.
 */
InvalidChildID::InvalidChildID(int id) {
    m_id = id;
}

/*! Virtual method for printing the message about invalid child id to the console.
 * \param interface Pointer to the interface, where the message will be displayed.
 */
void InvalidChildID::Print(CGUI * interface) const {
    string str;
    str.append("Console: ");
    str.append("This child id");
    str.append(" does not exist!");
    if (interface->IsTreeInitialized()) {
        interface->ConsolePrint(str.c_str());
        interface->TreeHandler();
    } else {
        interface->SetXMLOpened(false);
        interface->ConsolePrint(str.c_str());
        interface->Handler();
    }
}

/********************* INVALID XML FORMAT *******************************/

/*! Creates new exception for invalid XML format.
 * \param fileName Name of the file.
 */
InvalidXMLFormatException::InvalidXMLFormatException(const string & fileName) {
    m_fileName = fileName;
}

/*! Virtual method for printing the message about invalid XML format to the console.
 * \param interface Pointer to the interface, where the message will be displayed.
 */
void InvalidXMLFormatException::Print(CGUI * interface) const {
    string str;
    interface->SetXMLOpened(false);
    str.append("Console: ");
    str.append("File ");
    str.append(m_fileName);
    str.append(" is not a valid XML document!");
    interface->ConsolePrint(str.c_str());
    interface->Handler();
}