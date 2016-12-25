#ifndef CTAGSTACK_H
#define	CTAGSTACK_H

#include <cstdlib>
#include <string>

using namespace std;

///! Class, which implements a simple strings stack.
class CTagStack{
public:
    CTagStack();
    ~CTagStack();
    
    void Push(string & tag);
    string Pop();
    
    int GetStackCnt() const;
protected:
    void ReallocStack();
    
    
    ///! Array of strings representing the stack.
    string * m_stack;
    ///! Current max stack element count.
    int m_stackSize;
    ///! Count of elements in stack.
    int m_stackCnt;
};

#endif	/* CTAGSTACK_H */

