#ifdef UNDER_CE
#include "Device_CE.cpp"
#endif
#ifdef WING
#include "Device_WING.cpp"
#endif
#ifdef LINUX
#include "DeviceX.cpp"
#endif
#ifdef SDL

// By default we assume that the device to use is : SDL

#include <SDL/SDL.h>

#include "Device.h"

namespace Go4D
{

Application::Application(Device & device)
{
  fDevice = &device;
  fDevice->SetApplication(this);
}

class DeviceSDL : public Device
{
private :
  Application *fApplication;

public :
  DeviceSDL(int Width, int Height, const Palette *p=NULL);
  ~DeviceSDL(void);
  virtual void SetApplication(Application *);
  virtual void AutoRepeatOff();
  virtual void AcceptImage(Image *);
  virtual void DisplayImage();
  virtual void ChangeCurrentPalette(const Palette *pPal=NULL);
  virtual int  Width() const { return fWidth; }
  virtual int  Height() const { return fHeight; }
  virtual void StartChrono();
  virtual int  GetChrono();       // in millisecond, 1 day max !!Second
  virtual void Close();
  virtual int  MainLoop();

private :
  timeval  fChronoTime;
  bool     fDraging, fHaveMoved;
  int      fDragX, fDragY;
  int      fButton;
  bool     fCtrlPressed, fShiftPressed;

  int      fWidth, fHeight;
  SDL_Surface *fScreen;
  int MouseCode(Uint8 code);
  word Code(SDL_keysym code);
};


int Application::Launch()
{
  return fDevice->MainLoop();
}

Device * NewDevice(int Width, int Height, const Palette *p)
{
    return new DeviceSDL(Width, Height, p);
}

DeviceSDL::DeviceSDL(int Width, int Height, const Palette *p)
{
    fWidth = Width;
    fHeight = Height;
    fCtrlPressed = false;
    fShiftPressed = false;
    fDraging = false;
    fHaveMoved = false;
    fDragX = 0;
    fDragY = 0;
    fButton = 0;
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        cerr << "Unable to initialize SDL : " << SDL_GetError() << endl;
        exit(1);
    }
    assert(sizeof(Color) == 1);
    fScreen = SDL_SetVideoMode(fWidth, fHeight, sizeof(Color)*8, SDL_DOUBLEBUF);
    //cout << fScreen->format->BytesPerPixel << endl;
    if (fScreen == NULL) {
      cerr << "Unable to set video mode: " << SDL_GetError() << endl;
      exit(1);
    }
    ChangeCurrentPalette(p);
}

DeviceSDL::~DeviceSDL(void)
{
    Close();
}

void DeviceSDL::SetApplication(Application * app)
{
    fApplication = app;
}

void DeviceSDL::AutoRepeatOff()
{
}

void DeviceSDL::AcceptImage(Image * pImage)
{
    // change the buffer of the image to the one of the WinG
    assert( pImage->Width() == fWidth);
    assert( pImage->Height() == fHeight);
    pImage->NewBuffer( (byte *) fScreen->pixels);
}

void DeviceSDL::DisplayImage()
{
    SDL_Flip(fScreen);
}

void DeviceSDL::ChangeCurrentPalette(const Palette *pPal)
{
    fPalette = pPal;
    SDL_Color colors[pPal->Count()];
    int i;
    /* Fill colors with color information */
    for(i=0;i<pPal->Count();i++)
    {
        colors[i].r=(*pPal)[i].R();
        colors[i].g=(*pPal)[i].G();
        colors[i].b=(*pPal)[i].B();
    }
    SDL_SetPalette(fScreen, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, pPal->Count());
}

void DeviceSDL::StartChrono()
{
}

int  DeviceSDL::GetChrono()
{
}

void DeviceSDL::Close()
{
    SDL_Quit();
}

int DeviceSDL::MouseCode(Uint8 code)
{
  int result = 0;
  switch (code)
    {
    case SDL_BUTTON_LEFT : result = kMLEFT; break;
    case SDL_BUTTON_RIGHT : result = kMRIGHT; break;
    default:
      result = kNONE;
      break;
    }
  if(fCtrlPressed) result |= kCtrl;
  if(fShiftPressed) result |= kShift;
  return result;
}

word DeviceSDL::Code(SDL_keysym code)
{
  switch (code.sym)
    {
    case SDLK_LCTRL:
    case SDLK_RCTRL : return kCtrl;
    case SDLK_RSHIFT :
    case SDLK_LSHIFT : return kShift;
    case SDLK_END : return kEND;
    case SDLK_UP : return kUP;
    case SDLK_DOWN : return kDN;
    case SDLK_RIGHT : return kRI;
    case SDLK_LEFT : return kLE;
//    case SDLK_BACKSPACE : return kBS;
    case SDLK_TAB : return kTB;
    case SDLK_SPACE : return kSP;
    case SDLK_RETURN : return kLF;
    case SDLK_PAGEUP : return kBW;
    case SDLK_PAGEDOWN : return kFW;

    case SDLK_a : return kKA;
    case SDLK_z : return kKZ;
//    case SDLK_e : return kKE;
    case SDLK_r : return kKR;
    case SDLK_t : return kKT;
//    case SDLK_y : return kKY;
    case SDLK_u : return kKU;
    case SDLK_i : return kKI;
//    case SDLK_o : return kKO;
    case SDLK_p : return kKP;

    case SDLK_q : return kKQ;
    case SDLK_s : return kKS;
//    case SDLK_d : return kKD;
//    case SDLK_f : return kKF;
//    case SDLK_g : return kKG;
//    case SDLK_h : return kKH;
//    case SDLK_j : return kKJ;
//    case SDLK_k : return kKK;
//    case SDLK_l : return kKL;
//    case SDLK_m : return kKM;

    case SDLK_w : return kKW;
//    case SDLK_x : return kKX;
    case SDLK_c : return kKC;
    case SDLK_v : return kKV;
//    case SDLK_b : return kKB;
    case SDLK_n : return kKN;
    default:
      cerr << "Unsupported Key #" << code.sym << endl;
      return kNONE;
    }
}



int  DeviceSDL::MainLoop()
{
    SDL_Event event;
    bool quit = false;
    int x=0, y=0;
    int keyCode;

    while (!quit)
    {
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT: {
                    cout << "Quit requested, quitting." << endl;
                    quit = true;
                }

                case SDL_KEYDOWN :
                    keyCode = Code(event.key.keysym);
                    fCtrlPressed = (keyCode==kCtrl);
                    fShiftPressed = (keyCode==kShift);
                    #ifdef DEBUG
                    cerr << "KeyPressed#" << keyCode << " (" << keyCode << ")" << endl;
                    #endif
                    break;
                case SDL_KEYUP :
                    keyCode = Code(event.key.keysym);
                    fCtrlPressed &= (keyCode!=kCtrl);
                    fShiftPressed &= (keyCode!=kShift);
                    #ifdef DEBUG
                    cerr << "KeyReleased#" << keyCode << " (" << keyCode << ")" << endl;
                    #endif
                    fApplication->OnKeyPressed(keyCode);
                    break;

                case SDL_MOUSEBUTTONDOWN :
                    if (fDraging) break;
                    fDragX=event.motion.x; fDragY=event.motion.y;
                    fDraging = true;
                    break;
                case SDL_MOUSEBUTTONUP :
                    x=event.motion.x; y=event.motion.y;
                    if (!fHaveMoved)
                    {
                        fApplication->OnMouseClick(MouseCode(event.button.button), x, y);
                    }
                    else
                    {
                        fApplication->OnDragEnd(MouseCode(event.button.button), x, y);
                    }
                    fDraging = false;
                    fHaveMoved = false;
                    break;
                case SDL_MOUSEMOTION :
                    if (fDraging)
                    {
                        x=event.motion.x; y=event.motion.y;
                        if (x!=fDragX || y!=fDragY) // Protect against abusive move, occurs sometimes !
                        {
                          if (fHaveMoved)
                            fApplication->OnDragOver(MouseCode(event.button.button), x-fDragX, y-fDragY);
                          else
                            fApplication->OnDragStart(MouseCode(event.button.button), x, y);
                          fDragX=x; fDragY=y;
                          fHaveMoved = true;
                        }
                    }
                    break;

            }
        }
        fApplication->Cycle();
    }
}


}


#endif
