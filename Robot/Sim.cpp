/*****************************************************************************\
 * sim.cc
 *
 *
\*****************************************************************************/

#include <assert.h>
#include <fstream.h>
#include "sim.h"


//*****************************************************************************

#define ANGLE(i) ((i)*2*M_PI/ZONE)

Area::Area() : Object(ZONE*4+1, ZONE*4)
{
  // set the points
  plots[0] = Plot(0,0,0);
  for (int i=0; i<ZONE; i++)
		{
			plots[i+1] = Plot(cos(ANGLE(i))*LEN, sin(ANGLE(i))*LEN, 0);
			plots[i+1+ZONE] = Plot(cos(ANGLE(i))*LEN, sin(ANGLE(i))*LEN, DEPTH);
			plots[i+1+ZONE*2] = Plot(cos(ANGLE(i))*(LEN+RIGOL), sin(ANGLE(i))*(LEN+RIGOL), DEPTH);
			plots[i+1+ZONE*3] = Plot(cos(ANGLE(i))*(LEN+RIGOL), sin(ANGLE(i))*(LEN+RIGOL), WALL);
    }

  // set the faces
  #define BOW (j%2)?BLACK:WHITE
  for (int j=0; j<ZONE; j++)
		{
			NewFace3(j, 0, j+1, (j+1)%ZONE+1, byte(BOW), 1, TRUE);
      NewFace4(j+ZONE,
      	       j+1, (j+1)%ZONE+1, (j+1)%ZONE+1+ZONE, j+1+ZONE,
      	       MAG,NCOL,TRUE);
			NewFace4(j+ZONE*2,
							 j+1+ZONE, (j+1)%ZONE+1+ZONE, (j+1)%ZONE+1+ZONE*2, j+1+ZONE*2,
      	       BLUE,NCOL,TRUE);
      NewFace4(j+ZONE*3,
      	       j+1+ZONE*2,(j+1)%ZONE+1+ZONE*2,(j+1)%ZONE+1+ZONE*3, j+1+ZONE*3,
      	       PALBLUE,NCOL,TRUE);
    }

}

void Area::SetCenter(Plot & center)
{
  plots[0] = center;
}

//*****************************************************************************

Position::Position(real px, real py, real pspeed,real pangle, real inittime)
{
  x=px; y=py; speed=pspeed; ang=pangle; ti = inittime; type=0;
}
ostream & operator << (ostream & os, const Position & pos)
{
  os << pos.x << " " << pos.y << " " << pos.speed
      << " " << pos.ti << " " << pos.type << kCR;
	return os;
}

istream & operator >> (istream & is, Position &pos)
{
	is >> pos.x >> pos.y >> pos.speed >> pos.ti >> pos.type;
	return is;
}

//*****************************************************************************

PositionElem::PositionElem(real px, real py, real pspeed,real pangle, real it)
: position(px, py, pspeed,pangle,it)
{
	next = NULL;
}

PositionList::PositionList()
{
	root = NULL;
	queue = NULL;
	nbelems = 0;
	current = 0;
	oldj=0;
	oldcur=NULL;
	// default values
	convtime = CONVTIME;
	inittime = TI0;
	deltatime = PAST;
	deltaangle = PASA;
	adjusttime = ADJT;
}

PositionList::~PositionList()
{
	Flush();
}

int PositionList::SpeedUp()
{
	int speed = (*this)[current].speed;
	if (speed<15) speed++ ;
	(*this)[current].speed = speed;
	return speed;
}

int PositionList::SlowDown()
{
	int speed = (*this)[current].speed;
	if (speed>-14) speed--;
	(*this)[current].speed = speed;
	return speed;
}

real PositionList::IncrTime()
{
	return ((*this)[current].ti+=deltatime);
}

real PositionList::DecrTime()
{
	return ((*this)[current].ti-=deltatime);
}

int PositionList::Next()
{
 if (current<(nbelems-1)) current++ ;
 return current;
}

int PositionList::Previous()
{
 if (current>0) current-- ;
 return current;
}

void PositionList::AddPosition(real x, real y, real speed,real angle)
{	PPositionElem tmp;
	alloc(tmp = new PositionElem(x, y, speed,angle, inittime) );
	tmp->next = NULL;
	if (root==NULL)
		root = tmp;
	else
		queue->next = tmp;
	queue = tmp;
	nbelems++;
}

void PositionList::DelLast()
{
	PPositionElem cur=root;
	if (cur==NULL) return;
	if (cur->next == NULL)
		{
			delete root;
			root = NULL;
			queue = NULL;
			nbelems = 0;
			return;
		}
	while (cur->next->next != NULL)
		cur = cur->next;
	delete cur->next;
	queue = cur;
	cur->next = NULL;
	nbelems--;
}

void PositionList::Flush()
{
	PPositionElem tmp, cur;
	cur = root;
	while (cur != NULL)
		{
			tmp = cur->next;
			delete cur;
			cur = tmp;
		}
	root = NULL;
	queue = NULL;
	nbelems=0;
	current=0;
	oldj=0;
	oldcur=NULL;
}

int PositionList::Empty()
{
	return nbelems==0;
}

Position & PositionList::operator [] (int j)
{
	assert(j>=0);
	assert(j<nbelems);
	if (oldcur!=NULL)
	{
		if (j==oldj) return oldcur->position;
		if (j==oldj+1)
		{
			oldj++;
			oldcur=oldcur->next;
			return oldcur->position;
		}
  }
	PPositionElem cur=root;
	for (int k=0; k<j; k++)
		cur = cur->next;
	oldcur = cur;
	oldj = j;
	assert(cur!=NULL);
	return cur->position;
}

void PositionList::Calc(real ,real )
{
}

void PositionList::Save()
{
	ofstream out("robot.crb");

	out << nbelems << kCR;
	for (int i=0; i<nbelems; i++)
	{
		out << (*this)[i];
	}
}

void PositionList::Load()
{
	ifstream in("robot.crb");
	int tmp;

	Flush();
	in >> tmp;
	for (int i=0; i<tmp; i++)
	{
		Position p(0.0,0.0,0.0,angle0,inittime);
		in >> p;
		AddPosition(p.x, p.y, p.speed, p.ang);
		queue->position = p;
	}
}

void PositionList::SetConst(real ct, real it, real dt, real da, real at,
															real pangle0,real *ptabv,real *ptabtype)
{
	int i ;
	convtime = ct;
	inittime = it;
	deltatime = dt;
	deltaangle = da;
	adjusttime = at;
	angle0=pangle0 ;
	for (i=0 ; i<32 ; i++)  // 32 in robot.cst
		 {
			tabv[i]=ptabv[i];
		 }
	for (i=0 ; i<10 ; i++ )  // 10 in robot.cst
		 {
		 tabtype[i]=ptabtype[i];
		 }
}

void PositionList::BeginRead()
{
	oldi = 0;
	oldt = 0.0;
}

int PositionList::CurvLeft()
{
	int type = (*this)[current].type;
	type--;
	if (type<-3) type=-3;
	if (type>4) type=4;
	(*this)[current].type = type;
	return type;
}

int PositionList::CurvRight()
{
	int type = (*this)[current].type;
	type++;
	if (type<-3) type=-3;
	if (type>4) type=4;
	(*this)[current].type = type;
	return type;
}

void PositionList::GetInitPos(int i, real & x, real & y, real & ang)
{
  x=(*this)[i].x;
  y=(*this)[i].y;
  ang=(*this)[i].ang ;
}

void PositionList::SetInitPos(int i, real x, real y, real ang)
{
  (*this)[i].x=x;
  (*this)[i].y=y;
  (*this)[i].ang=ang;
}

void PositionList::GetInitSpeed(int i, real &v0, real &v1, int &type0, int &type1)
{
  v0=i==0?0.0:tabv[(*this)[i-1].speed+16] ;
  assert(-15<(*this)[i].speed<16);
  assert(-4<(*this)[i].type<5);
  v1=tabv[(*this)[i].speed+16];

  type0=i==0?5:(*this)[i-1].type+5 ;
  type1=(*this)[i].type+5;
}

void PositionList::ComputeNextPos(real t, real &posx, real &posy, real &ang,
  real v0, real v1, int type0, int type1)
{
  real vact=(t>=adjusttime)? v1
    : v0*(1.0-t/adjusttime)+v1*t/adjusttime;
  posx+=vact*cos(ang)*deltatime;
  posy+=vact*sin(ang)*deltatime;
  real vang=(t>=adjusttime)? tabtype[type1]
    :tabtype[type0]*(1.0-t/adjusttime)+tabtype[type1]*t/adjusttime;
  ang+=vang*vact*deltatime ;
}

int PositionList::ReadNext(real & x, real & y, real & angle)
{
	static real px;
	static real py;
	static real v0,v1;
	static real ang;
	static int type0;
	static int type1;
	if (oldi==nbelems) return FALSE;
	if (oldt==0.0)
	{
                GetInitPos(oldi, px, py, ang);
                GetInitSpeed(oldi, v0, v1, type0, type1);
		x = px;
		y = py;
		angle = ang;
		oldt+=deltatime;
		return TRUE;
	}

        ComputeNextPos(oldt, px, py, ang, v0, v1, type0, type1);

	oldt+=deltatime;
	x = px;
	y = py;
	angle = ang;
        
	if (oldt>(*this)[oldi].ti)
	{
		if (oldi<=nbelems-2)
		{
                        SetInitPos(oldi+1, px, py, ang);
		}
		oldt=0.0;
		oldi++;
	}
	return TRUE;
}

void PositionList::Trace(Drawer & drawer, const Camera & camera,
												 const Area & area)
{
	real px;
	real py;
	real v0,v1;
	real ang;
	int type0;
	int type1;
	for ( int i=0 ; i<nbelems ; i++ )
	{
                GetInitPos(i, px, py, ang);
                GetInitSpeed(i, v0, v1, type0, type1);

		int color = i==current ? GREEN : BLUE ;

		for (real t=0.0; t<=(*this)[i].ti ; t+=deltatime)
			{
				Plot begin, end;
				begin.Set(px,py, -SDIM);
                                ComputeNextPos(t, px, py, ang, v0, v1, type0, type1);
				end.Set(px,py, -SDIM);
				Draw3dLine(drawer, camera, area, begin, end,byte(t<adjusttime?RED:color));
			}
		if (i<=nbelems-2)
			{
                                SetInitPos(i+1, px, py, ang);
			}
	}
}

void PositionList::Draw3dLine(Drawer & drawer, const Camera & camera,
															const Area & area, Plot begin, Plot end,
															byte color)
{
  begin = area.GetRotation()*begin;
  begin = area.GetTranslation() + begin;
  end = area.GetRotation()*end;
  end = area.GetTranslation() + end;
  begin = camera.ChangeToBase(begin);
  end = camera.ChangeToBase(end);
  begin.ProjectionPerspective(camera.GetFocal(), drawer.MiddleX(),
			drawer.MiddleY(), camera.GetZoom(),
      minimum(drawer.Width(), drawer.Height()));
  end.ProjectionPerspective(camera.GetFocal(), drawer.MiddleX(),
    drawer.MiddleY(), camera.GetZoom(),
		minimum(drawer.Width(), drawer.Height()));
  begin.SetColor(color);
  end.SetColor(color);
	PointZ pb=begin.GetPointZ(), pe=end.GetPointZ();
  drawer.Line(pb, pe);
  pb.Decal(-1); pe.Decal(-1);
  drawer.Line(pb, pe);
  pb.Decal(2); pe.Decal(2);
  drawer.Line(pb, pe);
}

ostream & operator << ( ostream & os ,PositionList & p )
{
  for (int i=0 ; i<p.GetNbPositions() ; i++)
  {
		os << "section" << i << ":\n" ;
    os << "          sortir(prop=(" ;

		os << ((p[i].speed>=0)?"av,":"ar,") ;
    os << ((p[i].speed==0)?"off,":"on,") << abs(p[i].speed) << "),outil=(ar,off)" ;
    os 	<< ",levier=(av,on),dir="	<< int(90+p[i].type*10)
      	<< ",diode1=off,diode2=off)\n" ;
		os << "          pour " << (int)(p[i].ti*p.convtime) << " {\n\n\n" ;
    os << "          }\n" ;
	}
  return(os);
}
