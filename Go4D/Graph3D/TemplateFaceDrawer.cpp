#include "TemplateFaceDrawer.h"

namespace Go4D
{


  TFaceDrawer::TFaceDrawer(const Image &aImage, int PointsCount,
    const FaceDrawerData * aDrawerData)
    : inherited(aImage, PointsCount)
  {
    fData = aDrawerData;
  }

  DrawerImageData::DrawerImageData(const Image * aMappedImage)
  {
    fMappedImage = aMappedImage;
  }

  DrawerImageClutData::DrawerImageClutData(
    const Image * aMappedImage, const MultiClut * aMultiClut)
    : inherited(aMappedImage)
  {
    fMultiClut = aMultiClut;
  }


}
