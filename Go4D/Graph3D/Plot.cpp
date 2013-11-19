/**
*AUTHOR : Babel yoann
*TITLE  : Plot.cpp, Plot.h implementation
*CREATED: 10/07/96
*DATE   : 10/07/96
*BUGS   :
*/

#include "Plot.h"

namespace Go4D
{


  /*****************************************************************************\
  * APlot
  \*****************************************************************************/

  APlot & APlot::ProjectionParallele(int tx, int ty, real zoom, real lad)
  {
    if (!fProcessed)
    {
      fProcessed=true;
      fX = (int)(fElems[0]*zoom*lad) + tx;
      fY = (int)(fElems[1]*zoom*lad) + ty;
      fZ = fElems[2];
    }
    return *this;
  }

  APlot & APlot::ProjectionPerspective(real focal, int tx, int ty,
    real zoom, real ladder)
  {
    if (!fProcessed)
    {
      fProcessed=true;
      if ( fabs(fElems[2]) > zero-focal)
      {
        real k = ladder/2*zoom*focal/(fElems[2]+focal);
        fX = (int) (fElems[0]*k) + tx;
        fY = (int) (fElems[1]*k) + ty;
        fZ = fElems[2];
      }
      else error("not projected !!!!");
    }
    return *this;
  }

  PointZ APlot::GetPointZ() const
  {
    assert(fProcessed==true);
    assert(fColor<256);
    assert(fColor>=0);
    return PointZ(fX, fY, Color(fColor), fZ);
  }

  APlot & APlot::ProcessColor(Color aColorMin, Color aNbColors)
  {
    if (!fColorProcessed)
    {
      fColor += aColorMin;
      if (fColor >= aColorMin+aNbColors)
        fColor = aColorMin+aNbColors-1;
      else if (fColor < aColorMin)
        fColor = aColorMin;
      fColorProcessed = true;
    }
    return *this;
  }

  APlot & APlot::ProcessGouraudColor(Color aColorMin, Color aNbColors)
  {
    if (!fColorProcessed)
    {
      fColor = fCount==0?0:fColor/fCount;
      fColor += aColorMin;
      if (fColor >= aColorMin+aNbColors)
        fColor = aColorMin+aNbColors-1;
      else if (fColor < aColorMin)
        fColor = aColorMin;
      fColorProcessed = true;
    }
    return *this;
  }


  void APlot::ResetNormal()
  {
    fSumNormal.Set(0,0,0);
    fNormalCount = 0;
  }

  void APlot::AddNormal(const Vector3 &aNormal)
  {
    fSumNormal += aNormal;
    fNormalCount++;
  }

  Vector3 APlot::GetAverageNormal()
  {
    assert(fNormalCount > 0);
    Vector3 result = fSumNormal/real(fNormalCount);
    return result;
  }

#ifdef DEBUGOUT
  ostream & operator << (ostream & os, const APlot & aPlot)
  {
    return aPlot.DebugOut(os);
  }

  ostream & APlot::DebugOut(ostream & os) const
  {
    os << "APlot" << incendl
      << "{" << iendl
      << "X:" << fX << " - Y:" << fY << " - fZ:" << fZ << iendl
      << "Color:" << fColor << " - Count:" << fCount
      << " - Processed:" << fProcessed << iendl
      << "}" << decendl;
    return os;
  }
#endif

  APlot & APlot::operator +=(const Vector3 & v)
  {
    fElems[0] += v[0];
    fElems[1] += v[1];
    fElems[2] += v[2];
    return *this;
  }

  APlot & APlot::operator =(const Vector3 & v)
  {
    fElems[0] = v[0];
    fElems[1] = v[1];
    fElems[2] = v[2];
    return *this;
  }

}
