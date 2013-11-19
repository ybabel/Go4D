/*****************************************************************************\
* AUTHOR : Babel yoann Antras . babel@iie.cnam.fr
* TITLE  : device.cpp, device.h implementation
* CREATED: 22/10/95
* DATE   : 17/12/00
* CURRENT: testing to work under PocketPC, begin to work
* BUGS   : not tested
\*****************************************************************************/

#include "Device_CE.h"

namespace Go4D
{

  /************************************\
  * class Main
  \************************************/

#define kszCLASSNAME L"3D Demo"
#define kszTITLE L"3D Demo"

  HINSTANCE Main::hInstance = 0;
  HINSTANCE Main::hPrevInstance = 0;
  int Main::nCmdShow = 0;

  int Main::MessageLoop(Application & application)
  {
    MSG msg;
    // Main message loop:
    while (1)
    {
      application.Cycle();
      if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
      {
        if (msg.message == WM_QUIT)
        {
          break;
        }
        GetMessage( &msg, 0, 0, 0 );
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    return msg.wParam;
  }


  /************************************\
  * class Window
  \************************************/


  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
    return DeviceCE::MainDevice->WndProc( message, wParam, lParam );
  }


void play( unsigned short* s )
{
	PlaySound( s, NULL, SND_ASYNC + SND_FILENAME + SND_NOWAIT );
}

void stopsound( void )
{
	PlaySound( 0, NULL, SND_FILENAME + SND_ASYNC );
}



  /************************************\
  * class MainWindow
  \************************************/

  void MainWindow::Register( void )
  {
    WNDCLASS	wc;

    //wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.style			= 0;
    wc.lpfnWndProc		= (WNDPROC) Go4D::WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= Main::hInstance;
    wc.hIcon			= 0;
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName = kszCLASSNAME;

    if (! RegisterClass(&wc) )
      error("could not register window class");
  }

  MainWindow::MainWindow(int & Width, int & Height, const Palette * pPal)
  {
    fPalette = pPal;
    HWND	hWnd = NULL;
    hWnd = FindWindow(kszCLASSNAME, kszTITLE);	
    if (hWnd) 
    {
      SetForegroundWindow ((HWND) (((DWORD)hWnd) | 0x01));    
      return;
    } 

    hWnd = CreateWindow(kszCLASSNAME, kszTITLE, WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, 240, 320, NULL, NULL, Main::hInstance, NULL);
    if (!hWnd)
    {	
      error("could not create window");
      return;
    }

    // for palette changing
    hdc = GetWindowDC(hWnd);


    // Palette handling
    lpMem = (LOGPALETTE *) LocalAlloc (LMEM_FIXED, 
      sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 256);
    assert(lpMem != NULL);
    lpMem->palNumEntries = (WORD) 256;
    lpMem->palVersion = (WORD) 0x0300;
    CopyPalette(pPal);
    SetPalette();

    // Try opening the Display for Fullscreen access
    if (GXOpenDisplay(hWnd, GX_FULLSCREEN) == 0) {
      error("could not open GX display");
      return;				// we won't be able to draw.
    }

    // Initialize the Hardware Buttons
    GXOpenInput();

    // Get the Display properties
    g_gxdp = GXGetDisplayProperties();
    // Get information about the Hardware Keys
    g_gxkl = GXGetDefaultKeys(GX_NORMALKEYS);

    SHFullScreen( hWnd, SHFS_HIDETASKBAR + SHFS_HIDESIPBUTTON + SHFS_HIDESTARTICON );
    Show( Main::nCmdShow );
    Update();

    //  gxBuffer = (byte *)GXBeginDraw();

  }

  void MainWindow::CopyPalette(const Palette *pPal)
  {
    if (pPal==NULL) return;
    assert(pPal->Count() == 256);
    for (int i=0; i<256; i++)
    {
      lpMem->palPalEntry[i].peRed   = (*pPal)[i].R();
      lpMem->palPalEntry[i].peGreen = (*pPal)[i].G();
      lpMem->palPalEntry[i].peBlue  = (*pPal)[i].B();
      lpMem->palPalEntry[i].peFlags = 0;
    }
  }

  MainWindow::~MainWindow( void )
  {
		SHFullScreen( hWnd, SHFS_SHOWTASKBAR + SHFS_SHOWSIPBUTTON + SHFS_SHOWSTARTICON );
    SelectPalette(hdc, hOldPal, true);
    LocalFree ((HLOCAL) lpMem);
    DeleteObject(hPalette);
    ReleaseDC(hWnd, hdc);
    GXCloseInput();
    //  GXEndDraw();
  }

  void MainWindow::SetPalette()
  {
    hPalette = CreatePalette (lpMem);
    assert(hPalette != NULL);
    hOldPal = SelectPalette(hdc, hPalette, false);
    int RealizedColors = RealizePalette(hdc);
    assert(RealizedColors > 0);
    HasPalette = true;
  }

  void MainWindow::NewPalette()
  {
    HasPalette = false;
    SelectPalette(hdc, hOldPal, true);
    DeleteObject(hPalette);
    SetPalette();
  }

  LRESULT MainWindow::WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam )
  {
    switch (iMessage)
    {
    case WM_DESTROY:
      PostQuitMessage( 0 );
      break;
    default:
      return DefWindowProc( hWnd, iMessage, wParam, lParam );
    }
    return 0;
  }


  //******************************************************************************

  Application::Application(Device & device)
  {
    fDevice = &device;
    fDevice->SetApplication(this);
  }

  DeviceCE* DeviceCE::MainDevice=NULL;

  void DeviceCE::SetApplication(Application * papp)
  {
    papplication = papp;
  }

  DeviceCE::DeviceCE(int lWidth, int lHeight, const Palette * pPal)
    : inherited(lWidth, lHeight, pPal)
  {
    fWidth = lWidth;
    fHeight = lHeight;
    papplication = NULL;
    fNbColors = 256;
    fDepth = 8;
    fDraging = false;
    fHaveMoved = false;
    DeviceCE::MainDevice = this;
    SetCapture(hWnd);
  }

  DeviceCE::~DeviceCE(void)
  {
    ReleaseCapture();
#ifdef GXOPT
    GXEndDraw();
#endif
  }

  void DeviceCE::AutoRepeatOff()
  {
  }

  int GetMouseCode(bool shiftmode, bool rightmode, bool ctrlmode)
  {
    int result = 0;
    if (!rightmode)
      result = kMLEFT;
    if (rightmode)
      result = kMRIGHT;
    if (ctrlmode)
      result |= kCtrl;
    if (shiftmode)
      result |= kShift;
    return result;
  }

  LRESULT DeviceCE::WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam )
  {
    int x, y;
    static bool shiftmode=false;
    static bool rightmode=false;
    static bool ctrlmode=false;
#define MOUSECODE GetMouseCode(shiftmode, rightmode, ctrlmode)

    switch (iMessage)
    {
    case WM_PAINT:
      papplication->Draw();
      break;
    case WM_KEYDOWN:
      papplication->OnKeyPressed(Code(wParam));
      //extend mouse behaviour
      if (short(wParam) == g_gxkl.vkA) shiftmode = !shiftmode;
      if (short(wParam) == g_gxkl.vkB) rightmode = ! rightmode;
      if (short(wParam) == g_gxkl.vkC) ctrlmode = ! ctrlmode;
      break;
    case WM_LBUTTONDOWN :
      if (fDraging) break;
      fDragX=short(LOWORD(lParam)); fDragY=short(HIWORD(lParam))+26;
      fDraging = true;
      break;
    case WM_RBUTTONDOWN :
      if (fDraging) break;
      fDragX=short(LOWORD(lParam)); fDragY=short(HIWORD(lParam))+26;
      fDraging = true;
      break;
    case WM_MOUSEMOVE :
      if (fDraging)
      {
        x=short(LOWORD(lParam)); y=short(HIWORD(lParam))+26;
        if (x!=fDragX || y!=fDragY) // Protect against abusive move, occurs sometimes !
        {
          if (fHaveMoved)
            papplication->OnDragOver(MOUSECODE, x-fDragX, y-fDragY);
          else
            papplication->OnDragStart(MOUSECODE, x, y);
          fDragX=x; fDragY=y;
          fHaveMoved = true;
        }
      }
      break;
    case WM_LBUTTONUP :
      x=short(LOWORD(lParam)); y=short(HIWORD(lParam))+26;
      if (!fHaveMoved)
      {
        papplication->OnMouseClick(MOUSECODE, x, y);
      }
      else
      {
        papplication->OnDragEnd(MOUSECODE, x, y);
      }
      fDraging = false;
      fHaveMoved = false;
      break;
    case WM_RBUTTONUP :
      x=short(LOWORD(lParam)); y=short(HIWORD(lParam))+26;
      if (!fHaveMoved)
      {
        papplication->OnMouseClick(MOUSECODE, x, y);
      }
      else
      {
        papplication->OnDragEnd(MOUSECODE, x, y);
      }
      fDraging = false;
      fHaveMoved = false;
      break;
    default:
      inherited::WndProc(iMessage, wParam, lParam );
    }
    return 0;
  }

  void DeviceCE::AcceptImage(Image * pImage)
  {
    // change the buffer of the image to the one of the WinG
    fImage = pImage;
    assert( fImage->Width() == fWidth);
    assert( fImage->Height() == fHeight);

#ifdef GXOPT
    byte * pbLine = (byte *)GXBeginDraw();
    if (pbLine == NULL)	
    {
      error("could not draw");
      return;	// NOT OK TO DRAW, return failure.
    }
    fImage->NewBuffer(pbLine);
#else
    fImage->NewBuffer(new byte[fImage->Len()]);
#endif

    //fImage->NewBuffer(gxBuffer);
    fWidth = fImage->Width();
    fHeight = fImage->Height();
  }

  void DeviceCE::DisplayImage()
  {
    switch (g_gxdp.cBPP)
    {
    case 16:
      {
        int colRed = 55;
        int colBlue= 44;
        int colGreen=33;
        unsigned short PixelCol = 0;
        if (g_gxdp.ffFormat | kfDirect565) {
          PixelCol = (unsigned short) ((colRed & 0xff)<< 11 | (colGreen & 0xff) << 5 | (colBlue & 0xff));
        } else if (g_gxdp.ffFormat | kfDirect555) {
          PixelCol = (unsigned short) ((colRed & 0xff)<< 10 | (colGreen & 0xff) << 5 | (colBlue & 0xff));
        }

        unsigned short * pusLine = (unsigned short *)GXBeginDraw();
        if (pusLine == NULL)
        {
          error("could not display image");
          return; // NOT OK TO DRAW, return failure.
        }

        byte * pbSrc = fImage->GetBuffer();
        for (unsigned int y = 0; y < g_gxdp.cyHeight; y++) {
          unsigned short * pusDest = pusLine;
          for (unsigned int x = 0; x < g_gxdp.cxWidth; x++) {
            *pusDest = *pbSrc;
            pusDest += g_gxdp.cbxPitch >> 1;
            pbSrc++;
          }
          pusLine += g_gxdp.cbyPitch >> 1;
        }
        GXEndDraw();
        break;
      }
    case 8:	
      {
#ifdef GXOPT
        GXEndDraw();		
        byte * pbLine = (byte *)GXBeginDraw();
        fImage->NewBuffer(pbLine);
#else
        byte * pbLine = (byte *)GXBeginDraw();
        if (pbLine == NULL)	
        {
          error("could not draw");
          return;	// NOT OK TO DRAW, return failure.
        }

        /*byte * pbSrc = fImage->GetBuffer();
        for (unsigned int y = 0; y < g_gxdp.cyHeight; y++) {
        unsigned char * pbDest = pbLine;
        for (unsigned int x = 0; x < g_gxdp.cxWidth; x++) {
        *pbDest = *pbSrc;
        pbDest += g_gxdp.cbxPitch;
        pbSrc++;
        }
        pbLine += g_gxdp.cbyPitch;
        }*/
        memcpy(pbLine, fImage->GetBuffer(), fWidth*fHeight);
        GXEndDraw();		
#endif
        break;
      }
    }
    return;

  }

  void DeviceCE::ChangeCurrentPalette(const Palette *pPal)
  {
    inherited::CopyPalette(pPal);
    inherited::NewPalette();
  }

  int DeviceCE::MouseCode(int wParam)
  {
    int result = 0;
    if (wParam & MK_LBUTTON)
      result = kMLEFT;
    if (wParam & MK_RBUTTON)
      result = kMRIGHT;
    if (wParam & MK_CONTROL)
      result |= kCtrl;
    if (wParam & MK_SHIFT)
      result |= kShift;
    return result;
  }

  word DeviceCE::Code(int wParam)
  {
    short vkKey;
    vkKey = (short)wParam;
    if (vkKey == g_gxkl.vkUp) return kUP;
    if (vkKey == g_gxkl.vkDown) return kDN;
    if (vkKey == g_gxkl.vkRight) return kRI;
    if (vkKey == g_gxkl.vkLeft) return kLE;
    if (vkKey == g_gxkl.vkA) return kSP;
    if (vkKey == g_gxkl.vkB) return kLF;
    if (vkKey == g_gxkl.vkC) return kTB;
    if (vkKey == g_gxkl.vkStart) return kEND;

    return kNONE;
  }

  void DeviceCE::StartChrono()
  {
    GetSystemTime(&fChronoTime);
  }

  int DeviceCE::GetChrono()
  {
    SYSTEMTIME currentTime;
    GetSystemTime(&currentTime);
    return
      (currentTime.wMilliseconds-fChronoTime.wMilliseconds) +
      (currentTime.wSecond-fChronoTime.wSecond)*1000 +
      (currentTime.wMinute-fChronoTime.wMinute)*1000*60 +
      (currentTime.wHour-fChronoTime.wHour)*1000*60*60;
  }

  void DeviceCE::Close()
  {
    PostQuitMessage(0);
  }

  Device * NewDevice(int Width, int Height, const Palette *p)
  {
    Device * result = (Device *) new DeviceCE(Width, Height, p);
    assert(result != NULL);
    return result;
  }

}