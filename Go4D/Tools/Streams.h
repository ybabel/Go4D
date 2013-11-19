#ifndef _STREAMS_H_
#define _STREAMS_H_

/**
*AUTHOR     : Babel Yoann
*CREATION   : 20/12/00
*DATE       : 18/01/01
*TITLE      : Streams.h , Streams classes
*OBJECT     : this files have been created to ensure
*             streams compatibility with PocketPC
*/

//#ifdef UNDER_CE
#include <stdio.h>
#include "Platform/BinFile.h"

namespace Go4D
{

class ios
{
public :
  enum open_mode { in        = 0x01,
    out       = 0x02,
    ate       = 0x04,
    app       = 0x08,
    trunc     = 0x10,
    nocreate  = 0x20,
    noreplace = 0x40,
    binary    = 0x80 };

  enum seek_dir { beg=0, cur=1, end=2 };

  enum {  skipws     = 0x0001,
    left       = 0x0002,
    right      = 0x0004,
    internal   = 0x0008,
    dec        = 0x0010,
    oct        = 0x0020,
    hex        = 0x0040,
    showbase   = 0x0080,
    showpoint  = 0x0100,
    uppercase  = 0x0200,
    showpos    = 0x0400,
    scientific = 0x0800,
    fixed      = 0x1000,
    unitbuf    = 0x2000,
    stdio      = 0x4000
  };

  static const int undefined;
};

class istream
{
public:
  virtual void seekg(int count, int start=ios::beg)=0;
  virtual int read(char * buf, int size)=0;
  virtual void getline(char * but, int size)=0;
  virtual char get()=0;
  virtual bool eof()=0;
  virtual int tellg()=0;
  friend istream & operator >>(istream &, char &);
  friend istream & operator >>(istream &, char *);
  friend istream & operator >>(istream &, int &);
  friend istream & operator >>(istream &, double &);
};

class ifstream : public istream
{
public :
  ifstream(const char * fileName, int mode=ios::in);
  virtual void seekg(int count, int start=ios::beg);
  virtual int read(char * buf, int size);
  virtual void getline(char * but, int size);
  bool is_open() const;
  char get();
  void close();
  bool eof();
  int tellg();
  ~ifstream();

protected:
  FILE * file;
  BinaryFile * binfile;
};

class ostream
{
public:
  virtual void put(char )=0;
  virtual void setf(int mode) = 0;
  virtual void flush()=0;
  virtual void write(char * buf, int size)=0;
  int index_indent;
  friend ostream & operator <<(ostream &, const char );
  friend ostream & operator <<(ostream &, const char *);
  friend ostream & operator <<(ostream &, const int );
  friend ostream & operator <<(ostream &, const double );
  friend ostream& operator<<(ostream &, ostream& (*)(ostream&));
};

class ofstream : public ostream
{
public :
  ofstream(const char * filename, int mode=ios::out);
  ~ofstream();
  void write(char * buf, int size);
  bool is_open() const;
  void put(char );
  void close();
  void setf(int mode);
  void flush();
protected :
  FILE * file;
};

extern ofstream cout;
extern ofstream cerr;
extern ofstream clog;

ostream & flush(ostream &);
ostream & endl(ostream &);
ostream & incindent(ostream &);
ostream & decindent(ostream &);
ostream & reset(ostream &);
ostream & iendl(ostream &);
ostream & incendl(ostream &);
ostream & decendl(ostream &);

/*#else

#include <fstream.h>
ostream & incindent(ostream &);
ostream & decindent(ostream &);
ostream & reset(ostream &);
ostream & iendl(ostream &);
ostream & incendl(ostream &);
ostream & decendl(ostream &);

//#endif
*/


}
#endif
