/*****************************************************************************\
 *AUTHORS   : Babel Yoann
 *            Bitterlich Jean-Yves
 *TITLE     : geom.h, matrix and mathematical objects.
 *DATE      : 5/11/95
 *LAST REV  : 31.01.96
 *OBJECT    : Matrix, Matrix3x3, Matrix4x4
 *            Vector, Vector3, Plot
 *            Base, Line, Plane
 *ACTUAL    : no convertion between quaterion, rotation, and axis rotation
 *BUGS      :
 *NOTE      : becarefull not to set zero (cf common.h) to a too low value,
 *            it would cause some precision problems
\*****************************************************************************/

/*****************************************************************************\
 *
 *         /--------------------- Matrix ------------------\
 *	      /		                      |                       \
 *     Matrix4x4         /------- Vector ------\         Matrix3x3
 *                      /           |           \            |
 *                 Quaternion    Vector3       Plane       Rotation
 *		                 |
 *			             Point3D
 *
 *
 *  Base       Line
 *
\*****************************************************************************/

#ifndef _GEOM_H_
#define _GEOM_H_

#include <math.h>
#include <iostream.h>
#include "common.h"

/*****************************************************************************\
 *  MATRIX : <n,m>  width = n, height = m;
 *          NxM matrix, you can access elems with both [i], and (i,j)
 *          operators.
 *  operator *  : matrix multiplication, verifie that the arguments are
 *                compatible : Mmn = Mmi * Min (the dimesion `i` MUST be
 *                the same in the matrix
 *  operator !  : the matrix is transposed, return a temporary
 *  operator << : display the matrix, each vertex is separated by a blank
 *                line
\*****************************************************************************/

class Matrix
{
  protected :
  real * fElems;
  int fN, fM, fNM;

  public :
  Matrix(int n, int m);
  Matrix(const Matrix & m);
  ~Matrix();
  real & operator [] (int) const;
  real & operator () (int ,int) const;
  Matrix & operator = (const Matrix & );
  Matrix operator + (const Matrix & ) const; //use rather +=
  Matrix operator - (const Matrix & ) const; //use rather -=
  Matrix operator * (const Matrix & ) const; //use rather *=
  Matrix operator * (real) const; //use rather *=
  Matrix operator !() const; // transposition
  Matrix operator -() const;
  Matrix & operator += (const Matrix &);
  Matrix & operator -= (const Matrix &);
  Matrix & operator *= (const Matrix &); // use rather *
  Matrix & operator *= (real);
  bool operator == (const Matrix &);
  bool operator != (const Matrix &);
  friend ostream & operator << (ostream & , const Matrix & );
};


/*****************************************************************************\
 * Matrix4x4
 * Note : not used currently
\*****************************************************************************/

class Matrix4x4 : public Matrix
{
  public :
  Matrix4x4():Matrix(4,4){}
  Matrix4x4(const Matrix4x4 & m):Matrix(m){}
  Matrix4x4(const Matrix & m):Matrix(m){}
};


/*****************************************************************************\
 * Matrix3x3
\*****************************************************************************/

class Matrix3x3 : public Matrix
{
  friend class FilteredImage;
  public :
  Matrix3x3():Matrix(3,3){}
  Matrix3x3(const Matrix3x3 & m):Matrix(m){}
  Matrix3x3(const Matrix & m):Matrix(m){}
};


/*****************************************************************************\
 * Rotation : rotation matrix, use the methods Rotate... (X, Y, Z) to
 *            set the eulers angles, or directly RotateEuler
\*****************************************************************************/

class Rotation : public Matrix3x3
{
  public :
  Rotation():Matrix3x3(){}
  Rotation(const Rotation & r):Matrix3x3(r){}
  Rotation(const Matrix & r):Matrix3x3(r){}
  Rotation& RotateX(real teta);
  Rotation& RotateY(real phi);
  Rotation& RotateZ(real psy);
  Rotation& RotateEuler(real teta, real phi, real psy);
};


/*****************************************************************************\
 * VECTOR : operator + -, scalar product, multiplication by a constant
 *          and conversion operator (->MATRIX)
\*****************************************************************************/

class Vector : public Matrix
{
  public :
  Vector(int n):Matrix(1,n){}
  Vector(const Vector & v):Matrix(v){}
  Vector(const Matrix &m):Matrix(m){}
  real & operator [] (int i) const;
  real & operator () (int i) const;
  Vector operator + (const Vector & v) const;
  Vector operator - (const Vector & v) const;
  real   operator * (const Vector & v) const;
  Vector operator * (real r) const;
};


/*****************************************************************************\
 *  VECTOR3 : vectorial product added, Norme retrun the norme of the vector,
 *            Normalize process the vector that has the same direction,
 *            but wich Norme is 1 (precision problem here)
\*****************************************************************************/

class Vector3 : public Vector
{
  public :
  Vector3():Vector(3){}
  Vector3(const Vector3 &v):Vector(v){}
  Vector3(const Matrix &m):Vector(m){}
  Vector3(real, real, real);
  Vector3 operator^(const Vector3& v) const;  // vectorial product
  void Set(real a, real b, real c);
  real Norme() const;
  bool Normalize();
  real CosAngle(const Vector3&) const;        // = cos(*this,v)
  real SinAngle(const Vector3&) const;        // = sin(*this,v)
};


/*****************************************************************************\
 * Point3D
\*****************************************************************************/

class Point3D : public Vector3
{
  public :
  Point3D():Vector3(){}
  Point3D(const Point3D &p):Vector3(p){}
  Point3D(const Matrix &m):Vector3(m){}
  Point3D(real a, real b, real c):Vector3(a,b,c){}
  real Distance(const Point3D&) const;
};

/*****************************************************************************\
 * Quaternion
\*****************************************************************************/

class Quaternion : public Vector
{
  public :
  Quaternion ():Vector(4){}
  Quaternion(const Quaternion &v):Vector(v){}
  Quaternion(const Matrix &m):Vector(m){}
  Quaternion operator * (const Quaternion & q) const;
  real Real() const;
  Vector3 Imag() const;
  Matrix4x4 Q() const;
  Matrix4x4 W() const;
};


/*****************************************************************************\
 * Plane
\*****************************************************************************/

class Plane : public Vector
{
  public :
  Plane() : Vector(4){}
  Plane(const Plane &v):Vector(v){}
  Plane(const Matrix &m):Vector(m){}
  Plane(const Vector3 & , const Vector3 &, const Vector3 &);
  Plane(const Vector3 & normal, real coef);
  // the normal and the last coef of the planear equation
  Plane(const Vector3 &normal, const Vector3 & plot);
  // the normal, and a plot on the plane
  Vector3 Normal() const;
  real GetDist() const;
  real Dist(const Vector3 &plot) const;
  Vector3 Cut(const Vector3 &plot1, const Vector3 &plot2,
		       real dist1, real dist2, real & coord) const;
  //optimized cutting, dont process dist1 and dist2 anymore
};


/*****************************************************************************\
 * Base : You can initialize the coefs of the base with the type BaseCoefs
 *        wich contains the coordinates of the origin and all the vectors
 *        of the base
\*****************************************************************************/

typedef real BaseCoefs[4][3];

class Base
{
  public  :
  Point3D    origin;
  Rotation matrx;
  Base();
  Base(real Ox,  real Oy,  real Oz,
       real a11, real a12, real a13,
       real a21, real a22, real a23,
       real a31, real a32, real a33);
  Base(BaseCoefs coefs);
  Base(real Ox,   real Oy,  real Oz,
       real teta, real phi, real psi);
  Base(const Base&);
  Base(const Point3D & origin, const Rotation & matrx);
  void ChgPointToOwnBase(Point3D&) const;              // M in Main Base
  void ChgPointToMainBase(Point3D&) const;             // M in Own(base) Base
  void ChgPointToBase(Point3D&,Base& OldBase) const;   // (Old->Own)Base
  void ChgVectorToOwnBase(Vector3&) const;           // M in Main Base
  void ChgVectorToMainBase(Vector3&) const;          // M in Own(base) Base
  void ChgVectorToBase(Vector3&,Base& OldBase)const; // (Old->Own)Base
  ~Base();
};


/*****************************************************************************\
 * Line
\*****************************************************************************/

class Line
{
  public :
  Point3D    origin;
  Vector3  direction;
  Line();
  Line(Point3D&, Vector3&);
  ~Line();
  void Set(Point3D&, Vector3&);
  real Distance(const Point3D&) const;
};

#endif
