#ifndef EMOSTRM_H
#define EMOSTRM_H
#include "em_ios.h"

class emostrm : public em_ios
{

protected:
	virtual int input()   { /* no input*/  return(0);  }
	virtual void output(int) { /* no output */ }

public:

  emostrm(char *, int);
	emostrm();

	emostrm& put(char);
	emostrm& seekp(streampos);
	streampos tellp();
	emostrm& write(const char *, int);
	emostrm& operator<<(const char *);
	emostrm& operator<<( char );
	emostrm& operator<<( int );
	emostrm& operator<<( long );
	emostrm& operator<<( double );
	emostrm& operator<<( void *);

	emostrm& operator<<(emostrm& (*)(emostrm&));
	emostrm& operator<<(em_ios& (*)(em_ios&));

  int index_indent;

};

class emostrm_withassign : public emostrm
{

protected:
	virtual void output(int);

public:
	emostrm_withassign();
};

extern emostrm_withassign cout;
extern emostrm_withassign cerr;



emostrm& endl(emostrm&);
emostrm& ends(emostrm&);
emostrm& flush(emostrm&);

emostrm & incindent (emostrm &);
emostrm & decindent (emostrm &);
emostrm & reset (emostrm &);
emostrm & iendl(emostrm &);
emostrm & incendl(emostrm &);
emostrm & decendl(emostrm &);

#endif
