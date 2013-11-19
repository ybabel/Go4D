/* graphic interface for simples graphs drawing

   By  Le Mojahed
   and Le Cradinne
*/

#include "device.h"
#include "drawer.h"
#include "zdrawer.h"
#include "geom.h"

#define DIMX 300
#define DIMY 300
#define DIMSUMMIT 5

class Summit
{
  public :
  Summit();
  Summit(real x, real y);
  void SetPosition(real x, real y);
  void Draw(Drawer &, real zoom) const;
  int GetX(const Drawer &, real zoom) const;
  int GetY(const Drawer &, real zoom) const;
  friend ostream & operator << (ostream & os, const Summit &);
  
  private :
  real x, y;
};

class Connexion
{
  public :
  Connexion();
  void SetConnexion(Summit &, Summit &, real weight=0, int oriented=FALSE);
  void Draw(Drawer &, real zoom, const Fonts &) const;
  friend ostream & operator << (ostream & os, const Connexion &);

  private :
  Summit * begin, * end;
  real weight;
  int oriented;
};

class Graph
{
  public :
  Graph(const Matrix &, int dim, int oriented=FALSE);
  ~Graph();
  void Draw(Drawer &, real zoom, const Fonts &) const;
  friend ostream & operator << (ostream & os, const Graph &);

  private :
  int dim;
  int ncon;
  int oriented;
  Summit * summits;
  Connexion * connexions;
};


//*****************************************************************************

Summit::Summit()
{
  x=0; y=0;
}

void Summit::SetPosition(real px, real py)
{
  x=px;  y=py;
}

Summit::Summit(real px, real py)
{
  x=px;  y=py;
}

int Summit::GetX(const Drawer & drawer, real zoom) const
{
  return int(x*zoom)+drawer.Width()/2;
}

int Summit::GetY(const Drawer & drawer, real zoom) const
{
  return int(y*zoom)+drawer.Height()/2;
}

void Summit::Draw(Drawer & drawer, real zoom) const
{
  int sx = GetX(drawer,zoom);
  int sy = GetY(drawer,zoom);
  drawer.Ellipse(PointZ(sx, sy, 0), PointZ(sx+DIMSUMMIT, sy,0),
		 PointZ(sx, sy+DIMSUMMIT,0), 2);
}

ostream & operator << (ostream & os, const Summit & summit)
{
  os << "(" << summit.x << "," << summit.y << ")";
  return os;
}

Connexion::Connexion()
{
  begin = NULL;
  end = NULL;
  weight = 0;
  oriented = 0;
}

void Connexion::SetConnexion(Summit & sb, Summit & se, 
			     real pweight, int poriented)
{
  begin = &sb;
  end = &se;
  weight = pweight;
  oriented = poriented;
}

void Connexion::Draw(Drawer & drawer, real zoom, const Fonts & font) const
{
  assert(begin != NULL);
  assert(end != NULL);
  int bx = begin->GetX(drawer, zoom);
  int by = begin->GetY(drawer, zoom);
  int ex = end->GetX(drawer, zoom);
  int ey = end->GetY(drawer, zoom);
  int cx = (bx*2+ex)/3;
  int cy = (by*2+ey)/3;
  if (weight!=0)
    {
      drawer.Line(PointZ(bx, by,3),
		  PointZ(ex, ey, 3));
      char s[10];
      Int2Str(int(weight), s);
      int l = StrLen(s);
      int dx = font.GetDimX();
      int dy = font.GetDimY();
      drawer.Box(PointZ(cx-dx*l/2, cy-dy/2, 3),l*dx,dy);
      drawer.PutString(PointZ(cx, cy, 7), font, s);
    }
  //if (oriented) ; //not already done
}

ostream & operator << (ostream & os, const Connexion & connexion)
{
  assert(connexion.begin != NULL);
  assert(connexion.end != NULL);
  os << *(connexion.begin) << " -> " << *(connexion.end);
  os << " : " << connexion.weight;
  return os;
}

Graph::Graph(const Matrix & weights, int pdim, int poriented)
{
  assert(pdim>0);
  dim = pdim;
  summits = new Summit[dim];
  for (int i=0; i<dim; i++)
    {
      real angle=i*2*M_PI/dim;
      summits[i].SetPosition(cos(angle), sin(angle));
    }
  
  oriented = poriented;
  if (oriented)
    {
      ncon = dim*dim;
      connexions = new Connexion[ncon];
      for (int i=0; i<dim; i++)
	for (int j=0; j<dim; j++)
	  connexions[i*dim+j].SetConnexion(summits[i], 
					   summits[j], 
					   weights(j,i),
					   oriented);
    }
  else
    {
      ncon = (dim*(dim+1))/2;
      connexions = new Connexion[ncon];
      int tmp=0;
      for (int i=0; i<dim; i++)
	for (int j=0; j<=i; j++, tmp++)
	  connexions[tmp].SetConnexion(summits[i], 
				       summits[j], 
				       weights(j,i),
				       oriented);
      assert(tmp==ncon);
    }
}

void Graph::Draw(Drawer & drawer, real zoom, const Fonts & font) const
{
  for (int j=0; j<ncon; j++)
    connexions[j].Draw(drawer, zoom, font);
  for (int i=0; i<dim; i++)
    summits[i].Draw(drawer, zoom);
}

Graph::~Graph()
{
  delete [] summits;
  delete [] connexions;
}

ostream & operator << (ostream & os, const Graph & graph)
{
  for (int i=0; i<graph.ncon; i++)
    os << graph.connexions[i] << kCR;
  return os;
}

//*****************************************************************************

int Tmain(Device & device)
{
  Drawer drawer(DIMX,DIMY,0);
  device.AddImage(&drawer);
  Font6x10 font("font6x10.fon");

  Matrix g(4,4);
  g(0,0)=0;    g(0,1)=2;   g(0,2)=0;   g(0,3)=411;
               g(1,1)=1;   g(1,2)=0;   g(1,3)=-123;
                           g(2,2)=0;   g(2,3)=-22;
                                       g(3,3)=0;
  Graph gr(g, 4);
  real zoom=DIMX/4;
  byte c;

  /*
  while ((c=device.Read(&drawer))!=kEND)
    {
      switch (c)
      	{
      	  case kUP : zoom += 10; break;
      	  case kDN : zoom -= 10; break;
      	}
      drawer.Clear();
      gr.Draw(drawer, zoom, font);
      device.DisplayImage(&drawer);
    }
  */

  for (int i=0; i<200; i++)
    {
      drawer.Clear(230);
      gr.Draw(drawer, zoom, font);
      device.DisplayImage(&drawer);
      zoom += DIMX/100;
    }

  return 0;
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
						  int nCmdShow )
{
   Main::hInstance = hInstance;
   Main::hPrevInstance = hPrevInstance;
   Main::nCmdShow = nCmdShow;

   if ( ! Main::hPrevInstance ) {
      MainWindow::Register();
      }

   //MainWindow MainWnd;
   Device dev;

   Tmain(dev);

   return Main::MessageLoop();
}
