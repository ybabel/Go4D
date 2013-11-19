#include <string.h>
//#include <mem.h>
#include <memory.h>
#include <ctype.h>

#include "em_ios.h"

/*
   function:	em_ios
   parameters:	int - size of buffer
   purpose:	constructor - initialize data and create buffer of size
   return:	none
*/

em_ios::em_ios(int size)
{
	format = dec;
	int_mode = 10;
	format |= fixed;

	buffer = new char[size];
	bufOCount = bufICount = 0;
	bufLen = size;
	fill_char = ' ';
	memset(buffer, NULL, (size_t) bufLen);

}

/*
	function:	em_ios
	parameters:	char * 	- user buffer
					 int 	- size of buffer
	purpose:	constructor - initialize data and create buffer of size
	return:	none
*/

em_ios::em_ios(char *buf, int size)
{
	format = dec;
	int_mode = 10;
	format |= fixed;

	buffer = buf;
	bufOCount = bufICount = 0;
	bufLen = size;
	fill_char = ' ';

}


/*
	function:	sputc
	parameters:	int 	- character to put
	purpose:	lower level function to attempt putting character to out device
	return:	int -  last character that was attempted to be sent out
*/
int em_ios::sputc(int ch)
{
	int retVal;

	switch(mode& (em_ios::in | em_ios::out))
	{
		case out:
		{

			retVal = ch;
			if(bufOCount < bufLen)
				buffer[bufOCount++] = (char) ch;
			else
				retVal = overflow(ch);   // flush
		}
		break;

		case in:
		{
			retVal = ch;
			buffer[bufICount--] = (char) ch;
		}
		break;
	}

	return(retVal);
}

/*
	function:	sgetn
	parameters:	char * - place to put read characters
					int 	- number of characters to try to read
	purpose:	get characters from buffer and fill buffer if needed
	return:	int -  number of characters actually gotten
*/
int em_ios::sgetn(char *chrs, int num)
{
#define isdelim(x) isspace(x)

	int retVal, i;

	retVal = 1;
	for(i = 0; i < num; ++i)
	{
    if (bufICount>=bufLen)
      bufICount=0;

    if ((mode&em_ios::binary)==0)
  		if(isdelim(buffer[bufICount]))
  		{
	  		chrs[i] = buffer[bufICount++];
		  	break;
		  }

    if(bufICount > 0)
		{
      if ((mode&em_ios::binary)==0)
      {
			  if(isdelim(buffer[bufICount]))
			  {
			  	chrs[i] = buffer[bufICount++];
			  	while(((mode&em_ios::binary)==0) && isdelim(buffer[bufICount++]))
			  	{
			  		if(bufICount >= bufLen)
			  		{
			  		  --i;
			  			break; // remove whitespace
			  		}
			  	}
			  	--bufICount;
        }
        else
			  	chrs[i] = buffer[bufICount++];
      }
			else
				chrs[i] = buffer[bufICount++];
		}
		else
		{
  	  underflow('\r');  // get more, newline terminates getting
									// you may want to change the delimiter
			chrs[i] = buffer[bufICount++];
		}
	}

	return(i);

}


/*
	function:	overflow
	parameters:	char * - place to put read characters
					int 	- number of characters to try to read
	purpose:	called by sputc when the buffer has filled and more characters
				are being sent out.  flushes buffer, then resets buffer to let it
				fill more
	return:	int -  last character
*/
int em_ios::overflow(int ch)
{
	// flush and clear buffer

	long lastChar;
	char final;

	// last character is eihter bufLen position or NULL character;
	if(bufLen >= long(strlen(buffer)))
		lastChar = strlen(buffer); // null character found before buffer end
	else
		lastChar = bufLen;

	for(int i = 0; i < lastChar; ++i)
	{
		final = buffer[i];  // save it to return a possible error code
		if(buffer[i] == EOF)
			break;   // whoa, we're done with that
		output(buffer[i]);	// send it through
	}

	// buffer flushed, start filling again
	bufOCount = 0;

	if( (ch != NULL) && (ch != EOF) )
		buffer[bufOCount++] = (char) ch;

	return(final);


}


/*
	function:	underflow
	parameters:	int 	- end delimiter
	purpose:	called by sgetn when the buffer is empty and more characters
				are needed.  atempts to fill buffer
	return:	int -  last character
*/
void em_ios::underflow(int ch)
{
	// fill buffer because it is empty

	for(int i = 0; i < bufLen; ++i)
	{
		buffer[i] = input();  // save it to return a possible error code
    if ((mode&em_ios::binary)==0)
  		if(buffer[i] == ch)
	  		break;   // whoa, found delimiter so we're done with that
	}

}



/*
	function:	fill
	parameters:	char 	- fill character
	purpose:	sets the fill character to be used for fills and padding.
	return:	char -  fill character
*/
char em_ios::fill(char ch)
{
	char temp;

	temp = fill_char;
	fill_char = ch;

	return(temp);
}


// offset of first character is 0
/*
	function:	seekoff
	parameters:	streamoff 	- offset location to seek to
					seek_dir		- direction to begin seek from -
									  beginning, middle, or end
					int			- specifes which counters to adjust
									  bufOCount or bufICount

	purpose:	moves either the input or output counter to the location
				specifed by seek_dir and streanoff
	return:	streampos -  new position, can be either input or output depending
				on mode
*/
streampos em_ios::seekoff(streamoff off, seek_dir dir, int mode)
{

	switch(dir)
	{
		case beg:
			if(mode & em_ios::out)
			{
				bufOCount = 0;
				bufOCount += off;
			}

			if(mode & em_ios::in)
			{
				bufICount = 0;
				bufICount += off;
			}
		break;

		case cur:
			if(mode & em_ios::out)
				if((bufOCount + off) < bufLen)
					bufOCount += off;  // only seek if place inside buffer

			if(mode & em_ios::in)
				if((bufICount + off) < bufLen)
					bufICount += off;  // only seek if place inside buffer
		break;

		case end:
			if(mode & em_ios::out)
			{
				bufOCount = bufLen;
				bufOCount -= off;
			}

			if(mode & em_ios::in)
			{
				bufICount = bufLen;
				bufICount -= off;
			}
		break;
	}

	if( mode & (em_ios::in | em_ios::out))
		return(bufOCount);  // bufOCount and bufICount have same value
	else if(mode & em_ios::out)
		return(bufOCount);
	else
	   return(bufICount);


}


/*
	function:	show
	parameters:	int 	- determines I/O mode to show
	purpose:	shows where the input or output counter currently is pointing
				depends on int parameter for which is returned
	return:	streampos -  input or output position
*/
streampos em_ios::show(int mode)
{
	if(mode & em_ios::out)
		return(bufOCount);
	else
		return(bufICount);
}

long em_ios::setf(long f)
{
  format = f;
  return f;
}

long em_ios::unsetf(long f)
{
  return f;
}



/*
	function:	dec, hex, oct
	parameters:	em_ios& 	- reference to em_ios class for public access.
	purpose:	manipulators for changing how the em_ios class interprets
				integers.  changes format flag and sets int_mode
	return:	em_ios& -  reference to em_ios class for chaining
*/
em_ios& dec(em_ios& io)
{
	io.format &= ~fmask;
	io.format = em_ios::dec;
	io.int_mode = 10;
	return(io);
}

em_ios& hex(em_ios& io)
{
	io.format &= ~fmask;
	io.format = em_ios::hex;
	io.int_mode = 16;
	return(io);
}

em_ios& oct(em_ios& io)
{
	io.format &= ~fmask;
	io.format = em_ios::oct;
	io.int_mode = 8;
	return(io);
}



