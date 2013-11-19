/*****************************************************************************\
 *AUTHOR     : Babel Yoann
 *DATE       : 13/04/96
 *TITLE      : scene.h , 3d scene drawing
 *OBJECT     : 
 *CURRENTLY  :  
 *BUGS       :
 *NOTE       : improvements : plots projection must be set in camera 
\*****************************************************************************/

#ifndef _SCENE_H_
#define _SCENE_H_

#include "face.h"

/*****************************************************************************\
 *PLOTSET :
 *Rotate, Translate : apply the tranformation on all the points. becareful
 *  because the all the plots will be modified, if you don't have a backup
 *  copy of all your points you will loose their old definition
\*****************************************************************************/

struct PlotElem
{
  Plot plot;
  PlotElem * next;
  PlotElem(const Plot &);
};

class PlotSet
{
  public :
  PlotSet(int prealloc=0);
  ~PlotSet();
  void operator = (const PlotSet &);
  Plot& operator [] (int) const;
  void AddPlot(const Plot &);
  void Rotate(const Rotation &);
  void Translate(const Plot &);
  int GetNPlots() const {return nplots;}

#ifdef LIBGPP
  friend ostream & operator << (ostream &, const PlotSet &);
#endif

  private :
  void Purge(void);
  PlotElem *root, *queue;
  int nplots;
};

/*****************************************************************************\
 *FACESET : 
 *  this class is used to draw a set of faces (currently there is 3 differents
 *  algo to draw, 1. the faces are drawn in no particular oder, 2. the faces
 *  are sorted each time the method draw is called, 3. the faces are sorted
 *  once, and draw in that order.
 *AddFace :
 *  FaceSet is list of n faces, when you create a FaceSet the necessary memory
 *  is allocated, you must then fill the list with the faces you want. You
 *  must use this function to fill the list. You must fill all the list (
 *  that means that you call AddFace n times), otherwise an assertion will fail
 *Extension : face set and childs currently support only one light source.
\*****************************************************************************/

typedef Face * PFace;

struct FaceNode
{ // tree
  PFace f;
  FaceNode *right, *left;
};

struct FaceElem
{ // double list
  PFace f;
  FaceElem *next, *prev;
};

class Scene
{ // owner of the faces
  public :
  Scene(int nbfaces);
  virtual ~Scene();
  void AddFace(PFace );
  virtual void Draw(Drawer &, const Vector3 &light, real focal, real zoom);
  PFace & operator [] (int) const;

  protected :
  FaceElem * listroot, * listqueue;
  Vector3 CurLight;
  PFace * faces;
  int NbFaces;
  int CurNbFaces;
};

class SortedObject : public Scene
{ // sort in a binary tree
  public :
  SortedObject(int nbfaces):Scene(nbfaces){root = NULL;}
  virtual ~SortedObject(){ DelTree(root); }
  void Draw(Drawer &, const Vector3 &light, real focal, real zoom);
  
  protected :
  FaceNode * root;
  void Sort(); 
  void DelTree(FaceNode * &);
  void DrawFace(Drawer &, FaceNode *);
  virtual void AddFaceNode(PFace , FaceNode * &);
};

class PreSortedObject : public SortedObject
{
  public :
  PreSortedObject(int nbfaces):SortedObject(nbfaces)
    {alreadysorted=0;}
  void Draw(Drawer &, const Vector3 & light, real focal, real zoom);
  void ReSort() {alreadysorted = 0;}

  private :
  int alreadysorted;
};

/*****************************************************************************\
 *CAMERA :
\*****************************************************************************/

class Camera
{
  public :
  Camera(real teta, real phi, real psi, real x, real y, real z,
	 real focal, real zoom);
  void NewAngle(real teta, real phi, real psi);
  void NewPosition(real x, real y, real z);
  Plot ChangeToBase(const Plot &) const;
  Plot ReturnToOldBase(const Plot &) const;
  Plot GetPlot(const Drawer &, int x, int y, const Plane &);
  real GetFocal() const { return focal; }
  real GetZoom() const { return zoom; }
  Rotation GetRotation() const { return rotation; }
  Vector3 GetTranslation() const { return translation; }
  
  private :
  Rotation rotation;
  Vector3 translation;
  real focal;
  real zoom;
};



#endif
