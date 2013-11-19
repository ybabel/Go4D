#ifndef _IMAGE_H_
#define _IMAGE_H_

/*
*AUTHOR : Babel yoann
*TITLE  : image.h, basic images operations
*CREATED: 22/10/95
*DATE   : 08/03/96
*OBJECT : 256 color images (8 bits) only
*/

#include "Tools/Common.h"

namespace Go4D
{

  ///  Color type ! byte=256 colors, Word=65536 colors
  typedef byte Color;

  /**
  * POINTZ :
  *  This class is used to perform drawings on images. It hace an x,y coordinates
  *  but it also have a Z coordinate and a color. The PointZ identifes the
  *  edges of the drawing (the bounds of lines, or the center of a circle for
  *  example).
  *
  *  This class is also used to get the result of the projection of a 3D plot
  *  in the camera that is used to view the scene. A PointZ is not the same
  *  thing than a APlot, since the coordinates a stored in device coordinates.
  *  That means that 0,0 is the top left corner of the screen.
  */
  class PointZ
  {
  public :
    ///
    int x;
    ///
    int y;
    ///
    real z;
    ///
    Color color;

  public :
    ///
    PointZ & SetZ(real newz) { z=newz; return *this; }
    ///
    PointZ(){x=0; y=0; z=0; color=0;}
    ///
    PointZ(int px, int py, Color pc, real pz=0) // Mini = always visible
    {x=px; y=py; z=pz; color=pc;}
    ///
    void Decal(int dy) {y+=dy;}
  };

  /**
  *IMAGE :
  *   linear buffer, 1 octect, 1 pixel. the device class can output an
  *   image on the screen
  *   this class is con just handle image (load form disk, drawing)
  */
  class Image
  {
    friend class Device;

  protected :
    int fWidth;
    int fHeight;
    int fLen;
    Color * fBuffer;
    bool fOwner;
#ifdef DEBUGOUT
    virtual ostream & DebugOut(ostream &);
#endif

  public :
    /** Image : the constructor will allocate the necessary space in memory
    * (siwex*sizey octects) if the flag autoalloc is set. In the other case
    * fBuffer will be set to NULL. Becarefull to set manually (using NewBuffer)
    * the value of fBuffer. fBuffer can also be intialized in a
    * child-class-constructor.
    */
    Image(int Width, int Height, bool AutoAlloc=true);
    ///  ~Image : if the buffer had not change, free the memory.
    virtual ~Image();
    /** NewBuffer : this method had been created to allow the use of
    * shared memory under XWindow (or DIB under win95)
    * you don't have to use it, the DEVICE class will.
    * the class is no more the owner of the buffer.
    */
    void NewBuffer(Color * NewBuffer);
    /** Resize : if the buffer had been re-allocated (NewBuffer), you can resize it.
    * You will loose all the old datas. If itsn't the case only Width, Height,
    * Len, MiddleX, MiddleY will change, and you will need to resize the buffer.
    */
    virtual void Resize(int Width, int Height);
    ///  Plug : All the operations you do on this image will be reported on the plugged one. The image existing before is destroyed.
    ///  allowing a Image& instead of a Image* guarant that it's non NULL !
    virtual void Plug(const Image & );
    ///  Pixel : set the color of the pixel at the specified position
    void Pixel(int x, int y, Color color);
    ///  ClipPixel : same thing then Pixel but verifie that the pixel is inside the image. If it's not the case, don"t do anything
    void ClipPixel(int x, int y, Color color);
    ///  GetPixel : get the color of the pixel at the specified position, that must be inside the image
    Color GetPixel(int x, int y) const;
    ///  GetClipPixel : same thing but return an error if the pixel is not inside the image
    Color GetClipPixel(int x, int y) const;
    ///  set all the images pixels to the same color (0 by deault)
    virtual void ClearAll(Color color=0);
    ///  Width, Height, Len : image dimensions, len = width*height
    int Width() const;
    ///  Width, Height, Len : image dimensions, len = width*height
    int Height() const;
    ///  Width, Height, Len : image dimensions, len = width*height
    int Len() const;
    ///  MiddleX, MiddleY : return the coordinates of the screen's middle (use for projection)
    int MiddleX() const {return fWidth/2;}
    ///  MiddleX, MiddleY : return the coordinates of the screen's middle (use for projection)
    int MiddleY() const {return fHeight/2;}
    ///  return an internal pointer to the buffer
    Color * GetBuffer() const { return fBuffer; }
  };


  /**
  *ZIMAGE :
  *  This class is the same than an Image, but it adds a ZBuffer. A z coordinate
  *  stored as a realtype (fixed/float) is associated to each pixel of the screen
  *
  *  NOTE : the box method is here for historic reasons only and should be
  *    removed soon
  */
  class ZImage : public Image
  {
    typedef Image inherited;

  public:
    ///
    ZImage(int Width, int Height, bool AutoAlloc=true, bool AutoAllocZ=true);
    ///  change also the ZBuffer
    void NewZBuffer(realtype * NewZBuffer);
    ///  use the ZBuffer of the plugged image
    void PlugZ(const ZImage & );
    ///
    virtual ~ZImage();
    ///  return the ZBuffer (for fast access)
    realtype * GetZBuffer() const { return fZBuffer; }
    ///
    virtual void ClearAll(Color color=0);
    ///
    void ClearAllZ(realtype z=Maxi);
    ///
    void Box(const PointZ & p, int w, int h);
    ///
    realtype GetZ(int x, int y) const;

  protected:
    int ClipBox(const PointZ &p, int &x, int &y, int &w, int &h);
    realtype * fZBuffer;
  };

  /*****************************************************************************\
  *Inline methods, and properties
  \*****************************************************************************/

  inline void Image::Pixel(int x, int y, Color color)
  {
    assert((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight));
    *(fBuffer+x+fWidth*y) = color;
  }

  inline void Image::ClipPixel(int x, int y, Color color)
  {
    if((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight))
      *(fBuffer+x+fWidth*y) = color;
  }

  inline Color Image::GetPixel(int x, int y) const
  {
    assert((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight));
    return *(fBuffer+x+fWidth*y);
  }

  inline Color Image::GetClipPixel(int x, int y) const
  {
    if (!((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight)))
      error("Pixel ouside the image");
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

  inline realtype ZImage::GetZ(int x, int y) const
  {
    assert((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight));
    return *(fZBuffer+x+fWidth*y);
  }

}

#endif


