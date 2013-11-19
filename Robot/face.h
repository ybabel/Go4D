/*****************************************************************************\
 *AUTHOR     : Babel Yoann
 *DATE       : 8/11/95, last modification 29/02/96
 *TITLE      : face.h , 3D face management
 *OBJECT     : faces manipulation : different faces (flat shaded mapped)
 *             projecting, cutting, visiblity test, lightening and drawing.
 *CURRENTLY  :  
 *BUGS       : 
\*****************************************************************************/


/*****************************************************************************\
 *
 *     Vector3
 *	 |
 *     Plot
 *
 *              /----------/-- Face ------------------\
 *             /          /     |                      \
 *	   ShadedFace   Face3  Face4   /------AbstractMappedFace-------\
 *  	      |                       /              |                  \
 *     	  ShadedFace4            MappedFace    QuadMappedFace     HypMappedFace
 *     	                             |               |                   |
 *     			       	MappedFace4   QuadMappedFace4    HypMappedFace4
 *                                   |               |                   |
 *                              MappedImage   QuadMappedImage    HypMappedImage
 *
 *
 *		               FaceSet
 *		                  |
 *	                    SortedObject
 *	                          |
 *			   PreSortedObject
 *
\*****************************************************************************/


#ifndef _FACE_H_
#define _FACE_H_

#ifdef LIBGPP
#include <iostream.h>
#endif

#include "geom.h"
#include "drawer.h"

/*****************************************************************************\
 *PLOT : this is the basic plot for 3d, it can be projected and has a color.
 *  There is 2 ways to calculate the color, you can set it normaly (SetColor)
 *  or add a color to the current color (an average is performed with the old
 *  colors). The later is useful for gouraud shading (the color of a plot is
 *  automatically processed).
\*****************************************************************************/

class Plot : public Vector3
{
  public :
  Plot():Vector3()
    {px=0; py=0; pz=0; processed=0; color=0; nb=0; sum=0;}
  Plot(real a, real b, real c):Vector3(a,b,c)
    {px=0; py=0; pz=0; processed=0; color=0; nb=0; sum=0;}
  Plot(const Plot &p):Vector3(p)
    {px=0; py=0; pz=0; processed=0; color=0; nb=0; sum=0;}
  Plot(const Matrix &m):Vector3(m)
    {px=0; py=0; pz=0; processed=0; color=0; nb=0; sum=0;}
  void ProjectionParallele(int translateX, int translateY, 
			   real zoom, real ladder);
  void ProjectionPerspective(real focal, 
			     int translateX, int translateY,
			     real zoom, real ladder);
  inline int GetX() const {return px;}  //properties read-only
  inline int GetY() const {return py;}
  inline real GetZ() const {return pz;}
  inline byte GetColor() const {return color;}
  inline void SetColor(byte col){color=col;}
  void AddColor(byte c) {sum+=c; nb++; color=(byte)(sum/nb);}
  PointZ GetPointZ() const;

#ifdef LIBGPP
  friend ostream & operator << (ostream &, const Plot &);
#endif

  private :
  byte color, nb;
  int sum;
  int px, py;  //projection  read-only
  real pz;
  int processed;
};

/*****************************************************************************\
 *FACE : becarful that the plots of the face are coplanear, 
 *otherwise it will not bug, but the result is imprevisible
 *Update :
 *  When the plots have changed (remember that the class only
 *  points on plots), call this function to claculate the new values
 *  The plots (their projection, their color) are calculated only once
 *  (cf Plot class) when the projection of the world is calculated once again.
 *AddLight : 
 *  Add a light source, the color of the face is calulated one again.
 *SlitRight :
 *  Eliminate the part of the face that is at the left of the plane. Modifies
 *  visible (if all the face is at the left-most part of the plane the face
 *  is not visible anymore).
 *Project :
 *  Calculate the projection of the points, this method must be called before
 *  Draw. The plots a automatically translated to the middle of the drawer.
 *Draw : 
 *  draw the face in the drawer, assume that Update, Addlight, SplitRight,
 *  Project had been done before.
 *BasicDraw : make all the necessary operations and draw the face.
 *DrawShape : only draw the edges, don't fill.
 *Visible : Determine if the face is visble or not.
 *ConvexTest : look if the face is viewed by the eye (0,0,-focal).
 *EntirelyOffScreen : the face is visible but is projection is out of the 
 *  screen : eliminate it
 *SetColor : set the current color.
 *SetBaseColor : modifie colormin (use by AddLight)
 *BuildPlane : calulate the plane on wich lies the face (necessary for some 
 *  hidden face removal algo).
 *Normal : return the  normal to this plane (always calculated
\*****************************************************************************/

typedef Plot * PPlot;

class Face // flat face
{
  public :
  Face(byte colormin, byte nbcolors, int nbpoints, int alwaysvisible=TRUE);
  virtual ~Face();
  void Update(real focal);
  virtual void AddLight(const Vector3 & light);
  virtual void SplitLeft(const Plane &);
  virtual void SplitRight(const Plane &);
  void Project(Drawer &, real focal, real zoom);
  virtual void Draw(Drawer &);
  void BasicDraw(Drawer &, real focal, real zoom);
  void BasicDraw(Drawer &, real focal, real zoom, const Vector3 & light);
  void DrawShape(Drawer &);
  int Visible() const {return visible;}
  int ConvexTest(real focal) const;
  int EntirelyOffScreen(const Drawer &);
  void SetColor(byte color);
  void SetBaseColor(byte color) {colormin=color;}
  void SetShading(byte shading) {nbcolors=shading;}
  Plane & BuildPlane();
  PPlot & operator [] (int) const;
  real GetZ() const {return z;}
  const Vector3 & Normal() const {return normal;}

  protected :
  void PurgeCutting();
  int Cut(const Plane &, int &, int &, int &, real &, real &, int &);
  int Behind(const Face &);
  int InFront(const Face &);
  int npoints;
  PPlot * edges;
  PointZ * points;
  real z;
  Vector3 normal;
  byte color;
  byte colormin, nbcolors;
  int alwaysvisible;
  int visible;
  int cutted;
  Face * right, * left; // internal faces for cutting
  Plot first, second; //internal points for cutting
  real * dists;
  Plane plane;
};

class ShadedFace : public Face
{
  public :
  ShadedFace(byte colormin, byte nbcolors, int nbpoints, int alwayvisible=TRUE)
  :Face(colormin, nbcolors, nbpoints, alwayvisible) {}
  void AddLight(const Vector3 &light);
  void Draw(Drawer &);
  void SplitRight(const Plane &);
};

class AbstractMappedFace : public Face
{
  public :
  AbstractMappedFace(const Image * ima, int nbpoints, int alwaysvisble=TRUE);
  virtual ~AbstractMappedFace();
  void AddLight(const Vector3 &light);
  void Draw(Drawer &)=0;
  void SetMapPoint(int i, int x, int y);

  protected :
  MapPoint * mpoints, * dupmpoints;
  const Image * ima;
};

class MappedFace : public AbstractMappedFace
{
  public :
  MappedFace(const Image * ima, int nbpoints, int av=TRUE)
    :AbstractMappedFace(ima,nbpoints,av){}
  void Draw(Drawer &);
  void SplitRight(const Plane &);
};

class QuadMappedFace : public AbstractMappedFace
{
  public :
  QuadMappedFace(const Image * ima, int nbpoints, int av=TRUE)
    :AbstractMappedFace(ima,nbpoints,av){}
  void Draw(Drawer &);
  void SplitRight(const Plane &);
};

class HypMappedFace : public AbstractMappedFace
{
  public :
  HypMappedFace(const Image * ima, int nbpoints, int av=TRUE)
    :AbstractMappedFace(ima,nbpoints,av){}
  void Draw(Drawer &);
  void SplitRight(const Plane &);
};


class Face4 : public Face
{
  public :
  Face4(Plot &, Plot &, Plot &, Plot &,
	byte colormin, byte nbcolors, int alwayvisible=TRUE);
};


class Face3 : public Face
{
  public :
  Face3(Plot &, Plot &, Plot &,
	byte colormin, byte nbcolors, int alwayvisible=TRUE);
};


class ShadedFace4 : public ShadedFace
{
  public :
  ShadedFace4(Plot &, Plot &, Plot &, Plot &, 
	      byte colormin, byte nbcolors, int alwaysvisible=TRUE);
};

class MappedFace4 : public MappedFace
{
  public :
  MappedFace4(Plot&, Plot&, Plot&, Plot&,
    const Image *, int xb, int yb, int xe, int ye,
    int alwayvisible=TRUE);
};

class MappedImage : public MappedFace4
{
  public :
  MappedImage(Plot &, Plot &, Plot &, Plot &, const Image *,
	  int alwaysvisible=TRUE);
};

class QuadMappedFace4 : public QuadMappedFace
{
  public :
  QuadMappedFace4(Plot&, Plot&, Plot&, Plot&,
    const Image *, int xb, int yb, int xe, int ye,
	  int alwayvisible=TRUE);
};

class QuadMappedImage : public QuadMappedFace4
{
  public :
  QuadMappedImage(Plot &, Plot &, Plot &, Plot &, const Image *,
		int alwaysvisible=TRUE);
};

class HypMappedFace4 : public HypMappedFace
{
  public :
  HypMappedFace4(Plot&, Plot&, Plot&, Plot&,
		const Image *, int xb, int yb, int xe, int ye,
		int alwayvisible=TRUE);
};

class HypMappedImage : public HypMappedFace4
{
  public :
  HypMappedImage(Plot &, Plot &, Plot &, Plot &, const Image *,
		int alwaysvisible=TRUE);
};

#endif


