// mbstring.h: header file for more or less standard C++ strings
// Michael Berman, 1997
//
// mbstring inherits from the apstring class in order to extend its
// functionality to include the 'compare' operation (and perhaps others
// if I decide later I need them!)

#include "apstring.h"

class mbstring : public apstring
{
public:
    mbstring( ) { }
    mbstring( const char * s ) : apstring(s) { } 
        // construct from C string literal
    mbstring( const apstring & str ) : apstring(str) { }  
        // copy constructor

    int compare( const mbstring & s)
    {
	if (*this < s)
	    return -1;
	else if (*this > s)
	    return 1;
	else
	    return 0;
    }
};


