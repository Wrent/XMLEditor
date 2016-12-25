#include <fstream>
#include <cstdlib>

#include "CXML.h"
#include "CException.h"
#include "functions.h"

///! When reallocing, how many times will new array will be bigger
#define REALLOC_CONSTANT 2

///! Specifies that next node is text node.
#define NEXT_IS_TEXTNODE 0
///! Specifies that next node is parent node.
#define NEXT_IS_PARENTNODE 1
///! Specifies that next node is comment node.
#define NEXT_IS_COMMENT 2
///! Specifies that next node is simple node.
#define NEXT_IS_SIMPLE 3
///! Specifies that next node is end node.
#define NEXT_IS_ENDTAG 4
///! Specifies that parser reached end of file.
#define END_OF_FILE -1

using namespace std;

/********************* PUBLIC METHODS *******************************/

/*! Loads XML file to memory and starts the parsing into a tree.
 * \param filePath Specifies file to open.
 * \param interface Pointer to graphic interface.
 */
CXML::CXML(string & filePath, CGUI * interface){
    m_interface = interface;
    m_interface->AddXML(this); //tells the interface about XML file

    m_root = NULL;
    m_titlesSearchTree = NULL;
    
    m_filePath = filePath;
   
    CNode * current = NULL;
    CNode * previous = NULL;

    //tries to open the file
    ifstream file(filePath.c_str());
    if (file.fail() || !file.is_open()) {
        throw InvalidFileNameException(filePath);
    }
    
    //saves version data, if there are any
    StoreVersionData(file);

    //starts parsing XML
    string * nextTag = new string("");
    int nextTagType = TellTypeOfNextNode(file, nextTag);

    //parsing continues until EOF, or when an format error is detected
    while (nextTagType != END_OF_FILE) {
        //ncurses has problems with characters outside ASCII table, so remove them
        *nextTag = MakeASCII(*nextTag);
        if (nextTagType == NEXT_IS_PARENTNODE) {
            //get the title
            string title = ExtractXMLTitle(*nextTag);
            
            //create new parent node
            current = new CParentNode(title);
            current->SetParent(previous);

            m_stack.Push(title); //next end tag will have to be at the top of the stack

            //parse attributes of this node
            ParseAttributes(current, *nextTag);

            //next tags will be childs of this node
            previous = current;
            
            //if there is a parent, tell him, this is his child
            if (current->GetParent() != NULL) {
                current->GetParent()->InsertNode(current);
            }
        } else if (nextTagType == NEXT_IS_TEXTNODE) {
            //get the title and value
            string title = ExtractXMLTitle(*nextTag);
            string value = ExtractTextNodeValue(file);
            //make value only ASCII
            value = MakeASCII(value);
            
            //create new text node
            current = new CTextNode(title, value);
            current->SetParent(previous);

            //parse attributes of this node
            ParseAttributes(current, *nextTag);

            //insert this node to a possible parent
            if (current->GetParent() != NULL) {
                current->GetParent()->InsertNode(current);
            }
            //extract the end tag of the text node from the file stream
            ExtractTextNodeEndTag(file, title);
        } else if (nextTagType == NEXT_IS_COMMENT) {
            //get the comment text
            string value = ExtractCommentNodeValue(*nextTag);
            
            //make comment only ASCII
            value = MakeASCII(value);
            
            //create new comment
            current = new CCommentNode(value);
            current->SetParent(previous);

            //insert comment to a possible parent
            if (current->GetParent() != NULL) {
                current->GetParent()->InsertNode(current);
            }
        } else if (nextTagType == NEXT_IS_SIMPLE) {
            //remove the '/' from the end of tag
            nextTag->resize(nextTag->length() - 1);
            
            //get the title
            string title = ExtractXMLTitle(*nextTag);
            
            //create new simple node
            current = new CSimpleNode(title);       
            current->SetParent(previous);

            //parse attributes of this node
            ParseAttributes(current, *nextTag);

            //insert to a possible parent
            if (current->GetParent() != NULL) {
                current->GetParent()->InsertNode(current);
            }
        } else if (nextTagType == NEXT_IS_ENDTAG) {
            //this endtag must be on the top of the stack, or it is an error!
            //erase removes the '/' from the start of the tag
            if (nextTag->erase(0, 1) != m_stack.Pop())
                throw InvalidXMLFormatException(m_filePath);

            //root end tag won't be read here, so stack cannot be free here
            //there can be only one root element!
            if (m_stack.GetStackCnt() == 0)
                throw InvalidXMLFormatException(m_filePath);
            
            //inserting to this tag ends now
            previous = previous->GetParent();
        }

        //was it the first node?
        if (m_root == NULL) {
            m_root = current;
        }
        
        //continue parsing...
        nextTag->clear();
        nextTagType = TellTypeOfNextNode(file, nextTag);
        
        //if root element is not parent, there can be only one element at all
        if (nextTagType != END_OF_FILE && !m_root->HasChilds())
            throw InvalidXMLFormatException(m_filePath);
    }

    //the file has ended, stack must be free (after poping the root...)
    if (nextTag->erase(0, 1) != m_stack.Pop())
        throw InvalidXMLFormatException(m_filePath);
    if (m_stack.GetStackCnt() > 0) {
        throw InvalidXMLFormatException(m_filePath);
    }
    delete nextTag;
}

/*! Removes the whole XML tree and search tree.
 */
CXML::~CXML() {
    delete m_root;
    delete m_titlesSearchTree;
}

/********************* "PRINTING" TOOLS *******************************/

/*! Sends the whole tree to GUI.
 */
void CXML::Show() {
    if (m_root) {
        m_root->Print(m_interface, 0);
        delete m_titlesSearchTree;
        m_titlesSearchTree = new CSearchTree();
        m_root->PrepareSearching(m_titlesSearchTree);
    }
}

/*! Sends the whole tree to the file as an valid XML.
 */
void CXML::Save() const {
    ofstream file(m_filePath.c_str());

    if (m_versionData.length() > 0)
        file << m_versionData << endl;
    if (m_root)
        m_root->XMLPrint(file, 0);
}

/*! Starts filtering according to the given title.
 * \param title Title of the nodes to be shown.
 */
void CXML::Filter(string & title) const{
    if (m_root)
        m_root->CollapseAll();
    m_titlesSearchTree->Filter(title);
}

/********************* GETTERS / SETTERS *******************************/

/*! Gets the current file path.
 */
string CXML::GetFilePath() const {
    return m_filePath;
}

/*! Sets the root node.
 * \param node Pointer to new root node.
 */
void CXML::SetRoot(CNode * node) {
    m_root = node;
}

/********************* PRIVATE METHODS *******************************/

/********************* PARSING TOOLS *******************************/


/*! At start of the file, there can be some version information, this method saves them.
 * \param is File input stream.
 */
void CXML::StoreVersionData(ifstream & is) {
    IgnoreNextWhitespaces(is);
    char c = is.get();
    if (c != 60) // there must be < character
        throw InvalidXMLFormatException(m_filePath);

    //if the next character is ?, it is the header info and i will save it to the string
    char d = is.get();
    if (d == 63) {
        m_versionData.append(1, c);
        m_versionData.append(1, d);
        while (d != 62) {
            d = is.get();
            m_versionData.append(1, d);
        }
        IgnoreNextWhitespaces(is);
    } else {
        is.putback(d);
        is.putback(c);
    }
}

/*! Ignores white spaces in the file.
 * \param is File input stream.
 */
void CXML::IgnoreNextWhitespaces(ifstream & is) const {
    char c = 32;
    while (c == 32 || c == 10 || c == 9 || c == 13) { //SPACE, TAB, CR, LF
        c = is.get();
    }
    is.putback(c);
}

/*! Main parsing function, it find out the type of next node and saves the node (content between < > to the I/O variable
 * \param is File input stream.
 * \param tag To this string is saved content of the next tag.
 */
int CXML::TellTypeOfNextNode(ifstream& is, string * tag) const {
    IgnoreNextWhitespaces(is);
    char c = is.get();
    if (is.eof())
        return END_OF_FILE;
    if (c != 60) // after white spaces, there must be < character
        throw InvalidXMLFormatException(m_filePath);


    while (c != 62) { // looking for > char
        c = is.get();
        if (c != 62) 
            tag->append(1, c); //saving other chars to the tag
    }
    
    //now look what's next
    
    IgnoreNextWhitespaces(is);
    c = is.get();
    if (is.eof())
        return END_OF_FILE;
    if (c == 60) { //it is another tag!
        is.putback(c);
        if ((*tag)[0] == 47) //is it end tag?
            return NEXT_IS_ENDTAG;
        else if (IsValidSimpleTag(*tag)) {
            return NEXT_IS_SIMPLE;
        } else if (IsValidComment(*tag)) {
            return NEXT_IS_COMMENT;
        } else {
            return NEXT_IS_PARENTNODE;
        }
    } else { //its some kind of text
        is.putback(c);
        return NEXT_IS_TEXTNODE;
    }
}

/*! Function for parsing attributes, it finds attributes in the string and adds them to the node.
 * \param node Pointer to the node, where attributes will be added.
 * \param x String, from which attributes are parsed.
 * \return The type of next node, specified by constants.
 */
void CXML::ParseAttributes(CNode * node, const string & x) const {
    string attr, value;
    for (unsigned int i = 0; i < x.length(); i++) { //go through the whole string
        attr.clear();
        value.clear();
        if (x[i] == 32) { // after first space come attributes
            i++;
            if (i >= x.length())
                break;
            i = IgnoreNextWhiteSpaces(x, i);
            while (x[i] != 61) { //looking for attr separator, which is =
                if (i >= x.length())
                    break;
                if (x[i] == 32) { //there cannot be space without =
                    throw InvalidXMLFormatException(m_filePath);
                }
                attr.append(1, x[i]);
                i++;
            }
            //now there should be " or '
            i++;
            if (x[i] == 34 || x[i] == 39) {
                char quotes = x[i]; // i will be looking for the same quotes as the end tag
                i++;
                while (x[i] != quotes) {
                    if (i >= x.length())
                        break;
                    value.append(1, x[i]);
                    i++;
                }
            } else
                throw InvalidXMLFormatException(m_filePath);

            CAttribute * attribute = new CAttribute(attr, value);
            node->InsertAttribute(attribute);

            i = IgnoreNextWhiteSpaces(x, i);
        }
    }
}

/*! Gets the value of text node from file stream.
 * \param is File input stream.
 * \return The text node value.
 */
string CXML::ExtractTextNodeValue(ifstream & is) const {
    string ret;
    char c = is.get();
    while (c != 60) { //until there is an <
        ret.append(1, c);
        c = is.get();
    }
    is.putback(c);
    return ret;
}

/*! Text node has to have its end tag right after the text value. Extract it out from the stream.
 * \param is File input stream.
 * \param title Title of the text node.
 */
void CXML::ExtractTextNodeEndTag(ifstream & is, string & title) const {
    //i know there is a tag - and it must be the end tag of my title
    string endtag;
    char c = is.get();
    if (c != 60)
        throw InvalidXMLFormatException(m_filePath);
    c = is.get();
    if (c != 47) // second character must be /
        throw InvalidXMLFormatException(m_filePath);
    c = is.get();
    while (c != 62) {
        endtag.append(1, c);
        c = is.get();
    }
    //final comparison
    if (endtag != title)
        throw InvalidXMLFormatException(m_filePath);
}

/*! Gets the value of comment node from file stream.
 * \param x The comment node text.
 * \return The comment node value.
 */
string CXML::ExtractCommentNodeValue(const string & x) const {
    string ret;
    // first 3 characters of the comment and last 2 are not interesting
    for (unsigned int i = 4; i < (x.length() - 2); i++) {
        ret.append(1, x[i]);
    }
    return ret;
}
