/*****************************************************************************\
 *AUTHOR     : Babel Yoann
 *DATE       : 13/04/96
 *TITLE      : object.h , 3d objects
 *OBJECT     : 
 *CURRENTLY  :  
 *BUGS       : 
\*****************************************************************************/

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "scene.h"
#include "zdrawer.h"

typedef Face * PFace;
typedef Face4 * PFace4;


class Object
{
  public :
  Object(int nplots, int nfaces);
  Object(char * filename);
  virtual ~Object();
  void Attach(Scene &) const;
  void Update(const Camera &);
  virtual void Draw(const Camera &, Drawer &, const Vector3 & light) const;
  virtual void QuickDraw(const Camera &,Drawer &, const Vector3 & light) const;
  virtual void SetSize(real newsize);
  virtual void SetColor(byte newcolor);
  virtual void SetShading(byte nbcolors);
  void SetPosition(const Rotation &, const Vector3 &);
  const Rotation & GetRotation() const {return rotation;}
  const Vector3 & GetTranslation() const {return translation;}

  protected :
  void NewFace4(int num_face, int p1, int p2, int p3, int p4,
		byte color, byte nbcolor, int av);
  void NewMapFace4(int num_face, int p1, int p2, int p3, int p4,
		const Image * im, int av);
  void NewFace3(int num_face, int p1, int p2, int p3,
		byte color, byte nbcolor, int av);
  Plot * plots, * dupplots;
  PFace * faces;
  Rotation rotation;
  Vector3 translation;
  int nplots, nfaces;
};

class Cube : public Object
{
  public :
  Cube();
  void SetSize(real newsize);
};

class MappedCube : public Cube
{
  public :
  MappedCube(const Image *);
};

#endif
