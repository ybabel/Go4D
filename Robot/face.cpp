/*****************************************************************************\
 *AUTHOR     : Babel Yoann
 *CREATION   : 8/11/95, 
 *DATE       : 29/02/96
 *TITLE      : face.cc , 3D face management
 *OBJECT     :
 *CURRENTLY  : 
 *BUG        : cutting not finished (deformation) for mappedface 
 *             and quadmappedface
\*****************************************************************************/


#include <assert.h>
#include "face.h"


/*****************************************************************************\
 *   Plot
\*****************************************************************************/


void Plot::ProjectionParallele(int tx, int ty, real zoom, real lad)
{
  if (!processed)
    {
      processed=1;
      px = (int)(fElems[0]*zoom*lad) + tx;
      py = (int)(fElems[1]*zoom*lad) + ty;
      pz = fElems[2];
    }
}

void Plot::ProjectionPerspective(real focal, int tx, int ty, 
				 real zoom, real ladder)
{
  if (!processed)
    {
      processed=1;
      if ( fabs(fElems[2]) > epsilonprecision-focal)
	{
	  real k = ladder/2*zoom*focal/(fElems[2]+focal);
	  px = (int) (fElems[0]*k) + tx;
	  py = (int) (fElems[1]*k) + ty;
	  pz = fElems[2];
	}
      #ifdef LIBGPP
      else error("not projected !!!!");
      #endif
    }
}

PointZ Plot::GetPointZ() const
{
  assert(processed==1);
  return PointZ(px, py, color, pz);
}


#ifdef LIBGPP
ostream & operator << (ostream & os, const Plot & ps)
{
  os << "(x,y,z):(" << ps[0] << "," << ps[1] << "," << ps[2] << ")";
  os << "  px:"<< ps.px << "  py:" << ps.py << "  pz:" << ps.pz;
  os << "  color:" << ps.color;
  return os;
}
#endif


/*****************************************************************************\
 *   Face
\*****************************************************************************/


Face::Face(byte c, byte nbc, int nb, int av)
{
  z = 0;
  colormin = c;
  nbcolors = nbc;
  npoints = nb;
  alwaysvisible = av;
  visible = TRUE;
  color = colormin;
  alloc(edges = new PPlot [npoints]);
  alloc(points = new PointZ [npoints]);
  alloc(dists = new real [npoints]);
  right = NULL;
  left = NULL;
  cutted = FALSE;
}

Face::~Face()
{
  PurgeCutting();
  delete [] edges;
  delete [] points;
  delete [] dists;
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

Plot * & Face::operator [] (int i) const
{
  assert(i>=0 && i<npoints);
  return edges[i];
}

void Face::SetColor(byte col)
{
  if (col < colormin)
    color = colormin;
  else if (col>colormin+nbcolors) color = byte(colormin+nbcolors);
  else color = col;
  for (int i=0; i<npoints; i++)
    edges[i]->SetColor(color);
}

void Face::Update(real focal)
{
  Vector3 tmp0 = *edges[1] - *edges[0];
  Vector3 tmp1 = *edges[1] - *edges[2];
  normal = tmp0^tmp1;
  if (!normal.Normalize())
    assert(false);
  z = (*edges[0])[2];
  color = colormin;
  visible = ConvexTest(focal);
  PurgeCutting();
}

void Face::AddLight(const Vector3 & light)
{
  if (!visible) return;
  if (cutted)
    {
      if (right!=NULL) right->AddLight(light);
      if (left!=NULL) left->AddLight(light);
    }
  else
    {
      real s = normal*light;
      s = fabs(s);
      color += byte (s*nbcolors);
      if (color > colormin+nbcolors) color = byte(colormin+nbcolors);
    }
}

void Face::Project(Drawer &d, real focal, real zoom)
{
  if (!visible) return;
  for (int i=0; i<npoints; i++)
    edges[i]->ProjectionPerspective(focal, d.MiddleX(), d.MiddleY(),
				    zoom, minimum(d.Width(), d.Height()));
  if (cutted)
    {
      first.ProjectionPerspective(focal, d.MiddleX(), d.MiddleY(),
				  zoom, minimum(d.Width(), d.Height()));
      second.ProjectionPerspective(focal, d.MiddleX(), d.MiddleY(),
				   zoom, minimum(d.Width(), d.Height()));
    }
  visible = !EntirelyOffScreen(d);
}

void Face::Draw(Drawer &d)
{
  if (!visible) return;
  if (cutted)
    {
      if (right != NULL) right->Draw(d);
      if (left != NULL) left->Draw(d);
    }
  else
    {
      for (int i=0; i<npoints; i++)
	points[i] = edges[i]->GetPointZ();
      d.Fill(npoints, points, color);
    }
}

int Face::ConvexTest(real focal) const
{
  Vector3 tmp = *edges[0] + Vector3(0,0,focal);
  return (alwaysvisible || (normal*(tmp) < epsilonprecision) );
}

void Face::BasicDraw(Drawer &d, real focal, real zoom)
{
  Update(focal);
  SplitRight(Plane(Vector3(0,0,1),0));
  Project(d, focal, zoom);
  Draw(d);
}

void Face::BasicDraw(Drawer &d, real focal, real zoom, const Vector3 & light)
{
  Update(focal);
  SplitRight(Plane(Vector3(0,0,1),0));
  AddLight(light);
  Project(d, focal, zoom);
  Draw(d);
}

void Face::DrawShape(Drawer & d)
{
  if (!visible) return;
  if (cutted)
    {
      if (right != NULL) right->Draw(d);
      if (left != NULL) left->Draw(d);
    }
  else
    {
      for (int j=0; j<npoints; j++)
	points[j] = edges[j]->GetPointZ();
      for (int i=0; i<npoints-1; i++)
	d.Line(points[i], points[i+1]);
      d.Line(points[npoints-1], points[0]);
    }
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

int Face::Cut(const Plane & plane, int & first_positive, int & first_negative,
	      int & nb_positive, real & first_coord, real & second_coord,
	      int & first_sign)
{
  assert(!cutted);
  // calculate all the points's distances to the plane
  //--------------------------------------------------

  for (int k=0; k<npoints; k++) 
    {
      dists[k] = plane.Dist(*edges[k]);
      dists[k] = dists[k]==0?epsilonprecision:dists[k]; // dist==0 would be annoying
    }

  // Really need to cut ?
  //---------------------

  first_sign = sign(dists[0]);
  int realyneed = FALSE;
  for (int j=1; j<npoints; j++)
    if (sign(dists[j])!=first_sign) {realyneed=TRUE; break;}
  if (!realyneed) return FALSE;
  cutted = TRUE;

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

  return TRUE;
}

void Face::SplitRight(const Plane & plane)
{
  if (!visible) return;
  int first_positive, first_negative, nb_positive, first_sign;
  real first_coord, second_coord; 
  if (Cut(plane, first_positive, first_negative, nb_positive,
	  first_coord, second_coord, first_sign))
    {
      right = new Face(colormin, nbcolors, nb_positive+2, alwaysvisible);
      int ir=first_positive;
      for (int p=0; p<nb_positive; p++, ir=NEXT(ir))
	(*right)[p] = edges[ir];
      (*right)[nb_positive] = &second; (*right)[nb_positive+1] = &first;
    }
  else if (first_sign==-1) cutted=TRUE;
  visible = visible && !(cutted && right==NULL && left==NULL);
}

void Face::SplitLeft(const Plane & plane)
{  
  if (!visible) return;
  int first_positive, first_negative, nb_positive, first_sign;
  real first_coord, second_coord; 
  if (Cut(plane, first_positive, first_negative, nb_positive,
	  first_coord, second_coord, first_sign))
    {
      int nb_negative = npoints-nb_positive;
      left = new Face(colormin, nbcolors, nb_negative+2, alwaysvisible);
      int il=first_negative;
      for (int q=0; q<nb_negative; q++, il=NEXT(il))
	(*left)[q] = edges[il];
      (*left)[nb_negative] = &first; (*left)[nb_negative+1] = &second;
    }
  else if (first_sign==1) cutted=TRUE;
  visible = visible && !(cutted && right==NULL && left==NULL);
}

int Face::EntirelyOffScreen(const Drawer & drawer)
{
  int all_left=TRUE, all_right=TRUE, all_up=TRUE, all_down=TRUE;
  for (int i=0; i<npoints; i++)
    {
      if (edges[i]->GetX() >= 0) all_left=FALSE;
      if (edges[i]->GetX() < drawer.Width()) all_right=FALSE;
      if (edges[i]->GetY() >= 0) all_up=FALSE;
      if (edges[i]->GetY() < drawer.Height()) all_down=FALSE;
    }
  return all_left||all_right||all_up||all_down;
}

Plane & Face::BuildPlane()
{
  // assert face Updated
  plane = Plane(normal, *edges[0]);
  return plane;
}

int Face::Behind(const Face & face)
{
  //assert face and self BuildPlane
  int result = TRUE;
  for (int i=0; i<npoints; i++)
    if ( face.plane.Dist(*edges[i]) > epsilonprecision ) result = FALSE;
  return result;
}

int Face::InFront(const Face & face)
{
  //assert face and self BuildPlane
  int result = TRUE;
  for (int i=0; i<npoints; i++)
    if ( face.plane.Dist(*edges[i]) < -epsilonprecision ) result = FALSE;
  return result;
}


/*****************************************************************************\
 *   ShadedFace
\*****************************************************************************/


void ShadedFace::AddLight(const Vector3 & light)
{
  if (!visible) return;
  Face::AddLight(light);
  for (int i=0; i<npoints; i++)
    edges[i]->AddColor(color);
}

void ShadedFace::Draw(Drawer &d)
{
  if (!visible) return;
  if (cutted)
    {
      if (right != NULL) right->Draw(d);
      if (left != NULL) left->Draw(d);
    }
  else
    {
      for (int i=0; i<npoints; i++)
	points[i] = edges[i]->GetPointZ();
      d.Shade(npoints, points);
    }
}

void ShadedFace::SplitRight(const Plane & plane)
{
  if (!visible) return;
  int first_positive, first_negative, nb_positive, first_sign;
  real first_coord, second_coord;
  if (Cut(plane, first_positive, first_negative, nb_positive,
	  first_coord, second_coord, first_sign))
    {
      right = new ShadedFace(colormin, nbcolors, nb_positive+2, alwaysvisible);
      int ir=first_positive;
      for (int p=0; p<nb_positive; p++, ir=NEXT(ir))
	(*right)[p] = edges[ir];
      (*right)[nb_positive] = &second; (*right)[nb_positive+1] = &first;
    }
  else if (first_sign==-1) cutted=TRUE;
  visible = visible && !(cutted && right==NULL && left==NULL);
}


/*****************************************************************************\
 *   AbstractMappedFace
\*****************************************************************************/


AbstractMappedFace::AbstractMappedFace(const Image * pima, int nbpoints, int av)
:Face(0,0,nbpoints,av)
{
  alloc(mpoints = new MapPoint [npoints]);
  alloc(dupmpoints = new MapPoint [npoints]);
  ima = pima;
}

AbstractMappedFace::~AbstractMappedFace()
{
  delete [] mpoints;
  delete [] dupmpoints;
}

void AbstractMappedFace::AddLight(const Vector3 &)
{ }

void AbstractMappedFace::SetMapPoint(int i, int x, int y)
{
  assert(i>=0 && i<npoints);
  MapPoint m;
  m.x = x;
  m.y = y;
  mpoints[i] = m;
}


/*****************************************************************************\
 *   MappedFace
\*****************************************************************************/


void MappedFace::Draw(Drawer &d)
{
  if (!visible) return;
  if (cutted)
    {
      if (right != NULL) right->Draw(d);
      if (left != NULL) left->Draw(d);
    }
  else
    {
      for (int j=0; j<npoints; j++)
	      points[j] = edges[j]->GetPointZ();
      for (int i=0; i<npoints; i++)
   	    dupmpoints[i] = mpoints[i];
      d.Map(npoints, points, dupmpoints, ima);
    }
}

void MappedFace::SplitRight(const Plane & plane)
{
  if (!visible) return;
  int first_positive, first_negative, nb_positive, first_sign;
  real first_coord, second_coord;
  if (Cut(plane, first_positive, first_negative, nb_positive,
	  first_coord, second_coord, first_sign))
    {
      right = new MappedFace(ima, nb_positive+2, alwaysvisible);
      int ir=first_positive;
      for (int p=0; p<nb_positive; p++, ir=NEXT(ir))
	(*right)[p] = edges[ir];
      (*right)[nb_positive] = &second; (*right)[nb_positive+1] = &first;
      ir=first_positive;
      for (int q=0; q<nb_positive; q++, ir=NEXT(ir))
	((MappedFace *)(right))->mpoints[q] = mpoints[ir];

      ((MappedFace *)(right))->mpoints[nb_positive].x = (int)( 
	(mpoints[PREV(first_negative)].x-mpoints[first_negative].x)*
	  second_coord+mpoints[first_negative].x);
      ((MappedFace *)(right))->mpoints[nb_positive].y = (int)(
	(mpoints[PREV(first_negative)].y-mpoints[first_negative].y)*
	  second_coord+mpoints[first_negative].y);

      ((MappedFace *)(right))->mpoints[nb_positive+1].x = (int)(
	(mpoints[PREV(first_positive)].x-mpoints[first_positive].x)*
	  first_coord+mpoints[first_positive].x);
      ((MappedFace *)(right))->mpoints[nb_positive+1].y = (int)(
	(mpoints[PREV(first_positive)].y-mpoints[first_positive].y)*
	  first_coord+mpoints[first_positive].y);
    }
  else if (first_sign==-1) cutted=TRUE;
  visible = visible && !(cutted && right==NULL && left==NULL);
}


/*****************************************************************************\
 *   QuadMappedFace
\*****************************************************************************/


void QuadMappedFace::Draw(Drawer &d)
{
  if (!visible) return;
  if (cutted)
    {
      if (right != NULL) right->Draw(d);
      if (left != NULL) left->Draw(d);
    }
  else
    {
      for (int j=0; j<npoints; j++)
	points[j] = edges[j]->GetPointZ();
      for (int i=0; i<npoints; i++)
	dupmpoints[i] = mpoints[i];
      d.QuadMap(npoints, points, dupmpoints, ima);
    }
}

void QuadMappedFace::SplitRight(const Plane & plane)
{
  if (!visible) return;
  int first_positive, first_negative, nb_positive, first_sign;
  real first_coord, second_coord; 
  if (Cut(plane, first_positive, first_negative, nb_positive,
	  first_coord, second_coord, first_sign))
    {
      right = new QuadMappedFace(ima, nb_positive+2, alwaysvisible);
      int ir=first_positive;
      for (int p=0; p<nb_positive; p++, ir=NEXT(ir))
	(*right)[p] = edges[ir];
      (*right)[nb_positive] = &second; (*right)[nb_positive+1] = &first;
      ir=first_positive;
      for (int q=0; q<nb_positive; q++, ir=NEXT(ir))
	((QuadMappedFace *)(right))->mpoints[q] = mpoints[ir];

      ((QuadMappedFace *)(right))->mpoints[nb_positive].x = (int)( 
	(mpoints[PREV(first_negative)].x-mpoints[first_negative].x)*
	  second_coord+mpoints[first_negative].x);
      ((QuadMappedFace *)(right))->mpoints[nb_positive].y = (int)(
	(mpoints[PREV(first_negative)].y-mpoints[first_negative].y)*
	  second_coord+mpoints[first_negative].y);

      ((QuadMappedFace *)(right))->mpoints[nb_positive+1].x = (int)(
	(mpoints[PREV(first_positive)].x-mpoints[first_positive].x)*
	  first_coord+mpoints[first_positive].x);
      ((QuadMappedFace *)(right))->mpoints[nb_positive+1].y = (int)(
	(mpoints[PREV(first_positive)].y-mpoints[first_positive].y)*
	  first_coord+mpoints[first_positive].y);
    }
  else if (first_sign==-1) cutted=TRUE;
  visible = visible && !(cutted && right==NULL && left==NULL);
}


/*****************************************************************************\
 *   HypMappedFace
\*****************************************************************************/


void HypMappedFace::Draw(Drawer &d)
{
  if (!visible) return;
  if (cutted)
    {
      if (right != NULL) right->Draw(d);
      if (left != NULL) left->Draw(d);
    }
  else
    {
      for (int j=0; j<npoints; j++)
	points[j] = edges[j]->GetPointZ();
      for (int i=0; i<npoints; i++)
	dupmpoints[i] = mpoints[i];
      d.HypMap(npoints, points, dupmpoints, ima);
    }
}

void HypMappedFace::SplitRight(const Plane & plane)
{
  if (!visible) return;
  int first_positive, first_negative, nb_positive, first_sign;
  real first_coord, second_coord; 
  if (Cut(plane, first_positive, first_negative, nb_positive,
	  first_coord, second_coord, first_sign))
    {
      right = new HypMappedFace(ima, nb_positive+2, alwaysvisible);
      int ir=first_positive;
      for (int p=0; p<nb_positive; p++, ir=NEXT(ir))
	(*right)[p] = edges[ir];
      (*right)[nb_positive] = &second; (*right)[nb_positive+1] = &first;
      ir=first_positive;
      for (int q=0; q<nb_positive; q++, ir=NEXT(ir))
	((HypMappedFace *)(right))->mpoints[q] = mpoints[ir];

      ((HypMappedFace *)(right))->mpoints[nb_positive].x = (int)( 
	(mpoints[PREV(first_negative)].x-mpoints[first_negative].x)*
	  second_coord+mpoints[first_negative].x);
      ((HypMappedFace *)(right))->mpoints[nb_positive].y = (int)(
	(mpoints[PREV(first_negative)].y-mpoints[first_negative].y)*
	  second_coord+mpoints[first_negative].y);

      ((HypMappedFace *)(right))->mpoints[nb_positive+1].x = (int)(
	(mpoints[PREV(first_positive)].x-mpoints[first_positive].x)*
	  first_coord+mpoints[first_positive].x);
      ((HypMappedFace *)(right))->mpoints[nb_positive+1].y = (int)(
	(mpoints[PREV(first_positive)].y-mpoints[first_positive].y)*
	  first_coord+mpoints[first_positive].y);
    }
  else if (first_sign==-1) cutted=TRUE;
  visible = visible && !(cutted && right==NULL && left==NULL);
}


/*****************************************************************************\
 *   Face4
\*****************************************************************************/


Face4::Face4(Plot &plot1, Plot &plot2,
	     Plot &plot3, Plot &plot4, byte c, byte nbc, int av)
:Face(c,nbc,4,av)
{
  //Assume that the vertices are co-planar
  edges[0]=&plot1; edges[1]=&plot2; edges[2]=&plot3; edges[3]=&plot4;
}


/*****************************************************************************\
 *   Face3
\*****************************************************************************/


Face3::Face3(Plot &plot1, Plot &plot2, 
	     Plot &plot3, byte c, byte nbc, int av)
:Face(c,nbc,3,av)
{
  edges[0]=&plot1; edges[1]=&plot2; edges[2]=&plot3;
}


/*****************************************************************************\
 *   ShadedFace4
\*****************************************************************************/


ShadedFace4::ShadedFace4(Plot &plot1, Plot &plot2,
			 Plot &plot3, Plot &plot4,
			 byte c, byte nbc, int av)
: ShadedFace(c,nbc,4,av)
{
  edges[0]=&plot1; edges[1]=&plot2; edges[2]=&plot3; edges[3]=&plot4;
}


/*****************************************************************************\
 *   MappedFace4
\*****************************************************************************/


MappedFace4::MappedFace4(Plot &plot1, Plot &plot2,
			 Plot &plot3, Plot &plot4,
			 const Image * pima, int xb, int yb, int xe, int ye,
			 int av)
: MappedFace(pima, 4, av)
{
  edges[0]=&plot1; edges[1]=&plot2; edges[2]=&plot3; edges[3]=&plot4;
  SetMapPoint(0, xb, yb);
  SetMapPoint(1, xe, yb);
  SetMapPoint(2, xe, ye);
  SetMapPoint(3, xb, ye);
}


/*****************************************************************************\
 *   MappedImage
\*****************************************************************************/


MappedImage::MappedImage(Plot & p1, Plot & p2, Plot & p3, Plot & p4,
  const Image * pima, int av)
:MappedFace4(p1, p2, p3, p4,
  pima, 0, 0, pima->Width()-1, pima->Height()-1, av)
{}


/*****************************************************************************\
 *   QuadMappedFace4
\*****************************************************************************/


QuadMappedFace4::QuadMappedFace4(Plot &plot1, Plot &plot2,
  Plot &plot3, Plot &plot4,
	const Image * pima, int xb, int yb, int xe, int ye,
  int av)
: QuadMappedFace(pima, 4, av)
{
  edges[0]=&plot1; edges[1]=&plot2; edges[2]=&plot3; edges[3]=&plot4;
  SetMapPoint(0, xb, yb);
  SetMapPoint(1, xe, yb);
  SetMapPoint(2, xe, ye);
  SetMapPoint(3, xb, ye);
}


/*****************************************************************************\
 *   QuadMappedImage
\*****************************************************************************/


QuadMappedImage::QuadMappedImage(Plot & p1, Plot & p2, Plot & p3, Plot & p4,
  const Image * pima, int av)
:QuadMappedFace4(p1, p2, p3, p4,
  pima, 0, 0, pima->Width()-1, pima->Height()-1, av)
{}


/*****************************************************************************\
 *   HypMappedFace4
\*****************************************************************************/


HypMappedFace4::HypMappedFace4(Plot &plot1, Plot &plot2,
	Plot &plot3, Plot &plot4,
	const Image * pima, int xb, int yb, int xe, int ye,
  int av)
: HypMappedFace(pima, 4, av)
{
  edges[0]=&plot1; edges[1]=&plot2; edges[2]=&plot3; edges[3]=&plot4;
  SetMapPoint(0, xb, yb);
  SetMapPoint(1, xe, yb);
  SetMapPoint(2, xe, ye);
  SetMapPoint(3, xb, ye);
}


/*****************************************************************************\
 *   HypMappedImage
\*****************************************************************************/


HypMappedImage::HypMappedImage(Plot & p1, Plot & p2, Plot & p3, Plot & p4,
  const Image * pima, int av)
:HypMappedFace4(p1, p2, p3, p4,
  ima, 0, 0, pima->Width()-1, pima->Height()-1, av)
{}
