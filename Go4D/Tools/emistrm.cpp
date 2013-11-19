#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//#include <conio.h>		// machine dependent IO stuff

#include "emistrm.h"

/*
	function:	emistrm
	parameters:	char * - user buffer
					int - size of buffer
	purpose:	constructor - initialize data and point to buffer
   return:	none
*/
emistrm::emistrm(char *buf, int n) : em_ios(buf, n)
{
	count = 0;
	setmode(em_ios::in);
	format |= em_ios::skipws;
	setdelim('\r');
}

/*
	function:	emistrm
	parameters:	none
	purpose:	constructor - initialize data and create buffer of standard
								  size defined in em_ios.h
	return:	none
*/
emistrm::emistrm() : em_ios()
{
	count = 0;
	setmode(em_ios::in);
	format |= em_ios::skipws;
   setdelim('\r');
}



/*
	function:	getline
	parameters:	char * 	- buffer where to place characters
					int 		- number of characters to get
					char		- delimeter
	purpose:				 	- get characters from input buffer
	return:	emistrm&    - reference to allow chaining
*/
emistrm& emistrm::getline(char *chrs, int num, char delim)
{
  memset(chrs, NULL, num);
	setdelim(delim);
	sgetn(chrs, num);
	return(*this);
}


/*
	function:	get
	parameters:	char&		- reference to place character
	purpose:				 	- get character from input buffer
	return:	emistrm&    - reference to allow chaining
*/
emistrm& emistrm::get(char& chr)
{
	char temp[1];

	sgetn(temp, 1);

	chr = (char) temp[0];

	return(*this);
}

/*
	function:	get
	parameters:	none
	purpose:				 	- get character from input buffer
	return:	int    		- character
*/
int emistrm::get()
{
	char temp[1];

	sgetn(temp, 1);

	return( (int) temp[0]);
}

int emistrm::read(char * buf, int size)
{
  //for (int i=0; i<size; i++)
  //  if (sgetn(buf++, 1)==0)
  //    break;
  //return i;
  return sgetn(buf, size);
}

/*
	function:	get
	parameters:	int		- number of characters to ignore
					int		- delimiter
	purpose:				 	- ignores and tosses characters from input buffer
	return:	int    		- character
*/
emistrm& emistrm::ignore(int num, int delim)
{
	char temp[STDLEN];

	setdelim(delim);
	sgetn(temp, num);

	return(*this);
}


/*
	function:	peek
	parameters: none
	purpose:				 	- returns next character
	return:	int    		- character
*/
int emistrm::peek()
{
	return(look());
}


/*
	function:	putback
	parameters:	char		- character to put back
	purpose:				 	- puts character back in buffer
	return:	emistrm&		- reference to allow chaining
*/
emistrm& emistrm::putback(char chr)
{
	sputc(chr);

	return(*this);
}


/*
	function:	seekg
	parameters:	streampos	- position to move input counter to reletive
									  to start of input buffer
	purpose:				 		- move input counter to a new position
	return:	emistrm&			- reference to allow chaining
*/
emistrm& emistrm::seekg(streampos pos, int dir)
{
  seekoff(pos, em_ios::seek_dir(dir), em_ios::in);
	//seekoff(pos, beg, em_ios::in);

	return(*this);
}

/*
	function:	tellg
	parameters:	none
	purpose:				 		- show position of input counter
	return:	streampos		- position
*/
streampos emistrm::tellg()
{

	return(show(em_ios::in));
}


/*
	function:	operator>>
	parameters:	char *	- buffer to load characters into
	purpose:				 	- fills a buffer with characters from input buffer
								  stops on whitespace
	return:	emistrm&		- reference to allow chaining
*/
emistrm& emistrm::operator>>(char *ptr)
{
	char ch;

	for(;;)
	{
		get((char) ch);
		if(isspace(ch))
			break;  // found whitespace
		*ptr++ = (char)ch;
	}

	*ptr = 0;  // terminate


	return(*this);
}

/*
	function:	operator>>
	parameters:	char&		- place to store character
	purpose:				 	- get a character from input buffer
	return:	emistrm&		- reference to allow chaining
*/
emistrm& emistrm::operator>>( char& ch)
{
	get(ch);

	return(*this);
}

/*
	function:	operator>>
	parameters:	int&		- place to store integer
	purpose:				 	- get an integer from input buffer.  convert character
								  string to integer
	return:	emistrm&		- reference to allow chaining
*/
emistrm& emistrm::operator>>(int& ch)
{
	char string[10];

	getline(string, 10, ' ');

	ch = atoi(string);

	return(*this);
}

/*
	function:	operator>>
	parameters:	long&		- place to store long
	purpose:				 	- get a long from input buffer.  convert character
								  string to long
	return:	emistrm&		- reference to allow chaining
*/
emistrm& emistrm::operator>>(long& ch)
{
	char string[20];

	getline(string, 20, ' ');

	ch = atol(string);

	return(*this);
}


/*
	function:	operator>>
	parameters:	double&		- place to store double
	purpose:				 	- get a double from input buffer.  convert character
								  string to double
	return:	emistrm&		- reference to allow chaining
*/
emistrm& emistrm::operator>>(double& ch)
{
	char string[30];

	getline(string, 30, ' ');

	ch = atof(string);

	return(*this);
}



/*
	function:	operator>>
	parameters:	emistrm&	- reference to function pointer  with emistrm&
								  parameter
	purpose:				 	- allows chaining of manipulators into input stream
								  calls function pointer.  works with emistrm.
	return:	emistrm&		- reference to allow chaining
*/
emistrm& emistrm::operator>>(emistrm& (*func)(emistrm&))
{
	func(*this);

	return(*this);
}

/*
	function:	operator>>
	parameters:	em_ios&	- reference to function pointer  with em_ios&
								  parameter
	purpose:				 	- allows chaining of manipulators into input stream
								  calls function pointer.  works with em_ios.
	return:	emistrm&		- reference to allow chaining
*/
emistrm& emistrm::operator>>(em_ios& (*func)(em_ios&))
{
	func(*this);

	return(*this);
}


/*
	function:	ws
	parameters:	none
	purpose:				 	- white space manipulator.  causes whitespace to be
	     						  eaten
	return:	emistrm&		- reference to allow chaining
*/
emistrm& ws(emistrm& strm)
{
	strm.format |= em_ios::skipws;

	return(strm);

}



emistrm_withassign::emistrm_withassign() : emistrm()
{
}

// here are our hardware I/O implementation dependencies
// Using DOS I/O for example;  substitute your own hardware interface
int emistrm_withassign::input()
{
	return(getch());

}


emistrm_withassign cin;

