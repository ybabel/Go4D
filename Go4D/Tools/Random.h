#ifndef _RANDOM_H_
#define _RANDOM_H_
/**
*AUTHOR    : Babel Yoann
*TITLE     : Random.h
*DATE      : 02/06/99
*OBJECT    : Random vector3 generation, uniform/non uniform
*/

#include "Geom.h"


namespace Go4D
{

  /** A Randomizer is a class that is capable to generate random vectors. The
  * distribution in space of the generation depend on the method choosed
  * For example a uniform generation guarant that each space point are
  * equiprobable, whereas a non uniform generation guarant that some regions
  * are more probable than others...
  */
  class Vector3Randomizer
  {
  protected:
    real fRange;
    real fTransformCoef;
    real fTransformTranslate;
    Vector3 & Transform(Vector3 & );

  public:
    ///
    Vector3Randomizer();
    ///  The generation is done in a Cube that is centered at (0,0,0) and that size is Range.
    void SetRange(real);
    ///
    void SetAffineTransform(real coef, real translat);
    ///
    virtual Vector3 Generate()=0;
  };

  /// a particular randomizer used by beast project mainly
  class UniformVector3Randomizer : public Vector3Randomizer
  {
    typedef Vector3Randomizer inherited;
  public:
    ///
    virtual Vector3 Generate();
  };

  typedef real (*Repartition)(real);
  typedef bool (*Compare)(real, real);
  real SQRT(real);
  real EQUAL(real);
  bool SUP(real, real);
  bool SUPEQ(real, real);
  bool INF(real, real);
  bool INFEQ(real, real);


  /// an height field that can be generated fractally
  class HeightField
  {
  public:
    ///
    HeightField(int width, int height, real azero=0.0, real amin=0.0, real amax=1.0);
    ///
    ~HeightField();
    ///
    void SetHeight(int x, int y, real h);
    ///
    real GetHeight(int x, int y) const;
    ///  linear access
    inline int CellCount() const { return fWidth*fHeight; }
    ///
    inline real Cell(int i) const { return fField[i]; }
    ///
    void CellToXY(int cell, int &x, int &y);
    ///
    void GenerateFractalHeights(real max, Repartition func=EQUAL);
    ///
    int Width() const { return fWidth; }
    ///
    int Height() const { return fHeight; }
    ///
    real Max() const;
    ///
    real Min() const;
    ///
    int NumberCellsSuperior(real) const;
    ///
    int NumberCellsInferior(real) const;
    ///
    int NumberCells(real, Compare func) const;

  protected:
    void FractalHeight(int xmin, int ymin, int xmax, int ymax, real height, Repartition func);
    real * fField; // aggregated 2 dimensional array of real
    int fWidth;
    int fHeight;
    real fMin;
    real fMax;
    real fZero;
  };

  ///  Assumes that the heightfield MAX is < 1.0
  class NonUniformVector3Randomizer : public Vector3Randomizer
  {
    typedef Vector3Randomizer inherited;
  public:
    ///
    NonUniformVector3Randomizer(HeightField *);
    ///
    virtual ~NonUniformVector3Randomizer();
    ///
    virtual Vector3 Generate();

  protected:
    HeightField * fHeightField; // aggregated
  };

}
#endif
