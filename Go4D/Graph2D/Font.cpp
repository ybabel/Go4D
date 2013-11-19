/*****************************************************************************\
*AUTHOR : Babel yoann
*TITLE  : font.cc, font.h implementation
*CREATED: 22/10/95
*DATE   : 29/06/96
*BUGS   :
\*****************************************************************************/

#include "Font.h"
#include "Tools/Streams.h"

namespace Go4D
{


  /*****************************************************************************\
  *   AFont & Font6x10
  \*****************************************************************************/

  AFont::AFont(int x, int y, const char * filename)
  {
    dimx = x;
    dimy = y;
    alloc(chars = new byte [NBCHARS*dimy*dimx]);

    ifstream f(filename, ios::binary);
    if(!f.is_open())
      error1("Font load error ", filename);
    byte * buf;
    alloc(buf = new byte[NBCHARS*dimy]); //1 byte for the 6 plots of line, 10 lines -> 10 byte

    f.read((char *)buf, long(sizeof(byte)*NBCHARS*dimy));

    //convert in the appropriate format
    for (int i=0; i<NBCHARS; i++) // all the chars
      for (int j=0; j<dimy; j++) // all the lines of a char
        for (int k=0; k<dimx; k++) // all the pixel of a line
          chars[i*dimy*dimx+j*dimx+k] = byte(((buf[i*dimy+j]>>(7-k))&1)*ON);
    delete [] buf;
  }

  AFont::~AFont()
  {
    delete [] chars;
  }

}
