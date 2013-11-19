#ifndef EM_IOS_H
#define EM_IOS_H

#define STDLEN 1024*100	// arbitrary initial length of buffer
#define INCLEN 1024	// increment amount to increase buffer on overflows

#define EOF (-1)		// match stdio.h

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

typedef long streampos, streamoff;

#define fmask 0x0070       // used to mask format specifier

class em_ios
{

public:
	enum	io_state { goodbit=0, eofbit, failbit, badbit };
  enum open_mode { in        = 0x01,
      out       = 0x02,
      ate       = 0x04,
      app       = 0x08,
      trunc     = 0x10,
      nocreate  = 0x20,
      noreplace = 0x40,
      binary    = 0x80 };
	enum	seek_dir	{ beg, cur, end };

	// format control flags
	enum	{ skipws = 0x0001,
			  dec = 0x0010, oct = 0x0020, hex = 0x0040,
			  scientific = 0x0800, fixed = 0x1000,
			  unitbuf = 0x2000
			};


	long 	format;  // public so manipulators have access
	int	int_mode;


private:

	int delim;

	char 	*buffer;   	// main buffer
	streampos bufLen;   // need bufLen because performing strlen on buffer
							  // is based on NULL and I may want to include NULL
							  // characters in a string manipulation

	streampos bufICount, bufOCount; // input and output position counters

	char 	fill_char;	// fill character

	io_state err_state;		// contains fail flag

	open_mode mode;



	// ensure em_ios does not get coppied
	// these routines do not get bodies
	em_ios(em_ios&);
	void operator=(em_ios&);


protected:

	em_ios(int = STDLEN);  // only construct from derived class
	em_ios(char *, int);


	void setmode(open_mode md) 	{ mode = md; }
	open_mode getmode()				{ return(mode); }

	int sputc(int);
	int sgetn(char *, int);

	void setdelim(int ch) 			{ delim = ch; }


	// first charater is position 0
	streampos seekoff(streamoff, seek_dir, int = em_ios::in|em_ios::out);

	streampos show(int = em_ios::out);

	// hardware routine to input characters
	virtual int input() = 0;
	// hardware routine to output characters
	virtual void output(int) = 0;

	virtual void underflow(int = EOF); // for reading in the buffer




public:


	int 	bad() 					{ return (err_state & badbit); }
	void 	clear(int state=0) 	{ err_state = (io_state)state; }
	int 	eof() 					{ return (err_state & eofbit); }
	int 	fail() 					{ return (err_state & (badbit|failbit)); }
	char	fill()					{ return(fill_char); }
	char	fill(char);
	long	flags()					{ return(format); }
	long	flags(long);
	int	good()					{ return( !err_state); }

	int	operator!()          { return (err_state & (badbit|failbit)); }
	char	*rdbuf()					{ return(buffer); }	// this is streambuf in iostream
	int	rdstate()				{ return((int)err_state); }
  void  setstate(io_state st) { err_state = io_state(err_state|st); }
	long	setf(long);
	long	unsetf(long);

	int 	look()				{ return( buffer[bufICount]); }

	streampos length() 			{ return(bufLen); }


	virtual int overflow(int = EOF);  // for writing out the buffer

};

em_ios&	dec(em_ios&);
em_ios& hex(em_ios&);
em_ios&	oct(em_ios&);

#endif EM_IOS

