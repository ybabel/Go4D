#include <stdlib.h>
#include <string.h>
#include <conio.h>		// machine dependent IO stuff

#include "emostrm.h"

/*
	function:	emostrm
	parameters:	char * - user buffer
					int - size of buffer
	purpose:	constructor - initialize data and create buffer of size
   return:	none
*/

emostrm::emostrm(char *buf, int n) : em_ios(buf, n)
{
	setmode(em_ios::out);
}

/*
	function:	emostrm
	parameters:	none
	purpose:	constructor - initialize data and create buffer of standard
								  size defined in em_ios.h
	return:	none
*/
emostrm::emostrm() : em_ios()
{
	setmode(em_ios::out);
}



/*
	function:	put
	parameters:	char - character to output into buffer
	purpose:	put character into buffer
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::put(char ch)
{
	if(sputc((int) ch) == EOF)
		setstate(badbit);

	return(*this);
}

/*
	function:	seekp
	parameters:	streampos - position to seek to
	purpose:	seek to place of buffer from beginning of buffer
				output counter only
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::seekp(streampos pos)
{
	seekoff(pos, beg, em_ios::out);

	return(*this);
}

/*
	function:	tellp
	parameters:	none
	purpose:	show the current value of the output counter
	return:	streampos - current value of output counter
*/
streampos emostrm::tellp()
{

   return(show(em_ios::out));
}

/*
	function:	write
	parameters:	char * - string to write
					int    - number of characters to write
	purpose:	 write string to buffer
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::write(const char *ptr, int n)
{
	for( int i = 0; i < n; ++i)
		if(sputc((int) *ptr++) == EOF)
			setstate(badbit);

	return(*this);
}

/*
	function:	operator<<
	parameters:	char * - string to write
	purpose:	 write string to buffer
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::operator<<(const char *ptr)
{
	while( *ptr != NULL)
		put((char) *ptr++);

	return(*this);
}

/*
	function:	operator<<
	parameters:	char - character to write
	purpose:	 write character to buffer
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::operator<<( char ch)
{
	put(ch);

	return(*this);
}

/*
	function:	operator<<
	parameters:	int - integer to write
	purpose:	 write integer to buffer after int to ASCII conversion
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::operator<<(int ch)
{
	char string[10];
	int len;

	itoa(ch, string, em_ios::int_mode);
	len = strlen(string);

	write(string, len);

	return(*this);
}

/*
	function:	operator<<
	parameters:	long - integer to write
	purpose:	 write long integer to buffer after long to ASCII conversion
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::operator<<(long ch)
{
	char string[20];
	int len;

	ltoa(ch, string, em_ios::int_mode);
	len = strlen(string);

	write(string, len);

	return(*this);
}


/*
	function:	operator<<
	parameters:	double - integer to write
	purpose:	 write double integer to buffer after double to ASCII conversion
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::operator<<(double ch)
{
	char string[30];
	int len;

#ifndef UNDER_CE
  gcvt(ch, 5, string);
#else
  _gcvt(ch, 5, string);
#endif
	len = strlen(string);

	write(string, len);

	return(*this);
}


/*
	function:	operator<<
	parameters:	void * - pointer to memory to write
	purpose:	 write void * to buffer after char * cast
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::operator<<(void *ptr)
{
	char *temp;

	temp = (char *) ptr;

	while( *temp != NULL)
		put(*temp++);

	return(*this);
}

/*
	function:	operator<<
	parameters:	emostrm& - reference to function pointer with emostrm&
					parameter
	purpose:	 allows for chaning of manipulators into output stream.
				 calls function pointer.  operates with emostrm manipulators
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::operator<<(emostrm& (*func)(emostrm&))
{
	func(*this);

	return(*this);
}

/*
	function:	operator<<
	parameters:	em_ios& - reference to function pointer with em_ios&
					parameter
	purpose:	 allows for chaning of manipulators into output stream.
				 calls function pointer.  operates with em_ios manipulators
	return:	emostrm& - reference for chaining
*/
emostrm& emostrm::operator<<(em_ios& (*func)(em_ios&))
{
	func(*this);

	return(*this);
}

/*
	function:	endl
	parameters:	emostrm& - reference to emostrm for access to public data
	purpose:	 manipulator for putting a newline/carraige return onto the
				 buffer then flushing the buffer
	return:	emostrm& - reference for chaining
*/
emostrm& endl(emostrm& strm)
{
	strm.put('\n');
	strm.put('\r');
	strm.overflow(NULL);  // flush it

	return(strm);

}


/*
	function:	ends
	parameters:	emostrm& - reference to emostrm for access to public data
	purpose:	 manipulator for putting a NULL onto the
				 buffer then flushing the buffer
	return:	emostrm& - reference for chaining
*/
emostrm& ends(emostrm& strm)
{
	strm.put('\0');
	strm.overflow(NULL);   // flush it

	return(strm);

}

/*
	function:	flush
	parameters:	emostrm& - reference to emostrm for access to public data
	purpose:	 flushes the buffer.  if the buffer does not contain a NULL,
				 garbage may be output after the last character
	return:	emostrm& - reference for chaining
*/
emostrm& flush(emostrm& strm)
{
	// overflow flushes buffer and NULL will not be printed
	strm.overflow(NULL);

	return(strm);
}


// special class for overloading the output virtual function to output
// the buffer to hardware
emostrm_withassign::emostrm_withassign() : emostrm()
{
}

// here are our hardware I/O implementation dependencies
// Using DOS I/O for example;  substitute your own hardware interface
void emostrm_withassign::output(int ch)
{
	putch(ch);

}


// this assinment should make these classes compatable with IOstream
emostrm_withassign cout;
emostrm_withassign cerr;


emostrm & incindent (emostrm & os)
{
  ++os.index_indent;
  return os;
}

emostrm & decindent (emostrm & os)
{
  if (os.index_indent)
    --os.index_indent;
  return os;
}

emostrm & reset (emostrm & os)
{
  os.index_indent = 0;
  return os;
}

emostrm & iendl(emostrm & os)
{
  os << '\n';
  for (int i=0; i<os.index_indent*2; i++)
    os << " ";
  os << flush;
  return os;
}

emostrm & incendl(emostrm & os)
{
  return os << incindent << iendl;
}

emostrm & decendl(emostrm & os)
{
  return os << decindent << iendl;
}
 