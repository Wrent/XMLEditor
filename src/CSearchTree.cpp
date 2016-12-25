#include <cstdlib>

#include "CSearchTree.h"
#include "CNode.h"

using namespace std;

///! Default count of nodes.
#define DEFAULT_NODES_COUNT 50
///! When reallocing, how many times will new array will be bigger
#define REALLOC_CONSTANT 2

/********************* CLASS METHODS *******************************/

/*! Creates new search tree with no root.
 */
CSearchTree::CSearchTree() {
    m_root = NULL;
}

/*! Deletes whole search tree.
 */
CSearchTree::~CSearchTree() {
    delete m_root;
    m_root = NULL;
}

/*! Inserts an string to the tree.
 * \param val String value.
 * \param node Pointer to a XML node, which is connected with string.
 */
void CSearchTree::Insert(const string& val, CNode* node) {
    TElem * temp;
    if (m_root != NULL) {
        m_root->Add(val, node);
    } else {
        temp = new TElem(NULL, NULL, val, node);
        m_root = temp;
    }
}

/*! Finds specified value in the tree and expands its nodes all the way up to the root.
 * \param val String value.
 */
void CSearchTree::Filter(string& val) {
    m_root->Filter(val);
}

/********************* STRUCTURE (NODES) METHODS *******************************/

/*! Creates new node and allocs pointers to XML nodes.
 * \param left Pointer to a node, which should be on left.
 * \param left Pointer to a node, which should be on right.
 * \param val String value.
 * \param node Pointer to a XML node, which is connected with string.
 */
CSearchTree::TElem::TElem(TElem* left, TElem* right, const string& val, CNode* node) : m_val(val) {
    m_Left = left;
    m_Right = right;

    m_nodes = new CNode * [DEFAULT_NODES_COUNT];
    m_nodesSize = DEFAULT_NODES_COUNT;
    m_nodesCnt = 0;

    m_nodes[m_nodesCnt++] = node;
}

/*! Recursively removes the tree and deallocates pointers to XML nodes.
 */
CSearchTree::TElem::~TElem() {
    delete m_Left;
    delete m_Right;

    delete [] m_nodes;
}

/*! Memory management of pointers to XML nodes.
 */
void CSearchTree::TElem::ReallocNodes() {
    if (m_nodesCnt >= m_nodesSize - 2) {
        CNode ** tmp = new CNode * [m_nodesSize * REALLOC_CONSTANT];
        for (int i = 0; i < m_nodesCnt; i++) {
            tmp[i] = m_nodes[i];
        }

        delete [] m_nodes;
        m_nodes = tmp;
        m_nodesSize *= REALLOC_CONSTANT;
    }
}

/*! Recursively finds the position to insert string and creates new node with it.
 * \param val String value.
 * \param node Pointer to a XML node, which is connected with string.
 */
void CSearchTree::TElem::Add(const string& val, CNode* node) {
    TElem * temp;
    if (val == m_val) { //the node will be here
        ReallocNodes();
        m_nodes[m_nodesCnt++] = node;
        return;
    } else {
        if (val < m_val) { // the node will be in the left subtree
            if (m_Left) {
                m_Left->Add(val, node);
                return;
            } else {
                temp = new TElem(NULL, NULL, val, node);
                m_Left = temp;
                return;
            }
        } else { // the node will be in the right subtree
            if (m_Right) {
                m_Right->Add(val, node);
                return;
            } else {
                temp = new TElem(NULL, NULL, val, node);
                m_Right = temp;
                return;
            }
        }
    }
    return;
}

/*! Recursively finds specified value in the tree and expands its nodes all the way up to the root.
 * \param val String value.
 */
void CSearchTree::TElem::Filter(string& val) {
    if (m_val == val) {
        for (int i = 0; i < m_nodesCnt; i++) { // value found
            m_nodes[i]->ExpandAll();
            if (m_nodes[i]->GetParent())
                m_nodes[i]->GetParent()->ExpandUp();
        }
        return;
    } else {
        if (val < m_val) { //value should be in the left subtree
            if (m_Left)
                m_Left->Filter(val);
            return;
        } else { //value should be in the right subtree
            if (m_Right)
                m_Right->Filter(val);
            return;
        }
    }
}