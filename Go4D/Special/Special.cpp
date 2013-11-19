/******************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : special.cpp special.h implementation
 *CREATED: 29/06/96
 *DATE   : 29/06/96
 *BUG    :
\******************************************************************************/

#include <assert.h>
#include "Special.h"

/******************************************************************************\
 *   Stereogram                                                              *
\******************************************************************************/

Stereogram::Stereogram(int width, int height, int peyedist, real pfocal)
: inherited(width, height)
{
  eyedist = peyedist;
  focal=pfocal;
  source = new byte [fLen];
  AleaSource();
}

Stereogram::~Stereogram()
{
  delete [] source;
}

void Stereogram::AleaSource()
{
  for (int j=0; j<fHeight; j++)
    for (int i=0; i<fWidth; i++)
      *(source+i+j*fWidth) = byte(random(16));
}

void Stereogram::ImageSource(const Image & ima)
{
  for (int j=0; j<fHeight; j++)
    for (int i=0; i<fWidth; i++)
      *(source+i+j*fWidth) = ima.GetPixel(i%ima.Width(), j%ima.Height());
}

Stereogram & Stereogram::operator = (const ZDrawer &input)
{
  assert( input.Width() == fWidth && input.Height() == fHeight);

  byte * buf;
  alloc(buf = new byte [fWidth+eyedist]);
  int * decal;
  alloc(decal = new int [fWidth]);

  for (int j=0; j<fHeight; j++)
    {
      for (int i0=0; i0<fWidth; i0++)
      	{
       	  buf[i0] = source[fWidth*j+i0];
       	}

      for (int k=0; k<fWidth; k++)
        {
        	decal[k]=int((double)eyedist*((1.0-focal/roundf(input.GetZ(k,j)))));
        }

      for (int i=0; i<fWidth; i++)
        {
        	buf[i+decal[i]]= buf[i];
        }

      for (int x=0; x<fWidth; x++)
      	Pixel(x,j,buf[x]);
    }
  delete [] buf;
  delete [] decal;
  return *this;
}

/******************************************************************************\
 * StarField
\******************************************************************************/

StarField::StarField(int nbstars, byte pcolor, int psize, real radius)
{
  nb_stars = nbstars;
  alloc(stars = new APlot [nbstars]);
  Rotation rot;
  for (int i=0; i<nb_stars; i++)
    {
      stars[i].Set(radius,0,0);
      stars[i].Color() = color;
      real teta=2*M_PI*frand, phi=2*M_PI*frand, psi=2*M_PI*frand;
      rot.RotateEuler(teta, phi, psi);
      stars[i] = Vector3(rot*stars[i]);
    }
  color = pcolor;
  size = psize;
}

StarField::~StarField()
{
  delete [] stars;
}

void StarField::Draw(const Camera & camera, Drawer & drawer)
{
  Rotation rot = camera.GetRotation();
  for (int i=0; i<nb_stars; i++)
    {
      APlot cur = Vector3(rot*(stars[i]));
      cur.ProjectionPerspective(camera.Focal(), drawer.MiddleX(),
                         			  drawer.MiddleY(), camera.Zoom(),
                        			  min(drawer.Width(), drawer.Height()));
      cur.Color() = color;
      if (cur[2] > 0)
   	    drawer.Box(cur.GetPointZ().SetZ(256), size,size);
    }
}

/******************************************************************************\
 * Messager
\******************************************************************************/

Messager::Messager(Fonts & pfont, int ptime, int pmaxmessages, byte pcolor)
{
  font = & pfont;
  time = ptime;
	max_messages = pmaxmessages;
	alloc(messages = new string [max_messages]);
	alloc(timer = new int [max_messages]);
	for (int i=0; i<max_messages; i++)
		{
			messages[i][0] = 0;
			timer[i] = 0;
		}
	color = pcolor;
	last = 0;
}

Messager::~Messager()
{
	delete [] messages;
	delete [] timer;
}

void Messager::PutMessage(string message)
{
	strncpy(messages[last],message,255);
	// search the older message
	/**/
	int older=10000;
	int i;
	for (i=0; i<max_messages; i++)
		if (timer[i]<older)
		{
			last = i;
			older = timer[i];
		}
	timer[last] = time;
	/**/
	//last ++;
	//last %= max_messages;
}

void Messager::Draw(Drawer & drawer)
{
	int y = 0;
	int h = font->GetDimY();

	for (int i=0; i<max_messages; i++)
			if (timer[i]!=0)
				{
					drawer.WriteString(PointZ(0,y,color,0), *font, messages[i]);
					y += h;
				}

	DecreaseTime();
}

void Messager::DecreaseTime()
{
	for (int i=0; i<max_messages; i++)
		if (timer[i]>0) timer[i]--;
}
