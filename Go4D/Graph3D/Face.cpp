/*****************************************************************************\
*AUTHOR     : Babel Yoann
*CREATION   : 14/07/96,
*DATE       : 14/07/96
*TITLE      : AFace.cpp, AFace.h implementation
*OBJECT     :
*NOTE       : cutting not finished (deformation) for mappedface
*             and quadmappedface
\*****************************************************************************/

#include "Face.h"

namespace Go4D
{


  /*****************************************************************************\
  *   Face
  \*****************************************************************************/

  Face::Face(AFaceDrawer * aFaceDrawer, Color c, Color nbc, int nb, bool av)
  {
    colormin = c;
    nbcolors = nbc;
    npoints = nb;
    alwaysvisible = av;
    Init();
    fFaceDrawer = aFaceDrawer;
  }

  Face::Face(AFaceDrawer * aFaceDrawer, APlot &plot1, APlot &plot2,
    APlot &plot3, APlot &plot4, Color c, Color nbc, bool av)
  {
    colormin = c;
    nbcolors = nbc;
    npoints = 4;
    alwaysvisible = av;
    fFaceDrawer = aFaceDrawer;
    Init();
    //Assume that the vertices are co-planar
    edges[0]=&plot1; edges[1]=&plot2; edges[2]=&plot3; edges[3]=&plot4;
  }

  Face::~Face()
  {
    PurgeCutting();
    delete [] edges;
    delete [] points;
    delete [] dists;
    //delete fFaceDrawer;
  }

  void Face::Init()
  {
    visible = true;
    alloc(edges = new PPlot [npoints]);
    alloc(points = new PointZ [npoints]);
    alloc(dists = new real [npoints]);
    right = NULL;
    left = NULL;
    cutted = false;
  }


  /*****************************************************************************\
  *
  *                        first\|Plane
  *                        ------*------*  <-first_positive
  *                       /      |       \         ---+
  *                      /   -   |   +    \  Face     |
  *                     /        |--->     \          v
  *                    /  second\|Normal    \
  * first_negative->  *----------*------------
  *                     Left     |  Right
  *
  \*****************************************************************************/

  bool Face::Cut(const Plane & plane, int & first_positive, int & first_negative,
    int & nb_positive, real & first_coord, real & second_coord,
    int & first_sign)
  {
    assert(!cutted);
    // calculate all the points's distances to the plane
    //--------------------------------------------------

    for (int k=0; k<npoints; k++)
    {
      dists[k] = plane.Dist(*edges[k]);
      dists[k] = dists[k]==0?zero:dists[k]; // dist==0 would be annoying
    }

    // Really need to cut ?
    //---------------------

    first_sign = sign(dists[0]);
    bool realyneed = false;
    for (int j=1; j<npoints; j++)
      if (sign(dists[j])!=first_sign) {realyneed=true; break;}
      if (!realyneed) return false;
      cutted = true;

      // Look for the 2 new points
      //--------------------------

      //first of all, search the first point that is positive (after a negative)
      //consider that the list of points is circular
#define NEXT(i) ((i)==npoints-1?0:(i)+1)
#define PREV(i) ((i)==0?npoints-1:(i)-1)
      first_positive=0;
      while (dists[first_positive]>0) first_positive=NEXT(first_positive);
      while (dists[first_positive]<0) first_positive=NEXT(first_positive);
      first_negative=first_positive;
      nb_positive=0;
      // count the number of points that are at the right of the plane (positives)
      while (dists[first_negative]>0)
      {
        first_negative=NEXT(first_negative);
        nb_positive++;
      }
      first = plane.Cut(*edges[first_positive],
        *edges[PREV(first_positive)],
        dists[first_positive],
        dists[PREV(first_positive)],
        first_coord);
      second = plane.Cut(*edges[first_negative],
        *edges[PREV(first_negative)],
        dists[first_negative],
        dists[PREV(first_negative)],
        second_coord);

      return true;
  }

  void Face::PurgeCutting()
  {
    if (cutted)
    {
      if (right!=NULL) delete right;
      if (left!=NULL) delete left;
      cutted = FALSE;
    }
    right = NULL;
    left = NULL;
  }

  PPlot & Face::operator [] (int i) const
  {
    assert(i>=0 && i<npoints);
    return edges[i];
  }

  void Face::SetColor(Color aColor)
  {
    for (int i=0; i<npoints; i++)
      edges[i]->GetColor() = aColor;
  }

  void Face::Update()
  {
    Vector3 tmp0 = *edges[1] - *edges[0];
    Vector3 tmp1 = *edges[1] - *edges[2];
    normal = tmp0^tmp1;
    // if normalized fail, then the face is "not a face",
    // we put the non visible flag to not display it
    visible = normal.Normalize();
    PurgeCutting();
  }

  void Face::Update(const Camera & aCamera)
  {
    Update();
    // don't forget that visible flag is set by update
    visible = visible && ConvexTest(aCamera);
  }

  void Face::SplitRight(const Plane & plane)
  {
    if (!visible) return;
    int first_positive, first_negative, nb_positive, first_sign;
    real first_coord, second_coord;
    if (Cut(plane, first_positive, first_negative, nb_positive,
      first_coord, second_coord, first_sign))
    {
      right = SplitClone(nb_positive+2);
      int ir=first_positive;
      for (int p=0; p<nb_positive; p++, ir=NEXT(ir))
        (*right)[p] = edges[ir];
      (*right)[nb_positive] = &second;
      (*right)[nb_positive+1] = &first;
      SplitExtrem(right, first_positive, first_negative, nb_positive, first_sign,
        first_coord, second_coord);
      right->Update();
    }
    else if (first_sign==-1) cutted=true;
    visible = visible && !(cutted && right==NULL && left==NULL);
  }

  void Face::Lightning()
  {
  }

  void Face::AddLight(const ALight & aLight)
  {
    if (!visible) return;
    if (cutted)
    {
      if (right!=NULL) right->AddLight(aLight);
      if (left!=NULL) left->AddLight(aLight);
    }
    else for (int i=0; i<npoints; i++)
      aLight.EnLight(*edges[i], normal);
  }

  void Face::Lightned()
  {
  }

  void Face::Project(const Camera & aCamera)
  {
    if (!visible) return;
    if (cutted)
    {
      if (right != NULL) right->Project(aCamera);
      if (left != NULL) left->Project(aCamera);
    }
    else
    {
      for (int i=0; i<npoints; i++)
        aCamera.Project(*edges[i]);
      visible = !EntirelyOffScreen(aCamera);
    }
  }

  void Face::Draw()
  {
    if (!visible) return;
    if (cutted)
    {
      if (right != NULL) right->Draw();
      if (left != NULL) left->Draw();
    }
    else
      BasicDraw();
  }

  real Face::GetMinZ() const
  {
    real result = edges[0]->Z();
    for (int i=1; i<npoints; i++)
      if (edges[i]->Z()<result)
        result = edges[i]->Z();
    return result;
  }

  real Face::GetMaxZ() const
  {
    real result = edges[0]->Z();
    for (int i=1; i<npoints; i++)
      if (edges[i]->Z()>result)
        result = edges[i]->Z();
    return result;
  }

  real Face::GetAverageZ() const
  {
    real result = 0.0;
    for (int i=0; i<npoints; i++)
      result += edges[i]->Z();
    result /= real(npoints);
    return result;
  }

  bool Face::ConvexTest(const Camera & aCamera) const
  {
    Vector3 tmp = *edges[0] + Vector3(0,0,aCamera.Focal());
    return (alwaysvisible || (normal*(tmp) < zero) );
  }

  void Face::SimpleDraw(const Camera & aCamera)
  {
    Update(aCamera);
    SplitRight(Plane(Vector3(0,0,1),0));
    Project(aCamera);
    Draw();
  }

  void Face::SimpleDraw(const Camera & aCamera, const ALight & aLight)
  {
    Update(aCamera);
    SplitRight(Plane(Vector3(0,0,1),0));
    AddLight(aLight);
    Project(aCamera);
    Draw();
  }

  void Face::SplitExtrem(Face * aNewFace, int first_positive, int first_negative,
    int nb_positive, int first_sign, real first_coord, real second_coord)
  {
    aNewFace->fFaceDrawer->SplitExtrem(fFaceDrawer, first_positive, first_negative,
      nb_positive, first_sign, first_coord, second_coord);
  }

  bool Face::EntirelyOffScreen(const Camera & aCamera)
  {
    bool all_left=true, all_right=true, all_up=true, all_down=true;
    for (int i=0; i<npoints; i++)
    {
      if (edges[i]->X() >= 0) all_left=false;
      if (edges[i]->X() < aCamera.ScreenWidth()) all_right=false;
      if (edges[i]->Y() >= 0) all_up=false;
      if (edges[i]->Y() < aCamera.ScreenHeight()) all_down=false;
    }
    return all_left||all_right||all_up||all_down;
  }

  Plane & Face::BuildPlane()
  {
    // assert face Updated
    plane = Plane(normal, *edges[0]);
    return plane;
  }

  bool Face::Behind(const Face & face)
  {
    //assert face and self BuildPlane
    bool result = true;
    for (int i=0; i<npoints; i++)
      if ( face.plane.Dist(*edges[i]) > zero ) result = false;
    return result;
  }

  bool Face::InFront(const Face & face)
  {
    //assert face and self BuildPlane
    bool result = true;
    for (int i=0; i<npoints; i++)
      if ( face.plane.Dist(*edges[i]) < -zero ) result = false;
    return result;
  }

  void Face::BasicDraw()
  {
    for (int i=0; i<npoints; i++)
    {
      edges[i]->ProcessColor(colormin, nbcolors);
      points[i] = edges[i]->GetPointZ();
    }
    fFaceDrawer->Draw(npoints, points);
  }

  Face * Face::SplitClone(int newnbpoints)
  {
    return new Face(fFaceDrawer->Clone(newnbpoints), colormin, nbcolors, newnbpoints, alwaysvisible);
  }

  /*****************************************************************************\
  * FlatFace
  \*****************************************************************************/

  FlatFace::FlatFace(AFaceDrawer *aFaceDrawer, Color aColormin, Color aNbcolors,
    int aNbpoints, bool aAlwaysvisible)
    : inherited(aFaceDrawer, aColormin, aNbcolors, aNbpoints, aAlwaysvisible)
  {
  }

  FlatFace::FlatFace(AFaceDrawer *aFaceDrawer, APlot & aPlot0, APlot & aPlot1,
    APlot & aPlot2, APlot & aPlot3, Color aColormin, Color aNbcolors,
    bool aAlwaysvisible)
    : inherited(aFaceDrawer, aPlot0, aPlot1, aPlot2, aPlot3, aColormin, aNbcolors,
    aAlwaysvisible)
  {
  }

  void FlatFace::Update()
  {
    inherited::Update();
    fCenter = *edges[0];
  }

  void FlatFace::AddLight(const ALight & aLight)
  {
    if (!visible) return;
    if (cutted)
    {
      if (right!=NULL) right->AddLight(aLight);
      if (left!=NULL) left->AddLight(aLight);
    }
    else
      aLight.EnLight(fCenter, normal);
  }

  void FlatFace::BasicDraw()
  {
    fCenter.ProcessColor(colormin, nbcolors);
    for (int i=0; i<npoints; i++)
    {
      edges[i]->GetColor() = fCenter.GetColor();
      points[i] = edges[i]->GetPointZ();
    }
    fFaceDrawer->Draw(npoints, points);
  }

  Face * FlatFace::SplitClone(int newnbpoints)
  {
    return new FlatFace(fFaceDrawer->Clone(newnbpoints), colormin, nbcolors, newnbpoints, alwaysvisible);
  }

  /*****************************************************************************\
  * FlatShadedFace
  \*****************************************************************************/

  FlatShadedFace::FlatShadedFace(AFaceDrawer *aFaceDrawer, Color aColormin, Color aNbcolors,
    int aNbpoints, bool aAlwaysvisible)
    : inherited(aFaceDrawer, aColormin, aNbcolors, aNbpoints, aAlwaysvisible)
  {
    alloc(fColors = new int[npoints]);
  }

  FlatShadedFace::FlatShadedFace(AFaceDrawer *aFaceDrawer, APlot & aPlot0, APlot & aPlot1,
    APlot & aPlot2, APlot & aPlot3, Color aColormin, Color aNbcolors,
    bool aAlwaysvisible)
    : inherited(aFaceDrawer, aPlot0, aPlot1, aPlot2, aPlot3, aColormin, aNbcolors,
    aAlwaysvisible)
  {
    alloc(fColors = new int[npoints]);
  }

  FlatShadedFace::~FlatShadedFace()
  {
    delete [] fColors;
  }

  void FlatShadedFace::Lightning()
  {
    for (int i=0; i<npoints; i++)
      edges[i]->GetColor() = 0;
  }

  void FlatShadedFace::Lightned()
  {
    for (int i=0; i<npoints; i++)
      fColors[i] = edges[i]->ProcessColor(colormin, nbcolors).GetColor();
  }

  void FlatShadedFace::BasicDraw()
  {
    for (int i=0; i<npoints; i++)
    {
      edges[i]->GetColor() = fColors[i];
      points[i] = edges[i]->GetPointZ();
    }
    fFaceDrawer->Draw(npoints, points);
  }

  Face * FlatShadedFace::SplitClone(int newnbpoints)
  {
    return new FlatShadedFace(fFaceDrawer->Clone(newnbpoints), colormin, nbcolors, newnbpoints, alwaysvisible);
  }

  /*****************************************************************************\
  * GouraudFace
  \*****************************************************************************/

  GouraudFace::GouraudFace(AFaceDrawer *aFaceDrawer, Color aColormin, Color aNbcolors,
    int aNbpoints, bool aAlwaysvisible)
    : inherited(aFaceDrawer, aColormin, aNbcolors, aNbpoints, aAlwaysvisible)
  {
  }

  GouraudFace::GouraudFace(AFaceDrawer *aFaceDrawer, APlot & aPlot0, APlot & aPlot1,
    APlot & aPlot2, APlot & aPlot3, Color aColormin, Color aNbcolors,
    bool aAlwaysvisible)
    : inherited(aFaceDrawer, aPlot0, aPlot1, aPlot2, aPlot3, aColormin, aNbcolors,
    aAlwaysvisible)
  {
  }

  void GouraudFace::BasicDraw()
  {
    for (int i=0; i<npoints; i++)
    {
      edges[i]->ProcessGouraudColor(colormin, nbcolors);
      points[i] = edges[i]->GetPointZ();
    }
    fFaceDrawer->Draw(npoints, points);
  }

  Face * GouraudFace::SplitClone(int newnbpoints)
  {
    return new GouraudFace(fFaceDrawer->Clone(newnbpoints), colormin, nbcolors, newnbpoints, alwaysvisible);
  }


  /*****************************************************************************\
  * EnvMapFace
  \*****************************************************************************/

  void EnvMapFace::BeginDraw()
  {
    inherited::BeginDraw();
    for (int i=0; i<npoints; i++)
      edges[i]->ResetNormal();
  }

  void EnvMapFace::Update()
  {
    inherited::Update();
    if (!visible) return;
    for (int i=0; i<npoints; i++)
      edges[i]->AddNormal(normal);
  }

  void EnvMapFace::SetEnvMapPoint(int i, const ALight & aLigth)
  {
    real mapX = real(fFaceDrawer->GetMapWidth())/2.0;
    real mapY = real(fFaceDrawer->GetMapHeight())/2.0;
    int mx;
    int my;
    PPlot curEdge = (edges[i]);
    Vector3 edgeNormal = curEdge->GetAverageNormal();
    assert(edgeNormal.Norme()>0.0);
    Vector3 ligthDirection = aLigth.Direction(*curEdge);
    assert(ligthDirection.Norme()>0.0);
    Vector3 direction = edgeNormal^ligthDirection;
    direction.Normalize();
    mx = int(real( direction[0]*mapX+mapX ));
    my = int(real( direction[1]*mapY+mapY ));
    fFaceDrawer->SetMapPoint(i, mx, my);
  }

  void EnvMapFace::AddLight(const ALight & aLight)
  {
    for (int i=0; i<npoints; i++)
      SetEnvMapPoint(i, aLight);
  }

  EnvMapFace::EnvMapFace(AFaceDrawer *aFaceDrawer, Color aColormin, Color aNbcolors,
    int aNbpoints, bool aAlwaysvisible)
    : inherited(aFaceDrawer, aColormin, aNbcolors, aNbpoints, aAlwaysvisible)
  {
  }

  EnvMapFace::EnvMapFace(AFaceDrawer *aFaceDrawer, APlot & aPlot0, APlot & aPlot1,
    APlot & aPlot2, APlot & aPlot3, Color aColormin, Color aNbcolors,
    bool aAlwaysvisible)
    : inherited(aFaceDrawer, aPlot0, aPlot1, aPlot2, aPlot3, aColormin, aNbcolors,
    aAlwaysvisible)
  {
  }

  Face * EnvMapFace::SplitClone(int newnbpoints)
  {
    return new EnvMapFace(fFaceDrawer->Clone(newnbpoints), colormin, nbcolors, newnbpoints, alwaysvisible);
  }


}
