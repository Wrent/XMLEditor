#ifndef CNODE_H
#define	CNODE_H

#include <cstdlib>
#include <iostream>
#include <string>

#include "CAttribute.h"
#include "CGUI.h"
#include "CSearchTree.h"

using namespace std;

class CParentNode;

///! Abstract class, which represents one XML element (node).
class CNode {
public:
    CNode();
    CNode(string & title);
    ~CNode();

    //getters
    string GetTitle() const;
    int GetID() const;
    CNode * GetParent() const;

    //setters
    void SetTitle(string & title);
    void SetID(int id);
    void SetParent(CNode * parent);

    //collapse / expand tools
    void ExpandUp();
    void Collapse();
    void Expand();

    //public attribute tools
    void InsertAttribute(CAttribute * attribute);
    void RemoveAttribute(string & name);
    void GetAttributes(CGUI * interface);

    //virtual Print tools
    virtual void Print(CGUI * interface, int depth) = 0;
    virtual void XMLPrint(ofstream & file, int depth) = 0;
    virtual void PrepareSearching(CSearchTree * tree) = 0;

    //virtual type getters
    virtual bool HasChilds() const = 0;
    virtual bool HasAttributes() const = 0;

    //virtual tools for child nodes
    virtual void InsertNode(CNode * node) = 0;
    virtual void DeleteNode(int id) = 0;

    //virtual collapse / expand "recursive" tools
    virtual void CollapseAll() = 0;
    virtual void ExpandAll() = 0;
protected:
    bool AttributeExists(string & name) const;
    void ReallocAttributes();

    //node information
    ///! Title of the element
    string m_title;
    ///! Array of element attributes
    CAttribute ** m_attributes;
    ///! Count of element attributes
    int m_cntAtt;
    ///! Current max count of element attributes
    int m_sizeAtt;
    ///! Is the current node collapsed?
    bool m_isCollapsed;
    ///! Pointer to parent of the node, NULL if this node is root
    CNode * m_parent;
    ///! Child id of the node
    int m_id;

    //output strings
    ///! Pointer to tree output string
    string * m_output;
    ///! Pointer to XML output string
    string * m_xmloutput;
};

/************************** TEXT NODES **************************/

///! Class, which represents text XML elements - like <title>text</title>
class CTextNode : public CNode {
public:
    CTextNode(string & title, string & value);

    string GetValue() const;
    void SetValue(string & value);

    //virtual Print tools
    virtual void Print(CGUI * interface, int depth);
    virtual void XMLPrint(ofstream & file, int depth);
    virtual void PrepareSearching(CSearchTree * tree);

    //virtual child nodes tool (not used here)

    virtual void InsertNode(CNode * node) {
    };

    virtual void DeleteNode(int id) {
    };

    //virtual type getters
    virtual bool HasChilds() const;
    virtual bool HasAttributes() const;

    //virtual collapse / expand "recursive" tools
    virtual void CollapseAll();
    virtual void ExpandAll();
protected:
    ///! Text value of the text node
    string m_value;
};

/************************** COMMENT NODES **************************/

///! Class, which represents comment XML elements - like <!-- Comment -->
class CCommentNode : public CNode {
public:
    CCommentNode(string & comment);

    string GetComment() const;
    void SetComment(string & comment);

    //virtual Print tools
    virtual void Print(CGUI * interface, int depth);
    virtual void XMLPrint(ofstream & file, int depth);

    virtual void PrepareSearching(CSearchTree * tree) {
    }; //comment node is not filtered

    //virtual child nodes tool (not used here)

    virtual void InsertNode(CNode * node) {
    };

    virtual void DeleteNode(int id) {
    };

    //virtual type getters
    virtual bool HasChilds() const;
    virtual bool HasAttributes() const;

    //virtual collapse / expand "recursive" tools
    virtual void CollapseAll();
    virtual void ExpandAll();
protected:
    ///! Comment text
    string m_comment;
};

/************************* PARENT NODE ***************************/

///! Class, which represents parent XML elements - like <node>...more nodes...</node>
class CParentNode : public CNode {
public:
    CParentNode(string & title);
    ~CParentNode();

    //virtual Print tools
    virtual void Print(CGUI * interface, int depth);
    virtual void XMLPrint(ofstream & file, int depth);
    virtual void PrepareSearching(CSearchTree * tree);

    //virtual child nodes tools
    virtual void InsertNode(CNode * node);
    virtual void DeleteNode(int id);

    //virtual type getters
    virtual bool HasChilds() const;
    virtual bool HasAttributes() const;

    //virtual collapse / expand "recursive" tools
    virtual void CollapseAll();
    virtual void ExpandAll();
protected:
    void ReallocChilds();

    ///! Array of childs
    CNode ** m_childs;
    ///! Count of childs
    int m_cntChilds;
    ///! Max count of childs
    int m_sizeChilds;
};

/************************* SIMPLE NODE ***************************/

///! Class, which represents simple XML elements - like <title blabla />
class CSimpleNode : public CNode {
public:
    CSimpleNode(string & title);
    ~CSimpleNode();

    //virtual Print tools
    virtual void Print(CGUI * interface, int depth);
    virtual void XMLPrint(ofstream & file, int depth);
    virtual void PrepareSearching(CSearchTree * tree);

    //virtual child nodes tools (not used here)
    virtual void InsertNode(CNode * node) {
    };
    virtual void DeleteNode(int id) {
    };

    //virtual type getters
    virtual bool HasChilds() const;
    virtual bool HasAttributes() const;
    
    //virtual collapse / expand "recursive" tools
    virtual void CollapseAll();
    virtual void ExpandAll();
};

#endif	/* CNODE_H */

