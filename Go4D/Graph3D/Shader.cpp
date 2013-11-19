#include "Shader.h"

namespace Go4D
{

  DrawerBumpData::DrawerBumpData(const Image *aMappedImage,
    const MultiClut * aMultiClut, const Image *aShadeImage)
    : inherited(aMappedImage, aMultiClut)
  {
    fShadeImage = aShadeImage;
  }

}
