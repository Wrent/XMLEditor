#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

#include <cstdlib>
#include <string>

using namespace std;

///other helping functions

class CXML;
class CGUI;

//parsing validity functions
bool IsValidTitle(const string & x);
bool IsValidComment(const string & x);
bool IsValidSimpleTag(const string & x);

//parsing functions
string ExtractXMLTitle(const string & x);
int IgnoreNextWhiteSpaces(const string & x, int pos);
string MakeASCII(const string & x);

//function to open the new XML file
CXML * OpenFile(CXML * xml,string & filePath, CGUI * interface);

#endif	/* FUNCTIONS_H */

