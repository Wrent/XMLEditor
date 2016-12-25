#include "CTagStack.h"

///! Default size of stack.
#define DEFAULT_STACK_SIZE 10
///! When reallocing, how many times will new array will be bigger
#define REALLOC_CONSTANT 2

/********************* PUBLIC METHODS *******************************/

/*! Creates and allocates new string stack.
 */
CTagStack::CTagStack(){
    m_stack = new string [DEFAULT_STACK_SIZE];
    
    m_stackSize = DEFAULT_STACK_SIZE;
    m_stackCnt = 0;
}

/*! Deallocates stack.
 */
CTagStack::~CTagStack(){
    delete [] m_stack;
}

/*! Insert new string on the top of the stack.
 * \param tag XML element title.
 */
void CTagStack::Push(string& tag){
    ReallocStack();
    m_stack[m_stackCnt++] = tag;
}

/*! Gets the string from the top of the stack.
 * \return XML element title from the top of the stack.
 */
string CTagStack::Pop(){
    if (m_stackCnt == 0){
        return "0";
    }
    return m_stack[--m_stackCnt];
}


/*! Gets the number of items in the stack.
 */
int CTagStack::GetStackCnt() const{
    return m_stackCnt;
}

/********************* PRIVATE METHODS *******************************/

/*! Stack memory management.
 */
void CTagStack::ReallocStack(){
    if (m_stackCnt > m_stackSize - 1) {
        string * tmp = new string [m_stackSize * REALLOC_CONSTANT];
        for (int i = 0; i < m_stackCnt; i++) {
            tmp[i] = m_stack[i];
        }

        delete [] m_stack;
        m_stack = tmp;
        m_stackSize *= REALLOC_CONSTANT;
    }
}