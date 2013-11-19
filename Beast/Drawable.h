#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include "Geom.h"
#include "Image.h"
#include "Camera.h"
#include "TemplateFaceDrawer.h"
#include "Plotter.h"
#include "Shader.h"
#include "Mapper.h"
#include "List.h"

using namespace Go4D;

enum DrawableShape {dsTriangle, dsSquare, dsSquareMapped};

/**
* A class must support this interface if it want to be displayed by the renderer.
*/
class IDrawable
{
public:
  /// return the position where must be displayed the object
  virtual Vector3 * GetPosition() =0;
  /// return the direction or orientation of the object
  virtual Vector3 * GetDirection() =0;
  /// return the color of the object (if it have one)
  virtual Color GetColor() =0;
  /// return the shape of the object (it can be a square or a triangle for example)
  virtual DrawableShape GetShape() =0;
  /// return the size of the object
  virtual real GetSize() =0;
};

/**
* This interface must be supported by objects that are texture mapped
*/
class IDrawableMapped : public IDrawable
{
public:
  /// return the image that must be mapped on the faces of the object
  virtual const Image * GetMappedImage()=0;
};

/**
* This class performs the drawing of all the drawables that are registered
* The drawing is done in 3D (it depend of the camera point of view).
* USE : add all the objects that must be drawed by the rendered ONE time
*   (by using the AddDrawable method). Each time you call Draw, all the
*   registered object will be drawed, according to their position and size.
*/
class Renderer
{
public:
  /// the renderer will alway draw graphics in the image given in parameter
  Renderer(Image *);
  ///
  ~Renderer();
  /// clear the list of registered objects that must be drawed
  void Clear();
  /// add an object that must be drawed each time Draw is called. the object is registered
  void AddDrawable(IDrawable * );
  /// remove an object from the list of registered objects
  void DelDrawable(IDrawable * );
  /// draw all the objects in the image (cf constructor) according to the point of view of the camera
  void Draw(Camera *);

protected:
  Array<IDrawable *> fDrawables; // not aggregated list
  FaceDrawer<ImagePlotter, FlatShader, NullMapper> faceDrawer;
  //Obsolete::HypperbolicMappedFaceDrawer mappedFaceDrawer;
  MappedFaceDrawer<ImagePlotter, FlatShader, QuadraticMapper> mappedFaceDrawer;
  //MappedFaceDrawer<ImagePlotter, FlatShader, LinearMapper> mappedFaceDrawer; // YBA2013
  void ConvertInPlot(IDrawable *, APlot &, APlot &, APlot &, APlot &);
};

//---------------------------------------------------------------------------
#endif
