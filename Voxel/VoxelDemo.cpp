//---------------------------------------------------------------------------
#include "VoxelDemo.h"

MainApp::MainApp(Device & aDevice)
: inherited(aDevice),
voxeldrawer(fImage, aDevice.Width(), aDevice.Height()),
m(NbColors)
{
  x=0; y=30; d=64;
  z = -MaxZ*4+200;
  t=0;
  margedepth = 0;
  stop = 0;
  penteadd = -0.3;

  m.margedepth = margedepth;
  m.penteadd = penteadd;
  following = 1;
  m.BlurZ();
  m.BlurZ0();
  m.BlurZ1();
  m.BlurZ2();
  m.SetColor();
  m.BlurC();
  m.Satur();
  m.precalc(z,d,t,voxeldrawer);
}

#define Step 2

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
  case kUP : y+=Step*cos(t); x+=Step*sin(t); break;
  case kDN : y-=Step*cos(t); x-=Step*sin(t); break;
  case kLE : t-=M_PI/32; m.precalc(z,d,t,voxeldrawer); break;
  case kRI : t+=M_PI/32; m.precalc(z,d,t,voxeldrawer); break;
  case kFW :
    prof += 1;
#ifdef LIBGPP
    cout << "prof=" << prof << kCR;
#endif
    m.precalc(z,d,t,voxeldrawer);
    break;
  case kBW :
    prof -= 1;
#ifdef LIBGPP
    cout << "prof=" << prof << kCR;
#endif
    m.precalc(z,d,t,voxeldrawer);
    break;
  case kLO :
    margedepth += 1;
#ifdef LIBGPP
    cout << "margedepth=" << margedepth << kCR ;
#endif
    m.margedepth = margedepth;
    m.precalc(z,d,t,voxeldrawer);
    break;
  case kHI :
    margedepth -= 1;
    m.margedepth = margedepth;
#ifdef LIBGPP
    cout << "margedepth=" << margedepth << kCR ;
#endif
    m.precalc(z,d,t,voxeldrawer);
    break;
  case kSP : stop = 1-stop; break;
    //case kKU : scr.DefaultPalette(&voxeldrawer); break;
    //case kKI : scr.VoxelPalette(&voxeldrawer); break;
  case kKQ :
    z+=echy;
    m.precalc(z,d,t,voxeldrawer);
#ifdef LIBGPP
    cout << "z=" << z << kCR;
#endif
    break;
  case kKW :
    z-=echy;
    m.precalc(z,d,t,voxeldrawer);
#ifdef LIBGPP
    cout << "z=" << z << kCR;
#endif
    break;
  case kKA :
    d+=Step*8;
    m.precalc(z,d,t,voxeldrawer);
#ifdef LIBGPP
    cout << "d=" << d << kCR;
#endif
    break;
  case kKZ :
    d-=Step*8;
    m.precalc(z,d,t,voxeldrawer);
#ifdef LIBGPP
    cout << "d=" << d << kCR;
#endif
    break;
  case kTB : following = 1-following; break;
  default : inherited::OnKeyPressed(aKey);
  }
}


void MainApp::Draw()
{
  //fImage.Clear(255);
  real zmin = (m.MapZ(x+Step*sin(t)*(focal+1),
    y+Step*cos(t)*(focal+1))*echy - echy*10);
  if ( (z > zmin) || following )
  {
    z=  zmin;
    m.precalc(z,d,t,voxeldrawer);
  }

  //m.PutMapZ(voxeldrawer);
  //m.PutColor(voxeldrawer);
  m.VOXEL(x,y,voxeldrawer);

  inherited::Draw();
}


