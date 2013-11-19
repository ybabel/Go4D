// EasyCE.cpp : API implementation
// By Jacco Bikker, a.k.a. "The Phantom"
// --------------------------------------------------

#include "math.h"
#include "EasyCE.h"
#include "aygshell.h"
#include "mmsystem.h"

HINSTANCE hInst;
TCHAR szTitle[30] = TEXT("easyce");
TCHAR wincls[30] = TEXT("easyce");
HWND hWnd;
HBITMAP hBM;

// Framebuffer stuff
unsigned long* buffer, *temp;
char* m_chr[54][7];
int m_transl[256];
#ifdef USE_GAMEX
	GameX* gameX;
#endif
int cursorx, cursory;
int method = 1, BPP, xpitch, ypitch;
bool pendwn = false;
unsigned int basetime = 0;

// Greyscale emulator timing table and variables
int tm[32] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31 };
int time, tmindx = 0;

// Dithering variables
int pvtime;
unsigned char pv7[2*8] = { 0,0, 1,1, 2,2, 3,3,
                           0,1, 1,2, 2,3, 3,3 };
unsigned char pv16[5*16] = { 0,0,0,0,0, 1,1,1,1,1, 2,2,2,2,2, 3,
                             0,0,1,1,1, 1,1,2,2,2, 2,2,3,3,3, 3,
                             0,0,0,0,1, 1,1,1,1,2, 2,2,2,2,3, 3,
                             0,0,0,1,1, 1,1,1,2,2, 2,2,2,3,3, 3,
                             0,1,1,1,1, 1,2,2,2,2, 2,3,3,3,3, 3 };
unsigned char pv31[10*32] = { 0,1,1,1,1,1,0,0,0,0, 1,2,2,2,2,1,1,1,1,1, 2,3,3,3,3,2,2,2,2,2, 3,3,
                              0,0,0,0,0,0,1,1,1,1, 1,1,1,1,1,1,2,2,2,2, 2,2,2,2,2,2,3,3,3,3, 3,3,
                              0,0,0,0,0,1,1,1,1,1, 1,1,1,1,1,2,2,2,2,2, 2,2,2,2,2,3,3,3,3,3, 3,3,
                              0,0,0,0,1,0,0,1,1,1, 1,1,1,1,2,1,1,2,2,2, 2,2,2,2,3,2,2,3,3,3, 3,3,
                              0,0,0,1,0,1,1,0,1,1, 1,1,1,2,1,2,2,1,2,2, 2,2,2,3,2,3,3,2,3,3, 3,3,
                              0,0,1,0,0,0,1,1,0,1, 1,1,2,1,1,1,2,2,1,2, 2,2,3,2,2,2,3,3,2,3, 3,3,
                              0,0,0,0,1,1,0,1,1,1, 1,1,1,1,2,2,1,2,2,2, 2,2,2,2,3,3,2,3,3,3, 3,3,
                              0,0,0,1,0,0,1,0,1,1, 1,1,1,2,1,1,2,1,2,2, 2,2,2,3,2,2,3,2,3,3, 3,3,
                              0,0,0,0,1,1,0,1,1,1, 1,1,1,1,2,2,1,2,2,2, 2,2,2,2,3,3,2,3,3,3, 3,3,
                              0,0,0,0,0,0,1,1,1,1, 1,1,1,1,1,1,2,2,2,2, 2,2,2,2,2,2,3,3,3,3, 3,3 };
// Stacks
int mxpos, mypos, mcxpos, mcypos;
bool mcprocessed, mbutton;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
extern void main( void );
void initcharset( void );
HANDLE logfile = 0;

void error( char* s )
{
	if (!logfile) logfile = CreateFile( L"Windows\\Start Menu\\log.txt", GENERIC_WRITE, FILE_SHARE_READ,
	                        0, OPEN_ALWAYS, 0, 0 );
	unsigned long byteswritten;
	WriteFile( logfile, s, strlen( s ), &byteswritten, 0 );
}

void itoa( int v, char* s )
{
	if (v == 0)
	{
		*s = '0';
		*(s + 1) = 0;
		return;
	}
	int mul = 10000000;
	bool first = true;	
	char* cur = s;
	if ( v < 0)
	{
		*cur++ = '-';
		v = -v;
	}
	while (mul > 0)
	{
		int digit = (v / mul) % 10;
		if ((digit > 0) || (!first))
		{
			*cur++ = '0' + digit;
			first = false;
		}
		mul /= 10;
	}
	*cur = 0;
}

void error_nr( char* s, int n )
{
	if (!logfile) logfile = CreateFile( L"Windows\\Start Menu\\log.txt", GENERIC_WRITE, FILE_SHARE_READ,
	                        0, OPEN_ALWAYS, 0, 0 );
	unsigned long byteswritten;
	char* t = new char[512];
	strcpy( t, s );
	itoa( n, t + strlen( t ) );
	WriteFile( logfile, t, strlen( t ), &byteswritten, 0 );
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	if( msg.hwnd = FindWindow(szTitle,NULL) )
	{
		ShowWindow(msg.hwnd,SW_SHOW);
		return FALSE;
	}
	buffer = new unsigned long[160 * 240];
#ifdef USE_GAMEX
	gameX = new GameX();
	if (!gameX->OpenGraphics()) return 0;
	BPP = gameX->GetFBBpp();
	ypitch = gameX->GetFBModulo();
	if (BPP == 16) xpitch = 2; else xpitch = 1;
#else
	GXOpenDisplay( msg.hwnd, GX_FULLSCREEN );
	GXDisplayProperties dp = GXGetDisplayProperties();
	BPP = dp.cBPP;
	xpitch = dp.cbxPitch;
	ypitch = dp.cbyPitch;
#endif
	mcprocessed = true;
	mbutton = false;
	initcharset();
	cursorx = 2;
	cursory = 2;
	taskbar( false );
	resettimer();
   	if (!MyRegisterClass(hInstance)) return FALSE;
	if (!InitInstance (hInstance, nCmdShow)) return FALSE;
	memset( buffer, 0, 320 * 240 * 2 );
	update();
	while (msecs() < 500) {}; resettimer();
	print( "powered by easyce", 50, 150, 65535 );
	print( "www.cewarez.com/coding", 30, 158, 65535 );
	update();
	while (msecs() < 1200) {}; resettimer();
	memset( buffer, 0, 320 * 240 * 2 );
	update();
	while (msecs() < 300) {}; resettimer();
	main();
	taskbar( true );
#ifdef USE_GAMEX
	delete gameX;
#else
	GXCloseDisplay();
#endif
	delete buffer;
	if (logfile) CloseHandle( logfile );
	for ( int x = 0; x < 54; x++ ) for ( int y = 0; y < 7; y++ ) delete m_chr[x][y];
	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wcls;
	wcls.style			= 0;
	wcls.lpfnWndProc	= (WNDPROC)WndProc;
	wcls.cbClsExtra		= 0;
	wcls.cbWndExtra		= 0;
	wcls.hInstance		= hInstance;
	wcls.hIcon			= LoadIcon(hInstance,MAKEINTRESOURCE(1));
	wcls.hCursor		= 0;
	wcls.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);
	wcls.lpszMenuName	= 0;
	wcls.lpszClassName	= wincls;
	return RegisterClass(&wcls);
}

BOOL InitInstance(HINSTANCE hi, int nCmdShow)
{
	hInst = hi;
	hWnd = CreateWindow(wincls, szTitle, WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 240, 320, 0, 0, hi, 0);
	if (!hWnd) return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_LBUTTONDOWN:
			mcxpos = LOWORD( lParam );
			mcypos = HIWORD( lParam );
			mcprocessed = false;
			mbutton = true;
 			break;
		case WM_LBUTTONUP:
			mbutton = false;
			break;
		case WM_MOUSEMOVE:
			mxpos = LOWORD( lParam );
			mypos = HIWORD( lParam );
			// mbutton = (wParam == MK_LBUTTON);
			break;
		case WM_DESTROY:
			// ReleaseCapture();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void taskbar( bool show )
{
#ifdef USE_GAMEX
	// The winCE 2.x way
	#define TASKBAR_KEY _T("\\software\\microsoft\\shell") 
	HKEY hKey=0;
	RegOpenKeyEx( HKEY_LOCAL_MACHINE, TASKBAR_KEY, 0, KEY_ALL_ACCESS, &hKey );
	DWORD dwValue = 0;
	unsigned long lSize = sizeof( DWORD );
	DWORD dwType = REG_DWORD;
	RegQueryValueEx( hKey, _T("TBOpt"), 0, &dwType, (BYTE*)&dwValue, &lSize );
	if (show) dwValue &= 0xFFFFFFFF - 8;// reset bit to show taskbar
	 	 else dwValue |= 8;				// set bit to hide taskbar
	RegSetValueEx( hKey, _T("TBOpt"), 0, REG_DWORD, (BYTE*)&dwValue, lSize );
	MSG msg;
	msg.hwnd = FindWindow( _T("HHTaskBar"), NULL );
	::SendMessage( msg.hwnd, WM_COMMAND, 0x03EA, 0 );
	::SetForegroundWindow( hWnd );
#else
	// The Pocket PC way
	if (show)
	{
		SHFullScreen( hWnd, SHFS_SHOWTASKBAR + SHFS_SHOWSIPBUTTON + SHFS_SHOWSTARTICON );
	}
	else
	{
		SHFullScreen( hWnd, SHFS_HIDETASKBAR + SHFS_HIDESIPBUTTON + SHFS_HIDESTARTICON );
	}
#endif
}

void updatedisplay( void )
{
	if (BPP == 2)
	{
		// Screen update routine for 2bit devices (Nino, E10)
	#ifdef USE_GAMEX
		if (!gameX->BeginDraw()) return;
		unsigned int* dest = (unsigned int*)getscrptr();
	#else
		unsigned int* dest = (unsigned int*)GXBeginDraw();
	#endif
		int gapsize = (ypitch - 60) >> 2;
		unsigned short* a = (unsigned short*)buffer;
		time = tm[tmindx] * 16;
		if (method == 1)
		{
			for ( int y = 0; y < 320; y++ )
			{
				for ( int x = 0; x < 15; x++ )
				{
					int c = 0;
					c += (*(a   )>>14)<<6;
					c += (*(a+1 )>>14)<<4;
					c += (*(a+2 )>>14)<<2;
					c += (*(a+3 )>>14);
					c += (*(a+4 )>>14)<<14;
					c += (*(a+5 )>>14)<<12;
					c += (*(a+6 )>>14)<<10;
					c += (*(a+7 )>>14)<<8;
					c += (*(a+8 )>>14)<<22;
					c += (*(a+9 )>>14)<<20;
					c += (*(a+10)>>14)<<18;
					c += (*(a+11)>>14)<<16;
					c += (*(a+12)>>14)<<30;
					c += (*(a+13)>>14)<<28;
					c += (*(a+14)>>14)<<26;
					c += (*(a+15)>>14)<<24;
					a += 16;
					*dest++ = c;
				}
				dest += gapsize;
			}
		}
		else if (method == 2)
		{
			for ( int y = 0; y < 320; y++ )
			{
				for ( int x = 0; x < 15; x++ )
				{
					int c = 0;
					c += -((*a >> 11) > time) & 192;
					c += -((*(a + 1) >> 11) > time) & 48;
					c += -((*(a + 2) >> 11) > time) & 12;
					c += -((*(a + 3) >> 11) > time) & 3;
					c += -((*(a + 4) >> 11) > time) & 49152;
					c += -((*(a + 5) >> 11) > time) & 12288;
					c += -((*(a + 6) >> 11) > time) & 3072;
					c += -((*(a + 7) >> 11) > time) & 768;
					c += -((*(a + 8) >> 11) > time) & 12582912;
					c += -((*(a + 9) >> 11) > time) & 3145728;
					c += -((*(a + 10) >> 11) > time) & 786432;
					c += -((*(a + 11) >> 11) > time) & 196608;
					c += -((*(a + 12) >> 11) > time) & 3221225472;
					c += -((*(a + 13) >> 11) > time) & 805306368;
					c += -((*(a + 14) >> 11) > time) & 201326592;
					c += -((*(a + 15) >> 11) > time) & 50331648;
					a += 16;
					*dest++ = c;
				}
				time = (time + 8) & 31;
				dest += gapsize;
			}
		}
		else if (method == 3)
		{
			for ( int y = 0; y < 320; y++ )
			{
				for ( int x = 0; x < 15; x++ )
				{
					int c = 0;
					c += -((*a >> 11) > time) & 192;
					c += -((*(a + 1) >> 11) > time) & 48;
					c += -((*(a + 2) >> 11) > time) & 12;
					c += -((*(a + 3) >> 11) > time) & 3;
					c += -((*(a + 4) >> 11) > time) & 49152;
					c += -((*(a + 5) >> 11) > time) & 12288;
					c += -((*(a + 6) >> 11) > time) & 3072;
					c += -((*(a + 7) >> 11) > time) & 768;
					c += -((*(a + 8) >> 11) > time) & 12582912;
					c += -((*(a + 9) >> 11) > time) & 3145728;
					c += -((*(a + 10) >> 11) > time) & 786432;
					c += -((*(a + 11) >> 11) > time) & 196608;
					c += -((*(a + 12) >> 11) > time) & 3221225472;
					c += -((*(a + 13) >> 11) > time) & 805306368;
					c += -((*(a + 14) >> 11) > time) & 201326592;
					c += -((*(a + 15) >> 11) > time) & 50331648;
					a += 16;
					*dest++ = c;
				}
				time = (time + 4) & 31;
				dest += gapsize;
			}
		}
		if (method == 4)
		{
			unsigned char* dthptr = (unsigned char*)pv7 + pvtime*8;
			for ( int y = 0; y < 320; y++ )
			{
				for ( int x = 0; x < 15; x++ )
				{
					int c = 0;
					c += dthptr[*(a     )>>5] << 6;
					c += dthptr[*(a + 1 )>>5] << 4;
					c += dthptr[*(a + 2 )>>5] << 2;
					c += dthptr[*(a + 3 )>>5];
					c += dthptr[*(a + 4 )>>5] << 14;
					c += dthptr[*(a + 5 )>>5] << 12;
					c += dthptr[*(a + 6 )>>5] << 10;
					c += dthptr[*(a + 7 )>>5] << 8;
					c += dthptr[*(a + 8 )>>5] << 22;
					c += dthptr[*(a + 9 )>>5] << 20;
					c += dthptr[*(a + 10)>>5] << 18;
					c += dthptr[*(a + 11)>>5] << 16;
					c += dthptr[*(a + 12)>>5] << 30;
					c += dthptr[*(a + 13)>>5] << 28;
					c += dthptr[*(a + 14)>>5] << 26;
					c += dthptr[*(a + 15)>>5] << 24;
					a += 16;
					*dest++ = c;
				}
				dest += gapsize;
			}
			pvtime++; if (pvtime > 2) pvtime = 0;
		}
		else if( method==5 )
		{
			unsigned char* dthptr = (unsigned char*)pv16 + pvtime*16;
			for ( int y = 0; y < 320; y++ )
			{
				for ( int x = 0; x < 15; x++ )
				{
					int c = 0;
					c += dthptr[*(a     )>>4] << 6;
					c += dthptr[*(a + 1 )>>4] << 4;
					c += dthptr[*(a + 2 )>>4] << 2;
					c += dthptr[*(a + 3 )>>4];
					c += dthptr[*(a + 4 )>>4] << 14;
					c += dthptr[*(a + 5 )>>4] << 12;
					c += dthptr[*(a + 6 )>>4] << 10;
					c += dthptr[*(a + 7 )>>4] << 8;
					c += dthptr[*(a + 8 )>>4] << 22;
					c += dthptr[*(a + 9 )>>4] << 20;
					c += dthptr[*(a + 10)>>4] << 18;
					c += dthptr[*(a + 11)>>4] << 16;
					c += dthptr[*(a + 12)>>4] << 30;
					c += dthptr[*(a + 13)>>4] << 28;
					c += dthptr[*(a + 14)>>4] << 26;
					c += dthptr[*(a + 15)>>4] << 24;
					a += 16;
					*dest++ = c;
				}
				dest += gapsize;
			}
			pvtime++; if (pvtime > 4) pvtime = 0;
		}
		else if (method == 6)
		{
			unsigned char* dthptr = (unsigned char*)pv31 + pvtime*32;
			for ( int y = 0; y < 320; y++ )
			{
				for ( int x = 0; x < 15; x++ )
				{
					int c = 0;
					c += dthptr[*(a     )>>3] << 6;
					c += dthptr[*(a + 1 )>>3] << 4;
					c += dthptr[*(a + 2 )>>3] << 2;
					c += dthptr[*(a + 3 )>>3];
					c += dthptr[*(a + 4 )>>3] << 14;
					c += dthptr[*(a + 5 )>>3] << 12;
					c += dthptr[*(a + 6 )>>3] << 10;
					c += dthptr[*(a + 7 )>>3] << 8;
					c += dthptr[*(a + 8 )>>3] << 22;
					c += dthptr[*(a + 9 )>>3] << 20;
					c += dthptr[*(a + 10)>>3] << 18;
					c += dthptr[*(a + 11)>>3] << 16;
					c += dthptr[*(a + 12)>>3] << 30;
					c += dthptr[*(a + 13)>>3] << 28;
					c += dthptr[*(a + 14)>>3] << 26;
					c += dthptr[*(a + 15)>>3] << 24;
					a += 16;
					*dest++ = c;
				}
				dest += gapsize;
			}
			pvtime++; if (pvtime > 9) pvtime = 0;
		}
	#ifdef USE_GAMEX
		gameX->EndDraw();
	#else
		GXEndDraw();
	#endif
		tmindx++; if (tmindx > 15) tmindx = 0;
	}
	else if (BPP == 16)
	{
	#ifdef USE_GAMEX
		if (!gameX->BeginDraw()) return;
		unsigned short* dst = (unsigned short*)getscrptr();
		unsigned short* src = (unsigned short*)buffer;
		if (ypitch > 100)
		{
			for ( int y = 0; y < 320; y++ )
			{
				memcpy( dst, src, 480 );
				dst += ypitch >> 1;
				src += 240;
			}
		}
		else // Must be an aero (buffer rotated by 270 degrees)
		{
			dst += 320;
			for ( int y = 0; y < 320; y++ )
			{
				unsigned short* ldst = dst;
				for ( int x = 0; x < 240; x++ )
				{
					*(ldst) = *src++;
					ldst += (xpitch >> 1);
				}
				dst += ypitch >> 1;
			}
		}
		gameX->EndDraw();
	#else
		// Let's do this really clean (and slow:)
		unsigned short* dst = (unsigned short*)GXBeginDraw();
		if (ypitch < 0) dst += 320;
		unsigned short* src = (unsigned short*)buffer;
		for ( int y = 0; y < 320; y++ )
		{
			unsigned short* ldst = dst;
			for ( int x = 0; x < 240; x++ )
			{
				*(ldst) = *src++;
				ldst += (xpitch >> 1);
			}
			dst += ypitch >> 1;
		}
		GXEndDraw();
	#endif
	}
}

void update( bool updscr )
{
	MSG msg;
	// Nino screen update
	if (updscr) updatedisplay();
	if (PeekMessage( &msg, 0, 0, 0, PM_NOREMOVE ) == TRUE)
	{
		GetMessage( &msg, 0, 0, 0 );
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

bool loadtga( const unsigned short* file )
{
	unsigned long* tgabuff = new unsigned long[160 * 240 + 20];
	HANDLE tga = CreateFile( file, GENERIC_READ, FILE_SHARE_READ,
	                         0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if (tga == INVALID_HANDLE_VALUE) return false;
	unsigned long bytesread;
	ReadFile( tga, tgabuff, 160 * 240 + 20, &bytesread, 0 );
	if (CloseHandle( tga ) == 0) return false;
	unsigned char* dest = (unsigned char*)buffer;
	unsigned char* src = (unsigned char*)tgabuff + 18 + 319 * 480;
	for ( int i = 0; i < 320; i++ )
	{
		memcpy( dest, src, 480 );
		dest += 480;
		src -= 480;
	}
	delete tgabuff;
	return true;
}

bool loadtga( const unsigned short* file, int w, int h, unsigned short* dest, bool pal )
{
	int size;
	if (pal) size = w * h + 20 + 768; else size = w * 2 * h + 20;
	unsigned char* tgabuff = new unsigned char[size];
	HANDLE tga = CreateFile( file, GENERIC_READ, FILE_SHARE_READ,
	                         0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if (tga == INVALID_HANDLE_VALUE) return false;
	unsigned long bytesread;
	ReadFile( tga, tgabuff, size, &bytesread, 0 );
	if (CloseHandle( tga ) == 0) return false;
	if (pal)
	{
		unsigned int pal[256];
		for ( int i = 0; i < 256; i++ )
		{
			int b = *(tgabuff + 18 + i * 3);
			int g = *(tgabuff + 18 + i * 3 + 1);
			int r = *(tgabuff + 18 + i * 3 + 2);
			pal[i] = ((r >> 3) << 11) + ((g >> 2) << 5) + (b >> 3);
		}
		unsigned char* src = tgabuff + 18 + 768 + (h - 1) * w;
		for ( int y = 0; y < h; y++ )
		{
			for ( int x = 0; x < w; x++ ) *(dest + x) = pal[*(src + x)];
			dest += w;
			src -= w;
		}
	}
	else
	{
		unsigned short* src = (unsigned short*)tgabuff + 9 + (h - 1) * w;
		for ( int i = 0; i < h; i++ )
		{
			for ( int x = 0; x < w; x++ )
			{
				int red = (*(src + x) & (31 << 10)) >> 10;
				int green = (*(src + x) & (31 << 5)) >> 5;
				int blue = *(src + x) & 31;
				*(dest + x) = (red << 11) + (green << 6) + blue;
			}
			dest += w;
			src -= w;
		}
	}
	delete tgabuff;
	return true;
}

bool loadfile( const unsigned short* file, void* buff, int size )
{
	HANDLE f = CreateFile( file, GENERIC_READ, FILE_SHARE_READ,
	                         0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if (f == INVALID_HANDLE_VALUE) return false;
	unsigned long bytesread;
	ReadFile( f, buff, size, &bytesread, 0 );
	if (CloseHandle( f ) == 0) return false;
	return true;
}

void snapshot()
{
	HANDLE tga = CreateFile( L"Program Files\\EasyCE\\snapshot.raw", GENERIC_WRITE, FILE_SHARE_READ,
	                         0, CREATE_ALWAYS, 0, 0 );
	unsigned long byteswritten;
	unsigned char* temp = new unsigned char[240 * 320 * 3];
	unsigned short* src = (unsigned short*)buffer;
	for ( int i = 0; i < 240 * 320; i++ )
	{
		*(temp + i * 3) = (*(src + i) >> 11) << 3;
		*(temp + i * 3 + 1) = ((*(src + i) >> 5) << 2) & 255;
		*(temp + i * 3 + 2) = (*(src + i) << 3) & 255;
	}
	WriteFile( tga, temp, 320 * 240 * 3, &byteswritten, 0 );
	delete temp;
	CloseHandle( tga );
}

bool clicked( void )
{
	bool retval = mcprocessed;
	mcprocessed = true;
	return !retval;
}

void cursorxpos( int x )
{
	cursorx = x;
}

void cursorypos( int y )
{
	cursory = y;
}

int cursorxpos( void )
{
	return cursorx;
}

int cursorypos( void )
{
	return cursory;
}

int clickxpos( void )
{
	return mcxpos;
}

int clickypos( void )
{
#ifdef USE_GAPI
	return mcypos + 23;
#else
	return mcypos;
#endif
}

int penxpos( void )
{
	return mxpos;
}

int penypos( void )
{
#ifdef USE_GAPI
	return mypos + 23;
#else
	return mypos;
#endif
}

bool pendown( void )
{
	return mbutton;
}

void line( float x1, float y1, float x2, float y2, int color )
{
	if (y1 > y2)
	{
		float h = y1; y1 = y2; y2 = h;
		h = x1; x1 = x2; x2 = h;
	}
	int len = (int)fabs( x2 - x1);
	if ((int)fabs( y2 - y1 ) > len) len = (int)fabs( y2 - y1 );
	if (len == 0) return;
	float dx = (x2 - x1) / len;
	float dy = (y2 - y1) / len;
	for ( int i = 0; i < len; i++ )
	{
		plot( int(x1), int(y1), color );
		x1 += dx;
		y1 += dy;
	}
}

void hline( int x1, int y1, int len, int color )
{
	for ( int i = 0; i < len; i++ ) plot( x1 + i, y1, color );
}

void vline( int x1, int y1, int len, int color )
{
	for ( int i = 0; i < len; i++ ) plot( x1, y1 + i, color );
}

void box( int x1, int y1, int x2, int y2, int color )
{
	hline( x1, y1, x2 - x1, color );
	vline( x1, y1, y2 - y1, color );
	hline( x1, y2, x2 - x1, color );
	vline( x2, y1, y2 - y1, color );
}

void bar( int x1, int y1, int x2, int y2, int color )
{
	for ( int i = y1; i < y2; i++ ) hline( x1, i, x2 - x1, color );
}

void bar( int x1, int y1, int x2, int y2, int color, unsigned long* buff )
{
	unsigned long* temp = buffer;
	buffer = buff;
	for ( int i = y1; i < y2; i++ ) hline( x1, i, x2 - x1, color );
	buffer = temp;
}

void line_x( float x1, float y1, float x2, float y2, int color )
{
	if (y1 > y2)
	{
		float h = y1; y1 = y2; y2 = h;
		h = x1; x1 = x2; x2 = h;
	}
	int len = (int)fabs( x2 - x1);
	if ((int)fabs( y2 - y1 ) > len) len = (int)fabs( y2 - y1 );
	if (len == 0) return;
	float dx = (x2 - x1) / len;
	float dy = (y2 - y1) / len;
	for ( int i = 0; i < len; i++ )
	{
		plot_x( int(x1), int(y1), color );
		x1 += dx;
		y1 += dy;
	}
}

void hline_x( int x1, int y1, int len, int color )
{
	for ( int i = 0; i < len; i++ ) plot_x( x1 + i, y1, color );
}

void vline_x( int x1, int y1, int len, int color )
{
	for ( int i = 0; i < len; i++ ) plot_x( x1, y1 + i, color );
}

void box_x( int x1, int y1, int x2, int y2, int color )
{
	hline_x( x1, y1, x2 - x1, color );
	vline_x( x1, y1, y2 - y1, color );
	hline_x( x1, y2, x2 - x1, color );
	vline_x( x2, y1, y2 - y1, color );
}

void bar_x( int x1, int y1, int x2, int y2, int color )
{
	for ( int i = y1; i < y2; i++ ) hline_x( x1, i, x2 - x1, color );
}

void plot_x( int x1, int y1, int color )
{
	unsigned char* dest = (unsigned char*)buffer;
	int pixel = x1 + y1 * 240;
	int shft = 6 - ((pixel & 3) << 1);
	*(dest + (pixel >> 2)) = *(dest + (pixel >> 2)) ^ (color << shft);
}

void plot( int x1, int y1, int color )
{
	unsigned short* dest = (unsigned short*)buffer;
	*(dest + x1 + y1 * 240) = color;
}

int getpixel( int x1, int y1 )
{
	unsigned char* dest = (unsigned char*)buffer;
	int pixel = x1 + y1 * 240;
	int shft = 6 - ((pixel & 3) << 1);
	return (*(dest + (pixel >> 2)) & (3 << shft)) >> shft;
}

void clear( int color )
{
	memset( buffer, color, 320 * 240 );
}

unsigned long* getbuffer( void )
{
	return buffer;
}

#ifdef USE_GAMEX
unsigned long* getscrptr( void )
{
	return (unsigned long*)gameX->GetFBAddress();
}
#endif

void resettimer( void )
{
	basetime = GetTickCount();
}

int msecs( void )
{
	return GetTickCount() - basetime;
}

void setchar( int c, char* c1, char* c2, char* c3, char* c4, char* c5, char* c6, char* c7 )
{
	strcpy( m_chr[c][0], c1 );
	strcpy( m_chr[c][1], c2 );
	strcpy( m_chr[c][2], c3 );
	strcpy( m_chr[c][3], c4 );
	strcpy( m_chr[c][4], c5 );
	strcpy( m_chr[c][5], c6 );
	strcpy( m_chr[c][6], c7 );
}

void initcharset()
{
	for ( int x = 0; x < 54; x++ ) for ( int y = 0; y < 7; y++ ) m_chr[x][y] = new char[10]; 
	setchar(  0, "oooo:::","oo:oo::","oo::oo:","oo:::oo","ooooooo","oo:::oo","oo:::oo" );
	setchar(  1, "oooo:::","oo:oo::",":oo::::","ooooo::","oo::oo:","oo:::oo","oooooo:" );
	setchar(  2, "::ooooo",":oo::::","oo:::::","oo:::::","oo:::::","oo:::::",":oooooo" );
	setchar(  3, "oooo:::","oo:oo::","oo::oo:","oo:::oo","oo:::oo","oo:::oo","oooooo:" );
	setchar(  4, "ooooooo","oo:::::","oo:::::","ooooooo","oo:::::","oo:::::","ooooooo" );
	setchar(  5, "ooooooo","oo:::::","oo:::::","ooooo::","oo:::::","oo:::::","oo:::::" );
	setchar(  6, "::oooo:",":oo::::","oo:::::","oo:oooo","oo:::oo","oo:::oo",":oooooo" );
	setchar(  7, "oo::::o","oo::::o","oo::::o","ooooooo","oo::::o","oo::::o","oo::::o" );
	setchar(  8, "::oooo:",":::oo::",":::oo::",":::oo::",":::oo::",":::oo::","::oooo:" );
	setchar(  9, "::ooooo",":::::oo",":::::oo",":::::oo","oo:::oo",":oo:oo:","::ooo::" );
	setchar( 10, "oo::oo:","oo:oo::","oooo:::","ooo::::","ooooo::","oo::oo:","oo:::oo" );
	setchar( 11, "oo:::::","oo:::::","oo:::::","oo:::::","oo:::::","oo:::::","ooooooo" );
	setchar( 12, "o:::::o","oo:::oo","ooo:ooo","ooooooo","oo:o:oo","oo:::oo","oo:::oo" );
	setchar( 13, "oo:::oo","ooo::oo","oooo:oo","oo:oooo","oo::ooo","oo:::oo","oo:::oo" );
	setchar( 14, "::ooo::",":oo:oo:","oo:::oo","oo:::oo","oo:::oo","oo:::oo",":ooooo:" );
	setchar( 15, "ooooo::","oo::oo:","oo:::oo","oo:::oo","oooooo:","oo:::::","oo:::::" );
	setchar( 16, "::ooo::",":oo:oo:","oo:::oo","oo:o:oo","oo:oooo","oo::oo:",":ooo:oo" );
	setchar( 17, "ooooo::","oo::oo:","oo:::oo","oooooo:","oo:oo::","oo::oo:","oo:::oo" );
	setchar( 18, "::ooooo",":oo::::","oo:::::","ooooooo",":::::oo","::::oo:","ooooo::" );
	setchar( 19, "ooooooo",":::oo::",":::oo::",":::oo::",":::oo::",":::oo::",":::oo::" );
	setchar( 20, "oo:::oo","oo:::oo","oo:::oo","oo:::oo","oo:::oo","oo::oo:",":oooo::" );
	setchar( 21, "oo:::oo","oo:::oo",":oo:oo:",":oo:oo:","::ooo::","::ooo::",":::o:::" );
	setchar( 22, "oo:::oo","oo:::oo","oo:::oo","oo:::oo","oo:o:oo",":ooooo:","::o:o::" );
	setchar( 23, "oo::::o",":oo::oo","::oooo:",":::oo::","::oooo:",":oo::oo","oo::::o" );
	setchar( 24, "oo::::o",":oo::oo","::oooo:",":::oo::",":::oo::",":::oo::",":::oo::" );
	setchar( 25, "ooooooo","::::oo:",":::oo::","::oo:::",":oo::::","oo:::::","ooooooo" );
	setchar( 26, ":ooooo:","oo:::oo","oo:::oo",":::::::","oo:::oo","oo:::oo",":ooooo:" );
	setchar( 27, ":::::::",":::::oo",":::::oo",":::::::",":::::oo",":::::oo",":::::::" );
	setchar( 28, ":ooooo:",":::::oo",":::::oo",":ooooo:","oo:::::","oo:::::",":ooooo:" );
	setchar( 29, ":ooooo:",":::::oo",":::::oo",":ooooo:",":::::oo",":::::oo",":ooooo:" );
	setchar( 30, ":::::::","oo:::oo","oo:::oo",":ooooo:",":::::oo",":::::oo",":::::::" );
	setchar( 31, ":ooooo:","oo:::::","oo:::::",":ooooo:",":::::oo",":::::oo",":ooooo:" );
	setchar( 32, ":ooooo:","oo:::::","oo:::::",":ooooo:","oo:::oo","oo:::oo",":ooooo:" );
	setchar( 33, ":ooooo:",":::::oo",":::::oo",":::::::",":::::oo",":::::oo",":::::::" );
	setchar( 34, ":ooooo:","oo:::oo","oo:::oo",":ooooo:","oo:::oo","oo:::oo",":ooooo:" );
	setchar( 35, ":ooooo:","oo:::oo","oo:::oo",":ooooo:",":::::oo",":::::oo",":ooooo:" );
	setchar( 36, ":::::::",":::::::",":::o:::",":::::::",":::::::",":::o:::",":::::::" );
	setchar( 37, ":::oo::",":::oo::",":::oo::",":::oo::",":::::::",":::oo::",":::oo::" );
	setchar( 38, ":oooo::","::::oo:","::::oo:",":::oo::",":::::::","::oo:::","::oo:::" );
	setchar( 39, ":::::::",":::::::",":::::::","::oooo:",":::::::",":::::::",":::::::" );
	setchar( 40, ":::::::",":::::::",":::::::",":ooooo:",":::::::",":ooooo:",":::::::" );
	setchar( 41, ":::::::",":::o:::",":::o:::",":::o:::",":::o:::",":::o:::",":::::::" );
	setchar( 42, "::ooo::","::o::::","::o::::","::o::::","::o::::","::o::::","::ooo::" );
	setchar( 43, "::ooo::","::::o::","::::o::","::::o::","::::o::","::::o::","::ooo::" );
	setchar( 44, ":::::::",":::::::",":::::::",":::::::",":::::::",":::::::",":::::::" );
	setchar( 45, ":::::::",":::::::",":::::::",":::::::",":::::::",":::oo::",":::oo::" );
	setchar( 46, ":::::::",":::::::",":::::::",":::::::","::::o::","::::o::",":::o:::" );
	setchar( 47, ":o:::::","::o::::",":::o:::","::::o::",":::o:::","::o::::",":o:::::" );
	setchar( 48, "::::o::",":::o:::","::o::::",":o:::::","::o::::",":::o:::","::::o::" );
	setchar( 49, "ooooooo","ooooooo","ooooooo","ooooooo","ooooooo","ooooooo","ooooooo" );
	setchar( 50, ":::o:::",":::o:::","::o::::",":::::::",":::::::",":::::::",":::::::" );
	setchar( 51, "::ooo::",":o:::o:","o::oo:o","o:o:::o","o::oooo",":o:::::","::oooo:" );
	setchar( 52, "::::::o",":::::o:","::::o::",":::o:::","::o::::",":o:::::","o::::::" );
	setchar( 53, "o::::::",":o:::::","::o::::",":::o:::","::::o::",":::::o:","::::::o" );
	// Redir table
	char c[] = "abcdefghijklmnopqrstuvwxyz0123456789:!?-=+[] .,><#'@/S";
	for ( int i = 0; i < 256; i++ ) m_transl[i] = 44;
	for ( i = 0; i < 54; i++ ) m_transl[c[i]] = i;
}

void print( char* a_String, int x1, int y1, int color )
{
	int curx = x1;
	for ( int i = 0; i < (int)(strlen( a_String )); i++ )
	{	
		long pos = 0;
		if ((a_String[i] == 10) || (a_String[i] == 13)) curx = 400;
		else if (a_String[i] == 8)
		{
			if (curx > 2) curx -= 8;
		}
		else 
		{
			if ((a_String[i] >= 'A') && (a_String[i] <= 'Z')) pos = m_transl[a_String[i] - ('A' - 'a')];
														 else pos = m_transl[a_String[i]];
			for ( int h = 0; h < 7; h++ ) for ( int v = 0; v < 7; v++ )
				if (m_chr[pos][v][h] == 'o') plot( curx + h, y1 + v, color );
			curx += 8;
		}
		if (curx > 230)
		{
			curx = 2;
			if (y1 < 300) y1 += 9;
		}
	}
	cursorx = curx;
	cursory = y1;
}

void print( char* a_String, int color )
{
	print( a_String, cursorx, cursory, color );
}

void print( char* a_String, int color, unsigned long* buff )
{
	unsigned long* temp = buffer;
	buffer = buff;
	print( a_String, cursorx, cursory, color );
	buffer = temp;
}

void play( unsigned short* s )
{
	PlaySound( s, NULL, SND_ASYNC + SND_FILENAME + SND_NOWAIT );
}

void stopsound( void )
{
	PlaySound( 0, NULL, SND_FILENAME + SND_ASYNC );
}

// =========================================
// =========================================
// MISSING STUFF FROM WINDOWS FOR WINDOWS CE
// =========================================
// =========================================

double strtod( char* t, char** e )
{
	float mul = 1;
	float retval = 0;
	char* p = t;
	while (((*p >= '0') && (*p <= '9')) || (*p == '.'))
	{
		if (*p == '.')
		{
			if (mul < 1) 
			{
				p++;
				break;
			}
			mul = 0.1f;
		}
		else
		{
			if (mul > 0.1)
			{
				retval = retval * 10 + (*p - '0');
			}
			else
			{
				retval += mul * (float)(*p - '0');
				mul *= 0.1f;
			}
		}
		p++;
	}
	e = &p;
	return (double)retval;
}

int stricmp( char* s1, char* s2 )
{
	char* a = s1;
	char* b = s2;
	while ((*a) || (*b))
	{
		char c1 = *a;
		char c2 = *b;
		if ((c1 >= 'A') && (c1 <= 'Z')) c1 -= 'a' - 'A';
		if ((c2 >= 'A') && (c2 <= 'Z')) c2 -= 'a' - 'A';
		if (c1 == c2)
		{
			a++;
			b++;
		}
		else if (c1 < c2) return -1; else return 1; 
	}
	return 0;
}

// --------------------------------------------------
// EOF