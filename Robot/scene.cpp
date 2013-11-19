/*****************************************************************************\
 *AUTHOR     : Babel Yoann
 *CREATION   : 8/11/95, 
 *DATE       : 29/02/96
 *TITLE      : scene.cc , 3D scene drawing
 *OBJECT     :
 *CURRENTLY  : 
 *BUG        :
\*****************************************************************************/


#include <assert.h>
#include "scene.h"

/*****************************************************************************\
 *   PlotSet
\*****************************************************************************/


PlotElem::PlotElem(const Plot & pplot)
{
  plot = pplot;
  next = NULL;
}

PlotSet::PlotSet(int prealloc)
{
  root = NULL;
  queue = NULL;
  nplots = 0;
  assert(prealloc>=0);
  for (int i=0; i<prealloc; i++) AddPlot( Plot() );
}

PlotSet::~PlotSet()
{ 
  Purge();
}

Plot & PlotSet::operator [] (int i) const 
{
  assert(nplots>=0);
  assert(i>=0&&i<nplots);
  PlotElem *tmp;
  for (tmp=root; i!=0; i--) tmp=tmp->next;
  return tmp->plot;
}

void PlotSet::operator = (const PlotSet & ps)
{
  if (nplots==0)
    {
      for (int i=0; i<ps.nplots; i++) AddPlot( ps[i] );
    }
  else
    { // don't reallocate all the memory
      assert(ps.nplots==nplots);
      PlotElem *tmp=root, *pstmp=ps.root;
      for (int i=0; i<nplots; i++, tmp=tmp->next, pstmp=pstmp->next) 
	tmp->plot = pstmp->plot;
    }
}

void PlotSet::AddPlot(const Plot & plot)
{
  PlotElem * tmp;
  alloc(tmp = new PlotElem(plot));
  if (root==NULL)
    root = tmp;
  else
    {
      assert(queue->next == NULL);
      queue->next = tmp;
    }
  queue = tmp;
  nplots++;
}

void PlotSet::Rotate(const Rotation & r)
{
  PlotElem *tmp=root;
  for (int i=0; i<nplots; i++, tmp=tmp->next) tmp->plot = r*tmp->plot;
}

void PlotSet::Translate(const Plot & v)
{
  PlotElem *tmp=root;
  for (int i=0; i<nplots; i++, tmp=tmp->next) 
    tmp->plot = tmp->plot+v;
}

void PlotSet::Purge(void)
{
  PlotElem * tmp=root;
  while (tmp != NULL)
    {
      PlotElem * tmp1=tmp->next;
      delete tmp;
      tmp = tmp1;
    }  
  root = NULL;
  queue = NULL;
  nplots = 0;
}


#ifdef LIBGPP
ostream & operator << (ostream & os, const PlotSet & ps)
{
  PlotElem *tmp=ps.root;
  for (int i=0; i<ps.nplots; i++, tmp=tmp->next)
    os << tmp->plot << kCR;
  return os;
}
#endif

/*****************************************************************************\
 *   Scene
\*****************************************************************************/


Scene::Scene(int n)
{
  NbFaces = n;
  alloc(faces = new PFace [NbFaces]);
  for (int i=0; i<NbFaces; i++)
    faces[i] = NULL;
  CurNbFaces = 0;
}

Scene::~Scene()
{
  for (int i=0; i<NbFaces; i++)
    delete faces[i];
  delete [] faces;
}

void Scene::AddFace(PFace fp)
{
  assert(CurNbFaces < NbFaces);
  faces[CurNbFaces++] = fp;
}

PFace & Scene::operator [] (int i) const
{
  assert(i>=0&&i<NbFaces);
  return faces[i];
}

void Scene::Draw(Drawer & w, const Vector3 & light, real focal, real zoom)
{
  assert(CurNbFaces == NbFaces);
  CurLight = light;
  Plane screen(Vector3(0,0,1), 0);
  for (int j=0; j<NbFaces; j++)
    {
      assert(faces[j] != NULL);
      faces[j]->Update(focal);
      faces[j]->SplitRight(screen);
      faces[j]->AddLight(CurLight);
      faces[j]->Project(w, focal, zoom);
    }
  for (int i=0; i<NbFaces; i++)
    faces[i]->Draw(w);
}



/*****************************************************************************\
 *  SortedObject
\*****************************************************************************/


void SortedObject::Sort()
{
  for (int i=0; i<NbFaces; i++)
    if (faces[i]->Visible())
      AddFaceNode(faces[i],root);
}

void SortedObject::DelTree(FaceNode * &fn)
{
  if (fn==NULL) return;
  DelTree(fn->right);
  DelTree(fn->left);
  delete fn;
  fn = NULL;
}

void SortedObject::DrawFace(Drawer & w, FaceNode *fn)
{
  if (fn==NULL) return;
  DrawFace(w,fn->left);
  fn->f->Draw(w);
  DrawFace(w,fn->right);
}

void SortedObject::Draw(Drawer & w, const Vector3 & light,
			real focal, real zoom)
{
  assert(CurNbFaces == NbFaces);
  CurLight = light;
  Plane screen(Vector3(0,0,1), 0);
  for (int i=0; i<NbFaces; i++)
    {
      assert(faces[i] != NULL);
      faces[i]->Update(focal);
      faces[i]->SplitRight(screen);
      faces[i]->AddLight(CurLight);
      faces[i]->Project(w, focal, zoom);
    }
  Sort();
  DrawFace(w, root);
  DelTree(root);
}

void SortedObject::AddFaceNode(PFace f, FaceNode * & fn)
{
  assert(f!=NULL);
  if (fn==NULL)
    {
      alloc(fn = new FaceNode);
      fn->f = f;
      fn->right = NULL;  fn->left = NULL;
    }
  else
    { 
      // drawind : Left Middle Right
      //           back(z>)    front(z<)
      if (f->GetZ() < fn->f->GetZ() ) AddFaceNode(f, fn->right);
      else AddFaceNode(f, fn->left);
    }
}


/*****************************************************************************\
 *  PreSortedObject
\*****************************************************************************/


void PreSortedObject::Draw(Drawer & w, const Vector3 & light,
			   real focal, real zoom)
{
  assert(CurNbFaces == NbFaces);
  CurLight = light;
  Plane screen(Vector3(0,0,1), 0);
  for (int i=0; i<NbFaces; i++)
    {
      assert(faces[i] != NULL);
      faces[i]->Update(focal);
      faces[i]->SplitRight(screen);
      faces[i]->AddLight(CurLight);
      faces[i]->Project(w, focal, zoom);
    }
  if (!alreadysorted) { Sort(); alreadysorted = 1; }
  DrawFace(w, root);
}

/*****************************************************************************\
 *  Camera
\*****************************************************************************/

Camera::Camera(real pteta, real pphi, real ppsi, 
	       real px, real py, real pz, real pfocal, real pzoom)
: translation(px,py,pz)
{
  focal = pfocal;
  zoom = pzoom;
  rotation.RotateEuler(pteta, pphi, ppsi);
}

void Camera::NewAngle(real pteta, real pphi, real ppsi)
{
  rotation.RotateEuler(pteta, pphi, ppsi);  
}

void Camera::NewPosition(real x, real y, real z)
{
  translation.Set(x,y,z);
}

Plot Camera::ChangeToBase(const Plot & plot) const
{
  Vector3 result = rotation*plot;
  result = result-translation;
  return result;
}

Plot Camera::ReturnToOldBase(const Plot & plot) const
{
  Vector3 result = plot+translation;
  result = (!rotation)*result;
  return result;
}

Plot Camera::GetPlot(const Drawer & drawer, int x, int y, const Plane & plane)
{
  real tx = real(drawer.Width())/2;
  real ty = real(drawer.Height())/2;
  real f = real( minimum(tx,ty))*zoom;
  real sx = (x-tx)/f;
  real sy = (y-ty)/f;
  Vector3 origin(0,0,-focal);
  origin = origin+translation;
  origin = (!rotation)*origin;
  Vector3 pointed(sx, sy, 0);
  pointed = pointed+translation;
  pointed = (!rotation)*pointed;
  Vector3 director = pointed-origin;
  Vector3 normal = plane.Normal();
  
  real denom = director*normal;
  if (abs(denom) < epsilonprecision) return Plot(0,0,0);
  real t = (plane.GetDist() - real(origin*normal))/denom;

  Vector3 result = origin + director*t;
  return result;
}

