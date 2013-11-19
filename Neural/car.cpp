#include <time.h>
#include "car.h"

#define kColor 30
#define XB(i) boxes[i].xb
#define YB(i) boxes[i].yb
#define XE(i) boxes[i].xe
#define YE(i) boxes[i].ye

/*
  ------------------------------------------------------------
  Scenery
  ------------------------------------------------------------
  */

int Scenery::Collide(int x, int y)
{ // circular scene
#ifndef wrap
  if ((x<=0) || (x>=fWidth)) return 1 ;
  if ((y<=0) || (y>=fHeight)) return 1 ;
#endif
  return GetPixel(x,y) != 0; // more efficient
}

int Scenery::Prof(real x, real y, real theta, real maxprof)
{
  real l = real(fWidth);
  real h = real(fHeight);
  if (!( x>=0 && x<l && y>=0 && y < h )) return 1;
  real c = cos(theta);
  real s = sin(theta);
  int result=0;
#ifdef wrap
  while ( (GetPixel(int(x)%fWidth,int(y)%fHeight)==0) && result<maxprof )
    //maximum vision depth
#else
  while ( ( x>=0 && x<l && y>=0 && y < h ) &&
	 (GetPixel(int(x),int(y))==0) && result < maxprof)
#endif
    { x+=c; y+=s; result++; }
  return result;
}

void Scenery::Generate(int nb, int h)
{
  int nbboxes = nb;
  Box * boxes = new Box [nbboxes];
  int boxh = h;
  srand(time(NULL));
  ClearAll();
  for (int i=0; i<nbboxes; i++)
    {
      do
	{
	  XB(i) = random(fWidth-boxh);
	  YB(i) = random(fHeight-boxh);
	}
      while ( (XB(i)<40 && YB(i)<40) );
      XE(i) = XB(i)+boxh;
      YE(i) = YB(i)+boxh;
    }
#ifndef wrap
  Line(0,0,fWidth-1,0, kColor);
  Line(0,0,0,fHeight-1, kColor);
  Line(fWidth-1,0,fWidth-1,fHeight-1, kColor);
  Line(fWidth-1,fHeight-1,0,fHeight-1, kColor);
#endif
  for (int i1=0; i1<nbboxes; i1++)
    {
      Fill4(XB(i1), YB(i1), XE(i1), YB(i1),
	    XE(i1), YE(i1), XB(i1), YE(i1), kColor);
    }
  if (boxes != NULL) delete boxes;
}

/*
  ------------------------------------------------------------
  Porsche
  ------------------------------------------------------------
  */

Porsche::Porsche(Scenery & scene, int vix, int viy, real factort,
		 real factord, real speed, real vmi, real vma, real fro)
{
  x = 10; y = 10; v = speed; theta = M_PI/4; vtheta=0; a=0;
  collision = 0;
  scn = &scene;
  distance = 0;
  visionx = vix; visiony = viy;
  factortheta = factort;
  factordepth = factord;
  vmin = vmi;
  vmax = vma;
  vision = new real [visionx*visiony];
  frott = fro;
}

Porsche::~Porsche()
{
  delete vision;
}

void Porsche::Init(real speed)
{
  x=10; y=10; v = speed; collision = 0;
  distance = 0; theta = M_PI/4; vtheta=0; a=0;
}

void Porsche::SetAccel(real acc)
{
  a=acc;
}

void Porsche::SetRotation(real rot)
{
  vtheta=rot;
  vtheta=vtheta<-M_PI/3?-M_PI/3:vtheta;
  vtheta=vtheta>M_PI/3?M_PI/3:vtheta;
}

void Porsche::Iterate(Drawer2D & d)
{
  int oldx = int(x); int oldy = int(y);
  theta+=vtheta*dt;
  theta= theta<0?theta+2*M_PI:theta;
  theta= theta>2*M_PI?theta-2*M_PI:theta;
  v+=(a-frott*v)*dt;
  v = v<vmin?vmin:v;
  v = v>vmax?vmax:v;
  x+=v*cos(theta)*dt;
  y+=v*sin(theta)*dt;
  real l = real(scn->Width());
  real h = real(scn->Height());
  int flag=0;
#ifdef wrap
  if (x<0) {x+=l; flag=1; }
  if (x>l) {x-=l; flag=1; }
  if (y<0) {y+=h; flag=1; }
  if (y>h) {y-=h; flag=1; }
#endif
  collision=scn->Collide( int(x), int(y));
  if (collision) { x = real(oldx); y = real(oldy); }
  if ( (&d!=NULL) && !flag)
    d.Line(oldx,oldy, int(x), int(y), kColor+64 );
  distance += v*dt*(x/l)*(y/h);
}


/*
   when thereis something in the visual field, a neuron and all the
   neurons "behind him" are activated, there is neuron associated
   with evry distance in the visual field (in fact there is a
   neuron evry step of distance where step=visiondepth/nbneurons
   the neurons that are not activated are set to -1
*/
void Porsche::Vision(Drawer2D & win)
{
  real step=M_PI/300 * factortheta;
  real ttheta = theta - step*visionx/2;
  for (int i=0; i<visionx; i++, ttheta+=step)
    {
      byte coul = (i==visionx/2)?kColor-3:kColor+1;
      int dist = scn->Prof(x, y, ttheta, visiony*factordepth);
      if (&win!=NULL)
	win.Line(int(x), int(y),
		 int(x+real(dist)*cos(ttheta)),
		 int(y+real(dist)*sin(ttheta)), coul);
      for (int j=0; j<visiony; j++) vision[i+j*visionx] = 0;
      int ps = int (dist/factordepth);
      for (int j1= ps; j1<visiony; j1++)
        vision[i+j1*visionx] = 1;
      //ps = ps>=visiony?visiony-1:ps;
      //vision[i+ps*visionx] = 1;
    }
}

void MapPorsche::Vision(Drawer2D & win)
{
  real s = sin(theta)*visionx*real(factortheta);
  real c = cos(theta)*visionx*real(factortheta);
  int Ax = (int)( x - s/2);
  int Ay = (int)( y + c/2);
  int Bx = (int)( x + s/2);
  int By = (int)( y - c/2);

  int Dx = (int)( Ax + cos(theta)*visiony*factordepth -s );
  int Dy = (int)( Ay + sin(theta)*visiony*factordepth +c );
  int Cx = (int)( Bx + cos(theta)*visiony*factordepth +s );
  int Cy = (int)( By + sin(theta)*visiony*factordepth -c );

  if (&win!=NULL)
    {
      win.Line(Ax, Ay, Bx, By, kColor+32);
      win.Line(Bx, By, Cx, Cy, kColor+32);
      win.Line(Cx, Cy, Dx, Dy, kColor+32);
      win.Line(Dx, Dy, Ax, Ay, kColor+32);
    }

  viewer.Map4(*scn, Ax,Ay,Bx,By,Cx,Cy,Dx,Dy,
	      0,0, visionx-1,0, visionx-1,visiony-1, 0, visiony-1 );
  for (int i=0; i<visionx; i++)
    for (int j=0; j<visiony; j++)
      vision[i+j*visionx] = real(viewer.GetPixel(i,j)==0);
}


