#ifndef CSEARCHTREE_H
#define	CSEARCHTREE_H

#include <cstdlib>
#include <string>

class CNode;

using namespace std;

///! Class, which implements binary search tree for strings.
class CSearchTree {
public:
    CSearchTree();
    ~CSearchTree();
    void Insert(const string & val, CNode * node);
    void Filter(string & val);

protected:
    
    ///! Structure, which represents one node of binary search tree.
    struct TElem {
        TElem(TElem * left, TElem * right, const string & val, CNode * node);
        ~TElem();
        void Add(const string & val, CNode * node);
        void Filter(string & val);
        
        void ReallocNodes();
        
        ///! Pointer to the left child.
        TElem * m_Left;
        ///! Pointer to the right child.
        TElem * m_Right;
        ///! Node value
        string m_val;
        
        ///! Pointer to XML nodes with this value as their title
        CNode ** m_nodes;
        ///! Current max count of XML nodes pointers.
        int m_nodesSize;
        ///! Count of XML nodes counters.
        int m_nodesCnt;
    };
    
    ///! Pointer to the tree root.
    TElem * m_root;
};

#endif	/* CSEARCHTREE_H */

