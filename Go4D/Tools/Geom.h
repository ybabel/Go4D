#ifndef _GEOM_H_
#define _GEOM_H_

/**
*AUTHORS   : Babel Yoann
*            Bitterlich Jean-Yves
*TITLE     : geom.h, matrix and mathematical objects.
*DATE      : 5/11/95
*LAST REV  : 19/05/99
*OBJECT    : Matrix, Matrix3x3, Matrix4x4
*            Vector, Vector3, Plot
*            Base, Line, Plane
*ACTUAL    : no convertion between quaterion, rotation, and axis rotation
*BUGS      :
*NOTE      : becarefull not to set zero (cf common.h) to a too low value,
*            it would cause some precision problems
*/

/**
*
*         /--------------------- Matrix ------------------\
*	      /		                      |                       \
*     Matrix4x4         /------- Vector ------\         Matrix3x3
*                      /           |           \            |
*                 Quaternion    Vector3       Plane       Rotation
*
*  Base       Line
*
*/

#include <math.h>
#include "Common.h"


namespace Go4D
{

  /**
  *  MATRIX : <n,m>  width = n, height = m;
  *          NxM matrix, you can access elems with both [i], and (i,j)
  *          operators.
  *  operator *  : matrix multiplication, verifie that the arguments are
  *                compatible : Mmn = Mmi * Min (the dimesion `i` MUST be
  *                the same in the matrix
  *  operator !  : the matrix is transposed, return a temporary
  *  operator << : display the matrix, each vertex is separated by a blank
  *                line
  */
  class Matrix
  {
  protected :
    real * fElems;
    int fN, fM, fNM;

  public :
    ///
    Matrix(int n, int m);
    ///
    Matrix(const Matrix & m);
    ///
    ~Matrix();
    ///
    inline real & operator [] (int i) const {assert(i>=0&&i<fNM); return fElems[i]; }
    ///
    real & operator () (int ,int) const;
    ///
    Matrix & operator = (const Matrix & );
    ///
    Matrix operator + (const Matrix & ) const; //use rather +=
    ///
    Matrix operator - (const Matrix & ) const; //use rather -=
    ///
    Matrix operator * (const Matrix & ) const; //use rather *=
    ///
    Matrix operator * (real) const; //use rather *=
    ///
    Matrix operator / (real) const; //use rather /=
    ///
    Matrix operator !() const; // transposition
    ///
    Matrix operator -() const;
    ///
    Matrix & operator += (const Matrix &);
    ///
    Matrix & operator -= (const Matrix &);
    ///
    Matrix & operator *= (const Matrix &); // use rather *
    ///
    Matrix & operator *= (real);
    ///
    Matrix & operator /= (real);
    ///
    bool operator == (const Matrix &);
    ///
    bool operator != (const Matrix &);
#ifndef UNDER_CE
    ///
    friend ostream & operator << (ostream & , const Matrix & );
#endif
  };


  /**
  * Matrix4x4
  * Note : not used currently
  */
  class Matrix4x4 : public Matrix
  {
  public :
    ///
    Matrix4x4():Matrix(4,4){}
    ///
    Matrix4x4(const Matrix4x4 & m):Matrix(m){}
    ///
    Matrix4x4(const Matrix & m):Matrix(m){}
  };


  /**
  * Matrix3x3
  */
  class Matrix3x3 : public Matrix
  {
    friend class FilteredImage;
  public :
    ///
    Matrix3x3():Matrix(3,3){}
    ///
    Matrix3x3(const Matrix3x3 & m):Matrix(m){}
    ///
    Matrix3x3(const Matrix & m):Matrix(m){}
  };


  /**
  * Rotation : rotation matrix, use the methods Rotate... (X, Y, Z) to
  *            set the eulers angles, or directly RotateEuler
  */
  class Rotation : public Matrix3x3
  {
  public :
    ///
    Rotation():Matrix3x3(){}
    ///
    Rotation(const Rotation & r):Matrix3x3(r){}
    ///
    Rotation(const Matrix & r):Matrix3x3(r){}
    ///
    Rotation& RotateX(real teta);
    ///
    Rotation& RotateY(real phi);
    ///
    Rotation& RotateZ(real psy);
    ///
    Rotation& RotateEuler(real teta, real phi, real psy);
  };


  /**
  * VECTOR : operator + -, scalar product, multiplication by a constant
  *          and conversion operator (->MATRIX)
  */
  class Vector : public Matrix
  {
  public :
    ///
    Vector(int n):Matrix(1,n){}
    ///
    Vector(const Vector & v):Matrix(v){}
    ///
    Vector(const Matrix &m):Matrix(m){}
    ///
    real & operator [] (int i) const;
    ///
    real & operator () (int i) const;
    ///
    Vector operator + (const Vector & v) const;
    ///
    Vector operator - (const Vector & v) const;
    ///
    real   operator * (const Vector & v) const;
    ///
    Vector operator * (real r) const;
    ///
    Vector operator / (real r) const;
  };


  /**
  *  VECTOR3 : vectorial product added, Norme retrun the norme of the vector,
  *            Normalize process the vector that has the same direction,
  *            but wich Norme is 1 (precision problem here)
  */
  class Vector3 : public Vector
  {
  public :
    ///
    Vector3():Vector(3){}
    ///
    Vector3(const Vector3 &v):Vector(v){}
    ///
    Vector3(const Matrix &m):Vector(m){}
    ///
    Vector3(real, real, real);
    ///
    Vector3 operator^(const Vector3& v) const;  // vectorial product
    ///
    void Set(real a, real b, real c);
    ///
    real Norme() const;
    ///
    bool Normalize(); // return if Norme>0
    ///
    Vector3 Normalized(); // return if Norme>0
    ///
    real CosAngle(const Vector3&) const;        // = cos(*this,v)
    ///
    real SinAngle(const Vector3&) const;        // = sin(*this,v)
    ///
    real Distance(const Vector3&) const;
  };

  /**
  * Quaternion
  * note : not used currently
  */
  class Quaternion : public Vector
  {
  public :
    ///
    Quaternion ():Vector(4){}
    ///
    Quaternion(const Quaternion &v):Vector(v){}
    ///
    Quaternion(const Matrix &m):Vector(m){}
    ///
    Quaternion operator * (const Quaternion & q) const;
    ///
    real Real() const;
    ///
    Vector3 Imag() const;
    ///
    Matrix4x4 Q() const;
    ///
    Matrix4x4 W() const;
  };


  /**
  * Plane
  */
  class Plane : public Vector
  {
  public :
    ///
    Plane() : Vector(4){}
    ///
    Plane(const Plane &v):Vector(v){}
    ///
    Plane(const Matrix &m):Vector(m){}
    ///
    Plane(const Vector3 & , const Vector3 &, const Vector3 &);
    ///
    Plane(const Vector3 & normal, real coef);
    ///  the normal and the last coef of the planear equation
    Plane(const Vector3 &normal, const Vector3 & plot);
    ///  the normal, and a plot on the plane
    Vector3 Normal() const;
    ///
    real GetDist() const;
    ///
    real Dist(const Vector3 &plot) const;
    ///  optimized cutting, dont process dist1 and dist2 anymore
    Vector3 Cut(const Vector3 &plot1, const Vector3 &plot2,
      real dist1, real dist2, real & coord) const;
  };


  typedef real BaseCoefs[4][3];
  /**
  * Base : You can initialize the coefs of the base with the type BaseCoefs
  *        wich contains the coordinates of the origin and all the vectors
  *        of the base
  */
  class Base
  {
  public  :
    ///
    Vector3 origin;
    ///
    Rotation matrx;
    ///
    real fTet, fPhi, fPsi;
    ///
    real fSizeX, fSizeY, fSizeZ;

    ///
    Base();
    ///
    Base(real Ox,  real Oy,  real Oz,
      real a11, real a12, real a13,
      real a21, real a22, real a23,
      real a31, real a32, real a33);
    ///
    Base(BaseCoefs coefs);
    ///
    Base(real Ox,   real Oy,  real Oz,
      real teta, real phi, real psi);
    ///
    Base(const Base&);
    ///
    Base(const Vector3 & origin, const Rotation & matrx);
    ///  M in Main Base
    void ChgPointToOwnBase(Vector3&) const;
    ///  M in Own(base) Base
    void ChgPointToMainBase(Vector3&) const;
    ///  (Old->Own)Base
    void ChgPointToBase(Vector3&,Base& OldBase) const;
    ///  M in Main Base
    void ChgVectorToOwnBase(Vector3&) const;
    ///  M in Own(base) Base
    void ChgVectorToMainBase(Vector3&) const;
    ///  (Old->Own)Base
    void ChgVectorToBase(Vector3&,Base& OldBase)const;
    ///
    Vector3 ChangeCoord(const Vector3 &) const;
    ///
    Base & AddBase(const Base &);
    ///
    ~Base();
  };


  /**
  * Line
  */
  class Line
  {
  public :
    ///
    Vector3 origin;
    ///
    Vector3 direction;
    ///
    Line();
    ///
    Line(Vector3&, Vector3&);
    ///
    ~Line();
    ///
    void Set(Vector3&, Vector3&);
    ///
    real Distance(const Vector3&) const;
  };

}
#endif
