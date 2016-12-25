#include "functions.h"
#include "CXML.h"

///! Specifies that this title is valid.
#define IS_VALID_TITLE true;
///! Specifies that this title is invalid.
#define IS_INVALID_TITLE false;
///! Specifies that this is valid comment.
#define IS_VALID_COMMENT true;
///! Specifies that this is ivalid comment.
#define IS_INVALID_COMMENT false;
///! Specifies that this is valid simple node.
#define IS_VALID_SIMPLE_NODE true;
///! Specifies that this is invalid simple node.
#define IS_INVALID_SIMPLE_NODE false;

/********************* PARSING VALIDITY FUNCTIONS *******************************/

/*! Checks the validity of an XML title.
 * \param x String which is checked as XML title.
 * \return Returns if the string is valid XML title.
 */
bool IsValidTitle(const string & x) {
    if (!x.length())
        return IS_INVALID_TITLE;

    //first character must be a letter, : or _
    if ((x[0] < 65 && x[0] != 58) || (x[0] > 90 && x[0] < 95) || x[0] == 96 || x[0] > 122)
        return IS_INVALID_TITLE;

    //other characters can contain letters, numbers, :, ., _ and -
    for (unsigned int i = 1; i < x.length(); i++) {
        if (x[i] < 45 || x[i] == 47 || (x[i] > 58 && x[i] < 65) || (x[0] > 90 && x[0] < 95) || x[0] == 96 || x[0] > 122)
            return IS_INVALID_TITLE;
    }
    return IS_VALID_TITLE;
}

/*! Checks the validity of an XML comment.
 * \param x String which is checked as XML comment.
 * \return Returns if the string is valid XML comment.
 */
bool IsValidComment(const string & x) {
    if (!x.length())
        return IS_INVALID_COMMENT;

    //first characters must be !-- and last --
    if (x[0] == 33 && x[1] == 45 && x[2] == 45 && x[x.length() - 1 == 45] && x[x.length() - 2 == 45]) {
        return IS_VALID_COMMENT;
    } else {
        return IS_INVALID_COMMENT;
    }
}

/*! Checks, if the given tag is simple tag.
 * \param x String which is checked as simple XML node.
 * \return Returns if the string is simple XML node.
 */
bool IsValidSimpleTag(const string & x) {
    if (!x.length())
        return IS_INVALID_SIMPLE_NODE;

    //last character must be /
    if (x[x.length() - 1] == 47) {
        return IS_VALID_SIMPLE_NODE;
    } else {
        return IS_INVALID_SIMPLE_NODE;
    }
}

/********************* PARSING FUNCTIONS *******************************/

/*! Gets the XML title from a string.
 * \param x String, where I will be looking for XML title.
 * \return Returns the XML title.
 */
string ExtractXMLTitle(const string & x) {
    string ret;
    for (unsigned int i = 0; i < x.length(); i++) {
        if (x[i] != 32) //everything before first space is title
            ret.append(1, x[i]);
        else
            break;
    }
    return ret;
}

/*! Ignores white spaces in a string.
 * \param x String, where I will skip white spaces.
 * \return Position in the string after white spaces.
 */
int IgnoreNextWhiteSpaces(const string & x, int pos) {
    int i = pos;
    while (x[i] == 32 || x[i] == 10 || x[i] == 9 || x[i] == 13) {
        i++;
    }
    return i;
}

/*! Removes unASCII chars from the string.
 * \param x String, where I will remove unASCII chars.
 * \return String with removed characters.
 */
string MakeASCII(const string & x) {
    string str;
    for (unsigned int i = 0; i < x.length(); i++) {
        if (x[i] >= 0 && x[i] < 128) { //ASCII is 0-127
            if (x[i] != 10) //ncurses can not show CR
                str.append(1, x[i]);
        }
    }
    return str;
}


/********************* OTHER *******************************/

/*! Opens new xml file.
 * \param xml Pointer to old xml.
 * \param filePath New file address.
 * \param interface Pointer to graphic interface.
 * \return Pointer to new xml file.
 */
CXML * OpenFile(CXML * xml, string & filePath, CGUI * interface) {
    delete xml;

    xml = new CXML(filePath, interface);
    return xml;
}