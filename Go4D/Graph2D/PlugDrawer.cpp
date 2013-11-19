/*****************************************************************************\
*AUTHOR : Babel yoann
*TITLE  : PluggedDrawer.cpp, PluggedDrawer.h implementation
*CREATED: 13/07/96
*DATE   : 13/07/96
*BUGS   : The plug call in the constructor don't call plug as a virtual
*         function
\*****************************************************************************/

#include "PlugDrawer.h"

namespace Go4D
{

  /*****************************************************************************\
  * PluggedDrawer
  \*****************************************************************************/

  PluggedDrawer::PluggedDrawer(const Image & aImage)
    : inherited(aImage.Width(), aImage.Height(), false)
  {
    ///!!!! it seems that it call Plug as a static function instead of a virtual
    /// one !!! I don't known why.... but it cause some bugs
    Plug(aImage);
  }

  PluggedDrawer::PluggedDrawer(int Width, int Height, bool autoalloc)
    : inherited(Width, Height, autoalloc)
  {
  }

  void PluggedDrawer::SetClipZone(int clip_xmin, int clip_ymin, int clip_width, int clip_height)
  {
    xmin = clip_xmin;
    ymin = clip_ymin;
    xmax = clip_xmin+clip_width;
    ymax = clip_ymin+clip_height;
  }

  int PluggedDrawer::GetClipXMin() const
  {
    return xmin;
  }

  int PluggedDrawer::GetClipYMin() const
  {
    return ymin;
  }

  int PluggedDrawer::GetClipWidth() const
  {
    return xmax-xmin;
  }

  int PluggedDrawer::GetClipHeight() const
  {
    return ymax-ymin;
  }

}
