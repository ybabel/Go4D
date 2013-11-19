#ifndef EMISTRM_H
#define EMISTRM_H
#include "em_ios.h"

class emistrm : public em_ios
{

	int count;

protected:
	virtual int input()   { /* no input*/  return(0);  }
	virtual void output(int) { /* no output */ }

public:

	emistrm(char *, int);
	emistrm();

	int gcount() { return(count); }

	emistrm& getline(char *, int, char = '\n');

	emistrm& get(char&);
	int get();

	emistrm& ignore(int = 1, int = EOF);

	int peek();
  int read(char * buf, int size);

	emistrm& putback(char);
  emistrm& seekg(streampos, int dir=em_ios::beg);
	streampos tellg();


	emistrm& operator>>( char *);
	emistrm& operator>>( char& );
	emistrm& operator>>( int& );
	emistrm& operator>>( long& );
	emistrm& operator>>( double& );

	emistrm& operator>>(emistrm& (*)(emistrm&));
	emistrm& operator>>(em_ios& (*)(em_ios&));



};

class emistrm_withassign : public emistrm
{

protected:
	virtual int input();

public:
	emistrm_withassign();
};

extern emistrm_withassign cin;

emistrm& ws(emistrm&);


#endif
