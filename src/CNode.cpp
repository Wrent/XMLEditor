#include <cstdlib>
#include <string>
#include <fstream>

#include "CAttribute.h"
#include "CNode.h"
#include "functions.h"
#include "CException.h"

///! Default count of attributes
#define DEFAULT_ATTRIBUTES_SIZE 3
///! Default count of childs
#define DEFAULT_CHILDS_SIZE 10
///! When reallocing, how many times will new array will be bigger
#define REALLOC_CONSTANT 2

using namespace std;

/********************* PUBLIC SHARED METHODS *******************************/

/*! Creates new node, allocates its attributes.
 */
CNode::CNode() {
    m_cntAtt = 0;
    m_attributes = new CAttribute * [DEFAULT_ATTRIBUTES_SIZE];
    m_sizeAtt = DEFAULT_ATTRIBUTES_SIZE;

    m_id = 0;
    m_parent = NULL;

    m_output = new string;
    m_xmloutput = new string;

    m_isCollapsed = false;
}

/*! Creates new node with given title, allocates its attributes.
 * \param title Title of the element.
 */
CNode::CNode(string & title) {
    if (IsValidTitle(title))
        m_title = title;
    else
        throw InvalidXMLTitleException(title);
    m_cntAtt = 0;

    m_attributes = new CAttribute * [DEFAULT_ATTRIBUTES_SIZE];
    m_sizeAtt = DEFAULT_ATTRIBUTES_SIZE;

    m_id = 0;
    m_parent = NULL;

    m_output = new string;
    m_xmloutput = new string;

    m_isCollapsed = false;
}

/*! Tidies up.
 */
CNode::~CNode() {

    for (int i = 0; i < m_cntAtt; i++) {
        delete m_attributes[i];
    }
    delete [] m_attributes;
    delete m_output;
    delete m_xmloutput;
}

/********************* ATTRIBUTES METHODS *******************************/

/*! Inserts given attribute to attributes array.
 * \param attribute Given attribute.
 */
void CNode::InsertAttribute(CAttribute * attribute) {
    //is there enough space?
    ReallocAttributes();
    string attrName = attribute->GetName();

    //there cannot be two attributes with same name
    if (AttributeExists(attrName))
        throw AttributeAlreadyExistsException(attrName);

    m_attributes[m_cntAtt++] = attribute;
}

/*! Removes the attribute specified by name.
 * \param name Name of the attribute.
 */
void CNode::RemoveAttribute(string& name) {
    //if there is no attribute with given name, pos will remain negative
    int pos = -1;

    for (int i = 0; i < m_cntAtt; i++) {
        if (m_attributes[i]->GetName() == name)
            pos = i;
    }

    if (pos < 0)
        throw AttributeDoesNotExistException(name);

    delete m_attributes[pos];

    for (int i = pos; i < m_cntAtt; i++) {
        m_attributes[i] = m_attributes[i + 1];
    }
    m_cntAtt--;
}

/*! Sends the attributes of this element to interface.
 * \param interface Pointer to the interface.
 */
void CNode::GetAttributes(CGUI* interface) {
    for (int i = 0; i < m_cntAtt; i++)
        interface->AddAttributeItem(m_attributes[i]->GetNamePointer(), m_attributes[i]->GetValuePointer());
}

/********************* SETTERS *******************************/

/*! Sets the title of an element and checks its validity.
 * \param title New title.
 */
void CNode::SetTitle(string& title) {
    if (IsValidTitle(title))
        m_title = title;
    else
        throw InvalidXMLTitleException(title);
}

/*! Sets the id of an element.
 * \param id New id.
 */
void CNode::SetID(int id) {
    m_id = id;
}

/*! Sets the parent of an element.
 * \param parent Pointer to new parent.
 */
void CNode::SetParent(CNode* parent) {
    m_parent = parent;
}

/********************* GETTERS *******************************/

/*! Gets the element title.
 */
string CNode::GetTitle() const {
    return m_title;
}

/*! Gets the element id.
 */
int CNode::GetID() const {
    return m_id;
}

/*! Gets the pointer to the element parent.
 */
CNode * CNode::GetParent() const {
    return m_parent;
}

/********************* COLLAPSE / EXPAND TOOLS *******************************/

/*! Collapses current node.
 */
void CNode::Collapse() {
    m_isCollapsed = true;
}

/*! Expands current node.
 */
void CNode::Expand() {
    m_isCollapsed = false;
}

/*! Expands current node and all the parents all the way to root node.
 */
void CNode::ExpandUp() {
    Expand();
    if (m_parent)
        m_parent->ExpandUp();
}

/********************* SHARED PRIVATE TOOLS *******************************/

/*! Finds out if an attribute exists.
 * \param name The name of the attribute.
 * \return Return if attribute with specified name exists.
 */
bool CNode::AttributeExists(string& name) const {
    for (int i = 0; i < m_cntAtt; i++) {
        if (m_attributes[i]->GetName() == name)
            return true;
    }
    return false;
}

/*! Attributes memory management.
 */
void CNode::ReallocAttributes() {
    if (m_cntAtt >= m_sizeAtt - 2) {
        CAttribute ** tmp = new CAttribute * [m_sizeAtt * REALLOC_CONSTANT];
        for (int i = 0; i < m_cntAtt; i++) {
            tmp[i] = m_attributes[i];
        }

        delete [] m_attributes;
        m_attributes = tmp;
        m_sizeAtt *= REALLOC_CONSTANT;
    }
}


/********************* VIRTUAL PUBLIC TOOLS *******************************/

/********************* VIRTUAL PRINT *******************************/

/*! Prints the text node to the interface menu.
 * \param interface Pointer to the interface.
 * \param depth Specifies how deep in the tree current node is.
 */
void CTextNode::Print(CGUI * interface, int depth) {
    m_output->clear();
    for (int i = 0; i < depth; i++) {
        m_output->append("   ");
    }

    if (m_isCollapsed)
        m_output->append("[+] ");
    else
        m_output->append("[-] ");

    m_output->append(m_title);
    if (!m_isCollapsed) {
        m_output->append(" ");
        m_output->append("( ");
        for (int i = 0; i < m_cntAtt; i++) {
            m_output->append(m_attributes[i]->GetName());
            m_output->append("=");
            m_output->append(m_attributes[i]->GetValue());
            m_output->append(" ");
        }
        m_output->append(")");
        m_output->append(" - ");

        m_output->append(m_value);
    }
    interface->AddMenuItem(m_output, dynamic_cast<CNode *> (this), "T");
}

/*! Prints the comment node to the interface menu.
 * \param interface Pointer to the interface.
 * \param depth Specifies how deep in the tree current node is.
 */
void CCommentNode::Print(CGUI * interface, int depth) {
    m_output->clear();
    for (int i = 0; i < depth; i++) {
        m_output->append("   ");
    }
    if (m_isCollapsed)
        m_output->append("[+] ");
    else
        m_output->append("[-] ");

    m_output->append("// ");
    if (!m_isCollapsed)
        m_output->append(m_comment);

    interface->AddMenuItem(m_output, dynamic_cast<CNode *> (this), "C");
}

/*! Prints the parent node to the interface menu (and recursively childs).
 * \param interface Pointer to the interface.
 * \param depth Specifies how deep in the tree current node is.
 */
void CParentNode::Print(CGUI * interface, int depth) {
    m_output->clear();
    for (int i = 0; i < depth; i++) {
        m_output->append("   ");
    }
    if (m_isCollapsed)
        m_output->append("[+] ");
    else
        m_output->append("[-] ");

    m_output->append(m_title);
    if (!m_isCollapsed) {
        m_output->append(" ");
        m_output->append("( ");
        for (int i = 0; i < m_cntAtt; i++) {
            m_output->append(m_attributes[i]->GetName());
            m_output->append("=");
            m_output->append(m_attributes[i]->GetValue());
            m_output->append(" ");
        }
        m_output->append(")");
    }
    interface->AddMenuItem(m_output, dynamic_cast<CNode *> (this), "P");
    if (!m_isCollapsed) {
        for (int i = 0; i < m_cntChilds; i++) {
            m_childs[i]->Print(interface, depth + 1);
        }
    }
}

/*! Prints the simple node to the interface menu.
 * \param interface Pointer to the interface.
 * \param depth Specifies how deep in the tree current node is.
 */
void CSimpleNode::Print(CGUI * interface, int depth) {
    m_output->clear();
    for (int i = 0; i < depth; i++) {
        m_output->append("   ");
    }

    if (m_isCollapsed)
        m_output->append("[+] ");
    else
        m_output->append("[-] ");

    m_output->append(m_title);
    if (!m_isCollapsed) {
        m_output->append(" ");
        m_output->append("( ");
        for (int i = 0; i < m_cntAtt; i++) {
            m_output->append(m_attributes[i]->GetName());
            m_output->append("=");
            m_output->append(m_attributes[i]->GetValue());
            m_output->append(" ");
        }
        m_output->append(")");
    }
    interface->AddMenuItem(m_output, dynamic_cast<CNode *> (this), "S");
}

/********************* VIRTUAL XML PRINT *******************************/

/*! Prints the text node to the XML file in valid XML format.
 * \param file File stream.
 * \param depth Specifies how deep in the tree current node is.
 */
void CTextNode::XMLPrint(ofstream & file, int depth) {
    m_xmloutput->clear();
    for (int i = 0; i < depth; i++) {
        m_xmloutput->append("\t");
    }
    m_xmloutput->append("<");
    m_xmloutput->append(m_title);
    for (int i = 0; i < m_cntAtt; i++) {
        m_xmloutput->append(" ");
        m_xmloutput->append(m_attributes[i]->GetName());
        m_xmloutput->append("=\"");
        m_xmloutput->append(m_attributes[i]->GetValue());
        m_xmloutput->append("\"");
    }
    m_xmloutput->append(">");
    m_xmloutput->append(m_value);
    m_xmloutput->append("</");
    m_xmloutput->append(m_title);
    m_xmloutput->append(">");

    file << *m_xmloutput << endl;
}

/*! Prints the comment node to the XML file in valid XML format.
 * \param file File stream.
 * \param depth Specifies how deep in the tree current node is.
 */
void CCommentNode::XMLPrint(ofstream& file, int depth) {
    m_xmloutput->clear();
    for (int i = 0; i < depth; i++) {
        m_xmloutput->append("\t");
    }
    m_xmloutput->append("<!-- ");
    m_xmloutput->append(m_comment);
    m_xmloutput->append("-->");

    file << *m_xmloutput << endl;
}

/*! Prints the parent node to the XML file in valid XML format (and child recursively).
 * \param file File stream.
 * \param depth Specifies how deep in the tree current node is.
 */
void CParentNode::XMLPrint(ofstream& file, int depth) {
    m_xmloutput->clear();
    for (int i = 0; i < depth; i++) {
        m_xmloutput->append("\t");
    }
    m_xmloutput->append("<");
    m_xmloutput->append(m_title);
    for (int i = 0; i < m_cntAtt; i++) {
        m_xmloutput->append(" ");
        m_xmloutput->append(m_attributes[i]->GetName());
        m_xmloutput->append("=\"");
        m_xmloutput->append(m_attributes[i]->GetValue());
        m_xmloutput->append("\"");
    }
    m_xmloutput->append(">");

    file << *m_xmloutput << endl;
    for (int i = 0; i < m_cntChilds; i++) {
        m_childs[i]->XMLPrint(file, depth + 1);
    }
    m_xmloutput->clear();
    for (int i = 0; i < depth; i++) {
        m_xmloutput->append("\t");
    }
    m_xmloutput->append("</");
    m_xmloutput->append(m_title);
    m_xmloutput->append(">");
    file << *m_xmloutput << endl;
}

/*! Prints the simple node to the XML file in valid XML format (and child recursively).
 * \param file File stream.
 * \param depth Specifies how deep in the tree current node is.
 */
void CSimpleNode::XMLPrint(ofstream & file, int depth) {
    m_xmloutput->clear();
    for (int i = 0; i < depth; i++) {
        m_xmloutput->append("\t");
    }
    m_xmloutput->append("<");
    m_xmloutput->append(m_title);
    for (int i = 0; i < m_cntAtt; i++) {
        m_xmloutput->append(" ");
        m_xmloutput->append(m_attributes[i]->GetName());
        m_xmloutput->append("=\"");
        m_xmloutput->append(m_attributes[i]->GetValue());
        m_xmloutput->append("\"");
    }
    m_xmloutput->append(" />");
    file << *m_xmloutput << endl;
}


/********************* VIRTUAL PREPARE SEARCHING *******************************/

/*! Inserts the text node's title to the search binary tree.
 * \param tree Pointer to the tree.
 */
void CTextNode::PrepareSearching(CSearchTree* tree) {
    tree->Insert(m_title, this);
}

/*! Inserts the simple node's title to the search binary tree.
 * \param tree Pointer to the tree.
 */
void CSimpleNode::PrepareSearching(CSearchTree* tree) {
    tree->Insert(m_title, this);
}

/*! Inserts the parent node's title to the search binary tree (and childs recursively).
 * \param tree Pointer to the tree.
 */
void CParentNode::PrepareSearching(CSearchTree* tree) {
    tree->Insert(m_title, this);
    for (int i = 0; i < m_cntChilds; i++) {
        m_childs[i]->PrepareSearching(tree);
    }
}


/********************* VIRTUAL TYPE GETTERS *******************************/

/*! Finds out, if the text node can have childs.
 */
bool CTextNode::HasChilds() const {
    return false;
}

/*! Finds out, if the comment node can have childs.
 */
bool CCommentNode::HasChilds() const {
    return false;
}

/*! Finds out, if the parent node can have childs.
 */
bool CParentNode::HasChilds() const {
    return true;
}

/*! Finds out, if the simple node can have childs.
 */
bool CSimpleNode::HasChilds() const {
    return false;
}

/*! Finds out, if the text node can have attributes.
 */
bool CTextNode::HasAttributes() const {
    return true;
}

/*! Finds out, if the comment node can have attributes.
 */
bool CCommentNode::HasAttributes() const {
    return false;
}

/*! Finds out, if the parent node can have attributes.
 */
bool CParentNode::HasAttributes() const {
    return true;
}

/*! Finds out, if the simple node can have attributes.
 */
bool CSimpleNode::HasAttributes() const {
    return true;
}

/********************* VIRTUAL COLLAPSE / EXPAND TOOLS *******************************/

/*! Collapses current text node.
 */
void CTextNode::CollapseAll() {
    Collapse();
}

/*! Collapses current simple node.
 */
void CSimpleNode::CollapseAll() {
    Collapse();
}

/*! Collapses current comment node.
 */
void CCommentNode::CollapseAll() {
    Collapse();
}

/*! Expands current text node.
 */
void CTextNode::ExpandAll() {
    Expand();
}

/*! Expands current simple node.
 */
void CSimpleNode::ExpandAll() {
    Expand();
}

/*! Expands current comment node.
 */
void CCommentNode::ExpandAll() {
    Expand();
}

/*! Collapses current parent node and childs recursively.
 */
void CParentNode::CollapseAll() {
    Collapse();
    for (int i = 0; i < m_cntChilds; i++) {
        m_childs[i]->CollapseAll();
    }
}

/*! Expands current parent node and childs recursively.
 */
void CParentNode::ExpandAll() {
    Expand();
    for (int i = 0; i < m_cntChilds; i++) {
        m_childs[i]->ExpandAll();
    }
}


/********************** COMMENT NODE METHODS ******************************/

/*! Creates new comment node.
 * \param comment Comment text.
 */
CCommentNode::CCommentNode(string& comment) : CNode() {
    m_comment = comment;
}

/*! Gets the comment text.
 */
string CCommentNode::GetComment() const {
    return m_comment;
}

/*! Sets the comment text.
 * \param comment Comment text.
 */
void CCommentNode::SetComment(string& comment) {
    m_comment = comment;
}


/************************ PARENT NODE METHODS ****************************/

/*! Creates new parent node and allocates childs.
 * \param title Element title.
 */
CParentNode::CParentNode(string & title) : CNode(title) {
    m_cntChilds = 0;
    m_childs = new CNode * [DEFAULT_CHILDS_SIZE];
    m_sizeChilds = DEFAULT_CHILDS_SIZE;
}

/*! Deallocates childs.
 */
CParentNode::~CParentNode() {
    for(int i = 0; i < m_cntChilds; i++)
        delete m_childs[i];
    if (m_cntChilds)
        delete [] m_childs;
}

/*! Childs memory management.
 */
void CParentNode::ReallocChilds() {
    if (m_cntChilds >= m_sizeChilds - 1) {
        CNode ** tmp = new CNode * [m_sizeChilds * REALLOC_CONSTANT];
        for (int i = 0; i < m_cntChilds; i++) {
            tmp[i] = m_childs[i];
        }

        delete [] m_childs;
        m_childs = tmp;
        m_sizeChilds *= REALLOC_CONSTANT;
    }
}

/*! Inserts new node as a child.
 * \param node Pointer to a new node.
 */
void CParentNode::InsertNode(CNode* node) {
    ReallocChilds();
    m_childs[m_cntChilds] = node;
    node->SetID(m_cntChilds);
    node->SetParent(this);
    m_cntChilds++;
}

/*! Deletes a child.
 * \param id Child id.
 */
void CParentNode::DeleteNode(int id) {
    if (id < 0 || id > m_cntChilds)
        throw InvalidChildID(id);

    delete m_childs[id];

    for (int i = id; i < m_cntChilds - 1; i++) {
        m_childs[i] = m_childs[i + 1];
        m_childs[i]->SetID(i);
    }

    m_cntChilds--;
}


/************************* SIMPLE NODE METHODS ***************************/

/*! Creates new simple node.
 * \param title Element title.
 */
CSimpleNode::CSimpleNode(string& title) : CNode(title) {
}

/*************************** TEXT NODE METHODS *************************/

/*! Creates new text node.
 * \param title Element title.
 * \param value Value of the text node. 
 */
CTextNode::CTextNode(string & title, string & value) : CNode(title) {
    m_value = value;
}

/*! Sets the text node value.
 * \param value Value of the text node. 
 */
void CTextNode::SetValue(string& value) {
    m_value = value;
}

/*! Gets the text node value. 
 */
string CTextNode::GetValue() const {
    return m_value;
}