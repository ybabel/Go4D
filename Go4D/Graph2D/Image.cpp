/**
*AUTHOR : Babel yoann
*TITLE  : image.cpp, image.h implementation
*CREATED: 22/10/95
*DATE   : 29/06/96
*BUGS   :
*/

#include "Image.h"
#include <memory.h>

namespace Go4D
{


  /*****************************************************************************\
  *   Image
  \*****************************************************************************/

  Image::Image(int aWidth, int aHeight, bool AutoAlloc)
  {
    assert( (aWidth>0 && aHeight>0) || (!AutoAlloc) );
    // set image dimension
    fWidth = aWidth;
    fHeight = aHeight;
    fLen = fWidth * fHeight;
    //allocated memory for buffer
    if (AutoAlloc)
    {alloc(fBuffer = new Color[fLen]);} //!!! alloc is a macro with an if inside
    else
      fBuffer = NULL;
    fOwner = true;
  }

  Image::~Image()
  {
    // if necessary free the memory
    if (fOwner)
    {
      assert(fBuffer != NULL);
      delete [] fBuffer;
    }
  }

#ifdef DEBUGOUT
  ostream & Image::DebugOut(ostream & os)
  {
    os << "Image" << incendl
      << "{" << iendl
      << "Width:" << fWidth << iendl
      << "Height:" << fHeight << iendl
      << "Len:" << fLen << iendl
      << "Buffer:" << int(fBuffer) << iendl
      << "Owner:" << fOwner << iendl
      << "}" << decendl;
    return os;

  }
#endif

  void Image::NewBuffer(Color * aBuffer)
  {
    // detroy the old buffer, set fBuffer to the one passed in parameter
    if (fOwner && fBuffer!=NULL)
      delete [] fBuffer;
    fBuffer = aBuffer;
    fOwner = false;
  }

  void Image::Resize(int aWidth, int aHeight)
  {
    assert(fBuffer != NULL);
    if (fOwner) delete [] fBuffer;
    assert(aWidth>0 && aHeight>0);
    // set image dimension
    fWidth = aWidth;
    fHeight = aHeight;
    fLen = fWidth * fHeight;
    //allocated memory for buffer
    if (fOwner) { alloc(fBuffer = new Color[fLen]); }
  }

  void Image::Plug(const Image & aImage)
  {
    assert(aImage.fBuffer != NULL);
    NewBuffer(aImage.fBuffer);
    Resize(aImage.fWidth, aImage.fHeight);
  }

  void Image::ClearAll(Color aColor)
  {
    memset(fBuffer, aColor, fLen);
  }

  /*****************************************************************************\
  *   ZImage
  \*****************************************************************************/

  ZImage::ZImage(int Width, int Height, bool AutoAlloc, bool AutoAllocZ)
    : inherited(Width, Height, AutoAlloc)
  {
    if (AutoAllocZ)
    {
      alloc(fZBuffer = new realtype[fLen]); //!!! alloc is a macro with an if inside
    }
    else
      fZBuffer = NULL;
  }

  void ZImage::NewZBuffer(realtype * aZBuffer)
  {
    // detroy the old buffer, set fBuffer to the one passed in parameter
    if (fOwner && fZBuffer!=NULL)
      delete [] fZBuffer;
    fZBuffer = aZBuffer;
    fOwner = false;
  }

  void ZImage::PlugZ(const ZImage & aImage)
  {
    assert(aImage.fBuffer != NULL);
    assert(aImage.fZBuffer != NULL);
    NewBuffer(aImage.fBuffer);
    NewZBuffer(aImage.fZBuffer);
    Resize(aImage.fWidth, aImage.fHeight);
  }

  void ZImage::ClearAll(Color aColor)
  {
    inherited::ClearAll(aColor);
    ClearAllZ();
  }

  void ZImage::ClearAllZ(realtype z)
  {
    for (int j=0; j<fLen; j++)
      fZBuffer[j] = z;
  }

  ZImage::~ZImage()
  {
    // if necessary free the memory
    if (fOwner)
    {
      assert(fZBuffer != NULL);
      delete [] fZBuffer;
    }
  }

  int ZImage::ClipBox(const PointZ &p, int &x, int &y, int &w, int &h)
  {
    x = p.x;
    y = p.y;

    if (x >= fWidth) return true;
    if (y >= fHeight) return true;
    if (x+w < 0) return true;
    if (y+h < 0) return true;

    if (x+w >= fWidth) w = fWidth-1-x;
    if (y+h >= fHeight) h = fHeight-1-y;
    if (x < 0) {w-=(0-x); x=0;}
    if (y < 0) {h-=(0-y); y=0;}
    /*
    if (x >= xmax) return TRUE;
    if (y >= ymax) return TRUE;
    if (x+w < xmin) return TRUE;
    if (y+h < ymin) return TRUE;

    if (x+w >= xmax) w = xmax-1-x;
    if (y+h >= ymax) h = ymax-1-y;
    if (x < xmin) {w-=(xmin-x); x=xmin;}
    if (y < ymin) {h-=(ymin-y); y=ymin;}
    */
    return false;
  }

  void ZImage::Box(const PointZ & p, int w, int h)
  {
    Color color = p.color;
    int x, y;
    if (ClipBox(p,x,y,w,h)) return;
    Color * ptr = fBuffer+x+fWidth*y;

    realtype * zptr = fZBuffer+x+fWidth*y;
    realtype z = fconv(p.z);
    for (int j=0; j<h; j++)
    {
      for (int i=0; i<w; i++, ptr++, zptr++)
        if (z<*zptr)
        {
          *ptr = color;
          *zptr = z;
        }
        ptr += fWidth-w;
        zptr += fWidth-w;
    }
  }


}
