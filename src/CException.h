#ifndef CEXCEPTION_H
#define	CEXCEPTION_H

#include "CGUI.h"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

///! Abstract class for different exceptions.

class CException {
public:

    CException() {
    };
    virtual void Print(CGUI * interface) const = 0;
};

/****************************************************/

///! Class for exception, which is thrown, when specified file does not exist.

class InvalidFileNameException : public CException {
public:
    InvalidFileNameException(const string & fileName);
    virtual void Print(CGUI * interface) const;
protected:
    ///! Name of nonexisting file.
    string m_fileName;
};

/****************************************************/

///! Class for exception, which is thrown, when given XML title or attribute is not valid.

class InvalidXMLTitleException : public CException {
public:
    InvalidXMLTitleException(const string & title);
    virtual void Print(CGUI * interface) const;
protected:
    ///! Title of invalid XML node.
    string m_title;
};

/****************************************************/

///! Class for exception, which is thrown, when given XML attribute already exists.

class AttributeAlreadyExistsException : public CException {
public:
    AttributeAlreadyExistsException(const string & name);
    virtual void Print(CGUI * interface) const;
protected:
    ///! Duplicate attribute name
    string m_name;
};

/****************************************************/

///! Class for exception, which is thrown, when demanded XML attribute does not exist.

class AttributeDoesNotExistException : public CException {
public:
    AttributeDoesNotExistException(const string & name);
    virtual void Print(CGUI * interface) const;
protected:
    ///! Nonexisting attribute name.
    string m_name;
};

/****************************************************/

///! Class for exception, which is thrown, when demanded child of a node does not exist.

class InvalidChildID : public CException {
public:
    InvalidChildID(int id);
    virtual void Print(CGUI * interface) const;
protected:
    ///! Nonexisting attribute ID.
    int m_id;
};

/****************************************************/

///! Class for exception, which is thrown, when given XML file is not valid.

class InvalidXMLFormatException : public CException {
public:
    InvalidXMLFormatException(const string & fileName);
    virtual void Print(CGUI * interface) const;
protected:
    ///! File name of invalid formated XML.
    string m_fileName;
};

#endif	/* CEXCEPTION_H */

