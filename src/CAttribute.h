#ifndef CATTRIBUTE_H
#define	CATTRIBUTE_H

#include <cstdlib>
#include <string>

using namespace std;

///! Class for creating and working with XML attributes

class CAttribute {
public:
    CAttribute(string & name, string & value);
    ~CAttribute();
    string GetName() const;
    string GetValue() const;

    string * GetNamePointer() const;
    string * GetValuePointer() const;

    void SetName(string & name);
    void SetValue(string & value);
protected:
    ///! Pointer to attribute name
    string * m_name;
    ///! Pointer to attribute value
    string * m_value;
};

#endif	/* CATTRIBUTE_H */

