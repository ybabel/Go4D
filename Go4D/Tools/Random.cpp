#include "Random.h"
#include <math.h>


namespace Go4D
{

  /******************************************************************************\
  * Vector3Randomizer
  \******************************************************************************/

  Vector3Randomizer::Vector3Randomizer()
  {
    fRange = 1.0;
    fTransformCoef = 1.0;
    fTransformTranslate = 0.0;
  }

  void Vector3Randomizer::SetRange(real value)
  {
    fRange = value;
  }

  Vector3 & Vector3Randomizer::Transform(Vector3 & vector)
  {
    vector = vector*fTransformCoef+Vector3(fTransformTranslate, fTransformTranslate, fTransformTranslate);
    return vector;
  }

  void Vector3Randomizer::SetAffineTransform(real coef, real translat)
  {
    fTransformCoef = coef;
    fTransformTranslate = translat;
  }

  /******************************************************************************\
  * UniformVector3Randomizer
  \******************************************************************************/

  Vector3 UniformVector3Randomizer::Generate()
  {
    // YOB GCC4.1
    Vector3 temp = Vector3(frand*fRange, frand*fRange, frand*fRange);
    return Transform(temp);
  }

  /******************************************************************************\
  * HeightField
  \******************************************************************************/

  real SQRT(real x)
  {
    return sqrt(x);
  }

  real EQUAL(real x)
  {
    return x;
  }

  bool SUP(real a, real b)
  {
    return a>b;
  }

  bool SUPEQ(real a, real b)
  {
    return a>=b;
  }

  bool INF(real a, real b)
  {
    return a<b;
  }

  bool INFEQ(real a, real b)
  {
    return a<=b;
  }

#define LEN fWidth*fHeight
#define H(x,y) fField[x+fWidth*y]
  HeightField::HeightField(int width, int height, real azero, real amin, real amax)
  {
    fWidth = width;
    fHeight = height;
    fMin = amin;
    fMax = amax;
    fZero = azero;
    fField = new real[LEN];
  }

  HeightField::~HeightField()
  {
    delete [] fField;
  }

  void HeightField::SetHeight(int x, int y, real h)
  {
    h = h<fMin?fMin:h;
    h = h>fMax?fMax:h;
    h = h<fZero?0.0:h;
    H(x, y) = h;
  }

  real HeightField::GetHeight(int x, int y) const
  {
    return H(x, y);
  }

  void HeightField::GenerateFractalHeights(real max, Repartition func)
  {
    FractalHeight(0,0, fWidth, fHeight, max, func);
  }

  void HeightField::FractalHeight(int xmin, int ymin, int xmax, int ymax, real height, Repartition func)
  {
    assert(xmin>=0);
    assert(xmax<=fWidth);
    assert(ymin>=0);
    assert(ymax<=fHeight);
    assert(xmin<=xmax);
    assert(ymin<=ymax);
    if (
      (xmax-xmin<=1) && (ymax-ymin<=1)
      )
    {
      SetHeight(xmin, ymin, height);
      return;
    }
    int x = (xmin+xmax)/2;
    int y = (ymin+ymax)/2;
    FractalHeight(xmin, ymin, x, y, (*func)(frand)*height, func);
    FractalHeight(xmin, y, x, ymax, (*func)(frand)*height, func);
    FractalHeight(x, ymin, xmax, y, (*func)(frand)*height, func);
    FractalHeight(x, y, xmax, ymax, (*func)(frand)*height, func);
  }

  void HeightField::CellToXY(int cell, int &x, int &y)
  {
    assert(cell>=0);
    assert(cell<LEN);
    y = cell/fWidth;
    x = cell-y*fWidth;
    assert(x>=0);
    assert(x<fWidth);
    assert(y<fHeight);
  }

  real HeightField::Max() const
  {
    real result = FLT_MIN;
    for (int x=0; x<fWidth; x++)
      for (int y=0; y<fHeight; y++)
        if (H(x,y)>result)
          result = H(x,y);
    return result;
  }

  real HeightField::Min() const
  {
    real result = FLT_MAX;
    for (int x=0; x<fWidth; x++)
      for (int y=0; y<fHeight; y++)
        if (H(x,y)<result)
          result = H(x,y);
    return result;
  }

  int HeightField::NumberCellsSuperior(real threshold) const
  {
    int result = 0;
    for (int x=0; x<CellCount(); x++)
      if (Cell(x)>threshold)
        result++;
    return result;
  }

  int HeightField::NumberCellsInferior(real threshold) const
  {
    int result = 0;
    for (int x=0; x<CellCount(); x++)
      if (Cell(x)<threshold)
        result++;
    return result;
  }

  int HeightField::NumberCells(real threshold, Compare func) const
  {
    int result = 0;
    for (int x=0; x<CellCount(); x++)
      if ((*func)(Cell(x),threshold))
        result++;
    return result;
  }

  /******************************************************************************\
  * NonUniformVector3Randomizer
  \******************************************************************************/

  NonUniformVector3Randomizer::NonUniformVector3Randomizer(HeightField * field)
  {
    fHeightField = field;
    assert(fHeightField->Max() <= 1.0);
    assert(fHeightField->Min() >= 0.0);
  }

  NonUniformVector3Randomizer::~NonUniformVector3Randomizer()
  {
  }

  Vector3 NonUniformVector3Randomizer::Generate()
  {
    int count = 0;
    real threshold;
    while (count==0)
    {
      threshold = frand;
      count = fHeightField->NumberCellsSuperior(threshold);
    }
    int index = random(count);
    int realcell=0;
    for (int i=0; i<fHeightField->CellCount(); i++)
      if (fHeightField->Cell(i)>threshold)
      {
        if (realcell==index)
        {
          realcell = i;
          break;
        }
        realcell++;
      }
      assert(fHeightField->Cell(realcell) > threshold);
      int cellx=-1, celly=-1;
      fHeightField->CellToXY(realcell, cellx, celly);
      assert(fHeightField->GetHeight(cellx, celly) > threshold);
      // Get a random point in the cell
      real x = (real(cellx)+frand)/real(fHeightField->Width());
      real y = (real(celly)+frand)/real(fHeightField->Height());
      // YOB GCC4.1
      Vector3 temp = Vector3(x, y, frand);
      return Transform(temp);
  }

}
