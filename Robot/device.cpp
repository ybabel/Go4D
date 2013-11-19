/*****************************************************************************\
 * AUTHOR : Babel yoann Antras . babel@iie.cnam.fr
 * TITLE  : device.cpp, device.h implementation
 * CREATED: 22/10/95
 * DATE   : 29/02/96
 * CURRENT: only one window handled
 * BUGS   :
\*****************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef LIBGPP
#include <iostream.h>
#endif

#include <assert.h>
#include "device.h"

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

/* windows specific adds */

/************************************\
 * class Main
\************************************/


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
#elif defined(__LARGE__) || defined(__COMPACT__) || defined(__FLAT__)
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

LRESULT  CALLBACK _export WndProc( HWND hWnd, UINT iMessage, WPARAM wParam,
											LPARAM lParam )
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
  wndclass.lpfnWndProc   = ::WndProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = sizeof( MainWindow * );
  wndclass.hInstance     = Main::hInstance;
  wndclass.hIcon         = LoadIcon( Main::hInstance, "whello" );
  wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
  wndclass.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
  wndclass.lpszMenuName  = NULL;
  wndclass.lpszClassName = szClassName;

  if ( ! RegisterClass( &wndclass ) )
     exit( FALSE );
}

byte func(unsigned long col, unsigned long fdiv)
{
  if (col==0) return 0;
  return byte( (col*2-1)*(32-fdiv)/32 );
}

byte func2(unsigned long col, unsigned long fdiv)
{
  if (col==0) return 0;
  return byte( (col*2-1)*(16-fdiv)/16 );
}

MainWindow::MainWindow( void )
{
  WinG.Load();
  hWnd = CreateWindow( szClassName,
        szClassName,
        WS_BORDER|WS_CAPTION|WS_VISIBLE|WS_SYSMENU,
        CW_USEDEFAULT,
        0,
				WinX,
				WinY+21,         // title bar width
				NULL,
				NULL,
				Main::hInstance,
				(LPSTR) this );
  if ( ! hWnd )
     exit( FALSE );

  windc = GetDC(hWnd);
  Buffer = WinG.pCreateDC();
  if (!WinG.pRecommendDIBFormat( (BITMAPINFO *)&header)) exit(1);
  header.header.biWidth = WinX;
  header.header.biHeight = -WinY;

  for (int i=0; i<16; i++)
    {
      for (int j=0; j<16; j++)
      	{
          header.aColors[i*16+j].rgbRed = BYTE(func(kBase[i].Red,j));
          header.aColors[i*16+j].rgbGreen = BYTE(func(kBase[i].Green,j));
          header.aColors[i*16+j].rgbBlue = BYTE(func(kBase[i].Blue,j));
        }
    }
  NewPalette();

  hbm = WinG.pCreateBitmap(Buffer, (BITMAPINFO *)&header, &pBits);
  SelectObject(Buffer, hbm);

  Show( Main::nCmdShow );
  Update();
}

MainWindow::~MainWindow( void )
{
  ReleaseDC(hWnd, windc);
  ReleaseDC(hWnd, Buffer);
  DeleteObject(hbm);
  DeleteObject(pal);
  WinG.Free();
}

char MainWindow::szClassName[] = "Robot simulation -- M6 --";

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

void MainWindow::SetPalette()
{
  Palette.Version = 0x300;
  Palette.NumberOfEntries = 256;
  for (int j=0; j<256; j++)
  {
    Palette.aEntries[j].peRed = header.aColors[j].rgbRed;
    Palette.aEntries[j].peGreen = header.aColors[j].rgbGreen;
    Palette.aEntries[j].peBlue = header.aColors[j].rgbBlue;
    Palette.aEntries[j].peFlags = PC_RESERVED;
    Palette.aEntries[j].peFlags = 0;
  }
  pal = CreatePalette((LOGPALETTE*)&Palette);
  SelectPalette(windc, pal, FALSE);
  RealizePalette(windc);
  SelectObject(windc, pal);
}

void MainWindow::NewPalette()
{
  UnrealizeObject(pal);
  DeleteObject(pal);
  SetPalette();
}

/* end windows adds */



//******************************************************************************

Application::Application(Device & device)
{
  pdevice = &device;
  pdevice->SetApplication(this);
}

void Device::SetApplication(Application * papp)
{
  papplication = papp;
}

Device::Device() : MainWindow()
{
  papplication = NULL;
}

Device::~Device(void)
{
}

void Device::AutoRepeatOff()
{
}

LRESULT Device::WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	unsigned x, y;
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
      x=LOWORD(lParam); y=HIWORD(lParam);
      papplication->OnMousePressed(kMLEFT, int(x), int(y));
    case WM_RBUTTONDOWN :
      x=LOWORD(lParam); y=HIWORD(lParam);
      papplication->OnMousePressed(kMRIGHT, int(x), int(y));
    default:
      return DefWindowProc( hWnd, iMessage, wParam, lParam );
  }
  return 0;
}

void Device::AcceptImage(Image * pImage)
{
  // change the buffer of the image to the one of the WinG
  assert( pImage->Width() == WinX);
  assert( pImage->Height() == WinY);
  pImage->NewBuffer( (byte *) pBits);
}

void Device::DisplayImage()
{
  WinG.pBitBlt(windc,0,0, WinX,WinY, Buffer, 0,0);
}

word Device::Code(int wParam)
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
	::string s = "  pressed";
	s[0] = byte(wParam);
	MessageBox(hWnd, s, "key stroke", MB_OK);
	return kNONE;
}

void Device::RedPalette()
{
  for (int i=0; i<fNbColors; i++)
    {
      header.aColors[i].rgbRed = BYTE(i*256/fNbColors);
      header.aColors[i].rgbGreen = 0;
      header.aColors[i].rgbBlue = 0;
    }
  NewPalette();
}

void Device::BlackWhitePalette()
{
  for (int i=0; i<fNbColors; i++)
    {
      header.aColors[i].rgbRed = BYTE(i*256/fNbColors);
      header.aColors[i].rgbGreen = BYTE(i*256/fNbColors);
      header.aColors[i].rgbBlue = BYTE(i*256/fNbColors);
    }
  NewPalette();
}

void Device::VoxelPalette()
{
  for (int i=0; i<fNbColors-2; i++)
    {
      header.aColors[i].rgbRed = BYTE(i*256/fNbColors);
      header.aColors[i].rgbGreen = 0;
      header.aColors[i].rgbBlue = 0;
    }

  header.aColors[fNbColors-2].rgbRed = 0;
  header.aColors[fNbColors-2].rgbGreen = 50;
  header.aColors[fNbColors-2].rgbBlue = 200;

  header.aColors[fNbColors-1].rgbRed = 0;
  header.aColors[fNbColors-1].rgbGreen = 0;
  header.aColors[fNbColors-1].rgbBlue = 255;

  NewPalette();
}

void Device::DefaultPalette()
{
  for (int i=0; i<fNbColors/16; i++)
    {
      for (int j=0; j<16; j++)
      	{
          header.aColors[i*16+j].rgbRed = BYTE(func(kBase[i].Red,j));
          header.aColors[i*16+j].rgbGreen = BYTE(func(kBase[i].Green,j));
          header.aColors[i*16+j].rgbBlue = BYTE(func(kBase[i].Blue,j));
        }
    }
  NewPalette();
}

void Device::ImaPalette(const ImagePal & im)
{
  for (int i=0; i<fNbColors; i++)
    {
      header.aColors[i].rgbRed = BYTE(im.Pal()[i][0]*256/fNbColors);
      header.aColors[i].rgbGreen = BYTE(im.Pal()[i][1]*256/fNbColors);
      header.aColors[i].rgbBlue = BYTE(im.Pal()[i][1]*256/fNbColors);
    }
  NewPalette();
}

void Device::setcolor(byte col, byte r, byte g, byte b)
{
  header.aColors[col].rgbRed = BYTE(r);
  header.aColors[col].rgbGreen = BYTE(g);
  header.aColors[col].rgbBlue = BYTE(b);
  NewPalette();
}
