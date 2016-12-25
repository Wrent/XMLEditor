#ifndef CXML_H
#define	CXML_H

#include <cstdlib>
#include <string>

#include "CNode.h"
#include "CTagStack.h"
#include "CGUI.h"
#include "CSearchTree.h"

using namespace std;

///! Class for parsing the XML file, saving it and printing it to the interface.

class CXML {
public:
    CXML(string & filePath, CGUI * interface);
    ~CXML();

    // "printing" tools
    void Show();
    void Save() const;
    void Filter(string & title) const;

    string GetFilePath() const;
    void SetRoot(CNode * node);
protected:
    //parsing tools
    void StoreVersionData(ifstream & is);
    void IgnoreNextWhitespaces(ifstream & is) const;
    int TellTypeOfNextNode(ifstream & is, string * tag) const;
    void ParseAttributes(CNode * node, const string & x) const;
    string ExtractTextNodeValue(ifstream & is) const;
    void ExtractTextNodeEndTag(ifstream & is, string & title) const;
    string ExtractCommentNodeValue(const string & x) const;

    ///! Pointer to the root of the tree.
    CNode * m_root;

    ///! Pointer to the titles search tree.
    CSearchTree * m_titlesSearchTree;

    //pointer to the interface
    ///! Pointer to the GUI.
    CGUI * m_interface;

    ///! Stack used for XML parsing.
    CTagStack m_stack;

    ///! File name.
    string m_filePath;
    ///! Information about XML version (if there are any).
    string m_versionData;
};

#endif	/* CXML_H */

