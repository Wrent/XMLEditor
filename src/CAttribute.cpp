#include <cstdlib>
#include <string>

#include "CAttribute.h"
#include "functions.h"
#include "CException.h"

using namespace std;

/********************* PUBLIC METHODS *******************************/

/*! Creates new attribute and checks its validity.
 * \param name Name of the attribute.
 * \param value Value of the attribute.
 */
CAttribute::CAttribute(string & name, string & value) {
    if (IsValidTitle(name))
        m_name = new string(name);
    else
        throw InvalidXMLTitleException(name);
    m_value = new string(value);
}

/*! Frees the allocated memory for attribute.
 */
CAttribute::~CAttribute() {
    delete m_name;
    delete m_value;
}


/*! Gets the name of attribute.
 * \return Name of attribute. 
 */
string CAttribute::GetName() const {
    return *m_name;
}

/*! Gets the value of attribute.
 * \return Value of attribute. 
 */
string CAttribute::GetValue() const {
    return *m_value;
}

/*! Gets the pointer to value of attribute.
 * \return Pointer to value of attribute. 
 */
string * CAttribute::GetValuePointer() const {
    return m_value;
}

/*! Gets the pointer to name of attribute.
 * \return Pointer to name of attribute. 
 */
string * CAttribute::GetNamePointer() const {
    return m_name;
}


/*! Sets the value of attribute.
 * \param value Given value to be set.
 */
void CAttribute::SetValue(string & value) {
    *m_value = value;
}

/*! Sets the name of attribute and checks its validity.
 * \param name Given name to be set.
 */
void CAttribute::SetName(string & name) {
    if (IsValidTitle(name))
        *m_name = name;
    else
        throw InvalidXMLTitleException(name);
}

