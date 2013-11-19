#ifndef _SHADER_H_
#define _SHADER_H_

/**
*AUTHOR : Babel yoann
*TITLE  : Shader.h, add illumination to your faces
*CREATED: 19/05/99
*DATE   : 19/05/99
*OBJECT : Theses classes can perform shading (or even transparency) during the
*         face drawing process.
*/

#include "TemplateFaceDrawer.h"

namespace Go4D
{

  /**
  * SHADER
  */
  class Shader : public Filler
  {
    typedef Filler inherited;
  };

  /**
  * NULLSHADER
  */
  class NullShader : public Shader
  {
    typedef Shader inherited;
  protected :

  public :
  };

  /**
  * FLATSHADER
  */
  class FlatShader : public Shader
  {
    typedef Shader inherited;
  protected :
    mutable Color shade;

  public :
    /// 
    void Init(const Image * image, const FaceDrawerData * data)
    {
      inherited::Init(image, data);
    }
    /// 
    inline void FillInitDrawing(int npoints, PointZ * points) const
    {
      inherited::FillInitDrawing(npoints, points);
      shade = points[0].color;
    }
    inline Color HorizShadePixel(Color color, int offs) const { return Color(shade+color); }
  };

  /**
  * MULTICLUTSHADER
  */
  class MultiClutShader : public Shader
  {
    typedef Shader inherited;
  protected :
    mutable const MultiClut* fMultiClut;
    mutable Color shade;

  public :
    /// 
    void Init(const Image * image, const FaceDrawerData * data)
    {
      inherited::Init(image, data);
      if (data==NULL) return;
      fMultiClut = static_cast<const DrawerImageClutData *>(data)->fMultiClut;
    }
    /// 
    inline void FillInitDrawing(int npoints, PointZ * points) const
    {
      inherited::FillInitDrawing(npoints, points);
      shade = points[0].color;
    }
    inline Color HorizShadePixel(Color color, int offs) const { return (*fMultiClut)(shade, color); }
  };

  /// Data for multiclutimagehsader
  class DrawerBumpData : public DrawerImageClutData
  {
    typedef DrawerImageClutData inherited;

  public :
    /// 
    const Image * fShadeImage;
    /// 
    DrawerBumpData(const Image *, const MultiClut *, const Image *);
  };

  /**
  * MULTICLUTIMAGESHADER
  */
  class MultiClutImageShader : public Shader
  {
    typedef Shader inherited;
  protected :
    mutable const MultiClut* fMultiClut;
    Color * fShadeBuf;

  public :
    /// 
    void Init(const Image * image, const FaceDrawerData * data)
    {
      inherited::Init(image, data);
      if (data==NULL) return;
      fMultiClut = static_cast<const DrawerImageClutData *>(data)->fMultiClut;
      fShadeBuf = static_cast<const DrawerBumpData *>(data)->fShadeImage->GetBuffer();
    }
    inline Color HorizShadePixel(Color color, int offs) const
    { return (*fMultiClut)(*(fShadeBuf+offs), color); }
  };

}

#endif
