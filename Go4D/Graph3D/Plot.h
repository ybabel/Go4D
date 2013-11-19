#ifndef _APLOT_H_
#define _APLOT_H_

/**
*AUTHOR : Babel yoann
*TITLE  : Plot.h
*CREATED: 10/07/96
*DATE   : 10/07/96
*OBJECT : Plot class, this class is used to represent a plot in the 3D space.
*         it's optimzed in order to not be calculated many times during the
*         rendering process (a plot can be common to many faces, it would be
*         time consuming to project it each times it's used in a face)
*/

#include "FaceDrawer.h"
#include "Tools/Geom.h"

namespace Go4D
{


  /**
  * APLOT :
  *  This plot is used by the faces.
  */
  class APlot : public Vector3
  {
    typedef Vector3 inherited;
  public :
    ///
    APlot();
    ///
    APlot(real x, real y, real z);
    ///
    APlot(const APlot &);
    ///
    APlot(const Vector3 &);
    ///
    APlot & ProjectionParallele(int TranslateX, int TranslateY,
      real Zoom, real Ladder);
    ///
    APlot & ProjectionPerspective(real Focal,
      int TranslateX, int TranslateY,
      real Zoom, real Ladder);
    ///
    int X() const;
    ///
    int Y() const;
    ///
    real Z() const;
    ///
    int & GetColor();
    ///
    int Count() const;
    ///
    APlot & AddColor(int Color);
    ///
    PointZ GetPointZ() const;
    ///
    APlot & ProcessColor(Color ColorMin, Color NbColors);
    ///
    APlot & ProcessGouraudColor(Color ColorMin, Color NbColors);

    // normal processing methods. This methods can be use to assign an average
    // normal to a plot shared by many faces.
    void ResetNormal();
    ///
    void AddNormal(const Vector3 &);
    ///
    Vector3 GetAverageNormal();

#ifdef DEBUGOUT
    ///
    friend ostream & operator << (ostream &, const APlot &);
#endif
    ///
    APlot & operator +=(const Vector3 &);
    ///
    APlot & operator =(const Vector3 &);

  protected :
#ifdef DEBUGOUT
    ostream & DebugOut(ostream &) const;
#endif
    int fColor; // NOT byte because gouraud average calculation overload byte capacity
    int fCount;
    int fX, fY;
    real fZ;
    bool fProcessed;
    bool fColorProcessed;
    Vector3 fSumNormal;
    int fNormalCount;
  };

  /*****************************************************************************\
  * inline methods and properties
  \*****************************************************************************/

  inline APlot::APlot()
    :inherited()
  {
    fX = 0;
    fY = 0;
    fZ = 0;
    fProcessed = false;
    fColorProcessed = false;
    fColor = 0;
    fCount = 0;
  }

  inline APlot::APlot(real a, real b, real c)
    :inherited(a,b,c)
  {
    fX = 0;
    fY = 0;
    fZ = 0;
    fProcessed = false;
    fColorProcessed = false;
    fColor = 0;
    fCount = 0;
  }

  inline APlot::APlot(const APlot &p)
    :inherited(p)
  {
    fX = 0;
    fY = 0;
    fZ = 0;
    fProcessed = false;
    fColorProcessed = false;
    fColor = p.fColor;
    fCount = 0;
  }

  inline APlot::APlot(const Vector3 & aVector)
    :inherited(aVector)
  {
    fX = 0;
    fY = 0;
    fZ = 0;
    fProcessed = false;
    fColorProcessed = false;
    fColor = 0;
    fCount = 0;
  }

  inline APlot & APlot::AddColor(int aColor)
  {
    fColor += int(aColor);
    fCount++;
    return *this;
  }

  inline int APlot::X() const
  {
    return fX;
  }

  inline int APlot::Y() const
  {
    return fY;
  }

  inline real APlot::Z() const
  {
    return fZ;
  }

  inline int & APlot::GetColor()
  {
    return fColor;
  }

  inline int APlot::Count() const
  {
    return fCount;
  }

}

#endif
