#ifndef _FACE_H_
#define _FACE_H_

/*
*AUTHOR     : Babel Yoann
*CREATION   : 14/07/96
*DATE       : 14/07/96
*TITLE      : face.h , 3D face management
*OBJECT     : faces manipulation : different faces (flat shaded mapped bump)
*             projecting, cutting, visiblity test, lightening and drawing.
*/

#include "Tools/Geom.h"
#include "FaceDrawer.h"
#include "Plot.h"
#include "Light.h"
#include "Camera.h"

namespace Go4D
{


  typedef APlot * PPlot;

  /**
  * AFACE
  *   Abstract face.
  */
  class Face
  {
  public :
    Face(AFaceDrawer *, Color colormin, Color nbcolors, int nbpoints,
      bool alwaysvisible=true);
    ///
    Face(AFaceDrawer *, APlot &, APlot &, APlot &, APlot &,
      Color colormin, Color nbcolors, bool alwayvisible=true);
    virtual ~Face();
    ///
    virtual void BeginDraw() {}
    ///
    void Update(const Camera &);
    ///
    virtual void Lightning();
    ///
    virtual void AddLight(const ALight &);
    ///
    virtual void Lightned();
    ///
    virtual void SplitRight(const Plane &);
    ///
    virtual void Project(const Camera &);
    ///
    void Draw();
    ///
    void SimpleDraw(const Camera &);
    ///
    void SimpleDraw(const Camera &, const ALight & Light);
    ///
    bool Visible() const {return visible;}
    ///
    bool ConvexTest(const Camera &) const;
    ///
    bool EntirelyOffScreen(const Camera &);
    void SetColor(Color color);
    void SetBaseColor(Color color) {colormin=color;}
    void SetShading(Color shading) {nbcolors=shading;}
    Plane & BuildPlane();
    ///
    PPlot & operator [] (int) const;
    ///
    const Vector3 & Normal() const {return normal;}
    ///
    real GetMinZ() const;
    ///
    real GetMaxZ() const;
    ///
    real GetAverageZ() const;

  protected :
    virtual void Update();
    virtual Face * SplitClone(int nb_points);
    virtual void SplitExtrem(Face * NewFace, int first_positive, int first_negative,
      int nb_positive, int first_sign, real first_coord, real second_coord);
    void PurgeCutting();
    bool Cut(const Plane &, int &, int &, int &, real &, real &, int &);
    bool Behind(const Face &);
    bool InFront(const Face &);
    virtual void BasicDraw();
    AFaceDrawer * fFaceDrawer; // owner
    int npoints;
    PPlot * edges;
    PointZ * points;
    Vector3 normal;
    Color colormin, nbcolors;
    bool alwaysvisible;
    bool visible;
    bool cutted;
    Face * right, * left; // internal faces for cutting
    APlot first, second; //internal points for cutting
    real * dists;
    Plane plane;

  private :
    void Init();
  };

  /// a face draw with a constant color
  class FlatFace : public Face
  {
    typedef Face inherited;
  protected :
    APlot fCenter;
    virtual void Update();
    virtual void AddLight(const ALight &);
    virtual void BasicDraw();
    virtual Face * SplitClone(int nb_points);

  public :
    FlatFace(AFaceDrawer *, Color colormin, Color nbcolors, int nbpoints,
      bool alwaysvisible=true);
    ///
    FlatFace(AFaceDrawer *, APlot &, APlot &, APlot &, APlot &,
      Color colormin, Color nbcolors, bool alwayvisible=true);
  };

  /// a face draw with a constant color that depend on the current light
  class FlatShadedFace : public Face
  {
    typedef Face inherited;
  protected :
    int * fColors;
    virtual void Lightning();
    virtual void Lightned();
    virtual void BasicDraw();
    virtual Face * SplitClone(int nb_points);

  public :
    FlatShadedFace(AFaceDrawer *, Color colormin, Color nbcolors, int nbpoints,
      bool alwaysvisible=true);
    ///
    FlatShadedFace(AFaceDrawer *, APlot &, APlot &, APlot &, APlot &,
      Color colormin, Color nbcolors, bool alwayvisible=true);
    virtual ~FlatShadedFace();
  };

  /// a gouraud shaded face, based on a constant color
  class GouraudFace : public Face
  {
    typedef Face inherited;
  protected :
    virtual void BasicDraw();
    virtual Face * SplitClone(int nb_points);

  public :
    GouraudFace(AFaceDrawer *, Color colormin, Color nbcolors, int nbpoints,
      bool alwaysvisible=true);
    GouraudFace(AFaceDrawer *, APlot &, APlot &, APlot &, APlot &,
      Color colormin, Color nbcolors, bool alwayvisible=true);
  };

  /// an environment mapped face. Can be used to simulated phong shading
  class EnvMapFace : public Face
  {
    typedef Face inherited;
  protected :
    void SetEnvMapPoint(int i, const ALight &);
    virtual void BeginDraw();
    virtual void Update();
    virtual void AddLight(const ALight &);
    virtual Face * SplitClone(int nb_points);

  public :
    EnvMapFace(AFaceDrawer *, Color colormin, Color nbcolors, int nbpoints,
      bool alwaysvisible=true);
    EnvMapFace(AFaceDrawer *, APlot &, APlot &, APlot &, APlot &,
      Color colormin, Color nbcolors, bool alwayvisible=true);
  };

}

#endif
