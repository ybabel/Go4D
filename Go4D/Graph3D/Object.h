#ifndef _OBJECT_H_
#define _OBJECT_H_

/**
*AUTHOR     : Babel Yoann
*CREATED    : 13/04/96
*DATE       : 29/06/96
*TITLE      : object.h , 3d objects
*OBJECT     : baisc object managment + one useful object for tests : cube.
*/

#include "Camera.h"
#include "Face.h"

namespace Go4D
{


  typedef Face * PFace;

  typedef APlot InitPlotCallBack(int aPlotNum, void * UserData);
  typedef PFace InitFaceCallBack(void * UserData);

  enum FileType {ft3D, ftOFF};

  /**
  * OBJECT3D
  * This is an object that can be represented by a list of faces. You can change
  * many parameter dinamycally, like the color of the faces or their shading. You
  * can initialize an object by reading a file where it's vertices and faces are
  * stored. Berfore drawing the 3D object you must update it. You can change it's
  * position and size.
  */
  class Object
  {
  public :
    Object(Image * aImage, int nplots, int nfaces);
    /**
    * if you specife a filename the definition of the object will be read
    *   from that file that must contains the vertices, and the faces. The format
    *   of the file is :
    *   - the object name (80 char max).
    *   - the number of vertices
    *   - all the vertices (3 real)
    *   - the number of faces
    *   - all the faces (the number of edges + each edges <=> a integer poniting on
    *     the list of veritces 1-n).
    */
    Object(Image * aImage, const char * filename, FileType fileType, Image * mapImage=NULL);
    virtual ~Object();
    /// calculate the new position relative to the camera in the space.
    void Update(const Camera &);
    /// draw the object in the image, all the faces a drawn one by one.
    virtual void Draw(const Camera &, const ALight & Light, bool sorted=false) const;
    /// SetSize : change the size of the object (all the vertices are multiplied
    ///  by the parameter).
    virtual void SetSize(real newsize);
    /// SetColor, SetShading : change the color of the color or shading limits. Color
    /// is the basic color (when fully illuminated), shading is the number of
    ///  shaded version of the object. Usually Color+Shading = MultiClut.Count.
    virtual void SetColor(Color newcolor);
    /// SetColor, SetShading : change the color of the color or shading limits. Color
    /// is the basic color (when fully illuminated), shading is the number of
    ///  shaded version of the object. Usually Color+Shading = MultiClut.Count.
    virtual void SetShading(Color nbcolors);
    /// SetPosition, GetRotation, GetTranslation : position manangment.
    void SetPosition(const Rotation &, const Vector3 &);
    /// SetPosition, GetRotation, GetTranslation : position manangment.
    const Rotation & GetRotation() const {return rotation;}
    /// SetPosition, GetRotation, GetTranslation : position manangment.
    const Vector3 & GetTranslation() const {return translation;}

    PFace GetFace(int Index) { return faces[Index]; }
    int FaceCount() { return nfaces; }

  protected :
    virtual void PrepareDraw(const Camera &, const ALight & Light) const;
    virtual void BasicDraw() const;
    virtual void SortedDraw() const;
    // dupplots : a copy of the original plots is stored, new current values
    //   (depending on the camera) are stored in the dupplots
    APlot * plots, * dupplots;
    PFace * faces;
    Rotation rotation;
    Vector3 translation;
    int nplots, nfaces;
    /// fImage : the image where the object is drawed.
    Image * fImage;

  protected:
    void ReadOFFFile(const char * fileName, Image * mapImage);
    void Read3DFile(const char * fileName, Image * mapImage);
  };

}
#endif
