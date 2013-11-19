#ifndef _BINFILE_H_
#define _BINFILE_H_

/**
*AUTHOR : Babel yoann
*TITLE  : BinFile.h, portable binary file handling
*CREATED: 27/01/01
*DATE   : 27/01/01
*OBJECT :
*/

#include <fstream>

namespace Go4D
{

  /**
  * Simple wrapper for file reading
  */
  class BinaryFile
  {
  public :
    enum { beg=0, cur=1, end=2 };
    BinaryFile(const char * fileName);
    void seekg(int count, int start);
    int tellg();
    int read(char * buf, int size);
    bool is_open() const;
    char get();
    void close();
    bool eof();
    ~BinaryFile();

  private:
    #ifdef VSNET
    void * hFile; // = HANDLE for windows, fstream for Linux
    #else
    std::fstream file;
    #endif
  };

}

#endif
