/**
*AUTHOR : Babel yoann
*TITLE  : streams.cpp, streams.h implementation
*CREATED: 20/12/00
*DATE   : 20/12/00
*TODO   : handle output stream, and cout + cerr + cin
*BUGS   :
*/

#include "Streams.h"

//#ifdef UNDER_CE

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>


namespace Go4D
{

const int undefined=-1;


int convert(int aconst)
{
  switch (aconst)
  {
  case ios::beg : return SEEK_SET;
  case ios::end : return SEEK_END;
  case ios::cur : return SEEK_CUR;
  default : return -1;
  }
}

ifstream::ifstream(const char * fileName, int mode)
{
  file = NULL;
  binfile = NULL;
  if ( (mode&ios::binary)!=0 )
    binfile = new BinaryFile(fileName);
  else
    file = fopen(fileName, "ra");
  seekg(0, ios::beg);
}

ifstream::~ifstream()
{
  close();
}

bool ifstream::eof()
{
  if (binfile != NULL)
    return binfile->eof();
  else
    return feof(file)!=0;
}

void ifstream::seekg(int count, int start)
{
  if (binfile !=NULL)
    binfile->seekg(count, start);
  else
    fseek(file, count, convert(start));
}

int ifstream::tellg()
{
  if (binfile != NULL)
    return binfile->tellg();
  else
    return ftell(file);
}

int ifstream::read(char * buf, int size)
{
  if (binfile != NULL)
    return binfile->read(buf, size);
  else
    return fread((void *)buf, sizeof(char), size, file);
}

void ifstream::getline(char * buf, int size)
{
  if (file != NULL)
    fgets(buf, size, file);
}

bool ifstream::is_open() const
{
  if (binfile != NULL)
    return binfile->is_open();
  else
    return file!=NULL;
}

char ifstream::get()
{
  if (binfile != NULL)
    return binfile->get();
  else
    return fgetc(file);
}

void ifstream::close()
{
  if (binfile != NULL)
    delete binfile;
  else
  {
    if (file!=NULL)
      fclose(file);
  }
  file=NULL;
  binfile=NULL;
}

istream & operator >> (istream & is, char & res)
{
  res = is.get();
  return is;
}

istream & operator >> (istream &is, char * buf)
{
  char ch = is.get();
  while(isspace(ch) && ! is.eof() )
    ch = is.get();
  if (is.eof())
    return is;
  for(;;)
  {
    *buf++ = ch;
    ch = is.get();
    if(isspace(ch) || is.eof() )
      break;  // found whitespace
  }
  *buf = 0;  // terminate
  return is;
}

istream & operator >> (istream & is, int &result)
{
  char buf[128];
  is >> buf;
  result = atoi(buf);
  return is;
}

istream & operator >> (istream & is, double &result)
{
  char buf[128];
  is >> buf;
  result = atof(buf);
  return is;
}

ostream & operator <<(ostream &os, const char chr)
{
  os.put(chr);
  return os;
}

ostream & operator <<(ostream &os, const char *buf)
{
  while( *buf != 0)
    os.put(*buf++);
  return os;
}

ostream & operator <<(ostream &os, const int ch)
{
  char string[128];
#ifndef UNDER_CE
  // YOB GCC4.1
  //itoa(ch, string, 10);
  sprintf(string, "%d", ch);
#else
  _itoa(ch, string, 10);
#endif
  os << string;
  return os;
}

ostream & operator <<(ostream &os, const double ch)
{
  char string[128];
#ifndef UNDER_CE
  gcvt(ch, 5, string);
#else
  _gcvt(ch, 5, string);
#endif
  os << string;
  return os;
}

ostream& operator<<(ostream &os, ostream& (*func)(ostream&))
{
  func(os);
  return os;
}

ofstream::ofstream(const char * filename, int mode)
{
  if ( (mode&ios::binary)!=0 )
    file = fopen(filename, "wab");
  else
    file = fopen(filename, "wa");
}

ofstream::~ofstream()
{
  close();
}

void ofstream::write(char * buf, int size)
{
  fwrite(buf, sizeof(char), size, file);
}

bool ofstream::is_open() const
{
  return file!=NULL;
}

void ofstream::put(char ch)
{
  fputc(ch, file);
}

void ofstream::close()
{
  if (file!=NULL)
    fclose(file);
  file=NULL;
}

void ofstream::setf(int mode)
{
}

void ofstream::flush()
{
  fflush(file);
}

ostream & flush(ostream & os)
{
  os.flush();
  return os;
}

ostream& endl(ostream& strm)
{
  strm.put('\n');
  //strm.put('\r');
  return(strm);
}

ostream & incindent(ostream & os)
{
  ++os.index_indent;
  return os;
}

ostream & decindent(ostream & os)
{
  if (os.index_indent)
    --os.index_indent;
  return os;
}

ostream & reset(ostream & os)
{
  os.index_indent = 0;
  return os;
}

ostream & iendl(ostream & os)
{
  os.put('\n');
  for (int i=0; i<os.index_indent*2; i++)
    os << " ";
  os << flush;
  return os;
}

ostream & incendl(ostream & os)
{
  return os << incindent << iendl;
}

ostream & decendl(ostream & os)
{
  return os << decindent << iendl;
}

ofstream cout("out.txt");
ofstream cerr("err.txt");
ofstream clog("log.txt");


/*#else

  int iostream_index_indent = ios::xalloc();

  ostream & incindent (ostream & os)
  {
    ++os.iword(iostream_index_indent);
    return os;
  }

  ostream & decindent (ostream & os)
  {
    if (os.iword(iostream_index_indent))
      --os.iword(iostream_index_indent);
    return os;
  }

  ostream & reset (ostream & os)
  {
    os.iword(iostream_index_indent) = 0;
    return os;
  }

  ostream & iendl(ostream & os)
  {
    os << '\n';
    int mem=os.width(int(os.iword(iostream_index_indent)*2));
    os << "" << flush;
    os.width(mem);
    return os;
  }

  ostream & incendl(ostream & os)
  {
    return os << incindent << iendl;
  }

  ostream & decendl(ostream & os)
  {
    return os << decindent << iendl;
  }

#endif
*/

}
