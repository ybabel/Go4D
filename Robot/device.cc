/*****************************************************************************\
 * AUTHOR : Babel yoann Antras . babel@iie.cnam.fr
 * TITLE  : device.c, device.h implementation
 * CREATED: 22/10/95
 * DATE   : 29/02/96
 * BUGS   : share memory seem to be bugged
\*****************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef LIBGPP
#include <ostream.h>
#endif

#include <assert.h>
#include "device.h"

#ifdef text
#include <vga.h>
#endif

struct BaseCol
{
  unsigned long Red;
  unsigned long Green;
  unsigned long Blue;
};

struct BaseCol kBase [16] =
{       
  {  128L,   0L,   0L}, //0    RED
  {  128L,  32L,   0L}, //16
  {  128L,  64L,   0L}, //32
  {  128L,  96L,   0L}, //48
  {  128L, 128L,   0L}, //64
  {  0L,   128L,   0L}, //80   GREEN
  {  0L,   128L,  32L}, //96
  {  0L,   128L,  64L}, //112
  {  0L,   128L,  96L}, //128
  {  0L,   128L, 128L}, //144
  {  0L,     0L, 128L}, //160  BLUE
  {  32L,    0L, 128L}, //176
  {  64L,    0L, 128L}, //192
  {  96L,    0L, 128L}, //208
  {  128L,   0L, 128L}, //224
  {  128L, 128L, 128L}  //240 WHITE
};

#ifndef text

unsigned short func(unsigned long col, unsigned long fdiv)
{
  if (col==0) return 0;
  return ((fdiv+9)*col/16-1)<<8;
}

Device::Device()
{
  char * lDisplayName = NULL;
  if ((fDisplay = XOpenDisplay (lDisplayName))==NULL)
    error("unable to open server");
  fScreen = DefaultScreen(fDisplay);
  fMainWin = RootWindow(fDisplay, fScreen);
  fDepth = DefaultDepth(fDisplay, fScreen);
  fgc = DefaultGC(fDisplay, fScreen);
  fNbColors = XDisplayCells(fDisplay, fScreen);
  fWidth = XDisplayWidth(fDisplay, fScreen);
  fHeight = XDisplayHeight(fDisplay, fScreen);
  fVisual = DefaultVisual(fDisplay, fScreen);
  fNbWin = 0;
  XSynchronize(fDisplay, True);
  oldmove = kNONE;
  XFlush(fDisplay);
}

Device::~Device(void)
{
  for (long i=0; i<fNbWin; i++)
    {

#ifdef createcolor
      XFreeColormap(fDisplay,fListWin[i].fColormap);
#endif

#ifdef share
      XShmDetach(fDisplay, &fListWin[i].fShmInfo);
#endif
      XDestroyImage(fListWin[i].fXImage);

#ifdef share
      shmdt(fListWin[i].fShmInfo.shmaddr);
      shmctl(fListWin[i].fShmInfo.shmid, IPC_RMID, 0);
#endif
    }
  XAutoRepeatOn(fDisplay);
  XCloseDisplay(fDisplay);
  XFlush(fDisplay);
}
 
void Device::AutoRepeatOff()
{
  XAutoRepeatOff(fDisplay);
  XFlush(fDisplay);
}

#define SW fListWin[fNbWin]
  
void Device::AddImage(Image * pWindow)
{
  XSetWindowAttributes lAttr;
  unsigned long lMask = 0L;
  if (fNbWin == kMaxWin) 
    error("unable to create new window");
  fListWin[fNbWin].fWindow = pWindow;
  lAttr.event_mask = ButtonPressMask|KeyPressMask|PointerMotionMask;
  lMask |= (CWBackPixel)|(CWBorderPixel);
  fListWin[fNbWin].fXWindow = 
    XCreateWindow(fDisplay,fMainWin,0,0,
		  pWindow->Width(), pWindow->Height(),30,fDepth,InputOutput,
		  fVisual,lMask,&lAttr);
  XMapWindow(fDisplay,fListWin[fNbWin].fXWindow);
  
#ifdef share
  // create an XImage in shared memory instead
  SW.fShmInfo.shmid = 
    shmget(IPC_PRIVATE,pWindow->Width()*pWindow->Height(), 
	   IPC_CREAT|0777);
  if (SW.fShmInfo.shmid==-1) error("unable to allocated shm mem");
  SW.fXImage = 
    XShmCreateImage(fDisplay, fVisual, fDepth, ZPixmap, 
		    (char *)pWindow->GetBuffer(), 
		    &SW.fShmInfo, pWindow->Width(), pWindow->Height());
  SW.fShmInfo.shmaddr = SW.fXImage->data = 
    (char *)shmat( SW.fShmInfo.shmid, 0, 0);
  if (SW.fShmInfo.shmaddr == (char*) -1) error("unable to attach memory");
  pWindow->NewBuffer((byte *)SW.fXImage->data);
  pWindow->fWidth = SW.fXImage->bytes_per_line;  // ? change + 1  
    SW.fShmInfo.readOnly = False;
  XShmAttach(fDisplay, &SW.fShmInfo);
#else
  fListWin[fNbWin].fXImage = 
    XCreateImage(fDisplay, fVisual, fDepth, ZPixmap, 0, 
		 (char *)pWindow->GetBuffer(), 
		 pWindow->Width(), pWindow->Height() , 8, pWindow->Width());
#endif
  
#ifdef createcolor  
  fListWin[fNbWin].fColormap = 
    XCreateColormap(fDisplay, fListWin[fNbWin].fXWindow, fVisual, AllocAll);
#else
  fListWin[fNbWin].fColormap = DefaultColormap(fDisplay,fScreen);
#endif
  pWindow->fBufChange = 1; 
    // to avoid the other destructor to liberate the merory that 
    // is automatically freed by XDestroyImage 
  fNbWin++;
  XFlush(fDisplay);
}

void Device::AcceptImage(Image *)
{
}

void Device::DisplayImage(Image * pWindow)
{
  ScreenWin scrw = GetScreenWin(pWindow);
#ifdef share
  XShmPutImage(fDisplay, scrw.fXWindow, fgc, scrw.fXImage, 0,0,0,0, 
	       scrw.fWindow->Width()-1, scrw.fWindow->Height(), False );
#else
  XPutImage(fDisplay, scrw.fXWindow, fgc, scrw.fXImage, 
	    0,0,0,0, scrw.fWindow->Width(), scrw.fWindow->Height() );
#endif
  XFlush(fDisplay);
}

word Device::Code(int c)
{
  switch (c)
    {
#ifdef school
    case 170 : return kUP;
    case 169 : return kDN;
    case 168 : return kRI;
    case 167 : return kLE;
    case 210 : return kFW;
    case 217 : return kBW;
    case 236 : return kLO;
    case 221 : return kHI;
    case 212 : return kSP;
    case 225 : return kKU;
    case 230 : return kKI;
    case 193 : return kKQ;
    case 198 : return kKW;
    case 194 : return kKA;
    case 195 : return kKZ;
    case 190 : return kTB;
    case 189 : return kLF;
    case 191 : return kEND;
    case 188 : return kDEL;
#endif
#ifdef home
    case 98  : return kUP;
    case 104 : return kDN;
    case 102 : return kRI;
    case 100 : return kLE;
    case 41  : return kFW;
    case 56  : return kBW;
    case 46  : return kLO;
    case 43  : return kHI;
    case 65  : return kSP;
    case 30  : return kKU;
    case 31  : return kKI;
    case 38  : return kKQ;
    case 52  : return kKW;
    case 24  : return kKA;
    case 25  : return kKZ;
    case 23  : return kTB;
    case 36  : return kLF;
    case 9   : return kEND;
    case 22  : return kDEL;
#endif
#ifdef wotan
    case 41  : return kSP;
    case 97  : return kLE;
    case 96  : return kDN;
    case 106 : return kRI;
    case 99  : return kUP;
    case 28  : return kKA;
    case 26  : return kKZ;
    case 43  : return kFW;
    case 50  : return kBW;
    case 51  : return kHI;
    case 75  : return kLO;
    case 13  : return kTB;
    case 90  : return kLF;
#endif
#ifdef LIBGPP
      default : cout << c << kCR;
#else
      default : printf("%d\n",c);
#endif
      return kNONE;
    }
}

word Device::Wait(Image * pWindow)
{
  XSelectInput(fDisplay,GetScreenWin(pWindow).fXWindow,
	       ButtonPressMask|KeyPressMask|KeyReleaseMask);
  XEvent lEvent;
  XNextEvent(fDisplay,&lEvent);
  switch (lEvent.type)
    {
    case ButtonPress  : return kEND;
    case KeyPress : return Code(lEvent.xkey.keycode);
    }
  return kNONE;
}

word Device::Read(Image * pWindow)
{
  XSelectInput(fDisplay,GetScreenWin(pWindow).fXWindow,
	       ButtonPressMask|KeyPressMask);
  XEvent lEvent;
  lEvent.xkey.keycode=kNONE;
  while(XCheckMaskEvent(fDisplay,KeyReleaseMask|PointerMotionMask,&lEvent));
  while(XCheckMaskEvent(fDisplay,ButtonPressMask|KeyPressMask,&lEvent));
  switch ( lEvent.type )
    {
    case ButtonPress  : return kEND;
    case KeyPress : return Code(lEvent.xkey.keycode);
    }   
  return kNONE;
}

word Device::ReadMouse(Image * pWindow, int & x, int & y)
{
  XSelectInput(fDisplay,GetScreenWin(pWindow).fXWindow,
	       ButtonPressMask|KeyPressMask);
  XEvent lEvent;
  lEvent.xkey.keycode=kNONE;
  while(XCheckMaskEvent(fDisplay,KeyReleaseMask|PointerMotionMask,&lEvent));
  while(XCheckMaskEvent(fDisplay,ButtonPressMask|KeyPressMask,&lEvent));
  switch ( lEvent.type )
    {
    case ButtonPress  : 
      x = lEvent.xbutton.x; 
      y = lEvent.xbutton.y;
      switch (lEvent.xbutton.button)
	{
	  case 1 : return kMLEFT;
	  case 2 : return kMMIDDLE;
	  case 3 : return kMRIGHT;
	}
      return kMLEFT;
    case KeyPress : return Code(lEvent.xkey.keycode);
    }   
  return kNONE;
}

word Device::Move(Image * pWindow)
{
  XSelectInput(fDisplay,GetScreenWin(pWindow).
	       fXWindow,ButtonPressMask|KeyPressMask|KeyReleaseMask);
  XEvent lEvent;
  lEvent.xkey.keycode=kNONE;
  while(XCheckMaskEvent(fDisplay,ButtonPressMask|KeyPressMask,&lEvent))
    switch ( lEvent.type )
      {
      case ButtonPress  : return kEND;
      case KeyPress :
	switch(Code(lEvent.xkey.keycode))
	  {
	  case kUP : oldmove = oldmove & ~kKeyMask | kUP & ~kDNMask | kUPMask; 
	    break;
	  case kDN : oldmove = oldmove & ~kKeyMask | kDN & ~kUPMask | kDNMask;
	    break;
	  case kLE : oldmove = oldmove & ~kKeyMask | kLE & ~kRIMask | kLEMask;
	    break;
	  case kRI : oldmove = oldmove & ~kKeyMask | kRI & ~kLEMask | kRIMask;
	    break;
	    default : 
	      oldmove = oldmove & ~kKeyMask | Code(lEvent.xkey.keycode); break;
	  } break;
      }
  while(XCheckMaskEvent(fDisplay,KeyReleaseMask,&lEvent))
    switch ( lEvent.type )
      {
      case KeyRelease :
	switch(Code(lEvent.xkey.keycode))
	  {
	  case kUP : oldmove = oldmove & ~kUPMask & ~kKeyMask | kNONE; break;
	  case kDN : oldmove = oldmove & ~kDNMask & ~kKeyMask | kNONE; break;
	  case kLE : oldmove = oldmove & ~kLEMask & ~kKeyMask | kNONE; break;
	  case kRI : oldmove = oldmove & ~kRIMask & ~kKeyMask | kNONE; break;
	    default : oldmove = oldmove & ~kKeyMask | kNONE; break;
	  } break;
      }
  return oldmove;
}

ScreenWin & Device::GetScreenWin(Image * pWindow)
{
  long i;
  for (i=0; i<fNbWin; i++)
    if ( fListWin[i].fWindow == pWindow) break;
  assert(i!=fNbWin);
  return fListWin[i];
}

void Device::RedPalette(Image * pWindow)
{
  ScreenWin ScrW = GetScreenWin(pWindow);
  for (long i=0; i<fNbColors; i++)
    {             
      XColor lcol;
      lcol.red = (i*256/fNbColors)<<8;
      lcol.green = 0;
      lcol.blue = 0;
      lcol.flags = DoRed|DoGreen|DoBlue;
      lcol.pad = 0;
      lcol.pixel = i;
#ifdef createcolor  
      XStoreColor(fDisplay,ScrW.fColormap,&lcol);
#else
      XAllocColor(fDisplay,ScrW.fColormap,&lcol);
#endif
    }
  XSetWindowColormap(fDisplay, ScrW.fXWindow, ScrW.fColormap);
}

void Device::BlackWhitePalette(Image * pWindow)
{
  ScreenWin ScrW = GetScreenWin(pWindow);
  for (long i=0; i<fNbColors; i++)
    {             
      XColor lcol;
      lcol.red = (i*256/fNbColors)<<8;
      lcol.green = (i*256/fNbColors)<<8;
      lcol.blue = (i*256/fNbColors)<<8;
      lcol.flags = DoRed|DoGreen|DoBlue;
      lcol.pad = 0;
      lcol.pixel = i;
#ifdef createcolor  
      XStoreColor(fDisplay,ScrW.fColormap,&lcol);
#else
      XAllocColor(fDisplay,ScrW.fColormap,&lcol);
#endif
    }
  XSetWindowColormap(fDisplay, ScrW.fXWindow, ScrW.fColormap);
}

void Device::VoxelPalette(Image * pWindow)
{
  ScreenWin ScrW = GetScreenWin(pWindow);
  for (long i=0; i<fNbColors; i++)
    {             
      XColor lcol;
      if (i < fNbColors-2)
      { 
	lcol.red = (i*256/fNbColors)<<8;
	lcol.green = 0;
	lcol.blue = 0;
      }
      else
	if (i==fNbColors-2)
	  { lcol.red=0; lcol.green=50<<8; lcol.blue=200<<8; }
	else
	  { lcol.red=0; lcol.green=0<<8; lcol.blue=255<<8; }      
      lcol.flags = DoRed|DoGreen|DoBlue;
      lcol.pad = 0;
      lcol.pixel = i;
#ifdef createcolor  
      XStoreColor(fDisplay,ScrW.fColormap,&lcol);
#else
      XAllocColor(fDisplay,ScrW.fColormap,&lcol);
#endif
    }
  XSetWindowColormap(fDisplay, ScrW.fXWindow, ScrW.fColormap);
}

void Device::DefaultPalette(Image * pWindow)
{
  ScreenWin ScrW = GetScreenWin(pWindow);
  for (int i=0; i<fNbColors/16; i++)
    {        
      for (int j=0; j<16; j++)
	{         
	  XColor lcol;
	  lcol.red = func(kBase[i].Red,j);
	  lcol.green = func(kBase[i].Green,j);
	  lcol.blue = func(kBase[i].Blue,j);
	  lcol.flags = DoRed|DoGreen|DoBlue;
	  lcol.pad = 0;
	  lcol.pixel = i*16+j;
#ifdef createcolor  
	  XStoreColor(fDisplay,ScrW.fColormap,&lcol);
#else
	  XAllocColor(fDisplay,ScrW.fColormap,&lcol);
#endif
	}
    }
  XSetWindowColormap(fDisplay, ScrW.fXWindow, ScrW.fColormap);
}

void Device::ImaPalette(Image * pWindow, ImagePal & im)
{
  ScreenWin ScrW = GetScreenWin(pWindow);
  for (int i=0; i<fNbColors; i++)
    {
      XColor lcol;
      lcol.red = (im.Pal()[i][0]*256/fNbColors)<<8;
      lcol.green = (im.Pal()[i][1]*256/fNbColors)<<8;
      lcol.blue = (im.Pal()[i][2]*256/fNbColors)<<8;
      lcol.flags = DoRed|DoGreen|DoBlue;
      lcol.pad = 0;
      lcol.pixel = i;
#ifdef createcolor  
      XStoreColor(fDisplay,ScrW.fColormap,&lcol);
#else
      XAllocColor(fDisplay,ScrW.fColormap,&lcol);
#endif      
    }
  XSetWindowColormap(fDisplay, ScrW.fXWindow, ScrW.fColormap);
}

void Device::setcolor(int col, int r, int g, int b, Image * pWindow)
{
#ifdef createcolor
  ScreenWin ScrW = GetScreenWin(pWindow);
  XColor lcol;
  lcol.red = r<<8;
  lcol.green = g<<8;
  lcol.blue = b<<8;
  lcol.flags = DoRed|DoGreen|DoBlue;
  lcol.pad = 0;
  lcol.pixel = col; 
  XStoreColor(fDisplay,ScrW.fColormap,&lcol);  
#else
  col=r=g=b=0; //warning
  pWindow=NULL;
#endif
}

#else





//-----------------------------------------------------------------------------
// this part had been written for linux text mode (no x window)
//-----------------------------------------------------------------------------





unsigned short func(unsigned long col, unsigned long fdiv)
{
  if (col==0) return 0;
  return ((fdiv+9)*col/16-1);
}


Device::Device(void)
{
  if (vga_setmode(G320x200x256)!=0) exit(1);
  fNbColors = 256;
  fWidth = 320;
  fHeight = 200;
}

Device::~Device(void)
{
   vga_setmode(TEXT);
}

#define SW fListWin[fNbWin]
  
void Device::AddImage(Image * pWindow)
{
  if (pWindow->Width()!=320 || pWindow->Height()!=200)
    pWindow->Resize(320,200);
}

void Device::AcceptImage(Image * pWindow)
{
  if (pWindow->Width()!=320 || pWindow->Height()!=200)
    pWindow->Resize(320,200);
}

void Device::DisplayImage(Image * pWindow)
{
   int l=pWindow->fWidth;
   unsigned char * Buf = (unsigned char *)pWindow->GetBuffer();
   for (int i=0; i<200; i++)
     vga_drawscanline(i,(Buf+i*l));
}

word Device::Code(int)
{
   unsigned char c=vga_getkey();
   switch (c)
     {
      case 'u' : return kUP;
      case 'n' : return kDN;
      case 'k' : return kRI;
      case 'h' : return kLE;
      case 'f' : return kFW;
      case 'b' : return kBW;
      case 'l' : return kLO;
      case 'o' : return kHI;
      case 't' : return kTB;
      case 'a' : return kKA;
      case 'z' : return kKZ;
      case 'q' : return kKQ;
      case 'w' : return kKW;
      case 'i' : return kKI;
      case 'j' : return kKU;
      case ' ' : return kSP;
      case 27  : return kEND;
      default  : return c;
     }  
}

word Device::Wait(Image *)
{
  return Code(0);
}

word Device::Read(Image *)
{
  return Code(0);
}

word Device::ReadMouse(Image * , int & , int & )
{
  return Code(0);
}

word Device::Move(Image *)
{
  return Code(0);
}

void Device::RedPalette(Image * ima)
{
  for (int i=0; i<fNbColors; i++)
    {             
      setcolor(i,(i*63/fNbColors),0,0,ima);
    }
}

void Device::BlackWhitePalette(Image *ima)
{
  for (int i=0; i<fNbColors; i++)
    {             
      setcolor(i,(i*63/fNbColors),(i*63/fNbColors),(i*63/fNbColors),ima);
    }
}

void Device::VoxelPalette(Image * ima)
{
  for (int i=0; i<fNbColors; i++)
    {             
      if (i<fNbColors-2)
	setcolor(i,(i*63/fNbColors),0,0,ima);
      else if (i==fNbColors-2) setcolor(fNbColors-2, 0,50,200,ima);
      else setcolor(fNbColors-1,0,0,255,ima);
    }
}

void Device::DefaultPalette(Image * ima)
{
  for (int i=0; i<fNbColors/16; i++)
    {        
      for (int j=0; j<16; j++)
	{         
	  setcolor(i*16+j, func(kBase[i].Red,j),
		   func(kBase[i].Green,j),
		   func(kBase[i].Blue,j), ima);
	}
    }
}

void Device::ImaPalette(Image * ima, ImagePal & im)
{
  for (int i=0; i<fNbColors; i++)
    {
      setcolor(i,im.Pal()[i][0],im.Pal()[i][1],im.Pal()[i][2], ima);
    }
}

void Device::setcolor(int col, int r, int g, int b, Image *)
{
  vga_setpalette(col,r,g,b);
}
#endif
