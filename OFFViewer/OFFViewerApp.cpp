/*****************************************************************************\
* Sample application that draw cube
\*****************************************************************************/

#include "OFFViewerApp.h"

#define PhongX 256
#define PhongY 256

MainApp::MainApp(Device & aDevice, const char * OFFFile)
: inherited(aDevice),
fPhongImage(new Image(PhongX, PhongY)),
fFace(new Object(&fImage, OFFFile, ftOFF, fPhongImage)),
fScene(fFace->FaceCount()),
fFont("Font6x10.fon"),
fWriter(fImage)
{
  fWriter.Plug(fImage);
  fLight = new SunLight(Vector3(0,0,1),255-128);
  fLight->SetDirection(0,M_PI/3,0);

  fScene.AttachObject(*fFace);

  fCamera.TranslateZ(-(4.0*32.0)*fCamera.Focal());
  fCamera.SetZoom(1.0);

  fFace->SetColor(63);
  fFace->SetShading(255-64);

  // compute phong shades
  for (int x=0; x<PhongX; x++)
    for (int y=0; y<PhongY; y++)
    {
      real rx = (x-real(PhongX)/2.0) / real(PhongX);
      real ry = (y-real(PhongY)/2.0) / real(PhongY);
      real dist = sqrt( sqr(rx)+ sqr(ry) )*3.1415;
      Color color = Color(real( sqr(cos(dist/sqrt(2)))*(256.0-64) ))+63;
      fPhongImage->Pixel(x, y, color);
    }
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
  case kUP : fLight->RotateY(RotationStep*10); break;
  case kDN : fLight->RotateY(-RotationStep*10); break;
  case kLE : fLight->RotateX(RotationStep*10); break;
  case kRI : fLight->RotateX(-RotationStep*10); break;
  case kTB :
    break;
  default : inherited::OnKeyPressed(aKey);
  }
}


void MainApp::Draw()
{
  static int frameCount = 0;
  static int timeCount = 0;
  static int lastCount = 0;
  Object * object;
  fDevice->StartChrono();
  fImage.ClearAll();
  //SunLight light2(Vector3(0,0,1),NCLUT);
  object = fFace;
  object->Update(fCamera);
  // the object will be drawed sorted, but the algo is mmuch better in the scene
  //object->Draw(fCamera, *light, true);
  //for (int i = 0; i< 32; i++) // for speed mesurement only
  fScene.Draw(fCamera, *fLight);
  /* // to display the phong image
  LinearMappedFaceDrawer aDrawer(fImage, *fPhongImage);
  PointZ points[4];
  points[0] = PointZ(0,0,0);
  points[1] = PointZ(0, WinY, 0);
  points[2] = PointZ(WinX, WinY, 0);
  points[3] = PointZ(WinX, 0, 0);
  aDrawer.Draw(4, points);*/


  timeCount += fDevice->GetChrono();
  frameCount++;
  if (timeCount > 1000)
  {
    timeCount -= 1000;
    lastCount = frameCount;
    frameCount = 0;
  }

  fstring time;
  Int2Str(lastCount, time);
  ConcatStr(time, " fps");
  fWriter.WriteString(PointZ(0,0,128), fFont, time);
  inherited::Draw();
}


