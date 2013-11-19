/**
*AUTHOR : Babel yoann
*TITLE  : FileImage.h implementation
*CREATED: 22/10/95
*DATE   : 17/01/01
*BUGS   :
*NOTE   : tanks to Rex E. Bradford for his library ARTT
*/

#include "FileImage.h"
#include "ImageFormat.h"
#include <memory.h>

namespace Go4D
{

  /*****************************************************************************\
  * FileImage
  \*****************************************************************************/

  void ImageFile::Swap(Color oldcolor, Color newcolor)
  {
    Color * current = fBuffer;
    for (int i=0; i<fLen; i++, current++)
      if (*current==oldcolor)
        *current = newcolor;
  }

  ImageFile & ImageFile::ImportPalette(const Palette & pal)
  {
    assert(pal.Count()==256);
    assert(fPal.Count()==256);
    for (int i=0; i<fPal.Count(); i++)
      Swap(Color(i), Color(pal.Best(fPal[i])));
    return *this;
  }


  /*****************************************************************************\
  * Pcx
  \*****************************************************************************/

  typedef unsigned char uchar;
  typedef unsigned char ABYTE;
  typedef short AWORD16;
  typedef long AWORD32;

  typedef struct {
    ABYTE manufacturer;     // must be 10
    ABYTE version;          // 0, 2, 3, or 5 (what happned to 1 and 4?)
    ABYTE encoding;         // 1 = run-length image compression, 0 = uncomp.
    ABYTE bitsPerPixel;     // number of bits per pixel per plane
    AWORD16 xmin;           // left edge
    AWORD16 ymin;           // top edge
    AWORD16 xmax;           // right edge
    AWORD16 ymax;           // bottom edge
    AWORD16 hres;           // horizontal resolution in dpi
    AWORD16 vres;           // vertical resolution in dpi
    ABYTE colorMap16[48];   // palette for 16-color images
    ABYTE reserved;         // 0
    ABYTE nplanes;          // number of planes per pixel
    AWORD16 rowbytes;       // number of bytes per line (even)
    AWORD16 paletteInfo;    // 1 = color/bw  2 = grayscale
    AWORD16 screenWidth;    // in pixels
    AWORD16 screenHeight;   // in pixels
    ABYTE pad[54];          // 0
  } PcxHeader;

  Pcx::Pcx(char * filename)
  {
    ifstream pcxfile(filename, ios::binary);

    PcxHeader header;
    pcxfile.read((char *)&header, sizeof(header));

    if (header.manufacturer != 10) error("bad format while reading image manufacturer");
    if (header.version != 5) error("bad format while reading image version");
    if (header.bitsPerPixel != 8) error("bad format while reading image bitsperipxel");
    if (header.nplanes != 1) error("bad format while reading image nplanes");

    fWidth = header.rowbytes;
    fHeight = header.ymax - header.ymin;
    fLen = fWidth * fHeight;
    alloc(fBuffer = new Color[fLen]);

    ReadData(pcxfile, bool(header.encoding!=0) );
    ReadPalette(pcxfile);
  }

  void Pcx::ReadData(istream & is, bool compressed)
  {
    is.seekg(sizeof(PcxHeader),ios::beg);
    Color * p =  fBuffer;
    if (compressed)
    {
      byte * pend = p + fLen;
      while (p<pend)
      {
        byte c;
        short count;
        //assert(!is.eof());
        ReadToken(is, c, count);
        if ((p+count) > pend)
          //error("bad format while reading image");
          return;
        memset(p, c, count);
        p += count;
      }
    }
    else
      is.read((char *)p, fLen);
  }

  void Pcx::ReadToken(istream & is, byte & c, short & count)
  {
    is.read((char *)&c, 1);
    if (0xC0 == (0xC0 & c))
    {
      count = short(c & 0x3F);
      is.read((char *)&c, 1);
    }
    else
      count = 1;
  }

  void Pcx::ReadPalette(istream & is)
  {
    is.seekg(-769, ios::end);
    byte getpal;
    is.read((char *)&getpal, 1);
    if  (getpal == 12)
    {
      byte rgb[256*3];
      is.read((char *)rgb, sizeof(rgb));
      byte * prgb = rgb;
      for (int i=0; i<256; i++, prgb+=3)
      {
        fPal[i].R() = *(prgb+0);
        fPal[i].G() = *(prgb+1);
        fPal[i].B() = *(prgb+2);
      }
    }
  }

  /*****************************************************************************\
  * Bmp
  \*****************************************************************************/

  Bmp::Bmp(char * fileName)
  {
    Load(fileName);
  }

  int Bmp::Load(char* szFileName)
  {
    C_Image aImage;
    aImage.LoadBMP(szFileName);
    fWidth = aImage.Width;
    fHeight = aImage.Height;
    fLen = fWidth * fHeight;
    alloc(fBuffer = new Color[fLen]);
    for (int x=0; x<fWidth; x++)
      for (int y=0; y<fHeight; y++)
        Pixel(x, y, aImage.Pixel(x, y));
    if (aImage.BPP!=24)
    {
      for (int n=0;n< 1<<aImage.BPP;n++)
      {
        fPal[n].R()=aImage.Palette[n].r;
        fPal[n].G()=aImage.Palette[n].g;
        fPal[n].B()=aImage.Palette[n].b;
      }
    }
    else
      error("not done yet");
    return 0;
  }


  /*****************************************************************************\
  * Gif
  \*****************************************************************************/

  Gif::Gif(char * filename)
  {

  }

}
