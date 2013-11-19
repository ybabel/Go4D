#ifndef _PLUGGED_DRAWER_H_
#define _PLUGGED_DRAWER_H_

/**
*AUTHOR : Babel yoann
*TITLE  : PluggedDrawer.h : all routines to draw in an image
*CREATED: 13/07/96
*DATE   : 13/07/96
*OBJECT : this modules is the basis of all drawing routines, like all the
*          FaceDrawer hierarchie.
*/

#include "Image.h"

namespace Go4D
{


  /**
  * PLUGGEDDRAWER
  *   A PluggedDrawer is an Image that has not it's own image, it's plugged on a
  *   image already existing, all the drawings done on this class are made on
  *   image on wich the plugged drawer is plugged. This class is mainly the basis
  *   for all the FaceDrawer hierarchie. But there are also other drawing classes
  *   that inherits this class.
  *
  * PluggedDrawer : the constructor is most simple, just give it the image on
  *   wich the class is plugged.
  */
  class PluggedDrawer : public Image
  {
    typedef Image inherited;
  public :
    /// 
    PluggedDrawer(const Image & );
    /// 
    PluggedDrawer(int Width, int Height, bool autoalloc);
    ///
    void SetClipZone(int clip_xmin, int clip_ymin, int clip_width, int clip_height);
    ///
    int GetClipXMin() const;
    ///
    int GetClipYMin() const;
    ///
    int GetClipWidth() const;
    ///
    int GetClipHeight() const;

  protected :
    //clipping zone
    int  xmin, xmax, ymin, ymax;
  };

}

#endif
