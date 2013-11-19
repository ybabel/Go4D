#ifndef _FONT_H_
#define _FONT_H_

/*
*AUTHOR : Babel yoann
*TITLE  : font.h
*CREATED: 22/10/95
*DATE   : 08/03/96
*OBJECT : fonts
*/

#include "Tools/Common.h"

namespace Go4D
{

#define ON 0xff
#define OFF 0x00
#define NBCHARS 256

  /**
  *FONT : can load a from the disk, and give the information necessarry to
  *  draw characters, like the size of the font, and the definitions of the
  *  chars in a common format. Only private format supported (non proportional).
  *
  *Font file format : currently one byte in the file represent 8 pixel of the
  *  char (on the same line, that why CharSizeX is limited to 8). So that it
  *  take CharSizeY byte to store one char. The file contains the definition
  *  of 255 chars (CharSizeY*256 byte), no header, no footer.
  */
  class AFont
  {
  public :
    /** Font : load the file in the buffer chars, always load 255 characters.
    * the parameters describe the size of one character, in pixel. You must
    * also specifies the name of the file where the font is strored.
    * No verification is made on the format of the file. So becareful !!!!!.
    */
    AFont(int CharSizeX, int CharSizeY, const char * FontFileName);
    ///
    ~AFont();
    /** operator [] : return a pointer on the begining of the description of the
    * char. For each pixel that is on a byte is set to ON (255) int chars.
    * The size of chars is NBCHARS*CharSizeX*CharSizeY bytes. It may seem
    * too much, but this way of storing info is faster for drawing.
    */
    byte * operator [] (int i) const { return chars+dimx*i*dimy; }
    ///  GetDimX and GetDimY : return the size of the font
    int GetDimX() const {return dimx;}
    ///  GetDimX and GetDimY : return the size of the font
    int GetDimY() const {return dimy;}

  protected :
    int dimx, dimy;
    byte * chars;
  };

  /**
  * A font which size is 6 x 10
  */
  class Font6x10 : public AFont
  {
    typedef AFont inherited;
  public :
    ///
    Font6x10(const char * fontname) : AFont(6,10, fontname) {}
  };

}
#endif
