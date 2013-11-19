/**
* AUTHOR : Babel yoann Antras . babel@iie.cnam.fr
* TITLE  : DeviceWinG.cpp, device.h implementation
* CREATED: 22/10/95
* DATE   : 07/01/01
* CURRENT: only one window handled
* BUGS   :  
*/

#include "Device_WING.h"

namespace Go4D
{


  /************************************\
  * class Main
  \************************************/

#define kszCLASSNAME "3D Demo"

  HINSTANCE Main::hInstance = 0;
  HINSTANCE Main::hPrevInstance = 0;
  int Main::nCmdShow = 0;

  int Main::MessageLoop(Application & application)
  {
    MSG msg;

    while (1)
    {
      application.Cycle();
      if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
      {
        if (msg.message == WM_QUIT)
          break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    return msg.wParam;
  }


  /************************************\
  * class Window
  \************************************/


  // If data pointers are near pointers
#if defined(__SMALL__) || defined(__MEDIUM__)
  inline Window *GetPointer( HWND hWnd )
  {
    return (Window *) GetWindowWord( hWnd, 0 );
  }
  inline void SetPointer( HWND hWnd, Window *pWindow )
  {
    SetWindowWord( hWnd, 0, (WORD) pWindow );
  }

  // else pointers are far
#elif defined(__LARGE__) || defined(__COMPACT__) || defined(__FLAT__) || defined(VSNET)
  inline Window *GetPointer( HWND hWnd )
  {
    return (Window *) GetWindowLong( hWnd, 0 );
  }
  inline void SetPointer( HWND hWnd, Window *pWindow )
  {
    SetWindowLong( hWnd, 0, (LONG) pWindow );
  }

#else
#error Choose another memory model!
#endif

#ifdef VSNET
  LRESULT  CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam )
#else
  LRESULT  CALLBACK _export WndProc( HWND hWnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam )
#endif
  {
    Window *pWindow = GetPointer( hWnd );

    if ( pWindow == 0 )
    {
      if ( iMessage == WM_CREATE )
      {
        LPCREATESTRUCT lpcs;

        lpcs = (LPCREATESTRUCT) lParam;
        pWindow = (Window *) lpcs->lpCreateParams;
        SetPointer( hWnd, pWindow );
        return pWindow->WndProc( iMessage, wParam, lParam );
      }
      else
        return DefWindowProc( hWnd, iMessage, wParam, lParam );
    }
    else
      return pWindow->WndProc( iMessage, wParam, lParam );
  }


  /************************************\
  * class MainWindow
  \************************************/

  void MainWindow::Register( void )
  {
    WNDCLASS wndclass;   // Structure used to register Windows class.

    wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.lpfnWndProc   = Go4D::WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = sizeof( MainWindow * );
    wndclass.hInstance     = Main::hInstance;
    wndclass.hIcon         = 0;
    wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wndclass.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wndclass.lpszMenuName  = NULL;
    //wndclass.lpszClassName = szClassName;
    wndclass.lpszClassName = kszCLASSNAME;

    if ( ! RegisterClass( &wndclass ) )
      exit( FALSE );
  }

  MainWindow::MainWindow(int & Width, int & Height, const Palette * pPal)
  {
    assert(Width>0);
    assert(Height>0);
    WinG.Load();
    hWnd = CreateWindow( kszCLASSNAME,
      kszCLASSNAME,
      WS_BORDER|WS_CAPTION|WS_VISIBLE|WS_SYSMENU,
      CW_USEDEFAULT,
      0,
      Width+4,
      Height+25,         // title bar width (21)
      NULL,
      NULL,
      Main::hInstance,
      (LPSTR) this );
    if ( ! hWnd )
      error("Could not create window");

    windc = GetDC(hWnd);
    if (!WinG.pRecommendDIBFormat( (BITMAPINFO *)&header))
      error("WinG can't tell DIB format");
    Width = (Width+3)/4*4;  // must be a multiple of 4
    header.header.biWidth = Width;
    header.header.biHeight = -Height;


    CopyPalette(pPal);
    SetPalette();

    Show( Main::nCmdShow );
    Update();
  }

  void MainWindow::CopyPalette(const Palette *pPal)
  {
    fPalette = pPal;
    assert(pPal->Count() == 256);
    for (int i=0; i<256; i++)
    {
      header.aColors[i].rgbRed = byte(pPal!=NULL?(*pPal)[i].R():0);
      header.aColors[i].rgbGreen = byte(pPal!=NULL?(*pPal)[i].G():0);
      header.aColors[i].rgbBlue = byte(pPal!=NULL?(*pPal)[i].B():0);
    }
  }

  MainWindow::~MainWindow( void )
  {
    ReleaseDC(hWnd, windc);
    DeleteObject(Buffer);
    DeleteObject(hbm);
    WinG.Free();
  }

  void MainWindow::SetPalette()
  {
    Buffer = WinG.pCreateDC();
    WinG.pSetDIBColorTable(windc,0,256, header.aColors);

    hbm = WinG.pCreateBitmap(Buffer, (BITMAPINFO *)&header, &pBits);
    SelectObject(Buffer, hbm);

    HasPalette = true;
  }

  void MainWindow::NewPalette()
  {
    HasPalette = false;
    DeleteObject(Buffer);
    DeleteObject(hbm);
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

  /* end windows adds */



  //******************************************************************************

  Application::Application(Device & device)
  {
    fDevice = &device;
    fDevice->SetApplication(this);
  }

  void WinGDevice::SetApplication(Application * papp)
  {
    papplication = papp;
  }

  WinGDevice::WinGDevice(int lWidth, int lHeight, const Palette * pPal)
    : inherited(lWidth, lHeight, pPal)
  {
    fWidth = lWidth;
    fHeight = lHeight;
    papplication = NULL;
    fNbColors = 256;
    fDepth = 8;
    fDraging = false;
    fHaveMoved = false;
  }

  WinGDevice::~WinGDevice(void)
  {
  }

  void WinGDevice::AutoRepeatOff()
  {
  }

  LRESULT WinGDevice::WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam )
  {
    int x, y;
    switch (iMessage)
    {
    case WM_PAINT:
      papplication->Draw();
      break;
    case WM_DESTROY:
      PostQuitMessage( 0 );
      break;
    case WM_KEYDOWN:
      papplication->OnKeyPressed(Code(wParam));
      break;
    case WM_LBUTTONDOWN :
      if (fDraging) break;
      SetCapture(hWnd);
      fDragX=short(LOWORD(lParam)); fDragY=short(HIWORD(lParam));
      fDraging = true;
      break;
    case WM_RBUTTONDOWN :
      if (fDraging) break;
      SetCapture(hWnd);
      fDragX=short(LOWORD(lParam)); fDragY=short(HIWORD(lParam));
      fDraging = true;
      break;
    case WM_MOUSEMOVE :
      if (fDraging)
      {
        x=short(LOWORD(lParam)); y=short(HIWORD(lParam));
        if (fHaveMoved)
          papplication->OnDragOver(MouseCode(wParam), x-fDragX, y-fDragY);
        else
          papplication->OnDragStart(MouseCode(wParam), x, y);
        fDragX=x; fDragY=y;
        fHaveMoved = true;
      }
      break;
    case WM_LBUTTONUP :
      ReleaseCapture();
      x=short(LOWORD(lParam)); y=short(HIWORD(lParam));
      if (!fHaveMoved)
        papplication->OnMouseClick(MouseCode(wParam), x, y);
      else
        papplication->OnDragEnd(MouseCode(wParam),x, y);
      fDraging = false;
      fHaveMoved = false;
      break;
    case WM_RBUTTONUP :
      ReleaseCapture();
      x=short(LOWORD(lParam)); y=short(HIWORD(lParam));
      if (!fHaveMoved)
        papplication->OnMouseClick(MouseCode(wParam), x, y);
      else
        papplication->OnDragEnd(MouseCode(wParam), x, y);
      fDraging = false;
      fHaveMoved = false;
      break;
    default:
      return DefWindowProc( hWnd, iMessage, wParam, lParam );
    }
    return 0;
  }

  void WinGDevice::AcceptImage(Image * pImage)
  {
    // change the buffer of the image to the one of the WinG
    assert( pImage->Width() == fWidth);
    assert( pImage->Height() == fHeight);
    pImage->NewBuffer( (byte *) pBits);
  }

  void WinGDevice::DisplayImage()
  {
    WinG.pBitBlt(windc, 0, 0, fWidth, fHeight, Buffer, 0, 0);
  }

  void WinGDevice::ChangeCurrentPalette(const Palette *pPal)
  {
    inherited::CopyPalette(pPal);
    inherited::NewPalette();
  }

  int WinGDevice::MouseCode(int wParam)
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

  word WinGDevice::Code(int wParam)
  {
    switch (wParam)
    {
    case VK_UP : return kUP;
    case VK_DOWN : return kDN;
    case VK_RIGHT : return kRI;
    case VK_LEFT : return kLE;
    }

    switch( byte(wParam) )
    {
    case 'F' : return kFW;
    case 'B' : return kBW;
    case 'L' : return kLO;
    case 'H' : return kHI;
    case ' ' : return kSP;
    case 'U' : return kKU;
    case 'I' : return kKI;
    case 'Q' : return kKQ;
    case 'W' : return kKW;
    case 'A' : return kKA;
    case 'Z' : return kKZ;
    case   9 : return kTB;
    case  13 : return kLF;
    case  27 : return kEND;
    case   8 : return kDEL;
    case 'T' : return kKT;
    case 'N' : return kKN;
    case 'P' : return kKP;
    case 'C' : return kKC;
    case 'V' : return kKV;
    case '+' : return kPL;
    case '-' : return kMN;
    case '1' : return kK1;
    case '2' : return kK2;
    case '3' : return kK3;
    case '4' : return kK4;
    case '5' : return kK5;
    case '6' : return kK6;
    case '7' : return kK7;
    case '8' : return kK8;
    case '9' : return kK9;
    case '0' : return kK0;
    case 'S' : return kKS;
    case 'R' : return kKR;
    }
    return kNONE;
  }

  void WinGDevice::StartChrono()
  {
    GetSystemTime(&fChronoTime);
  }

  int WinGDevice::GetChrono()
  {
    SYSTEMTIME currentTime;
    GetSystemTime(&currentTime);
    return
      (currentTime.wMilliseconds-fChronoTime.wMilliseconds) +
      (currentTime.wSecond-fChronoTime.wSecond)*1000 +
      (currentTime.wMinute-fChronoTime.wMinute)*1000*60 +
      (currentTime.wHour-fChronoTime.wHour)*1000*60*60;
  }

  void WinGDevice::Close()
  {
      PostQuitMessage( 0 );
  }

  Device * NewDevice(int Width, int Height, const Palette *p)
  {
    return new WinGDevice(Width, Height, p);
  }

}