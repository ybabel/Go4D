#ifndef _PLOTTER_H_
#define _PLOTTER_H_

/*
*AUTHOR : Babel yoann
*TITLE  : Plotter.h
*CREATED: 19/05/99
*DATE   : 19/05/99
*OBJECT : Here are defined Plotter that are used to draw 3D faces in an image.
*         You can used them to draw faces in images or ZImage or other images.
*/

#include "TemplateFaceDrawer.h"

namespace Go4D
{


  /**
  * PLOTTER
  *  This plotter allow to draw faces
  */
  class Plotter : public Filler
  {
    typedef Filler inherited;
  };


  /**
  * IMAGEPLOTTER
  *  This plotter allow to draw faces in an Image
  */
  class ImagePlotter : public Plotter
  {
    typedef Plotter inherited;
  protected :
    mutable Color * Cur;
    Color * fBuffer;

  public :
    /// 
    void Init(const Image * image, const FaceDrawerData * data)
    {
      inherited::Init(image, data);
      fBuffer = image->GetBuffer();
    }
    ///  horiz methods
    inline void HorizInitPixel(int offs) const { Cur = fBuffer+offs; }
    inline void HorizPutPixel(Color color) const { *Cur = color; }
    inline void HorizNextPixel() const { Cur++; }
  };

  /**
  * ZIMAGEPLOTTER
  *  This plotter allow to draw faces in an ZImage. It implements the ZBuffer
  *  alogrithm : a point is draw with it's color and it's depth (Z axis)
  *  only if the previous drawed point have a < Z
  */
  class ZImagePlotter : public Plotter
  {
    typedef Plotter inherited;
  protected :
    mutable Color * CurBuf;
    mutable realtype * CurBufZ;
    Color * fBuffer;
    realtype * fZBuffer;
    int fLength;
    mutable realtype cur_z, incr_z;

  public :
    /// 
    void Init(const Image * aImage, const FaceDrawerData * data)
    {
//#ifndef UNDER_CE
//      const ZImage * aZImage = dynamic_cast<const ZImage *>(aImage);
//      // !!! BECAREFUL dynamic_cast should be used
//#else
      const ZImage * aZImage = reinterpret_cast<const ZImage *>(aImage);
//#endif
      inherited::Init(aImage, data);
      fBuffer = aImage->GetBuffer();
      fZBuffer = aZImage->GetZBuffer();
      fLength = aImage->Len();
    }
    ///  horiz methods
    inline void HorizCoefInit(realtype xb, realtype xe, realtype zb, realtype ze) const
    {
      incr_z = frapport( (ze-zb), (xe-xb) );
      cur_z = zb;
    }
    /// 
    inline void HorizClip(int xmin, int rxb) const
    { cur_z += incr_z*(xmin-rxb); }
    /// 
    inline void HorizInitPixel(int offs) const
    { CurBuf = fBuffer+offs; CurBufZ = fZBuffer+offs; }
    inline void HorizPutPixel(Color color) const
    {
      assert(int(CurBuf-fBuffer)>=0);
      assert(int(CurBuf-fBuffer)<fLength);
      if (cur_z< *CurBufZ)
      {
        *CurBuf = color;
        *CurBufZ = cur_z;
      }
    }
    /// 
    inline void HorizNextPixel() const { CurBuf++; CurBufZ++; cur_z+=incr_z; }
  };


}

#endif
