/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : image.h
 *CREATED: 22/10/95
 *DATE   : 08/03/96
 *OBJECT : 256 color images
\*****************************************************************************/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <assert.h>
#include "common.h"



/*****************************************************************************\
 *FONT : can load a from the disk, and give the information necessarry to 
 *  draw characters, like the size of the font, and the definitions of the 
 *  chars in a common format.
 *
 *Font : load the file in the buffer chars, always load 255 characters.
 *  the parameters describe the size of one character, in pixel. You must 
 *  also specifies the name of the file where the font is strored. 
 *  No verification is made on the format of the file. So becareful !!!!!.
 *
 *Font file format : currently one byte in the file represent 8 pixel of the 
 *  char (on the same line, that why CharSizeX is limited to 8). So that it
 *  take CharSizeY byte to store one char. The file contains the definition
 *  of 255 chars (CharSizeY*256 byte), no header, no footer.
 *
 *operator [] : return a pointer on the begining of the description of the 
 *  char. For each pixel that is on a byte is set to ON (255) int chars.
 *  The size of chars is NBCHARS*CharSizeX*CharSizeY bytes. It may seem
 *  too much, but this way of storing info is faster for drawing.
 * 
 *GetDimX and GetDimY : return the size of the font
\*****************************************************************************/

#define ON 0xff
#define OFF 0x00
#define NBCHARS 256

class Fonts
{
  public :
  Fonts(int CharSizeX, int CharSizeY, char * FontFileName);
  ~Fonts();
  byte * operator [] (int i) const { return chars+dimx*i*dimy; }
  int GetDimX() const {return dimx;}
  int GetDimY() const {return dimy;}

  protected :
  int dimx, dimy;
  byte * chars;
};

class Font6x10 : public Fonts
{
  public :
  Font6x10(char * fontname) : Fonts(6,10, fontname) {}
};

class Drawer;
class ZDrawer;
class SDrawer;

/*****************************************************************************\
 *IMAGE : linear buffer, 1 octect, 1 pixel. the device class can output an 
 *   image on the screen
 *   this class is con just handle image (load form disk, drawing)
 *Image : the constructor will allocate the necessary space in memory 
 *   (siwex*sizey octects)
 *~Image : if the buffer had not change, free the memory.
 *NewBuffer : this method had been created to allow the use of 
 *   shared memory under XWindow (or DIB under win95)
 *   you don't have to use it, the DEVICE class will.
 *GetPixel : get the color of the pixel at the specified position, 
 *   return 0 in case you access out of the image
 *Width, Height, Len : image dimensions
 *fBufChange : to avoid destroying buffer, because it is automatically done 
 *   by XDestroyImage and XShmDestroyImage
 *MiddleX, MiddleY : return the coordinates of the screen's middle (use for 
 *   projection)
 *GetBuffer : return a pointer on the image's data (used by Device only)
\*****************************************************************************/

class Device;

class Image
{
  friend Device;
  friend Drawer;
  friend ZDrawer;
  friend SDrawer;

  public :
  Image(int sizex, int sizey, int autoalloc=TRUE);
  virtual ~Image();
  virtual void Resize(int newsizex, int newsizey);
  byte GetPixel(int x, int y) const;
  void NewBuffer(byte * pNewBuf);
  int Width() const;
  int Height() const;
  int Len() const; //Widht*Height
  int MiddleX() const {return fWidth/2;}
  int MiddleY() const {return fHeight/2;}

  protected :
  byte * GetBuffer() const {return fBuffer;}

  int fWidth;
  int fHeight;
  int fLen;
  byte * fBuffer;
  int fBufChange;
};

/*****************************************************************************\
 *IMAGEPAL : all the image that can be loaded from the disk, with their palette
 *  this class is  never use in the programs. The classes Bmp and Targa are
 *  use instead.
\*****************************************************************************/

typedef byte Palette[256][3];

class ImagePal : public Image
{
  public :
  ImagePal():Image(0,0,FALSE){} // modifed later by targa or bmp
  Palette & Pal() const;

  protected :
  Palette fPal;
};

/*****************************************************************************\
 *TARGA : to alow loading targa images. output the size of the image, 
 *   exit in case of error because gcc 2.6 don't handle exceptions.
 *In : test if a color is already in the palette
\*****************************************************************************/

class Targa : public ImagePal
{
  public :
  Targa(char * filename);
  
  private :
  int In(byte r, byte g, byte b, int mxscan);
};

/*****************************************************************************\
 *BMP : load bmp images, exit in case of error
\*****************************************************************************/

class Bmp : public ImagePal
{
  public :
  Bmp(char * filename);
};

/*****************************************************************************\
 *Inline methods, and properties
\*****************************************************************************/

inline byte Image::GetPixel(int x, int y) const
{
  assert ((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight));
  return *(fBuffer+x+fWidth*y);
}

inline int Image::Width() const
{
  return fWidth;
}

inline int Image::Height() const
{
  return fHeight;
}

inline int Image::Len() const
{
  return fLen;
}

inline Palette & ImagePal::Pal() const
{
  return fPal;
}

#endif
