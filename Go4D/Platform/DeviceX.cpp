/*
 * AUTHOR : Babel yoann Antras . babel@iie.cnam.fr
 * TITLE  : device.c, device.h implementation
 * CREATED: 22/10/95
 * DATE   : 28/11/95
 * BUGS   :
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ostream>
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/time.h>
#include <unistd.h>

#include "DeviceX.h"


namespace Go4D
{

//=============================================================================
//= Application

Application::Application(Device & device)
{
  fDevice = &device;
  fDevice->SetApplication(this);
}


//=============================================================================
//= Device


#define DEV_XEVENTMASK     (KeyPressMask|KeyReleaseMask|         \
                            ButtonPressMask|ButtonReleaseMask|   \
                            ButtonMotionMask|PointerMotionMask)

Device * NewDevice(int Width, int Height, const Palette *p)
{
    Device * result = new DeviceX(Width, Height, p);
    return result;
}

DeviceX::DeviceX(int lWidth, int lHeight, const Palette * pPal)
{
  fButton = 0;
  fCtrlPressed = false;
  fShiftPressed = false;

  fWidth = lWidth;
  fHeight = lHeight;
  fWindow = new Image(lWidth, lHeight);

  papplication = NULL;

  if ((fDisplay = XOpenDisplay(NULL))==NULL)
    {
      cerr << "error: unable to open X display" << endl;
      exit(1);
    }
  fScreen = DefaultScreen(fDisplay);
  fMainWin = RootWindow(fDisplay, fScreen);
  fDepth = DefaultDepth(fDisplay, fScreen);
  assert(fDepth == 8);
#ifdef DEBUG
  cerr << "fDepth = " << fDepth << endl;
#endif
  fgc = DefaultGC(fDisplay, fScreen);
  fNbColors = XDisplayCells(fDisplay, fScreen);
#ifdef DEBUG
  cerr << "fNbColors = " << fNbColors << endl;
#endif
  fVisual = DefaultVisual(fDisplay, fScreen);
  XSynchronize(fDisplay, True);
  XFlush(fDisplay);

  XSetWindowAttributes lAttr;
  unsigned long lMask = 0L;

  lAttr.event_mask = DEV_XEVENTMASK;
  lMask |= (CWBackPixel)|(CWBorderPixel);
  fXWindow =
    XCreateWindow(fDisplay, fMainWin, 0, 0,
		  fWindow->Width(), fWindow->Height(), 30, fDepth,
		  InputOutput, fVisual, lMask, &lAttr);
  XMapWindow(fDisplay, fXWindow);

#ifdef share
  fXImage =  XShmCreateImage(fDisplay, fVisual, fDepth, ZPixmap,
			     (char *)fWindow->fBuffer, &fShmInfo,
			     fWindow->Width(), fWindow->Height());

  fShmInfo.shmid = shmget(IPC_PRIVATE,
                          fXImage->bytes_per_line*fWindow->Height(),
                          IPC_CREAT|0777);
  fShmInfo.shmaddr = fXImage->data = (char *)shmat( fShmInfo.shmid, 0, 0);
  fWindow->NewBuffer((byte *)fXImage->data);
#ifdef DEBUG
  cerr << "fXImage->bpl = " << fXImage->bytes_per_line << endl;
  cerr << "fWindow->Width() = " << fWindow->Width() << endl;
  cerr << "fWindow->Height() = " << fWindow->Height() << endl;
#endif
  //fWindow->fWidth = fXImage->bytes_per_line;  // ? change + 1
  fShmInfo.readOnly = False;
  XShmAttach(fDisplay, &fShmInfo);
#else
  //assert(sizeof(Color) == 1);
  fXImage = XCreateImage(fDisplay, fVisual, fDepth, ZPixmap, 0,
			 (char *)fWindow->GetBuffer(),
			 fWindow->Width(),
			 fWindow->Height() ,
			 8, fWindow->Width());
  assert(fXImage != NULL);
#endif

#ifdef createcolor
  fColormap = XCreateColormap(fDisplay, fXWindow, fVisual, AllocAll);
#else
  fColormap = DefaultColormap(fDisplay,fScreen);
#endif

  ChangeCurrentPalette(pPal);
  XFlush(fDisplay);
}


void DeviceX::Close()
{
#ifdef createcolor
  XFreeColormap(fDisplay,fColormap);
#endif

#ifdef share
  XShmDetach(fDisplay, &fShmInfo);
#endif
  XDestroyImage(fXImage);

#ifdef share
  shmdt(fShmInfo.shmaddr);
  shmctl(fShmInfo.shmid, IPC_RMID, 0);
#endif
  XAutoRepeatOn(fDisplay);
  XCloseDisplay(fDisplay);
  XFlush(fDisplay);
}

DeviceX::~DeviceX(void)
{
  Close();
}

void DeviceX::SetApplication(Application *papp)
{
  papplication = papp;
}


void DeviceX::AutoRepeatOff()
{
  XAutoRepeatOff(fDisplay);
  XFlush(fDisplay);
}


void DeviceX::DisplayImage()
{
#ifdef share
  XShmPutImage(fDisplay, fXWindow, fgc, fXImage, 0, 0, 0, 0,
	       fWindow->Width(), fWindow->Height(), False);
#else
  XPutImage(fDisplay, fXWindow, fgc, fXImage,
	    0, 0, 0, 0, fWindow->Width(), fWindow->Height());
#endif
  XFlush(fDisplay);
}


int DeviceX::MouseCode(int wParam)
{
  int result = 0;
  switch (wParam)
    {
    case 1 : result = kMLEFT; break;
    case 3 : result = kMRIGHT; break;
    default:
      result = kNONE;
      break;
    }
  if(fCtrlPressed) result |= kCtrl;
  if(fShiftPressed) result |= kShift;
  return result;
}


word DeviceX::Code(int wParam)
{
  switch (wParam)
    {
#ifdef _LINUX_KBFR_
    case  37 :
    case 109 : return kCtrl;
    case  50 :
    case  62 : return kShift;
    case   9 : return kEND;
    case  98 : return kUP;
    case 104 : return kDN;
    case 100 : return kRI;
    case 102 : return kLE;
      //case  22 : return kBS; // BackSpace ?
    case  23 : return kTB; // Tab ?
    case  65 : return kSP;
    case  36 : return kLF;
    case  99 : return kBW; // Page UP ?
    case 105 : return kFW; // Page Down ?

    case  24 : return kKA;
    case  25 : return kKZ;
      //case  26 : return kKE;
    case  27 : return kKR;
    case  28 : return kKT;
      //case  29 : return kKY;
    case  30 : return kKU;
    case  31 : return kKI;
      //case  32 : return kKO;
    case  33 : return kKP;

    case  38 : return kKQ;
    case  39 : return kKS;
      //case  40 : return kKD;
      //case  41 : return kKF;
      //case  42 : return kKG;
      //case  43 : return kKH;
      //case  44 : return kKJ;
      //case  45 : return kKK;
      //case  46 : return kKL;
      //case  47 : return kKM;

    case  52 : return kKW;
      //case  53 : return kKX;
    case  54 : return kKC;
    case  55 : return kKV;
      //case  56 : return kKB;
    case  57 : return kKN;
#endif
    default:
      cerr << "Unsupported Key #" << wParam << endl;
      return kNONE;
    }
}


void DeviceX::StartChrono()
{
  gettimeofday(&fChronoTime, NULL);
}


int DeviceX::GetChrono()
{
  timeval currentTime;
  gettimeofday(&currentTime, NULL);
  return
    (currentTime.tv_usec-fChronoTime.tv_usec)/1000 +
    (currentTime.tv_sec-fChronoTime.tv_sec)*1000;
}


void DeviceX::AcceptImage(Image *pImage)
{
  assert( pImage->Width() == fWidth );
  assert( pImage->Height() == fHeight );
  pImage->NewBuffer((byte *)fXImage->data);
}

int DeviceX::MainLoop()
{
  XEvent lEvent;
  bool loopback = true;
  int x=0, y=0;
  int keyCode;

  assert(papplication != NULL);

  XSelectInput(fDisplay, fXWindow, DEV_XEVENTMASK);
  fDraging = false;
  fHaveMoved = false;
  while(loopback) {
    if(XCheckMaskEvent(fDisplay, DEV_XEVENTMASK, &lEvent)) {
      switch (lEvent.type) {
      case KeyPress :
	keyCode = Code(lEvent.xkey.keycode);
	fCtrlPressed = (keyCode==kCtrl);
	fShiftPressed = (keyCode==kShift);
#ifdef DEBUG
	cerr << "KeyPressed#" << keyCode << " (" << keyCode << ")" << endl;
#endif
	break;
      case KeyRelease :
	keyCode = Code(lEvent.xkey.keycode);
	fCtrlPressed = (keyCode!=kCtrl);
	fShiftPressed = (keyCode!=kShift);
#ifdef DEBUG
	cerr << "KeyReleased#" << keyCode << " (" << keyCode << ")" << endl;
#endif
	papplication->OnKeyPressed(keyCode);
	break;
      case ButtonPress :
	if(fDraging) break;
#ifdef DEBUG
	cerr << "begin DRAG (" << x << "," << y << ")" << endl;
#endif
	fButton = MouseCode(lEvent.xbutton.button);
	fDragX = x;
	fDragY = y;
	fDraging = true;
	break;
      case ButtonRelease :
	if(!fHaveMoved) {
#ifdef DEBUG
	  cerr << "mouse click " << MouseCode(lEvent.xbutton.button)
	       << " (" << x << "," << y << ")" << endl;
#endif
	  papplication->OnMouseClick(MouseCode(lEvent.xbutton.button), x, y);
	} else {
	  if(fDraging) {
#ifdef DEBUG
	    cerr << "end DRAG (" << x << "," << y << ")" << endl;
#endif
	    papplication->OnDragEnd(fButton, x, y);
	    fDraging = false;
	    fHaveMoved = false;
	  }
	}
	break;
      case MotionNotify :
	x = lEvent.xmotion.x;
	y = lEvent.xmotion.y;
#ifdef DEBUG
	cerr << "drag click " << fButton
	     << " (" << x << "," << y << ")" << endl;
#endif
	if(fDraging) {
	  papplication->OnDragOver(fButton, x-fDragX, y-fDragY);
#ifdef DEBUG
	  cerr << "draging " << x-fDragX << "," << y-fDragY << endl;
#endif
	  fDragX = x;
	  fDragY = y;
	  fHaveMoved = true;
	}
	break;
      default:
	cerr << "Unknown Event" << endl;
	break;
      }
    } else {
      papplication->Cycle();
    }
#ifdef DEBUG
    cerr << "." << flush;
#endif
  }
  return 0;
}


void DeviceX::ChangeCurrentPalette(const Palette *pPal)
{
#ifdef DEBUG
  cerr << "Changing Current Palette" << endl;
#endif
  for (int i=0; i<fNbColors; i++)
    {
      XColor lcol;
      lcol.red = ((*pPal)[i].R()*256/fNbColors)<<8;
      lcol.green = ((*pPal)[i].G()*256/fNbColors)<<8;
      lcol.blue = ((*pPal)[i].B()*256/fNbColors)<<8;
      lcol.flags = DoRed|DoGreen|DoBlue;
      lcol.pad = 0;
      lcol.pixel = i;
#ifdef createcolor
      XStoreColor(fDisplay,fColormap,&lcol);
#else
      XAllocColor(fDisplay,fColormap,&lcol);
#endif
    }
  XSetWindowColormap(fDisplay, fXWindow, fColormap);
}


}
