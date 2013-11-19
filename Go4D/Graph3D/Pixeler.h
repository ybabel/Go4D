#ifndef _PIXELER_H_
#define _PIXELER_H_

/**
*AUTHOR : Babel yoann
*TITLE  : Pixeler.h, various ways to draw an image
*CREATED: 19/05/99
*DATE   : 19/07/99
*OBJECT : This units provides some basic ways to draw in images. With this
*         classes you can draw easily in an Image, a ZImage, or wathever you
*         want
*/

#include "Graph2D/Drawer.h"

namespace Go4D
{


  /**
  * Use a ZBuffer, but assume that Z is constant for a drawing pass
  */
  class CstZPixeler : public Pixeler
  {
    typedef Pixeler inherited;
  public :

    void SetImage(const Image & image)
    {
      inherited::SetImage(image);
      fStartZ = static_cast<const ZImage *>(fImage)->GetZBuffer();
    }
    inline void SetStart(const PointZ & start)
    {
      inherited::SetStart(start);
      fCurZ=fStartZ+start.x+start.y*fWidth;
      fZ = fconv(start.z);
    }
    inline void Next() { inherited::Next(); fCurZ++; }
    inline void NextL(int L) { inherited::NextL(L); fCurZ+=L; }
    inline void Pixel()
    {
      if (*fCurZ>fZ)
      {
        inherited::Pixel();
        *fCurZ=fZ;
      }
    }
    inline void PixelAt(int x, int y, realtype z)
    {
      if (*(fStartZ+x+y*fWidth)>fZ)
      {
        inherited::PixelAt(x, y, fZ);
        *(fStartZ+x+y*fWidth) = fZ;
      }
    }

  protected :
    realtype * fStartZ;
    realtype * fCurZ;
    realtype fZ;
  };

  /// handle the Z-Buffer drawing
  class ZPixeler : public Pixeler
  {
    typedef Pixeler inherited;
  public :

    void SetImage(const Image & image)
    {
      inherited::SetImage(image);
      fStartZ = static_cast<const ZImage *>(fImage)->GetZBuffer();
    }
    inline void SetStart(const PointZ & start)
    {
      inherited::SetStart(start);
      fCurZ=fStartZ+start.x+start.y*fWidth;
      fZ = fconv(start.z);
    }
    inline void SetEnd(const PointZ & end)
    {
      inherited::SetEnd(end);
      fZdiff = fconv(end.z)-fZ;
    }
    inline void SetSteps(int steps)
    {
      if (steps==0)
        iZ = 0;
      else
        // be carefull, fZDiff is a realtype, ,steps is an integer, no need to
        // use realtype operators !!! (like frapport or rapport)
        iZ = fZdiff/steps;
    }
    inline void Next() { inherited::Next(); fCurZ++; fZ+=iZ;}
    inline void NextL(int L) { inherited::NextL(L); fCurZ+=L; fZ+=iZ; }
    inline void Pixel()
    {
      if (*fCurZ>fZ)
      {
        inherited::Pixel();
        *fCurZ=fZ;
      }
    }
    inline void PixelAt(int x, int y, realtype z)
    {
      if (*(fStartZ+x+y*fWidth)>z)
      {
        inherited::PixelAt(x, y, z);
        *(fStartZ+x+y*fWidth) = z;
      }
    }

  protected :
    realtype * fStartZ;
    realtype * fCurZ;
    realtype fZ;
    realtype fZdiff;
    realtype iZ; // Z increment
  };

  /// TODO : implement this pixeler
  class ShadePixeler : public Pixeler
  {
    typedef Pixeler inherited;
  };

  /**
  * Same a ZPixeler moreover write in an Obj Buffer
  * TODO : implement it
  */
  class ZOPixeler : public ZPixeler
  {
    typedef ZPixeler inherited;
  public :

  };


  /**
  * Same a ZPixeler but pixel near are also drawed
  * Draw BUG pixels, used this drawer to draw "enhanced" thing
  */
  class EnhancedZPixeler : public ZPixeler
  {
  public :
    inline void Pixel()
    {
      if (*fCurZ>fZ)
      {
        *fCurPixel=color;
        *(fCurPixel+1)=color;
        *(fCurPixel-1)=color;
        *(fCurPixel+fWidth)=color;
        *(fCurPixel-fWidth)=color;
        *fCurZ=fZ;
      }
    }
  };

}

#endif
