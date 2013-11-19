/*****************************************************************************\
* Sample application that draw cube
\*****************************************************************************/

#include "CubeDemo.h"

using namespace Go4D;

char imagename[256];
char filtername[256];

#define PhongX 256
#define PhongY 256

MainApp::MainApp(Device & aDevice, const Image * aMappedImage,
                 const MultiShadeClut * aMultiClut1,
                 const MultiShadeClut * aMultiClut2,
                 const TransluentClut * aTransluentClut,
                 FilteredImage * aBumpImage)
                 : inherited(aDevice),
                 fMappedImage(aMappedImage),
                 fPhongImage(new Image(PhongX, PhongY)),
                 fMultiClut1(aMultiClut1),
                 fMultiClut2(aMultiClut2),
                 fTransluentClut(aTransluentClut),
                 fBumpImage(aBumpImage),
                 fTransluentCube(&fImage, fMappedImage, fTransluentClut),
                 fLinBumpCube(&fImage, fMappedImage, fMultiClut1, fBumpImage),
                 fLinShadeCube(&fImage, fMappedImage, fMultiClut2),
                 //fLinCube(&fImage, fMappedImage),
                 fLinCube(&fImage, fPhongImage),
                 fQuadCube(&fImage, fMappedImage),
                 fHypCube(&fImage, fMappedImage),
                 fFlatCube(&fImage),
                 fGourCube(&fImage)
{

  fCamera.TranslateZ(-4*fCamera.Focal());
  fCamera.SetZoom(3);
  fTransluentCube.FaceInit();
  fLinBumpCube.FaceInit();
  fLinShadeCube.FaceInit();
  fLinCube.FaceInit();
  fQuadCube.FaceInit();
  fHypCube.FaceInit();
  fFlatCube.FaceInit();
  fGourCube.FaceInit();

  fLinBumpCube.SetColor(0);
  fLinBumpCube.SetShading(NCLUT);
  fLinShadeCube.SetColor(0);
  fLinShadeCube.SetShading(NCLUT);
  fFlatCube.SetColor(64);
  fFlatCube.SetShading(255-64);
  fGourCube.SetColor(64);
  fGourCube.SetShading(255-64);
  fCubeType = ctFlat;

  // compute phong shades
  for (int x=0; x<PhongX; x++)
    for (int y=0; y<PhongY; y++)
    {
      real rx = (x-real(PhongX)/2.0) / real(PhongX);
      real ry = (y-real(PhongY)/2.0) / real(PhongY);
      real dist = sqrt( sqr(rx)+ sqr(ry) )*3.1415;
      Color color = Color(real( sqr(cos(dist))*256.0 ));
      fPhongImage->Pixel(x, y, color);
    }
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
  case kTB :
    fCubeType= CubeType(int(fCubeType)+1);
    if (fCubeType==ctLAST) fCubeType = ctFlat;

    switch (fCubeType)
    {
    case ctTransluent :
      fDevice->ChangeCurrentPalette(fClutPalette);
      break;
    case ctLinBump :
      fDevice->ChangeCurrentPalette(fClutPalette);
      break;
    case ctLinShaded :
      fDevice->ChangeCurrentPalette(fClutPalette);
      break;
    case ctLinMapped :
      //fDevice->ChangeCurrentPalette(fClutPalette);
      fDevice->ChangeCurrentPalette(fDefaultPalette);
      break;
    case ctQuadMapped :
      fDevice->ChangeCurrentPalette(fClutPalette);
      break;
    case ctHypMapped :
      fDevice->ChangeCurrentPalette(fClutPalette);
      break;
    case ctFlat :
      fDevice->ChangeCurrentPalette(fDefaultPalette);
      break;
    case ctGouraud :
      fDevice->ChangeCurrentPalette(fDefaultPalette);
      break;
    }

    break;
  default : inherited::OnKeyPressed(aKey);
  }
}


void MainApp::Draw()
{
  Object * object;
  fImage.ClearAll();
  SunLight light1(Vector3(0,0,1),255-64);
  SunLight light2(Vector3(0,0,1),NCLUT);
  SunLight * light;

  switch (fCubeType)
  {
  case ctTransluent :
    object = & fTransluentCube; light = &light1;
    break;
  case ctLinBump :
    object = & fLinBumpCube; light = &light2;
    break;
  case ctLinShaded :
    object = & fLinShadeCube; light = &light2;
    break;
  case ctLinMapped :
    object = & fLinCube; light = &light1;
    break;
  case ctQuadMapped :
    object = & fQuadCube; light = &light1;
    break;
  case ctHypMapped :
    object = & fHypCube; light = &light1;
    break;
  case ctFlat :
    object = & fFlatCube; light = &light1;
    break;
  case ctGouraud :
    object = & fGourCube; light = &light1;
    break;
  }
  object->Update(fCamera);
  object->Draw(fCamera, *light, true);

  /*
  // special : add filter on the linear mapped cube, in order to see if the
  // filter works !!!
  if (fCubeType == ctLinMapped)
  {
  byte * ima = fImage.GetBuffer();
  for (int i=0; i<fImage.Len(); i++, ima++)
  *ima = (*fTransluentClut)[*ima];
  }
  */
  inherited::Draw();
}

int Start(char * cmdLine)
{
  if ((cmdLine == NULL) || (cmdLine[0] == '\0'))
  {
    CopyStr(imagename, "./test1.pcx");
    CopyStr(filtername,"./bump2.pcx");
  }
  else
  {
    int i,j;
    for (i=0; cmdLine[i]!=' '; i++)
      imagename[i] = cmdLine[i];
    imagename[i++] = '\0';
    for (j=0; cmdLine[i]!='\0'; i++, j++)
      filtername[j] = cmdLine[i];
    filtername[j] = '\0';
  }

  Pcx image(imagename);
  Pcx filter(filtername);
  FilteredImage bump(filter.Width(), filter.Height(), filter);

  // building palette for shaded mapping
  PaletteBuilder palette(image.GetPalette().Count()*(NCLUT*2+1), NCLUT*2);
  CountPalette cpal(image.GetPalette());
  cpal.AddImage(image);

  real coef=0;
  int i;
  for (i=0; i<NCLUT*2; i++, coef+=STEP/2)
    palette.AddPalette(cpal*coef);
  RGBColor argb(0,255,255);
  for (i=0; i<NCLUT; i++)
    palette.AddPalette(cpal^argb);
  Palette best = palette.Best();

  image.ImportPalette(best);
  MultiShadeClut clut1(best, NCLUT*2);
  coef=0;
  for (i=0; i<NCLUT*2; i++, coef+=STEP/2)
    clut1.SetClut(i,coef);
  MultiShadeClut clut2(best, NCLUT);
  coef=STEP;
  for (i=0; i<NCLUT; i++, coef+=STEP)
    clut2.SetClut(i,coef);
  TransluentClut tclut(best);
    tclut.Init(argb);
  //ofstream out("test.txt");
  //out << tclut << (cpal^argb);

  Palette defPal;
  defPal.RedShade();
  Device * dev= NewDevice(240,320,&defPal);
  MainApp app(*dev, &image, &clut1, &clut2, &tclut, &bump);
  app.fDefaultPalette = &defPal;
  app.fClutPalette = &best;

  return app.Launch();
}


void MainApp::Cycle()
{
  Draw();
}
